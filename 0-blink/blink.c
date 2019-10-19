#include <mega16.h>
#include <delay.h>

#define DEBOUNCE_COUNT 5

void main(void){

    int button0 = 0;
    int button0_laststate = 0;
    int button1 = 0;
    int button1_laststate = 0;
    int button2 = 0;
    int button2_laststate = 0;



    DDRA = 0xFF;
    DDRB = 0xFF;
    DDRC = 0xFF;

    DDRD=(0<<DDD7) | (0<<DDD6) | (1<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);

    PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (1<<PORTD2) | (1<<PORTD1) | (1<<PORTD0);


    TCCR1A= (0<<COM1A1) | (1<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
    TCCR1B= (0<<WGM13) | (1<<WGM12) | (1<<CS12) | (0<<CS11) | (1<<CS10);
    TCNT1H=0x00;
    TCNT1L=0x00;
    // OCR1AH=0x00;
    // OCR1AL=0x00;
    OCR1A = 0x444;


    while (1){
                if(PIND.0 == 0){
                    if(button0 < DEBOUNCE_COUNT) button0++;
                }else{
                    if(button0 > 0) button0--;
                }
                if(button0 == DEBOUNCE_COUNT && !button0_laststate) {
                    OCR1A += 0x40;                
                    TCNT1H=0x00;
                    TCNT1L=0x00;      
                    button0_laststate = 1;
                }else if(button0 == 0) {
                    button0_laststate = 0;
                }
                
                
                if(PIND.1 == 0){
                    if(button1 < DEBOUNCE_COUNT) button1++;
                }else{
                    if(button1 > 0) button1--;
                }
                if(button1 == DEBOUNCE_COUNT && !button1_laststate) {
                    OCR1A -= 0x40;   
                    TCNT1H=0x00;
                    TCNT1L=0x00;
                    button1_laststate = 1;
                }else if(button1 == 0){
                    button1_laststate = 0;
                }
                           
                
                
                
                if(PIND.2 == 0){
                    if(button2 < DEBOUNCE_COUNT) button2++;
                }else{
                    if(button2 > 0) button2--;
                }
                if(button2 == DEBOUNCE_COUNT && !button2_laststate) {
                    if(TCCR1B & (1<<CS10) > 0){
                        TCCR1B &= ~((1<<CS12) | (1<<CS11) | (1<<CS10));
                    }else{
                        TCCR1B |= ((1<<15) | (0<<CS11) | (1<<CS10));
                    }                      
                    button2_laststate = 1;
                }else if(button2 == 0){
                    button2_laststate = 0;
                }
                                             
                
                
                                                                 
                PORTA = OCR1AL;   
                PORTB = OCR1AH;
                PORTC = TCNT1L;
                
                delay_ms(15);
          }
}
