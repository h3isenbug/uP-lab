#include <mega16.h>
#include <delay.h>
#include <stdbool.h>

#define DEBOUNCE_COUNT 5
#define ADC_VREF_TYPE ((0<<REFS1) | (0<<REFS0) | (0<<ADLAR))
#define ADC_START_INDEX 0
#define ADC_COUNT 3
#define EEPROM_MAGIC 3141592L

unsigned int current_adc_index  = 0;
unsigned int calibration_thingy = 0;
unsigned int adc_data[ADC_COUNT];
eeprom unsigned int stored_sensor_thresholds[ADC_COUNT];
eeprom unsigned long eeprom_magic;
unsigned int sensor_thresholds[ADC_COUNT];


// ADC interrupt service routine
interrupt [ADC_INT] void adc_isr(void) {
    // Read the AD conversion result
    adc_data[current_adc_index - ADC_START_INDEX] = ADCW; 


    current_adc_index++;   
    if(calibration_thingy < 3) calibration_thingy++;
    if(current_adc_index == ADC_START_INDEX + ADC_COUNT) {
        current_adc_index = ADC_START_INDEX;
    }
    ADMUX = ADC_VREF_TYPE | (ADC_START_INDEX + current_adc_index); 
                             
    delay_us(10);
    
    ADCSRA|=~(1<<ADSC);
}

void main(void) {
    // Declare your local variables here
    int i;                              
    int button0 = 0;
    int button0_laststate = 0;
                                            
    if(eeprom_magic != EEPROM_MAGIC) {
        for( i=0 ; i<ADC_COUNT ; i++ ) stored_sensor_thresholds[i] = 0x200;
        eeprom_magic = EEPROM_MAGIC;
    }
                                      
    for( i=0 ; i<ADC_COUNT ; i++ ) adc_data[i] = 0;
    for( i=0 ; i<ADC_COUNT ; i++ ) sensor_thresholds[i] = stored_sensor_thresholds[i];                            
    
    DDRB=0xFF;
    DDRC=0xFF;
    DDRD=0x00;
    PORTD = 0xFF;  
    
    // ADC initialization
    // ADC Clock frequency: 500.000 kHz
    // ADC Voltage Reference: AREF pin
    // ADC Auto Trigger Source: Free Running
    ADMUX = ADC_VREF_TYPE | ADC_START_INDEX;  
    ADCSRA= (1<<ADEN)  | (1<<ADSC)  | (1<<ADATE) | (0<<ADIF) | (1<<ADIE) | (1<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);
    SFIOR = (0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);

    // Global enable interrupts
    #asm("sei")

    while (1) {                         
        if(PIND.0 == 0){
            if(button0 < DEBOUNCE_COUNT) button0++;
        }else{
            if(button0 > 0) button0--;
        }
        if(button0 == DEBOUNCE_COUNT && !button0_laststate) {
            PORTC = 0xFF;
           // current_adc_index = 0;
            calibration_thingy = 0;
            while(calibration_thingy != 3);               
            for( i=0 ; i<ADC_COUNT ; i++ ) {
                sensor_thresholds[i] = adc_data[i]   ;                        
                stored_sensor_thresholds[i] = sensor_thresholds[i];
            }                             
            PORTC = 0x00;
            delay_ms(500);
            PORTC = 0xFF;
            delay_ms(500);
            delay_ms(500);
            PORTC = 0x00;
            
            button0_laststate = 1;
        }else if(button0 == 0) {
            button0_laststate = 0;
        }
                                     
    //    if(adc_data[0] > sensor_thresholds[0]) PORTB.0 = 1; else PORTB.0 = 0;
    //    if(adc_data[1] > sensor_thresholds[1]) PORTB.1 = 1; else PORTB.1 = 0;
    //    if(adc_data[2] > sensor_thresholds[2]) PORTB.2 = 1; else PORTB.2 = 0;   
        
        for( i=0 ; i<ADC_COUNT ; i++ ) {
            PORTB = (PORTB & ~(1<<i)) | (adc_data[i] > sensor_thresholds[i]) << i;
        }    
    }
}
