This is a code for developing an Interavtive Voice Response where basic peripherals interfaced will respond to the voice commands captured.
There are 4 modes of operation
- Mode 1 is used simply as an audio equalizer where the frequency domain profiles of audio is displayed on the lcd
- Mode 2,3,4 is used for peripheral response to captured voice

Simple Dominant frequency algorithm with envelope detection is used to determine words from the captured audio

Firmware codes were written for
1) USART (universal serial asynchronous Receive and Transmit)
	usart.h
2) Graphical LCD
	lcd.h
3) Analog to Digital Converter
	adc.h
4) Interfacing PB0 as an External Interrupt input pin to toggle between modes
	EXT0_interrupt.h 

These driver codes are written to interface peripherals to ARM Cortex M4 Processor (STMF4 Discovery board) which is then compiled and run on CooCox IDE

Additional Library files for DSP processing can be obtained from cmsis libraries provided by STM.
