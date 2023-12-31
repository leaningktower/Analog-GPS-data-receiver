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
#define FPGA 1
#define KEYBOARD 2

//system parameters
    uint8_t STATE = S_IDLE;
    uint8_t keybuff = key_na;
    uint8_t keybuff_p = key_na;
    uint8_t n_channel = 0;
    uint32_t n_set = 0;
    uint16_t n_freq = 0;
    uint16_t n_vco = 0; 
    uint16_t n_pwm = 511;
    uint16_t f_dop = 2048;
    uint8_t digit_cnt = 0;
    uint8_t digit_mod = 0;
    uint16_t keylow = 0;
    uint16_t keyhigh = 0;
    uint8_t config = 0;
    uint8_t ch_rcv = 0;

    static const byte key_decoder[]={0xff,7,8,9,0xff,4,5,6,0xff,1,2,3,0xff,0,0xff,0xff,0xff};

//functions
void idlecode(void);
void csetcode(void);
void fsetcode(void);
void vsetcode(void);
void vdiscode(void);
void fdiscode(void);
void setincode(void);

//union
typedef union{        uint16_t data16;        uint8_t  data8[2];} theconv;

//spi codes

#define I2C_DELAY_USEC 1

// Initialize SCL/SDA/CSN pins
void SoftSpiMasterInit(uint8_t dev) {
        if(dev == KEYBOARD) {
                PORTB &= ~(1<<TWI_SDA_PIN);
	        DDRB  |=  (1<<TWI_SDA_PIN);
	        PORTB |=  (1<<TWI_SCL_PIN);
	        DDRB  |=  (1<<TWI_SCL_PIN);
	        PORTB |=  (1<<TWI_CSN_PIN);
	        DDRB  |=  (1<<TWI_CSN_PIN);
        }
        else if(dev == FPGA) {
	        PORTB &= ~(1<<FPGA_SDI_PIN);
	        DDRB  &= ~(1<<FPGA_SDI_PIN);
                PORTB &= ~(1<<FPGA_SDO_PIN);
	        DDRB  |=  (1<<FPGA_SDO_PIN);
	        PORTB |=  (1<<FPGA_SCL_PIN);
	        DDRB  |=  (1<<FPGA_SCL_PIN);
	        PORTB |=  (1<<FPGA_CSN_PIN);
	        DDRB  |=  (1<<FPGA_CSN_PIN);
        }
}

// De-initialize SCL/SDA/CSN pins
void SoftSpiMasterDeInit(uint8_t dev) {
        if(dev == KEYBOARD) {
	        PORTB &= ~(1<<TWI_SDA_PIN);
	        DDRB  &= ~(1<<TWI_SDA_PIN);
	        PORTB |=  (1<<TWI_SCL_PIN);
	        DDRB  |=  (1<<TWI_SCL_PIN);
	        PORTB |=  (1<<TWI_CSN_PIN);
	        DDRB  |=  (1<<TWI_CSN_PIN);
        }
        else if(dev == FPGA) {
	        PORTB &= ~(1<<FPGA_SDI_PIN);
	        DDRB  &= ~(1<<FPGA_SDI_PIN);
                PORTB &= ~(1<<FPGA_SDO_PIN);
	        DDRB  &= ~(1<<FPGA_SDO_PIN);
	        PORTB |=  (1<<FPGA_SCL_PIN);
	        DDRB  |=  (1<<FPGA_SCL_PIN);
	        PORTB |=  (1<<FPGA_CSN_PIN);
	        DDRB  |=  (1<<FPGA_CSN_PIN);
        }
}

// Read a byte from SPI
uint8_t SoftSpiMasterRead(void) {
	uint8_t b = 0;
	// Make sure pull-up enabled
	PORTB &= ~(1<<TWI_SDA_PIN);
	DDRB  &= ~(1<<TWI_SDA_PIN);
	// Read byte
        PORTB &= ~(1<<TWI_SCL_PIN);
	for (uint8_t i = 0; i < 8; i++) {
		// Don't change this loop unless you verify the change with a scope
                b >>= 1;
		_delay_us(I2C_DELAY_USEC);
		PORTB |= (1<<TWI_SCL_PIN);
		if (bit_is_set(PINB, TWI_SDA_PIN)) b |= 0x80;
		PORTB &= ~(1<<TWI_SCL_PIN);
	}
	// End
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
	PORTB &= ~(1<<TWI_SCL_PIN);
        uint8_t m = 0x01;
	for (uint8_t i = 0; i < 8; i++) {
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
                m <<= 1;
	}
	// End
	PORTB &= ~(1<<TWI_SDA_PIN);
	_delay_us(I2C_DELAY_USEC);
}

// Issue a start condition
void SoftSpiMasterStart(uint8_t dev) {
	PORTB |=  (1<<TWI_CSN_PIN);
	_delay_us(I2C_DELAY_USEC);
        if(dev == KEYBOARD) {
	        PORTB &= ~(1<<TWI_CSN_PIN);
        }
        else if(dev == FPGA) {
                PORTB &= ~(1<<FPGA_CSN_PIN);
        }
}
 

// Issue a stop condition
void SoftSpiMasterStop(uint8_t dev) {
	PORTB |=  (1<<TWI_SCL_PIN);
	_delay_us(I2C_DELAY_USEC);
        if(dev == KEYBOARD) {
	        PORTB &= ~(1<<TWI_CSN_PIN);
	        _delay_us(I2C_DELAY_USEC);
	        PORTB |=  (1<<TWI_CSN_PIN);
	        _delay_us(I2C_DELAY_USEC);
        }
        else if(dev == FPGA) {
	        PORTB &= ~(1<<FPGA_CSN_PIN);
	        _delay_us(I2C_DELAY_USEC);
	        PORTB |=  (1<<FPGA_CSN_PIN);
	        _delay_us(I2C_DELAY_USEC);
        }
}

// FPGA Write and read
uint8_t SoftSpiMasterWriteAndRead(uint8_t data) {
        uint8_t b = 0;
	// Prepare input and output
	PORTB &= ~(1<<FPGA_SDO_PIN);
	DDRB  |=  (1<<FPGA_SDO_PIN);
	PORTB &= ~(1<<FPGA_SDI_PIN);
	DDRB  &= ~(1<<FPGA_SDI_PIN);
	_delay_us(I2C_DELAY_USEC);
	// Write byte
	PORTB &= ~(1<<FPGA_SCL_PIN);
	for (uint8_t m = 0x80; m != 0; m >>= 1) {
		// Don't change this loop unless you verify the change with a scope
                b <<= 1;
		if (m & data) { 
			PORTB |= (1<<FPGA_SDO_PIN); 
		}
		else { 
			PORTB &= ~(1<<FPGA_SDO_PIN); 
		}
		PORTB |= (1<<FPGA_SCL_PIN);
		_delay_us(I2C_DELAY_USEC);
		if (bit_is_set(PINB, FPGA_SDI_PIN)) b |= 0x01;
		PORTB &= ~(1<<FPGA_SCL_PIN);
	}
	// End
	PORTB &= ~(1<<FPGA_SDO_PIN);
        PORTB &= ~(1<<FPGA_SDI_PIN);
	_delay_us(I2C_DELAY_USEC);
        return b;
}

// FPGA functions
uint8_t FPGA_update(uint8_t sta) {
        theconv valo;
        theconv vali;
        uint8_t ch_in;
        if (sta == S_CSET) {
                valo.data16 = n_channel;
        }
        else if (sta == S_FSET) {
                valo.data16 = n_freq;
        }
        else if (sta == S_VSET) {
                valo.data16 = n_set;
        }
        else {
                valo.data16 = 0;
        }      
        SoftSpiMasterStart(FPGA);
        ch_in         = SoftSpiMasterWriteAndRead(sta);
        vali.data8[1] = SoftSpiMasterWriteAndRead(valo.data8[1]);
        vali.data8[0] = SoftSpiMasterWriteAndRead(valo.data8[0]);     
        SoftSpiMasterStop(FPGA);
        if (ch_in == 4) {
            n_pwm = vali.data16;
        } else if (ch_in == 5) {
            f_dop = vali.data16;
        }
        return ch_in;
}

// TM1638 init and intensity set
bool TM1638_enable(uint8_t inten) {
        SoftSpiMasterStart(KEYBOARD);
        SoftSpiMasterWrite(0x88 + inten);
        SoftSpiMasterStop(KEYBOARD);
        return true;
}

// TM1638 numnber display
bool TM1638_DISPLAY(uint32_t num, uint8_t digits) {
        // automatic addressing mode to display
        SoftSpiMasterStart(KEYBOARD);
        SoftSpiMasterWrite(0x40);
        SoftSpiMasterStop(KEYBOARD);
        // display buffer setting
        uint32_t buff = num;
        static const byte seg_code[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
        byte disreg[8] = {0, 0, 0 ,0 ,0 ,0 ,0 ,0};
        for (uint8_t i = 0; i < digits; i++) {
                disreg[i] = seg_code[buff % 10];
                buff = buff / 10;
        }
        // to common anode
        byte datas[16] = {0, 0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0};
        uint8_t m = 0x01;
        for (uint8_t i = 0; i < 8; i++) {
                for (uint8_t j = 0; j < 8; j++) {
                        datas[i] <<= 1;
                        if(disreg[7 - j] & m)   datas[i] |= 0x01;
                }
                m <<= 1; 
        }
        SoftSpiMasterStart(KEYBOARD);
        // display digits
        SoftSpiMasterWrite(0xc0);
        for (uint8_t i = 0; i < 8 ; i++) {
                SoftSpiMasterWrite(datas[i]);
                SoftSpiMasterWrite(datas[i + 8]);
        }
        SoftSpiMasterStop(KEYBOARD);
        return true;
}

// TM1638 key read
uint8_t TM1638_KEY(void) {
        // automatic addressing mode to read keys
        uint8_t readBuffer[4] = {0, 0, 0, 0};
        SoftSpiMasterStart(KEYBOARD);
        SoftSpiMasterWrite(0x42);
	for (uint8_t i = 0; i < 4; i++) {
                readBuffer[i] = SoftSpiMasterRead();
        }        
        SoftSpiMasterStop(KEYBOARD);
        uint8_t keyvalue = 0;
        for (uint8_t i = 0; i < 4; i++) {
                if (0x40 & readBuffer[i]) {
                        keyvalue = 2 + 2 * i;
                }
                else if (0x04 & readBuffer[i]) {
                        keyvalue = 1 + 2 * i;
                }
                else if (0x20 & readBuffer[i]) {
                        keyvalue = 10 + 2 * i;
                }
                else if (0x02 & readBuffer[i]) {
                        keyvalue = 9 + 2 * i;
                }
                else {
                }
        } 
        return keyvalue;
}

// TM1638 string display
bool TM1638_STRING(char *displaybuffer) {
        // automatic addressing mode to display
        SoftSpiMasterStart(KEYBOARD);
        SoftSpiMasterWrite(0x40);
        SoftSpiMasterStop(KEYBOARD);
        // display buffer setting
        static const byte letter_code[]={0x77,0x7c,0x39,0x5e,0x79,0x71,0x3d,0x74,
                                      0x30,0x1e,0x75,0x38,0x15,0x37,0x3f,0x73,
                                      0x67,0x33,0x6d,0x78,0x3e,0x2e,0x2a,0x76,
                                      0xbe,0x4b};
        byte disreg[8] = {0, 0, 0 ,0 ,0 ,0 ,0 ,0};
        uint8_t i = 7;
        while (*displaybuffer != '\0') {
                if ((*displaybuffer >= 'a') && (*displaybuffer <= 'z')) {
                        disreg[i] = letter_code[*displaybuffer - 'a'];
                }
                displaybuffer = displaybuffer + 1;
                if(i > 0)    i = i - 1;
        }
        // to common anode
        byte datas[16] = {0, 0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0};
        uint8_t m = 0x01;
        for (uint8_t i = 0; i < 8; i++) {
                for (uint8_t j = 0; j < 8; j++) {
                        datas[i] <<= 1;
                        if(disreg[7 - j] & m)   datas[i] |= 0x01;
                }
                m <<= 1; 
        }
        SoftSpiMasterStart(KEYBOARD);
        // display digits
        SoftSpiMasterWrite(0xc0);
        for (uint8_t i = 0; i < 8 ; i++) {
                SoftSpiMasterWrite(datas[i]);
                SoftSpiMasterWrite(datas[i + 8]);
        }
        SoftSpiMasterStop(KEYBOARD);
        return true;
}