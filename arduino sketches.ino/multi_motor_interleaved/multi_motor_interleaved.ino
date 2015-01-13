#include <DRV8835MotorShield1.h>
#include <DRV8835MotorShield2.h>
#include <Encoder.h>
#include <PID_v1.h>
#include <stdlib.h> 

/*
 * interleave commands to multiple motors simultaneously; doesn't work very well
 * the serial is being read twice; why??
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
  Serial.println("done setting up");
  if (DEBUG){
    Serial.println("done setting up");
  }
  for (int x=1; x<=4; x++){
    prevMillis[x] = millis();
    counters[x] = 0;
    terminated[x] = false;
  }
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
          pid1B.Compute();
          driver1.setM2Speed(Output1B);
          Input1B = (double) enc1B.read();
        }
      } else if (Input1B - a > MARGIN) {
        driver1.flipM2(true);
        digitalWrite(LED_PIN, HIGH);
        while (Input1B - a > MARGIN){
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
  int newlyCommandedMotor = 5; //if no new serial comes in, we will execute the previous commands for all four motors; see for-loop to understand how this works
  if (Serial.available()){ 
    //parse serial for command
    Serial.println("RECEIVING");
    char f = Serial.read();
    int m = Serial.parseInt();
    int a;
    char s = Serial.read();
    if (s == '-'){
      a = -(double) Serial.parseInt();
    } else if (s == '+'){
      a = (double) Serial.parseInt();
    }
    
    int scaledCommand = a/5; //scale down 
    terminated[m-1] = false; //set terminated bool for the proper motor (-1 for 1indexing to 0indexing)
    prevDeltas[m-1] = scaledCommand; //set previous delta for proper motor
    moveDeltaDistance(scaledCommand, m); //move the correct motor 
    newlyCommandedMotor = m; //set the newly commanded motor
  } 
  for (int x=1; x<=4; x++){ //iterate through all the motors
    if (x==newlyCommandedMotor){ //don't use the previous command for the newly-commanded motor
      break;
    } else{
      if (counters[x-1] < 5 && !terminated[x-1]){
        if (prevMillis[x-1] != millis()){        
          moveDeltaDistance(prevDeltas[x-1], x); 
          prevMillis[x-1] = millis();
          counters[x-1] ++;
        }
      } else {
        counters[x-1] = 0;
        terminated[x-1] = true;
      }
    }
  }
}
