//  ACLK = LFXT1/8 = 32768/8, MCLK = SMCLK = target DCO
//  External watch crystal 32768Hz installed on XIN XOUT is required for ACLK 
//
//           MSP430x2xxx
//         ---------------
//     /|\|            XIN|-
//      | |               | 32kHz
//      --|RST        XOUT|-
//        |               |
//        |           P1.0|--> LED
//        |           P1.4|--> SMLCK = target DCO



#include <msp430.h>
#include <stdint.h>

#define DELTA_10MHZ    4883               // 4883 x 2048Hz = 10MHz
#define CALDCO_10MHz * (uint8_t *)0x10BE  //CALDCO_D calibration
#define CALBC1_10MHz * (uint8_t *)0x10BF  //CALBC1_D calibration


uint8_t CAL_DATA[2];                          // Temp. storage for constants
volatile uint16_t i;
int16_t j;
int8_t *Flash_ptrB;                           // Segment B pointer
void Set_DCO(uint16_t Delta);

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  
  BCSCTL3 |= XCAP_3;                       // Launchpad watch crystals need ~12.5 pF

  P1OUT = 0;                               // Clear P1 output latches
  P1SEL = BIT4;                             // P1.4 SMCLK output
  P1DIR = BIT0+BIT4+BIT6;                   // P1.0,4,6 output

  while (IFG1 & OFIFG){
    IFG1 &= ~OFIFG; // Clear OSCFault flag
    j=10;            
    while (j){                                // Delay for XTAL stabilization
      P1OUT ^= BIT0;                          // Toggle RED LED
      for (i = 0; i < 0x8000; i++);           // SW Delay
      j--;
    }                                       
  }    
  P1OUT = BIT0;                             // Red  LED on
  j = 0;                                    // Reset pointer
  
  Set_DCO(DELTA_10MHZ);                     // Set DCO and obtain constants
  CAL_DATA[j++] = DCOCTL;
  CAL_DATA[j++] = BCSCTL1;
  

  #define dco1MHz 1000000/2048              // (1000000/4096=244)
  Set_DCO(dco1MHz);                         // Set DCO to 1MHz, we need that to flash information memory

  Flash_ptrB = (int8_t *)0x1080;            // Point to beginning of seg B - so we can keep original cal consts in seg A
  FCTL2 = FWKEY + FSSEL0 + FN1;             // MCLK/3 for Flash Timing Generator
  FCTL1 = FWKEY + ERASE;                    // Set Erase bit
  FCTL3 = FWKEY;                            // Clear LOCK bit
  *Flash_ptrB = 0x00;                       // Dummy write to erase Flash seg B
  FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation
  Flash_ptrB = (int8_t *)0x10BE;            // Point to beginning of cal consts
  for (j = 0; j < 2; j++)
  *Flash_ptrB++ = CAL_DATA[j];              // re-flash DCO calibration data
  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK  bit

  if (CALDCO_10MHz ==0xFF || CALBC1_10MHz == 0xFF)                                     
  {  
   while(1)LPM4;                               // If calibration constants erased do not load, trap CPU!!
  } 
  
  DCOCTL=CALDCO_10MHz;
  BCSCTL1=CALBC1_10MHz;
  
  P1OUT = 0;                                // Red  LED off	
  while (1){                                //you can measure P1.4 it should output MCLK = SMCLK = 10MHz
    j=10;            
    while (j){                                
      P1OUT ^= BIT6;                     // Toggle Green LED
      for (i = 0; i < 0x8000; i++);      // SW Delay
      j--;
    }                                     
  }
}

void Set_DCO(unsigned int Delta)            // Set DCO to selected frequency
{
  uint16_t Compare, Oldcapture = 0;

  BCSCTL1 |= DIVA_3;                        // ACLK = LFXT1CLK/8
  TACCTL0 = CM_1 + CCIS_1 + CAP;            // CAP, ACLK
  TACTL = TASSEL_2 + MC_2 + TACLR;          // SMCLK, cont-mode, clear

  while (1)
  {
    while (!(CCIFG & TACCTL0));             // Wait until capture occured
    TACCTL0 &= ~CCIFG;                      // Capture occured, clear flag
    while (!(CCIFG & TACCTL0));             // Wait until capture occured
    TACCTL0 &= ~CCIFG;                      // Capture occured, clear flag - this increase resolution 2 times
    Compare = TACCR0;                       // Get current captured SMCLK
    Compare = Compare - Oldcapture;         // SMCLK difference
    Oldcapture = TACCR0;                    // Save current captured SMCLK

    if (Delta == Compare)
      break;                                // If equal, leave "while(1)"
    else if (Delta < Compare)
    {
      DCOCTL--;                             // DCO is too fast, slow it down
      if (DCOCTL == 0xFF)                   // Did DCO roll under?
        if (BCSCTL1 & 0x0f)
          BCSCTL1--;                        // Select lower RSEL
    }
    else
    {
      DCOCTL++;                             // DCO is too slow, speed it up
      if (DCOCTL == 0x00)                   // Did DCO roll over?
        if ((BCSCTL1 & 0x0f) != 0x0f){
          BCSCTL1++;                        // Sel higher RSEL
        }
    }
  }
  TACCTL0 = 0;                              // Stop TACCR0
  TACTL = 0;                                // Stop Timer_A
  BCSCTL1 &= ~DIVA_3;                       // ACLK = LFXT1CLK
}
