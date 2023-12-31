//                  +-\/-+
// Ain0 (D 5) PB5  1|o   |8  Vcc
// Ain3 (D 3) PB3  2|    |7  PB2 (D 2) Ain1
// Ain2 (D 4) PB4  3|    |6  PB1 (D 1) pwm1
//            GND  4|    |5  PB0 (D 0) pwm0
//                  +----+

#define F_CPU 1000000

//PINS

#define TWI_SDA_PIN    PB0
#define TWI_SCL_PIN    PB1
#define TWI_CSN_PIN    PB4
#define MUX            PB3
#define CE             PB2

#define CEPIN (1 << CE)


#include <avr/io.h>  
#include <avr/interrupt.h>
#include <util/delay.h>
#include "setup.c"

int main(void) {
    DDRB |= (1 << CE);
    PORTB &= ~CEPIN;
    
//    static const byte reg0[] = {0x93, 0xFE, 0x5D};
//    static const byte reg1[] = {0x20, 0x03, 0x70};
//    static const byte reg2[] = {0x15, 0xDE, 0x22};

    static const byte reg0[] = {0x93, 0xFE, 0x5D};
    static const byte reg1[] = {0x8C, 0x0A, 0x70};
    static const byte reg2[] = {0x15, 0xF9, 0x29};

    SoftSpiMasterInit();

    _delay_ms(100); 
    
    soft_spi_adf4113_write_bytes(reg0);
    soft_spi_adf4113_write_bytes(reg1);
    soft_spi_adf4113_write_bytes(reg2);


    PORTB |= CEPIN;
    
    while(1) {
    //loop here!
    }
    SoftSpiMasterDeInit();	 
    return 0;     
}