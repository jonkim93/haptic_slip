#include <DRV8835MotorShield1.h>
#include <DRV8835MotorShield2.h>
#include <Encoder.h>
#include <PID_v1.h>
#include <stdlib.h> 

/*
 * Deterministic motor control of the haptic slip device; send packets over serial in the form:
 * 'fmsd', where 
 *     f = {'t','f'} for whether or not the finger is down, 
 *     m = {1,2,3,4} for which motor we are commanding, 
 *     s = {'+', '-'} for the sign of the delta command,
 *     d = 0-10000, where each increment is an additional tick of 0.01cm
 */

#define LED_PIN 13
#define MARGIN 0
#define SCALE_FACTOR 114.591559026
#define M1A 1
#define M1B 2
#define M2A 3
#define M2B 4
#define DEBUG false
#define INCREMENT 0.01

double Setpoint1A, Input1A, Output1A, Setpoint1B, Input1B, Output1B;
double Setpoint2A, Input2A, Output2A, Setpoint2B, Input2B, Output2B;
boolean Flipped;
boolean FingerDown;
boolean Homed;
boolean terminated;
int prevDeltaDist;
int prevMotor;
int prevMillis;
int counter;

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
  
  Input1A = (double) enc1A.read();
  Setpoint1A = Input1A;
  pid1A.SetMode(AUTOMATIC);
  pid1A.SetSampleTime(2);
  
  Input1B = (double) enc1B.read();
  Setpoint1B = Input1B;
  pid1B.SetMode(AUTOMATIC);
  pid1B.SetSampleTime(2);
  
  Input2A = (double) enc2A.read();
  Setpoint2A = Input2A;
  pid2A.SetMode(AUTOMATIC);
  pid2A.SetSampleTime(2);
  
  Input2B = (double) enc2B.read();
  Setpoint2B = Input2B;
  pid2B.SetMode(AUTOMATIC);
  pid2B.SetSampleTime(2);
  if (DEBUG){
    Serial.println("done setting up");
  }
  prevMillis = millis();
  counter = 0;
  terminated = false;
}

void goHome(){
  moveToAngle(180.0, M1A);
  moveToAngle(180.0, M1B); 
  moveToAngle(180.0, M2A);
  moveToAngle(180.0, M2B);
}

void moveDeltaDistance(int d, int motor){ //moves a certain distance in cm
  double deltaDistance = d * INCREMENT;
  double deltaAngle = deltaDistance * 114.6;
  double currAngle;
  switch (motor){
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
  if (DEBUG){
    Serial.println(currAngle);
    Serial.println(currAngle+deltaAngle);
  }
  moveToAngle(currAngle + deltaAngle, motor);
}

void moveToAngle(double a, int motor){
  switch (motor){
    case M1A:
      Setpoint1A = a;
      Input1A = (double) enc1A.read(); 
      if (a - Input1A > MARGIN){
        driver1.flipM1(false);
        digitalWrite(LED_PIN, HIGH);
        while (a - Input1A > MARGIN){
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
        while (Input1A - a > MARGIN){
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
        while (a - Input1B > MARGIN){
          if (DEBUG){
            Serial.println("here");
            Serial.println(a);
            Serial.println(Output1B);
            Serial.println(enc1B.read());
          }
          pid1B.Compute();
          driver1.setM2Speed(Output1B);
          Input1B = (double) enc1B.read();
        }
      } else if (Input1B - a > MARGIN) {
        driver1.flipM2(true);
        digitalWrite(LED_PIN, HIGH);
        while (Input1B - a > MARGIN){
          if (DEBUG){
            Serial.println("here1");
          }
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
        while (a - Input2A > MARGIN){
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
        while (Input2A - a > MARGIN){
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
        while (a - Input2B > MARGIN){
          if (DEBUG){
            Serial.println("here");
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
        while (Input2B - a > MARGIN){
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
  if (Serial.available()){ 
    if (DEBUG){
      Serial.println("new command");
    }
    char f = Serial.read();
    int m = Serial.parseInt();
    int a;
    char s = Serial.read();
    if (s == '-'){
      a = -(double) Serial.parseInt();
    } else if (s == '+'){
      a = (double) Serial.parseInt();
    }
    
    a = a/5; //scale down 
    terminated = false;
    prevDeltaDist = a;
    prevMotor = m;
    moveDeltaDistance(a, m); 
  } else {
    if (counter < 5 && !terminated){
      if (prevMillis != millis()){
        if (DEBUG){
          Serial.println(counter);
          Serial.println(prevDeltaDist);
        }
        moveDeltaDistance(prevDeltaDist, prevMotor); 
        prevMillis = millis();
        counter ++;
        if (DEBUG){
          Serial.println("executing command");
        }
      }
    } else {
      counter = 0;
      terminated = true;
    }
  }
}
