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
#define SCALE_FACTOR 114.591559026
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
double inputs [4];
double outputs [4];
double setpoints [4];

DRV8835MotorShield1 driver1;
DRV8835MotorShield2 driver2;
Encoder enc1A(2,3);
Encoder enc1B(4,5);
Encoder enc2A(6,7);
Encoder enc2B(8,9);
Encoder encoders[] = {enc1A, enc1B, enc2A, enc2B};
double K_p = 2.0;
double K_i = 5.0;
double K_d = 3.0;
PID pid1A(&Input1A, &Output1A, &Setpoint1A, K_p, K_i, K_d, DIRECT);
PID pid1B(&Input1B, &Output1B, &Setpoint1B, K_p, K_i, K_d, DIRECT);
PID pid2A(&Input2A, &Output2A, &Setpoint2A, K_p, K_i, K_d, DIRECT);
PID pid2B(&Input2B, &Output2B, &Setpoint2B, K_p, K_i, K_d, DIRECT);
PID pids[] = {pid1A, pid1B, pid2A, pid2B};

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Homed = false;
  
  for (int i=0; i<4; i++){
     inputs[i] = (double) encoders[i].read();
     setpoints[i] = inputs[i];
     pids[i].SetMode(AUTOMATIC);
     pids[i].SetSampleTime(1);
     prevMillis[x] = millis();
     counters[x] = 0;
     terminated[x] = false;
  }
}

void moveDeltaDistance(int d, int motor){ //moves a certain distance in cm
  double deltaDistance = d * INCREMENT;
  double deltaAngle = deltaDistance * 114.6;
  double currAngle = (double) encoders[motor].read();
  moveToAngle(currAngle + deltaAngle, motor);
}

void moveToAngle(double a, int motor){
  int prevMillis;
  setpoints[motor] = a;
  inputs[motor] = (double) encoders[motor].read();
  if (a - inputs[motor] > MARGIN){
     
  } else if (inputs[
  
  switch (motor){
    case M1A:
      Setpoint1A = a;
      Input1A = (double) enc1A.read(); 
      if (a - Input1A > MARGIN){
        driver1.flipM1(false);
        digitalWrite(LED_PIN, HIGH);
        prevMillis = millis();
        while (a - Input1A > MARGIN && prevMillis == millis()){
          pid1A.Compute();
          driver1.setM1Speed(Output1A);
          Input1A = (double) enc1A.read();
          if (DEBUG){
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
            Serial.println(Input1A);
            Serial.println(Output1A); 
          }
        }
      }
      driver1.setM1Speed(0);
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
      driver1.setM2Speed(0);
      digitalWrite(LED_PIN, LOW);
      break; 
      
    case M2A:
      Setpoint2A = a;
      Input2A = (double) enc2A.read(); 
      if (a - Input2A > MARGIN){
        driver2.flipM1(false);
        if (DEBUG){
          Serial.println("greater than margin");
        }
        digitalWrite(LED_PIN, HIGH);
        prevMillis = millis();
        while (a - Input2A > MARGIN && prevMillis==millis()){
          if (DEBUG){
            Serial.println("here");
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
          if (DEBUG){
            Serial.println("here1");
          }
          Input2A = Setpoint2A;
          Setpoint2A = (double) enc2A.read();
          pid2A.Compute();
          driver2.setM1Speed(Output2A);
          Input2A = (double) enc2A.read();
        }
      }
      driver2.setM1Speed(0);
      digitalWrite(LED_PIN, LOW);
      break; 
      
    case M2B:
      Setpoint2B = a;
      Input2B = (double) enc2B.read(); 
      if (a - Input2B > MARGIN){
        driver2.flipM2(false);
        if (DEBUG){
          Serial.println("greater than margin");
        }
        digitalWrite(LED_PIN, HIGH);
        prevMillis = millis();
        while (a - Input2B > MARGIN && prevMillis == millis()){
          if (DEBUG){
            Serial.println("here2B");
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
      driver2.setM2Speed(0);
      digitalWrite(LED_PIN, LOW);
      break; 
      
  }  
}

void loop()
{
  int x;
  int scaledCommand;
  if (Serial.available()){ 
    //parse serial for command
    /**char msg_array [20]; 
    Serial.readBytesUntil('\n', msg_array, 20);
    for (int c=0; c<20; c++){
      Serial.print(msg_array[c]); 
    }
    Serial.print('\n');**/
    char f = Serial.read();
    Serial.println(f);
    int a;
    char s;
    
    if (f=='t'){
      for (x=0; x<4; x++){
        s = Serial.read();
        Serial.printf("received and parsing: %d, %c\n", x, s);
        if (s == '-'){
          a = -(double) Serial.parseInt();
        } else if (s == '+'){
          a = (double) Serial.parseInt();
        }
        //scaledCommand = a/MS_PER_STEP; //scale down
        prevDeltas[x] = a/MS_PER_STEP; //scaledCommand;
        terminated[x] = false;
      }
      
      for (x=0; x<4; x++){
        Serial.printf("commanding motor %d: %d\n", x, prevDeltas[x]);
        moveDeltaDistance(prevDeltas[x], x);
      }
    }
  } else {
    for (x=0; x<4; x++){ //iterate through all the motors 
      if (counters[x] < MS_PER_STEP && !terminated[x]){
        if (prevMillis[x] != millis()){        
          Serial.printf("commanding motor %d: %d at %d\n", x, prevDeltas[x], prevMillis[x]);
          moveDeltaDistance(prevDeltas[x], x); 
          prevMillis[x] = millis();
          counters[x] ++;
        }
      } else {
        counters[x] = 0;
        terminated[x] = true;
      }
    }
  }
}
