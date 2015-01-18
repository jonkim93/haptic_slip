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
 * SERIAL IS BEING READ TWICE; WHY IS THAT??
 */

#define LED_PIN 13
#define MARGIN 0
#define SCALE_FACTOR 114.52 //136.42
#define M1A 0
#define M1B 1
#define M2A 2
#define M2B 3
#define DEBUG false
#define INCREMENT 0.01
#define MS_PER_STEP 5

double Setpoint1A, Input1A, Output1A, Setpoint1B, Input1B, Output1B;
double Setpoint2A, Input2A, Output2A, Setpoint2B, Input2B, Output2B;
boolean Flipped;
boolean FingerDown;
boolean Homed;

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
double K_p = 2.0;
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
  pid1A.SetSampleTime(1);
  pid1A.SetOutputLimits(0,400);

  pid1B.SetMode(AUTOMATIC);
  pid1B.SetSampleTime(1);
  pid1B.SetOutputLimits(0,400);

  pid2A.SetMode(AUTOMATIC);
  pid2A.SetSampleTime(1);
  pid2A.SetOutputLimits(0,400);
  
  pid2B.SetMode(AUTOMATIC);
  pid2B.SetSampleTime(1);
  pid2B.SetOutputLimits(0,400);
}

void moveDeltaDistance(int d, int motor){ //moves a certain distance in cm
  double deltaDistance = d * INCREMENT; //convert to cm
  double deltaAngle = deltaDistance * SCALE_FACTOR; // convert to encoder counts
  double currAngle;
  switch(motor){
    case M1A:
      currAngle = (double) enc1A.read();
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
  moveToAngle(currAngle + deltaAngle, motor);
}

void moveToAngle(double a, int motor){
  int prevMillis;
  switch (motor){
    case M1A:
      Setpoint1A = a;
      Input1A = (double) enc1A.read(); 
      if (a - Input1A > MARGIN){ //if the setpoint is greater than the current angle
        driver1.flipM1(false);
        digitalWrite(LED_PIN, HIGH);
        prevMillis = millis();
        while (a - Input1A > MARGIN && prevMillis == millis()){
          pid1A.Compute();
          driver1.setM1Speed(Output1A);
          Input1A = (double) enc1A.read();
          if (DEBUG){
            Serial.println("m1a stuff");
            Serial.println(Setpoint1A);
            Serial.println(Input1A);
            Serial.println(Output1A); 
          }
        }
      } else if (Input1A - a > MARGIN) {
        driver1.flipM1(true);
        digitalWrite(LED_PIN, HIGH);
        prevMillis = millis();
        while (Input1A - a > MARGIN && prevMillis == millis()){
          Input1A = Setpoint1A;
          Setpoint1A = (double) enc1A.read();
          pid1A.Compute();
          driver1.setM1Speed(Output1A);
          Input1A = (double) enc1A.read();
          if (DEBUG){
            Serial.println("m1a stuff, reversed");
            Serial.println(Setpoint1A);
            Serial.println(Input1A);
            Serial.println(Output1A); 
          }
        }
      }
      //driver1.setM1Speed(0);
      digitalWrite(LED_PIN, LOW);
      break;
      
    case M1B:
      Setpoint1B = a;
      Input1B = (double) enc1B.read(); 
      if (a - Input1B > MARGIN){
        driver1.flipM2(false);
        if (DEBUG){
          Serial.println("greater than margin");
        }
        digitalWrite(LED_PIN, HIGH);
        prevMillis = millis();
        while (a - Input1B > MARGIN && prevMillis==millis()){
          pid1B.Compute();
          driver1.setM2Speed(Output1B);
          Input1B = (double) enc1B.read();
        }
      } else if (Input1B - a > MARGIN) {
        driver1.flipM2(true);
        digitalWrite(LED_PIN, HIGH);
        prevMillis = millis();
        while (Input1B - a > MARGIN && prevMillis==millis()){
          Input1B = Setpoint1B;
          Setpoint1B = (double) enc1B.read();
          pid1B.Compute();
          driver1.setM2Speed(Output1B);
          Input1B = (double) enc1B.read();
        }
      }
      //driver1.setM2Speed(0);
      digitalWrite(LED_PIN, LOW);
      break; 
      
    case M2A:
      Setpoint2A = a;
      Input2A = (double) enc2A.read(); 
      if (a - Input2A > MARGIN){
        driver2.flipM1(false);
        digitalWrite(LED_PIN, HIGH);
        prevMillis = millis();
        while (a - Input2A > MARGIN && prevMillis==millis()){
          if (DEBUG){
            Serial.println(a);
            Serial.println(Output2A);
            Serial.println(enc2A.read());
          }
          pid2A.Compute();
          driver2.setM1Speed(Output2A);
          Input2A = (double) enc2A.read();
        }
      } else if (Input2A - a > MARGIN) {
        driver2.flipM1(true);
        digitalWrite(LED_PIN, HIGH);
        prevMillis = millis();
        while (Input2A - a > MARGIN && prevMillis==millis()){
          Input2A = Setpoint2A;
          Setpoint2A = (double) enc2A.read();
          pid2A.Compute();
          driver2.setM1Speed(Output2A);
          Input2A = (double) enc2A.read();
        }
      }
      //driver2.setM1Speed(0);
      digitalWrite(LED_PIN, LOW);
      break; 
      
    case M2B:
      Setpoint2B = a;
      Input2B = (double) enc2B.read(); 
      if (a - Input2B > MARGIN){
        driver2.flipM2(false);
        digitalWrite(LED_PIN, HIGH);
        prevMillis = millis();
        while (a - Input2B > MARGIN && prevMillis == millis()){
          if (DEBUG){
            Serial.println(a);
            Serial.println(Output2B);
            Serial.println(enc2B.read());
          }
          pid2B.Compute();
          driver2.setM2Speed(Output2B);
          Input2B = (double) enc2B.read();
        }
      } else if (Input2B - a > MARGIN) {
        driver2.flipM2(true);
        digitalWrite(LED_PIN, HIGH);
        prevMillis = millis();
        while (Input2B - a > MARGIN && prevMillis == millis()){
          if (DEBUG){
            Serial.println("here1");
          }
          Input2B = Setpoint2B;
          Setpoint2B = (double) enc2B.read();
          pid2B.Compute();
          driver2.setM2Speed(Output2B);
          Input2B = (double) enc2B.read();
        }
      }
      //driver2.setM2Speed(0);
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
      for (x=0; x<4; x++){
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
      
      for (x=0; x<4; x++){
        if (DEBUG){ Serial.printf("commanding motor %d: %d\n", x, prevDeltas[x]); }
        moveDeltaDistance(prevDeltas[x], x);
      }
    }
  } else {
    for (x=0; x<4; x++){ //iterate through all the motors 
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
        Serial.printf("commanding motor %d: zero command\n", x);
        moveDeltaDistance(0, x);
      }
    }
    //delay(500);
  }
}
