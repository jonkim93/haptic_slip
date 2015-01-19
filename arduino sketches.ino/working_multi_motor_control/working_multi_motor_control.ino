#include <DRV8835MotorShield1.h>
#include <DRV8835MotorShield2.h>
#include <Encoder.h>
#include <PID_v1.h>
#include <stdlib.h> 

/*
 * interleave commands; change communications protocol to : 
 *   fsd1sd2sd3sd4 
 *      where f = {'t', 'f'} for finger down
 *            s = {'+', '-'} for sign of delta command
 *            d1 = motor1 delta command
 *            d2 = motor2 delta command...etc
 *
 */

#define LED_PIN 13
#define MARGIN 0
//#define SCALE_FACTOR 136.42 //114.52 //136.42
#define M1A 0
#define M1B 1
#define M2A 2
#define M2B 3
#define DEBUG false
//#define INCREMENT 0.01
#define MS_PER_STEP 5 //1000ms/200Hz
#define NUM_MOTORS 4

double Setpoint1A, Input1A, Output1A, Setpoint1B, Input1B, Output1B;
double Setpoint2A, Input2A, Output2A, Setpoint2B, Input2B, Output2B;
boolean Flipped;
boolean FingerDown;
boolean Homed;

double encValues [100];
int j;
boolean notPrinted = true;

double INCREMENT = 0.01;
double SCALE_FACTOR = 136.42;

boolean terminated [4];
int prevDeltas [4];
int prevMillis [4];
int counters [4];

DRV8835MotorShield1 driver1;
DRV8835MotorShield2 driver2;
Encoder enc1A(2,3);
Encoder enc1B(4,5);
Encoder enc2A(6,7);
Encoder enc2B(8,9);
double K_p = 0.5;
double K_i = 5.0;
double K_d = 3.0;
PID pid1A(&Input1A, &Output1A, &Setpoint1A, K_p, K_i, K_d, DIRECT);
PID pid1B(&Input1B, &Output1B, &Setpoint1B, K_p, K_i, K_d, DIRECT);
PID pid2A(&Input2A, &Output2A, &Setpoint2A, K_p, K_i, K_d, DIRECT);
PID pid2B(&Input2B, &Output2B, &Setpoint2B, K_p, K_i, K_d, DIRECT);

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Homed = false;
  
  for (int i=0; i<4; i++){
     prevMillis[i] = millis();
     counters[i] = 0;
     terminated[i] = false;
  }
  pid1A.SetMode(AUTOMATIC);
  pid1A.SetSampleTime(2);
  pid1A.SetOutputLimits(-400,400);

  pid1B.SetMode(AUTOMATIC);
  pid1B.SetSampleTime(1);
  pid1B.SetOutputLimits(-400,400);

  pid2A.SetMode(AUTOMATIC);
  pid2A.SetSampleTime(1);
  pid2A.SetOutputLimits(-400,400);
  
  pid2B.SetMode(AUTOMATIC);
  pid2B.SetSampleTime(1);
  pid2B.SetOutputLimits(-400,400);
}

void moveDeltaDistance(int d, int motor){ //moves a certain distance in cm
  double INCREMENT = 0.01;
  double SCALE_FACTOR = 136.42;
  double deltaDistance = ((double) d) *  ( INCREMENT); //convert to cm
  double deltaAngle = deltaDistance * (SCALE_FACTOR); // convert to encoder counts
  double currAngle;
  switch(motor){
    case M1A:
      currAngle = (double) enc1A.read();
      encValues[j] = currAngle;
      j++;
      //Serial.printf("\n  INCREMENT: %f\n  SCALE_FACTOR: %f", INCREMENT, SCALE_FACTOR);
      break;
    case M1B:
      currAngle = (double) enc1B.read();
      break;
    case M2A:
      currAngle = (double) enc2A.read();
      break;
    case M2B:
      currAngle = (double) enc2B.read();
      break;
  }
  //Serial.printf("\nREADING\n  CURRANGLE: %f\n  DELTAANGLE: %f", currAngle, deltaAngle);
  moveToAngle(currAngle + deltaAngle, motor);
}

void stayStill(int motor){
  switch(motor){
    case M1A:
      driver1.setM1Speed(0);
      break;
    case M1B:
      driver1.setM2Speed(0);
      break;
    case M2A:
      driver2.setM1Speed(0);
      break;
    case M2B:
      driver2.setM2Speed(0);
      break;
  } 
}

void moveToAngle(double a, int motor){
  int prevMillis;
  //Serial.println(a);
  switch (motor){
    case M1A:
      Setpoint1A = a;
      Input1A = (double) enc1A.read(); 
      if (abs(a - Input1A) > MARGIN){ //if the setpoint is greater than the current angle
        digitalWrite(LED_PIN, HIGH);
        prevMillis = millis();
        while (abs(a - Input1A) > MARGIN && prevMillis == millis()){
          pid1A.Compute();
          driver1.setM1Speed(Output1A);
          //Serial.println(Output1A);
          Input1A = (double) enc1A.read();
          if (DEBUG){
            Serial.printf("\nm1a stuff:\n  setpoint: %d\n  input: %d\n  output: %d", Setpoint1A, Input1A, Output1A);
          }
        }
      }
      digitalWrite(LED_PIN, LOW);
      break;
    case M1B:
      Setpoint1B = a;
      Input1B = (double) enc1B.read(); 
      if (abs(a - Input1B) > MARGIN){ //if the setpoint is greater than the current angle
        digitalWrite(LED_PIN, HIGH);
        prevMillis = millis();
        while (abs(a - Input1B) > MARGIN && prevMillis == millis()){
          pid1B.Compute();
          driver1.setM2Speed(Output1B);
          Input1B = (double) enc1B.read();
          if (DEBUG){
            Serial.printf("\nm1b stuff:\n  setpoint: %d\n  input: %d\n  output: %d", Setpoint1B, Input1B, Output1B);
          }
        }
      }
      digitalWrite(LED_PIN, LOW);
      break;
    case M2A:
      Setpoint2A = a;
      Input2A = (double) enc2A.read(); 
      if (abs(a - Input2A) > MARGIN){ //if the setpoint is greater than the current angle
        digitalWrite(LED_PIN, HIGH);
        prevMillis = millis();
        while (abs(a - Input2A) > MARGIN && prevMillis == millis()){
          pid2A.Compute();
          driver2.setM1Speed(Output2A);
          Input2A = (double) enc2A.read();
          if (DEBUG){
            Serial.printf("\nm2a stuff:\n  setpoint: %d\n  input: %d\n  output: %d", Setpoint2A, Input2A, Output2A);
          }
        }
      }
      digitalWrite(LED_PIN, LOW);
      break;
    case M2B:
      Setpoint2B = a;
      Input2B = (double) enc2B.read(); 
      if (abs(a - Input2B) > MARGIN){ //if the setpoint is greater than the current angle
        digitalWrite(LED_PIN, HIGH);
        prevMillis = millis();
        while (abs(a - Input2B) > MARGIN && prevMillis == millis()){
          pid2B.Compute();
          driver2.setM2Speed(Output2B);
          Input2B = (double) enc2B.read();
          if (DEBUG){
            Serial.printf("\nm2b stuff:\n  setpoint: %d\n  input: %d\n  output: %d", Setpoint2B, Input2B, Output2B);
          }
        }
      }
      digitalWrite(LED_PIN, LOW);
      break;
      
  }  
}

void loop()
{
  int x;
  int scaledCommand;
  if (Serial.available()){ 
    char f = Serial.read();
    int a;
    char s;
    
    if (f=='t'){
      for (x=0; x<NUM_MOTORS; x++){
        s = Serial.read();
        if (DEBUG) {Serial.printf("received and parsing: %d, %c\n", x, s); }
        if (s == '-'){
          a = -(double) Serial.parseInt();
        } else if (s == '+'){
          a = (double) Serial.parseInt();
        }
        prevDeltas[x] = a/MS_PER_STEP; //scaledCommand;
        terminated[x] = false;
      }
      
      for (x=0; x<NUM_MOTORS; x++){
        if (DEBUG){ Serial.printf("commanding motor %d: %d\n", x, prevDeltas[x]); }
        moveDeltaDistance(prevDeltas[x], x);
      }
    }
  } else {
    for (x=0; x<NUM_MOTORS; x++){ //iterate through all the motors 
      if (counters[x] < MS_PER_STEP && !terminated[x]){
        if (prevMillis[x] != millis()){        
          if (DEBUG) {Serial.printf("commanding motor %d: %d at %d\n", x, prevDeltas[x], prevMillis[x]); }
          moveDeltaDistance(prevDeltas[x], x); 
          prevMillis[x] = millis();
          counters[x] ++;
        }
      } else {
        counters[x] = 0;
        terminated[x] = true;
        stayStill(x);
      }
    }
    if (notPrinted && j>=100){
      for (int i=0; i<100; i++){
        Serial.println(encValues[i]); 
      }
      notPrinted = false;
    }
  }
}
