swEth430
========

Software Ethernet for msp430 with USCI using 74HC86.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, IT MAY DISTROY HARDWARE AND LEAD TO DATA LOSS, NOW I HAVE YOUR SOUL, HAVE A NICE DAY.
This is alpha version, stay tuned.

  - What is main disadvantage of swEth430?
  - No Rx(receive), Tx only and I can't get it to work with my old 10/100mbps switch, but it did work with my router.

  - What is this?
  - This is Software Ethernet for msp430, similar to IgorPlug-USB (AVR) http://www.cesko.host.sk/IgorPlugUDP/IgorPlug-UDP%20(AVR)_eng.htm
  
  - Why would you do this?
  - There is Bit-Bang USB on MSP430 http://forum.43oh.com/topic/2962-bit-bang-usb-on-msp430g2452/ , similar to http://www.cesko.host.sk/IgorPlugUSB/IgorPlug-USB%20(AVR)_eng.htm

  - What are alternatives to this?
  - For under $4.20 you can get ENC28J60 Module or for under $10 you can get W5100 Ethernet Shield for Arduino from ebay. Or you can use wireless solution, like those cheap RF 433/315 modules or NRF24L01+ modules which are quite good and cheap.

  - What hardware is needed for swEth430?
  - One 74HC86 (under $2.7 for 10pcs on ebay), 2pcs 68 Ohm and one 220 Ohm resistor.

  - What is Ethernet 10BASE-T?
  - Ethernet 10BASE-T uses Manchester coded signaling, copper twisted pair(UTP) cabling with characteristic impedance 100 Ohms, star topology, voltage levels +/- 2.5V (over twisted pair), NLP(normal link pulses) generated at 8-24mS interval, unipolar positive-only electrical pulses of a nominal duration of 100 ns, with a maximum pulse width of 200 ns.

  - Does it honour Ethernet specification?
  - No, swEth430 use +/- 3.6V voltage levels.

  - What msp430 hardware peripherals did swEth430 use?
  - WDT as interval timer, USCI A or USCI B channel and DCO running at 10MHz.

  - WDT?
  - WDT is used as interval timer to generate NLP every 12000/4/64 = 47Hz => one pulse every 21mS.

  - USCI?
  - All MSP430G2xx3(for example MSP430G2553) have USCI - Universal Serial Communication Interface.

  - 10MHz?
  - DCO on MSP430G2211 at room temperature can go up to 21MHz, unfortunately DCO on MSP430G2553 can go only up to 19MHz, so thanks to oPossum5150, I manage to make it work at 10Mhz reloading USCI Tx buffer every 8 cycles.

  - How to run msp430 at 10MHz?
  - There is CalibrateDCO10Mhz program, that using 32KHz watch xtal will setup 10MHz DCO calibration values at the end of information memory segment B, leaving factory calibration in segment A intact. But there is a catch, by default CCS4 erases information memory, so you have to go to Project -> Properties -> CCS Debug -> Target -> MSP430 Properties ->  Download Options -> Erase Options -> Erase main memory only < select this as shown in the picture debugconfig.gif . You have to do this for every project if you don't want to lose 10MHz DCO calibration values in information memory segment B.

  - What are supported protocols?
  - Pregenerated raw Ethernet packets(swEth430Magic) and UDP/IP packets(swEth430UDP).

  - How can I pregenerate Ethernet packets.
  - When I was looking for Ethernet implementations I find this article http://www.fpga4fun.com/10BASE-T.html , there you can find Delphi program EthernetUDP that can generate UDP packets for you, also there you can find really good Ethernet tutorial.
