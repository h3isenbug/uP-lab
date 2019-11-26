#include <mega16.h>
#include <delay.h>
#include <alcd.h>
#include <stdio.h>
#include <stdbool.h>



// Declare your global variables here
volatile unsigned long frequency=0;
volatile bool data_ready = false;

volatile unsigned int value0 = 0;
volatile unsigned int value1 = 0;
volatile unsigned int value_index = 0;


interrupt [TIM1_CAPT] void timer1_capt_isr(void) {
    unsigned long ICR1 = ((unsigned long)ICR1H << 8) | ICR1L;
    
    if(data_ready) return;
    
    if(value_index == 1){
        value_index = 0;
        value1 = ICR1;
        data_ready = true;
    }else{
        value_index = 1;
        value0 = ICR1;
    }                 
}

void main(void) {
    // Declare your local variables here
    char data[24];                  
    
    ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);
    SFIOR=(0<<ACME);

    ADCSRA=(0<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);

    TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
    TCCR1B=(1<<ICNC1) | (1<<ICES1) | (0<<WGM13) | (1<<WGM12) | (0<<WGM11) | (0<<WGM10) | (1<<CS12) | (0<<CS11) | (0<<CS10);
    TCNT1 = 0;
    ICR1H=0x00;
    ICR1L=0x00;
    OCR1A=0xFFFF;

    TIMSK=(0<<OCIE2) | (0<<TOIE2) | (1<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (0<<TOIE0);    
    #asm("sei")

    // Alphanumeric LCD initialization
    // Connections are specified in the
    // Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
    // RS - PORTA Bit 0
    // RD - PORTA Bit 1
    // EN - PORTA Bit 2
    // D4 - PORTA Bit 4
    // D5 - PORTA Bit 5
    // D6 - PORTA Bit 6
    // D7 - PORTA Bit 7
    // Characters/line: 24
    lcd_init(24);                                                                  
    
    PORTD = 0b01000000;
    DDRD  = 0b10000000;     
    
    lcd_clear();
    lcd_gotoxy(1,3);
    lcd_puts("waiting for input wave");
    
    while (1) {
          // Place your code here  
        if(data_ready) {                
            PORTD.7 = !PORTD.7;
            if(value1 > value0)
                frequency = 1000000 / (value1 - value0);
            else
                frequency = 1000000 / (0xFFFFL + value1 - value0);
                                                        
            data_ready = false;
                  
                lcd_clear();
                lcd_gotoxy(0, 0);     
                  
                sprintf(data, "Freq: %u", frequency);
                  
                lcd_puts(data);         
                     
                delay_ms(500);
        }
    }
}
