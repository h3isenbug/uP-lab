#include <delay.h>
#include <mega16.h>

#define DEBOUNCE_COUNT 5
#define IS_REVERSED PORTD.3
#define MAX_AMPLITUDE 5
#define STEP 51

inline void reverse() {
  if (IS_REVERSED) {
    PORTD.2 = 1;
    PORTD.3 = 0;
  } else {
    PORTD.2 = 0;
    PORTD.3 = 1;
  }
}

// Declare your global variables here

void main(void) {
  // Declare your local variables here
  int button0 = 0;
  int button0_laststate = 0;
  int button1 = 0;
  int button1_laststate = 0;

  // Input/Output Ports initialization

  DDRB = (0 << DDB7) | (0 << DDB6) | (0 << DDB5) | (0 << DDB4) | (1 << DDB3) |
         (0 << DDB2) | (0 << DDB1) | (0 << DDB0);

  PORTB = (0 << PORTB7) | (0 << PORTB6) | (0 << PORTB5) | (0 << PORTB4) |
          (0 << PORTB3) | (0 << PORTB2) | (0 << PORTB1) | (0 << PORTB0);

  
  DDRD = (0 << DDD7) | (0 << DDD6) | (0 << DDD5) | (0 << DDD4) | (1 << DDD3) |
         (1 << DDD2) | (0 << DDD1) | (0 << DDD0);
  PORTD. (0 << PORTD7) | (0 << PORTD6) | (0 << PORTD5) | (0 << PORTD4) |
          (0 << PORTD3) | (1 << PORTD2) | (1 << PORTD1) | (1 << PORTD0);      
          
  // Timer/Counter 0 initialization
  // Clock source: System Clock
  // Clock value:
  // Mode: Fast PWM top=OC0
  // OC0 output: Non-Inverted PWM
  TCCR0 = (1 << WGM00) | (1 << COM01) | (0 << COM00) | (1 << WGM01) |
          (1 << CS02) | (0 << CS01) | (0 << CS00);
  TCNT0 = 0x00;
  OCR0 = 0x00;

  // Timer(s)/Counter(s) Interrupt(s) initialization
  TIMSK = (0 << OCIE2) | (0 << TOIE2) | (0 << TICIE1) | (0 << OCIE1A) |
          (0 << OCIE1B) | (0 << TOIE1) | (0 << OCIE0) | (0 << TOIE0);

  while (1) {
    if (PIND .0 == 0) {
      if (button0 < DEBOUNCE_COUNT) button0++;
    } else {
      if (button0 > 0) button0--;
    }
    if (button0 == DEBOUNCE_COUNT && !button0_laststate) {
      if (IS_REVERSED) {
        if (OCR0 - STEP >= 0)
          OCR0 -= STEP;
        else {
          OCR0 = -(OCR0 - STEP);

          reverse();
        //  PORTD.2 = 1;
        //  PORTD.3 = 0;
        }
      } else {
        if (OCR0 + STEP <= 255) OCR0 += STEP;
      }

      button0_laststate = 1;
    } else if (button0 == 0) {
      button0_laststate = 0;
    }

    if (PIND .1 == 0) {
      if (button1 < DEBOUNCE_COUNT) button1++;
    } else {
      if (button1 > 0) button1--;
    }
    if (button1 == DEBOUNCE_COUNT && !button1_laststate) {
      if (!IS_REVERSED) {
        if (OCR0 - STEP >= 0)
          OCR0 -= STEP;
        else {
          OCR0 = -(OCR0 - STEP);

          reverse();
        }
      } else {
        if (OCR0 + STEP <= 255) OCR0 += STEP;
      }

      button1_laststate = 1;
    } else if (button1 == 0) {
      button1_laststate = 0;
    }

    delay_ms(15);
  }
}
