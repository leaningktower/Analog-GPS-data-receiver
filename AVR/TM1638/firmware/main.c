//                  +-\/-+
// Ain0 (D 5) PB5  1|o   |8  Vcc
// Ain3 (D 3) PB3  2|    |7  PB2 (D 2) Ain1
// Ain2 (D 4) PB4  3|    |6  PB1 (D 1) pwm1
//            GND  4|    |5  PB0 (D 0) pwm0
//                  +----+

#define F_CPU 8000000

//PINS
//#define TWI_SDA_PIN    PB0
//#define TWI_SCL_PIN    PB1
//#define TWI_CSN_PIN    PB2
//#define FPGA_EN        PB3
//#define FPGA_DATA_OUT  PB4

#define TWI_SDA_PIN    PB3
#define TWI_SCL_PIN    PB2
#define TWI_CSN_PIN    PB4
#define FPGA_CSN_PIN   PB0
#define FPGA_SDI_PIN   PB1
#define FPGA_SDO_PIN   PB3
#define FPGA_SCL_PIN   PB2

//KEYS
#define key_na         0
#define key7           1 
#define key8           2
#define key9           3
#define key_page       4
#define key4           5
#define key5           6
#define key6           7
#define key_digit      8
#define key1           9
#define key2           10
#define key3           11
#define key_plus       12
#define key0           13
#define key_cancel     14
#define key_enter      15
#define key_minus      16

//FSM
#define S_IDLE         0
#define S_CSET         1
#define S_FSET         2
#define S_VSET         3
#define S_VDIS         4
#define S_FDIS         5
#define S_SET_IN       6

#include <avr/io.h>  
#include <avr/interrupt.h>
#include <util/delay.h>
#include "setup.c"

int main(void) {
    // enable SPI
    SoftSpiMasterInit(KEYBOARD);
    SoftSpiMasterInit(FPGA);
    // wait
    _delay_ms(10); 
    // display intensity set
    TM1638_enable(0); 
    // finite state machine
    while(1) {
        //loop here!
        //key
        keybuff_p = keybuff;
        keybuff = TM1638_KEY();
        //menu
        if(STATE == S_IDLE){
	        idlecode();
        }
        else if(STATE == S_CSET){
                csetcode();
        }
        else if(STATE == S_FSET){
                fsetcode();
        }
        else if(STATE == S_VSET){
                vsetcode();
        }
        else if(STATE == S_VDIS){
                vdiscode();
        }
        else if(STATE == S_FDIS){
                fdiscode();
        }
        else if(STATE == S_SET_IN){
                setincode();
        }
        else{
                STATE = S_IDLE;	
        }
        _delay_ms(50);
        ch_rcv = FPGA_update(STATE);
        _delay_ms(50);  
        //TM1638_DISPLAY(STATE, 2); 
    }
    SoftSpiMasterDeInit(KEYBOARD);
    SoftSpiMasterDeInit(FPGA);	 
    return 0;    
}

void idlecode(void) {
        if (keybuff == key_na) {
                STATE = S_IDLE;
                TM1638_STRING("gpsrcv"); 
        }
        else if (keybuff == key_page) {
                STATE = S_CSET;
                TM1638_STRING("svidset");
                _delay_ms(2000);
        }
        else {
        } 
        //TM1638_DISPLAY(keybuff, 2);
}

void keymod(uint32_t lowlim, uint32_t highlim) {
        if (config == 0) {
                n_set = n_channel;
        }
        else if (config == 1) {
                n_set = n_freq;
        }
        else if (config == 2) {
                n_set = n_vco;
        }
        else {
        }
        uint32_t mul = 1;
        for (uint8_t i = 0; i < digit_mod; i++) {
                mul = mul * 10;
        }
        if (keybuff == key_minus) {
                if (n_set >= mul) {
                        n_set = n_set - mul;
                }
        } 
        else if (keybuff == key_plus){
                if (n_set + mul <= 99999999) {
                        n_set = n_set + mul;
                }
        }
        else {
        }
        mul = 10;
        uint8_t d_count = 1;
        for (uint8_t i = 0; i < 8; i++) {
                if((n_set < mul) && (n_set >= mul / 10))    d_count = i + 1;
                mul = mul * 10;
        } 
        TM1638_enable(7); 
        TM1638_DISPLAY(n_set, d_count);
        _delay_ms(50); 
        if((n_set >= lowlim) && (n_set <= highlim)) {
                if (config == 0) {
                        n_channel = n_set;
                }
                else if (config == 1) {
                        n_freq = n_set;
                }
                else if (config == 2) {
                        n_vco = n_set;
                }
                else {
                }
        } else {
                TM1638_enable(0);
                TM1638_STRING("err");
                _delay_ms(500); 
        }
} 


void csetcode(void) {
        config = 0;
        keylow = 1;
        keyhigh = 32;
        TM1638_enable(0);
        TM1638_DISPLAY(n_channel, 2);
        if (keybuff == key_na) {
                STATE = S_CSET;
        }
        else if (keybuff == key_page) {
                STATE = S_FSET;
                TM1638_STRING("freqset");
                _delay_ms(2000);
        }
        else if (keybuff == key_enter) {
                STATE = S_SET_IN;
                n_set = 0;
                digit_cnt = 1;
                TM1638_STRING("set");
                _delay_ms(500);
        }
        else if ((keybuff == key_digit) && (keybuff_p == key_na)) {
                if(digit_mod < 8) {
                        digit_mod = digit_mod + 1; 
                } else {
                        digit_mod = 0;
                }
        }
        else if ((keybuff == key_plus) || (keybuff == key_minus)) {
                keymod(keylow, keyhigh);
        }
        else { 
        } 
}

void fsetcode(void) {
        config = 1;
        keylow = 0;
        keyhigh = 4095;
        TM1638_enable(0);
        TM1638_DISPLAY(n_freq, 4);
        if (keybuff == key_na) {
                STATE = S_FSET;
        }
        else if (keybuff == key_page) {
                STATE = S_VSET;
                TM1638_STRING("vcoset");
                _delay_ms(2000);
        }
        else if (keybuff == key_enter) {
                STATE = S_SET_IN;
                n_set = 0;
                digit_cnt = 1;
                TM1638_STRING("set");
                _delay_ms(500);
        }
        else if ((keybuff == key_digit) && (keybuff_p == key_na)) {
                if(digit_mod < 8) {
                        digit_mod = digit_mod + 1; 
                } else {
                        digit_mod = 0;
                }
        }
        else if ((keybuff == key_plus) || (keybuff == key_minus)) {
                keymod(keylow, keyhigh);
        }
        else {
        } 
}

void vsetcode(void) {
        config = 2;
        keylow = 0;
        keyhigh = 1023;
        TM1638_enable(0);
        TM1638_DISPLAY(n_vco, 4);
        if (keybuff == key_na) {
                STATE = S_VSET;
        }
        else if (keybuff == key_page) {
                STATE = S_VDIS;
                TM1638_STRING("vcovalue");
                _delay_ms(2000);
        }
        else if (keybuff == key_enter) {
                STATE = S_SET_IN;
                n_set = 0;
                digit_cnt = 1;
                TM1638_STRING("set");
                _delay_ms(500);
        }
        else if ((keybuff == key_digit) && (keybuff_p == key_na)) {
                if(digit_mod < 8) {
                        digit_mod = digit_mod + 1; 
                } else {
                        digit_mod = 0;
                }
        }
        else if ((keybuff == key_plus) || (keybuff == key_minus)) {
                keymod(keylow, keyhigh);
        }
        else {
        } 
}

void vdiscode(void) {
        TM1638_enable(0);
        TM1638_DISPLAY(n_pwm, 4);
        if (keybuff == key_na) {
                STATE = S_VDIS;
        }
        else if (keybuff == key_page) {
                STATE = S_FDIS;
                TM1638_STRING("iffreq");
                _delay_ms(2000);
        }
        else {
        } 
}

void fdiscode(void) {
        TM1638_enable(0);
        TM1638_DISPLAY(f_dop, 4);
        if (keybuff == key_na) {
                STATE = S_FDIS;
        }
        else if (keybuff == key_page) {
                STATE = S_CSET;
                TM1638_STRING("svidset");
                _delay_ms(2000);
        }
        else {
        } 
}

void setincode(void) {        
        TM1638_enable(7);
        if (keybuff == key_na) {
                STATE = S_SET_IN;
        }
        else if ((keybuff == key_enter) && (keybuff_p == key_na)) {
                if((n_set >= keylow) && (n_set <= keyhigh)) {
                        if (config == 0) {
                                n_channel = n_set;
                        }
                        else if (config == 1) {
                                 n_freq = n_set;
                        }
                        else if (config == 2) {
                                 n_vco = n_set;
                        }
                        else {
                        }
                } else {
                        TM1638_enable(0);
                        TM1638_STRING("err");
                        _delay_ms(500); 
                }
                  
                STATE = config + 1;
        }
        else if ((key_decoder[keybuff] != 0xff) && ((keybuff_p == key_na) || (keybuff_p == key_enter))) {
                n_set = key_decoder[keybuff] + n_set * 10;
                TM1638_enable(7); 
                TM1638_DISPLAY(n_set, digit_cnt);
                digit_cnt = digit_cnt + 1; 
        }
        else if ((keybuff == key_cancel) && (keybuff_p == key_na)) {
                n_set = n_set / 10;
                digit_cnt = digit_cnt - 1; 
                TM1638_enable(7); 
                TM1638_DISPLAY(n_set, digit_cnt - 1);
        }
        else {
        } 
}





