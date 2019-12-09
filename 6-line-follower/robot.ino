#define LEFT_SPEED 6
#define LEFT_0 3
#define LEFT_1 4

#define RIGHT_SPEED 5
#define RIGHT_0 7
#define RIGHT_1 2
#define CALIBRATION_BUTTON 8
#define CALIBRATION_LED 9

void left_wheel(int);
void right_wheel(int);
unsigned char read_eyes();

int last_eye_index = -1;
int threshold = 300;

int right_motor_speeds[]  = {0, 130, 140, 170, 180, 200};
int left_motor_speeds[]   = {200, 180, 170, 140, 130, 0};

void setup() {
  Serial.begin(9600);
  
  pinMode(LEFT_0, OUTPUT);
  pinMode(LEFT_1, OUTPUT);
  pinMode(LEFT_SPEED, OUTPUT);
  
  pinMode(RIGHT_0, OUTPUT);
  pinMode(RIGHT_1, OUTPUT);
  pinMode(RIGHT_SPEED, OUTPUT);

  pinMode(CALIBRATION_BUTTON, INPUT_PULLUP);
  pinMode(CALIBRATION_LED, OUTPUT);

  left_wheel(0);
  right_wheel(0);

  

  while(digitalRead(CALIBRATION_BUTTON));
  int white_value = analogRead(A3);
  digitalWrite(CALIBRATION_LED, HIGH);
  delay(1000);
  digitalWrite(CALIBRATION_LED, LOW);
  
  while(digitalRead(CALIBRATION_BUTTON));
  digitalWrite(CALIBRATION_LED, HIGH);
  int black_value = analogRead(A3);
  delay(1000);
  digitalWrite(CALIBRATION_LED, LOW);
  delay(1000);
  threshold = (white_value + black_value) / 2;
}
void loop() {
  Serial.println("====");
  
  char eye_index = read_eyes();

  if(eye_index == -1)
    eye_index = last_eye_index;
  else
    last_eye_index = eye_index;
    
    left_wheel(left_motor_speeds[eye_index]);
    right_wheel(right_motor_speeds[eye_index]);
}


inline void left_wheel(int speed){
  digitalWrite(LEFT_0, speed < 0);
  digitalWrite(LEFT_1, speed > 0);
  analogWrite(LEFT_SPEED, abs(speed));
}

inline void right_wheel(int speed){
  digitalWrite(RIGHT_0, speed < 0);
  digitalWrite(RIGHT_1, speed > 0);
  analogWrite(RIGHT_SPEED, abs(speed));
}

unsigned char read_eyes(){
  if(analogRead(A0) > threshold) return 0;
  if(analogRead(A1) > threshold) return 1;
  if(analogRead(A5) > threshold) return 5;
  if(analogRead(A4) > threshold) return 4;
  if(analogRead(A2) > threshold) return 2;
  if(analogRead(A3) > threshold) return 3;
  return -1;
}
