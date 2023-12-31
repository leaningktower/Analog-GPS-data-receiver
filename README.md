<img width="1100" alt="Pasted Graphic" src="https://github.com/leaningktower/Analog-GPS-data-receiver/assets/17516571/4cfeafa1-44cb-4742-809d-766c274e0fdf">

The picture above shows a pure analog Global Positioning System (GPS) data receiver that can receive the Navigation (NAV) Message from the GPS satellite. The receiver is a dual conversion receiver that includes five homemade PCB boards. A tinyFPGA is used for generating the Coarse Acquisition (C/A) code which retrieve the direct-sequence spread spectrum (DSSS) GPS signal submerged below the thermal noise level.

I was originally motivated by the works [1, 2] of Matjaz Vidmar (S53MV) and Andrew Holme. Both of the receivers are based on the 1-bit analog-to-digital converter (ADC) structure that uses on a single comparator that greatly the hardware of the design. The receiver of S53MV was built more than 30 years ago, based on a digital signal processor (DSP) that is made from fully discreet logic chips and a MC68010 processor. And the receiver of Andrew Holme is based on FPGA with a Forth-like Embedded CPU and a Raspberry Pi in charge of the fast Fourier transform (FFT) to speed up the satellite tracking process. Both two projects include great amount of minor details in software make them a bit daunted to the people who wants to learn from it and build their own receiver.

Another impressive analog GPS receiver comes from the book [3] of Dan Doberstein. In the Chapter 7 of this book, he provides a very detailed description of the GPS100SC receiver, which is a single channel data receiver and you can still literally build four of these receivers and find out your position! However, the receiver is not fully open sourced but makes me feel excited to build my own receiver from scratch. As a fan of analog electronics, it is also a fun project!

### Hardware Architecture

<img width="1100" alt="Pasted Graphic 1" src="https://github.com/leaningktower/Analog-GPS-data-receiver/assets/17516571/9cb75191-05ad-4593-9742-cd3b5acdf3d3">

The block diagram of the receiver is shown above. The receiver includes 5 different part, the 1st mixer board, 1st LO board, 2nd mixer and correlation board, IF signal processing board and FPGA interfacing board (not shown). It is indicated by the dashed blocks in the diagram.

A commercial GPS antenna with internal amplifier and filters is used that can be easily find on AliExpress and Amazon. The first IF is set to around 60 MHz, which is relatively high to avoid the interference between antenna and local oscillator with poor shielding. LNA (BGA420) is placed before the 1st mixer (MAX2680) to decrease the noise figure of the receiver with a long coax cable between the antenna and receiver.

The 1st LO is based on an old fashioned integer-N frequency synthesiser (ADF4113) and discreet VCO (YSGM151708). Any frequency error caused by integer-N can be easily compensated by using a VCXO as the frequency reference. Further more, the VCXO also provides the frequency scan and the doppler frequency tracking of this receiver.

A 4th order LC filter with ~10 MHz bandwidth is placed after the 1st mixer, which removes most of the LO leakage of the first mixer. The LNA after the 1st mixer is optional but it is necessary for the impedance matching of the 1st mixer’s output.  The 2nd IF is 10.7 MHz and use a random 48 MHz oscillator as the 2nd LO. Instead of single tone signal, the 2nd LO is modulated with the dithered C/A code using Tau dither method, which switches the C/A code between the early and late phase at a frequency around 166 Hz. When the local C/A code is synchronised to the received signal, the DSSS signal is demodulated in to a Binary Phase-shift keying (BPSK) signal with 50 Hz chip rate and recovered from the thermal noise level. The down converted 10.7 MHz 2nd IF signal is sent to a crystal filter with 1.8 kHz bandwidth centred at 10.699 MHz.

The IF signal processing is based on SA604/SA605 FM receiver chip. The GPS signal strength is around -50 dBm before the IF signal processing board, which is more than enough for this chip with 90 dB dynamic range. The IF limiter output is filtered and sent to the comparator for finding out the IF frequency and used for tracking the doppler shift. The limiter output is also sent to another 10.7 MHz crystal used for quadrature detector, which converts the phase flip of the BPSK signal to the pulses at audio output of the detector. On the other hand, the amplitude of the signal is indicated by the RSSI output, two comparator is used to determine whether the signal strength is strong enough for the code phase or doppler frequency locking. A 166 Hz active band-pass filter is used to tell the FPGA to slow down or speed up the NCO to synchronise the code phase.

### FPGA Architecture

**C/A code tracking/lock and NAV data recovery**

<img width="1100" alt="Pasted Graphic 3" src="https://github.com/leaningktower/Analog-GPS-data-receiver/assets/17516571/bbeb4a01-8cf7-4c16-a5cc-ab19d57110cf">

The block diagram above shows the block diagram for the C/A code tracking and locking system. The system clock is 24.552 MHz, which is 24 times of the 1.023 MHz chip rate. When the system is unlocked, a 8 bit u/d counter continuous trigger the div by 23 input of the 5 bit ring counter. That gives the 1.023 MHz C/A code clock frequency deviated by 64 kHz / 256 / 24 = 10.4 Hz. This offset frequency helps the system find the correlation peak every 1023 / 10.4 = 98 second where 1023 is the length of the C/A code. When the system finds the correlation peak, the code lock signal is enabled and the the u/d counter is controlled by the XOR between the 166 Hz dither signal and dither induced AM signal from RSSI. The detailed description can be found in [3].

**Doppler tracking/lock**

<img width="1100" alt="Pasted Graphic 4" src="https://github.com/leaningktower/Analog-GPS-data-receiver/assets/17516571/cb070d28-9a8b-42f1-b7ff-b5300fab0a75">

The block diagram of doppler tracking/lock is shown above. Due to the narrowband 10.7 MHz filter for finding the IF frequency, the doppler lock needs higher RSSI trigger level than the code lock and the system is only locked when both signals are enabled. When doppler lock is not enabled, the 6 bit counter continues over flowed with a rate of 200 / 64 = 3 Hz, which scan the PWM output from 0 to 1023 in 5 minutes, this serves as the doppler scan for corse finding the satellite doppler shift between -5 to 5 kHz. When both the doppler scan and code scan located around correct position, the RSSI will be high enough to enable both signal. When the doppler lock is enabled the 6 bit up/down counter is controlled by the magnitude comparator which compares the measured IF center frequency with the setting frequency, which is 10.699 MHz. Since the counter is reset every 1/25 second, the 12 bit frequency counter will be overflowed 104 times and gives a final reading around 1976. A slightly different frerqnency is used in the receiver considering the best sensitivity of the quadrature detector circuit, which is determined by the center frequency of the quartz crystal being used.

### Detailed schematic

I made some small modifications to the circuit and the plots below gives the schematic that is updated in real design, which may show minor difference to the files in the folder.

<img width="1100" alt="Pasted Graphic 6" src="https://github.com/leaningktower/Analog-GPS-data-receiver/assets/17516571/e58991fe-3bbd-45c6-b69b-cfb48afc4deb">
First mixer and preamp  

The transistor Q1 and bias tee U1 provides the clean power supply to the active antenna, two low noise amplifier is placed before and after the mixer U3. The impedance matching network of the mixer is calculated by the S parameters given by the manufactor. The board convert the GPS signal from 1575.42 MHz to 58.7 MHz and provides a overall gain around 41 dB.

<img width="1100" alt="Pasted Graphic 7" src="https://github.com/leaningktower/Analog-GPS-data-receiver/assets/17516571/10925ff6-cfae-40ce-9bf7-f681c0ce8558">
Second mixer and correlator  

Two mixer is used for down converting and C/A code modulation of the LO signal. The maximum output power of U3 is only 0 dBm which is significant lower than the require LO power of 7 dBm, however this doesn’t seems to be a big problem and only add several dB loss to the system. Three JFET follower is used for monitoring the 1st IF, 2nd IF and 2nd LO, note that the drain and source is reversed in the schematic and PCB design. The value of C12 is used to tune the bandwidth of the filter, which needs additional tuning for different crystals.

<img width="1100" alt="Pasted Graphic 9" src="https://github.com/leaningktower/Analog-GPS-data-receiver/assets/17516571/476e41a1-15dd-4b62-a2f9-d5ba7201a986">
IF signal processing  

A 10.7 MHz crystal oscillator Y1 is used but it can be a normal ceramic filter since there is already a narrowband filter before IF signal processing board. The fast comparator U2 and the OpAmp U8 needs dual power supply and a negative voltage generator is built from the CMOS inverter U4. Note that the VEE pin of U8 is tied to ground in the PCB design.

<img width="1100" alt="Pasted Graphic 10" src="https://github.com/leaningktower/Analog-GPS-data-receiver/assets/17516571/6c1deb64-3232-4f76-9308-bfa06e135830">
First local oscillator  

An ATtiny85 is used for configure the ADF4331 setting after power up. The potentiometer is used for coarse adjusting the out frequency and frequency can be externally tuned by the tuning voltage through the connector J1. The U5 amplifier is optional considering the tuning voltage range of the VCO. The output power of this board is around 0 dBm.

<img width="1100" alt="Pasted Graphic 11" src="https://github.com/leaningktower/Analog-GPS-data-receiver/assets/17516571/65ca15c5-7847-4f02-8ce7-5411a9b4a879">
FPGA interfacing   

This boards provides all the interconnect between the TinyFPGA and external circuits. Another ATtiny85 is used for communicating between the FPGA and external keyboard/display. The external keyboard is used to set the SVID of the satellite, centre IF frequency and initialised output value of PWM. It is also used to monitor the realtime PWM output setting and IF counter frequency. Note that the power supply of U5 is reversed in the PCB design.

### Result

The final success takes several weekends for trouble shooting. The most significant problem is cause by the interference of the 24.552 MHz system clock generator. Originally I was using a Si5351’s programmed output as the system clock, however it generates a bunch of tones within the bandwidth of sensitive GPS antenna. Since it is hard to find any crystal works at this frequency, a final solution is done by the decapping a 24.576 MHz crystal and modifying the frequency by a sharpie pen. It is also frequency stabilised through external temperature controller.

It usually takes several minutes to lock to a satellite. A digital voltage meter is added to the RSSI output of the SA615 for monitoring the signal strength. The background noise gives a RSSI output around 2 V and gives a reading between 2.3 to 3 V when it is locked to a satellite. I use another GPS receiver to determine the SVID of the satellite I could find. 

<img width="500" alt="Pasted Graphic 14" src="https://github.com/leaningktower/Analog-GPS-data-receiver/assets/17516571/d5704f1b-9002-4e4e-974f-ccb2223af5e5">

The GPS signal at the 2nd IF monitoring port, which shows the demodulated GPS signal recovered from the thermal noise background.

<img width="500" alt="Pasted Graphic 13" src="https://github.com/leaningktower/Analog-GPS-data-receiver/assets/17516571/46aa5be1-0261-4078-9be5-002186c9cc55">

The NAV data output (yellow) and quadrature detector output (cyan) of the GPS data receiver.


______________________________________________________________________________________________________________
Reference:

[1] GPS/GLONASS receiver by Matjaž Vidmar, S53MV; https://lea.hamradio.si/~s53mv/navsats/theory.html

[2] Homemade GPS Receiver by Andrew Holme; http://www.aholme.co.uk/GPS/Main.html

[3] PRINCIPLES OF GPS RECEIVERS - A HARDWARE APPROACH by Dan Doberstein; http://www.dkdinst.com/gpstxt.html
