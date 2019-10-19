#include <delay.h>
#include <mega16.h>

#define DOT_CYCLES 2
#define DASH_CYCLES 6
#define MAX_CYCLES_TO_RESET 3
#define EEPROM_MAGIC 31415L
#define DEFAULT_PASSWORD 0b11110000
#define STATE_ENTER_PASSWORD 0
#define STATE_SELECT_ACTION 1
#define STATE_NEW_PASSWORD 2

#define PORT_WRONG_PASSWORD PORTD.2
#define PORT_ENTER_PASSWORD PORTD.3
#define PORT_ENTER_NEW_PASSWORD PORTD.4
#define PORT_SELECT_ACTION PORTD.5
#define PORT_DOOR_CONTROL PORTD.6

// Declare your global variables here
long eeprom eeprom_magic;
unsigned char eeprom stored_password;

void main(void) {
  // Declare your local variables here
  int button0_down = 0;
  int button0_up = MAX_CYCLES_TO_RESET;
  int button0_last_state = 0;
  unsigned char password;
  int current_state = STATE_ENTER_PASSWORD;
  unsigned char input = 0;
  unsigned char input_length = 0;

  int wrong_password_cycles = 0;

  if (eeprom_magic != EEPROM_MAGIC) {
    stored_password = DEFAULT_PASSWORD;
    password = DEFAULT_PASSWORD;
    eeprom_magic = EEPROM_MAGIC;
  } else {
    password = stored_password;
  }

  // Port D initialization
  // Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=Out Bit1=Out Bit0=In
  DDRD = (0 << DDD7) | (1 << DDD6) | (1 << DDD5) | (1 << DDD4) | (1 << DDD3) |
         (1 << DDD2) | (0 << DDD1) | (0 << DDD0);

  DDRA = 0xFF;      
  DDRB = 0xFF;

  // State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=1
  PORTD = (0 << PORTD7) | (0 << PORTD6) | (0 << PORTD5) | (0 << PORTD4) |
          (0 << PORTD3) | (0 << PORTD2) | (0 << PORTD1) | (1 << PORTD0);

  // Watchdog Timer initialization
  // Watchdog Timer Prescaler: OSC/16k
  WDTCR = (0 << WDTOE) | (1 << WDE) | (0 << WDP2) | (0 << WDP1) | (0 << WDP0);

  while (1) {
    if (PIND .0 == 0) {
      if (button0_down < DASH_CYCLES) button0_down++;
      if (button0_up > 0) button0_up--;
    } else {
      if (button0_up < MAX_CYCLES_TO_RESET) button0_up++;
    }
    if (button0_up == MAX_CYCLES_TO_RESET) {
      button0_down = 0;
    }

    if (button0_down < DASH_CYCLES && button0_down >= DOT_CYCLES) {  // DOT
      button0_last_state = 1;
    } else if (button0_down == DASH_CYCLES) {  // Dash
      button0_last_state = 2;
    } else {  // EMPTY
      if (button0_last_state == 1) {
        input = input << 1;
        input_length++;
      }
      if (button0_last_state == 2) {
        input = input << 1 | 1;
        input_length++;
      }

      button0_last_state = 0;
    }

    PORTA = input;
    PORTB = password;

    switch (current_state) {
      case STATE_ENTER_PASSWORD:

        PORT_ENTER_NEW_PASSWORD = 0;
        PORT_ENTER_PASSWORD = 1;
        PORT_SELECT_ACTION = 0;

        if (input_length == 8) {
          if (input == password) {
            PORT_DOOR_CONTROL = 1;
            current_state = STATE_SELECT_ACTION;
          } else {
            wrong_password_cycles = 30;
          }
          input_length = 0;
          input = 0;
        } else if (input_length < 8) {
          // PORT_ENTER_PASSWORD = 1;
        }

        break;

      case STATE_SELECT_ACTION:

        PORT_ENTER_NEW_PASSWORD = 0;
        PORT_ENTER_PASSWORD = 0;
        PORT_SELECT_ACTION = 1;

        if (input_length == 0) {
          PORT_SELECT_ACTION = 1;
        } else {
          input_length = 0;
          if (input & 1 == 1) {  // change password        
            input = 0;
            current_state = STATE_NEW_PASSWORD;
          } else {  // close the door
            PORT_DOOR_CONTROL = 0;
            current_state = STATE_ENTER_PASSWORD;
          }
        }

        break;

      case STATE_NEW_PASSWORD:

        PORT_ENTER_NEW_PASSWORD = 1;
        PORT_ENTER_PASSWORD = 0;
        PORT_SELECT_ACTION = 0;

        if (input_length == 8) {
          input_length = 0;
          stored_password = input;
          password = input;
          input = 0;
          current_state = STATE_ENTER_PASSWORD;
        } else if (input_length < 8) {
        }

        break;

      default:

        PORT_ENTER_NEW_PASSWORD = 1;
        PORT_ENTER_PASSWORD = 1;
        PORT_SELECT_ACTION = 1;
    }
    if (wrong_password_cycles > 0) {
      wrong_password_cycles--;
      PORT_WRONG_PASSWORD = 1;
    } else {
      PORT_WRONG_PASSWORD = 0;
    }

    delay_ms(50);

#asm("wdr")
  }
}
