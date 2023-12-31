/*
 AVR Soft I2C Master - Example for I2C EEPROMs
 Version: 1.0
 Author: Alex from insideGadgets (www.insidegadgets.com)
 Created: 12/02/2012
 Last Modified: 12/02/2012
 
 Using code from http://forums.adafruit.com/viewtopic.php?f=25&t=13722

 */

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#ifndef boolean
	typedef uint8_t boolean;
#endif 
#ifndef bool
	typedef uint8_t bool;
#endif 
#ifndef byte
	typedef uint8_t byte;
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#define LOW 0
#define HIGH 1
#define false 0
#define true 1

#define I2C_DELAY_USEC 4
#define I2C_READ 8
#define I2C_WRITE 0

// Initialize SCL/SDA/CSN pins
void SoftSpiMasterInit(void) {
	PORTB &= ~(1<<TWI_SDA_PIN);
	DDRB  |=  (1<<TWI_SDA_PIN);
	PORTB &= ~(1<<TWI_SCL_PIN);
	DDRB  |=  (1<<TWI_SCL_PIN);
	PORTB |=  (1<<TWI_CSN_PIN);
	DDRB  |=  (1<<TWI_CSN_PIN);
}

// De-initialize SCL/SDA/CSN pins
void SoftSpiMasterDeInit(void) {
	PORTB &= ~(1<<TWI_SDA_PIN);
	DDRB  &= ~(1<<TWI_SDA_PIN);
	PORTB &= ~(1<<TWI_SCL_PIN);
	DDRB  |=  (1<<TWI_SCL_PIN);
	PORTB |=  (1<<TWI_CSN_PIN);
	DDRB  |=  (1<<TWI_CSN_PIN);
}

// Read a byte from SPI
uint8_t SoftSpiMasterRead(void) {
	uint8_t b = 0;
	// Make sure pull-up enabled
	PORTB &= ~(1<<TWI_SDA_PIN);
	DDRB &= ~(1<<TWI_SDA_PIN);
	// Read byte
	for (uint8_t i = 0; i < 8; i++) {
		// Don't change this loop unless you verify the change with a scope
		b <<= 1;
		_delay_us(I2C_DELAY_USEC);
		PORTB |= (1<<TWI_SCL_PIN);
		if (bit_is_set(PINB, TWI_SDA_PIN)) b |= 1;
		PORTB &= ~(1<<TWI_SCL_PIN);
	}
	// End
	PORTB &= ~(1<<TWI_SCL_PIN);
	PORTB &= ~(1<<TWI_SDA_PIN);
	_delay_us(I2C_DELAY_USEC);
	return b;
}

// Write a byte to SPI
void SoftSpiMasterWrite(uint8_t data) {
	// Make sure output enabled
	PORTB &= ~(1<<TWI_SDA_PIN);
	DDRB  |=  (1<<TWI_SDA_PIN);
	_delay_us(I2C_DELAY_USEC);
	// Write byte
	for (uint8_t m = 0x80; m != 0; m >>= 1) {
		// Don't change this loop unless you verify the change with a scope
		if (m & data) { 
			PORTB |= (1<<TWI_SDA_PIN); 
		}
		else { 
			PORTB &= ~(1<<TWI_SDA_PIN); 
		}
		PORTB |= (1<<TWI_SCL_PIN);
		_delay_us(I2C_DELAY_USEC);
		PORTB &= ~(1<<TWI_SCL_PIN);
	}
	// End
	PORTB &= ~(1<<TWI_SCL_PIN);
	PORTB &= ~(1<<TWI_SDA_PIN);
	_delay_us(I2C_DELAY_USEC);
}

// Issue a start condition
void SoftSpiMasterStart(void) {
	PORTB |=  (1<<TWI_CSN_PIN);
	_delay_us(I2C_DELAY_USEC);
	PORTB &= ~(1<<TWI_CSN_PIN);
}
 

// Issue a stop condition
void SoftSpiMasterStop(void) {
	PORTB &= ~(1<<TWI_SCL_PIN);
	_delay_us(I2C_DELAY_USEC);
	PORTB &= ~(1<<TWI_CSN_PIN);
	_delay_us(I2C_DELAY_USEC);
	PORTB |=  (1<<TWI_CSN_PIN);
	_delay_us(I2C_DELAY_USEC);
}

bool soft_spi_adf4113_read_bytes(byte *readBuffer) {
	
	// Issue a start condition, send device address and write direction bit
        SoftSpiMasterStart();

	// Read data from the device
	for (uint8_t i = 3; i > 0; i--) {
		// Send Ack until last byte then send Ack
		readBuffer[i - 1] = SoftSpiMasterRead();
	}

	// Issue a stop condition
	SoftSpiMasterStop();

	// Delay 10ms for the write to complete
	_delay_ms(1);
	
	return true;
}

bool soft_spi_adf4113_write_bytes(const byte *writeBuffer) {
	
	// Issue a start condition, send device address and write direction bit
        SoftSpiMasterStart();

	// Write data to the device
	for (uint8_t i = 3; i > 0; i--) {
		// Send Ack until last byte then send Ack
                SoftSpiMasterWrite(writeBuffer[i - 1]);
	}

	// Issue a stop condition
	SoftSpiMasterStop();

	// Delay 10ms for the write to complete
	_delay_ms(1);
	
	return true;
}
