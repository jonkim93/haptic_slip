#include <DRV8835MotorShield.h>
#include <Encoder.h>
#include <PID_v1.h>
#include <stdlib.h> 


/*
 * This example uses the DRV8835MotorShield library to drive each motor with the
 * Pololu DRV8835 Dual Motor Driver Shield for Arduino forward, then backward. 
 * The yellow user LED is on when a motor is set to a positive speed and off when
 * a motor is set to a negative speed.
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

double Setpoint, Input, Output;
boolean Flipped;
boolean FingerDown;
boolean Homed;

double aggKp=2, aggKi=5, aggKd=3;
double consKp=2, consKi=5, consKd=3;
int sampleTime = 2;

DRV8835MotorShield driver1;
Encoder enc1A(2,3);
PID pid1A(&Input, &Output, &Setpoint, 2, 5, 3, DIRECT);

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Homed = false;
  
  Input = (double) enc1A.read();
  Setpoint = Input;
  pid1A.SetMode(AUTOMATIC);
  pid1A.SetSampleTime(sampleTime);
  
  if (DEBUG){
    Serial.println("done setting up");
  }
}

void goHome(){
  moveToAngleDirect(0.0, M1A);
}

void moveToAngleAdaptive(double a, int motor){
  switch (motor){
    case M1A:
    
      Setpoint = a;
      Input = (double) enc1A.read(); 
    
      double gap;
      
      if (a - Input > MARGIN){
        driver1.flipM1(false);
        digitalWrite(LED_PIN, HIGH);
        while (a - Input > MARGIN){
          gap = abs(Setpoint-Input); //distance away from setpoint
          if(gap<10)
          {  //we're close to setpoint, use conservative tuning parameters
            pid1A.SetTunings(consKp, consKi, consKd);
          }
          else
          {
            //we're far from setpoint, use aggressive tuning parameters
            pid1A.SetTunings(aggKp, aggKi, aggKd);
          }
          pid1A.Compute();
          if (DEBUG){
            //Serial.printf("OUTPUT: %d\n", Output);
            //Serial.print(Output);
          }
          driver1.setM1Speed(Output);
          Input = (double) enc1A.read();
        }
      } else if (Input - a > MARGIN) {
        driver1.flipM1(true);
        digitalWrite(LED_PIN, HIGH);
        while (Input - a > MARGIN){
          Input = Setpoint;
          Setpoint = (double) enc1A.read();
          gap = abs(Setpoint-Input); //distance away from setpoint
          if(gap<10)
          {  //we're close to setpoint, use conservative tuning parameters
            pid1A.SetTunings(consKp, consKi, consKd);
          }
          else
          {
            //we're far from setpoint, use aggressive tuning parameters
            pid1A.SetTunings(aggKp, aggKi, aggKd);
          }
          pid1A.Compute();
          if (DEBUG){
            Serial.println("OUTPUT: ");
            Serial.print(Output);
          }
          driver1.setM1Speed(Output);
          Input = (double) enc1A.read();
        }
      }
      driver1.setM1Speed(0);
      digitalWrite(LED_PIN, LOW);
      break;
  } 
}



void moveToAngleDirect(double a, int motor){
  switch (motor){
    case M1A:
    
      Setpoint = a;
      Input = (double) enc1A.read(); 
    
      double gap;
      
      if (a - Input > MARGIN){
        driver1.flipM1(false);
        digitalWrite(LED_PIN, HIGH);
        while (a - Input > MARGIN){
          pid1A.Compute();
          if (DEBUG){
            //Serial.printf("OUTPUT: %d\n", Output);
            //Serial.print(Output);
          }
          driver1.setM1Speed(Output);
          Input = (double) enc1A.read();
        }
      } else if (Input - a > MARGIN) {
        driver1.flipM1(true);
        digitalWrite(LED_PIN, HIGH);
        while (Input - a > MARGIN){
          Input = Setpoint;
          Setpoint = (double) enc1A.read();
          pid1A.Compute();
          if (DEBUG){
            //Serial.println("OUTPUT: ");
            //Serial.print(Output);
          }
          driver1.setM1Speed(Output);
          Input = (double) enc1A.read();
        }
      }
      driver1.setM1Speed(0);
      digitalWrite(LED_PIN, LOW);
      break;
  } 
}


void loop()
{
  if (!Homed){
    goHome();
    Homed = true; 
  }
  FingerDown = true;
  if (Serial.available()){
    if (DEBUG){
      Serial.println("Receiving serial");
    }
    double K_p = (double) Serial.parseFloat();
    double K_i = (double) Serial.parseFloat();
    double K_d = (double) Serial.parseFloat();
    int a = Serial.parseInt();
    char c = Serial.read();
    
    if (c=='\n'){
      if (DEBUG){
        Serial.printf("K_p: %f\n", K_p);
        Serial.printf("K_i: %f\n", K_i); 
        Serial.printf("K_d: %f\n", K_d);
        Serial.printf("angle: %d\n", a);
      }
    
      pid1A.SetTunings(K_p, K_i, K_d);
      long s = millis();
      moveToAngleDirect(a, M1A);
      long e = millis();
      long elapsed = e-s;
      Serial.print(elapsed);
      Serial.print("\n");
      Serial.print((int) enc1A.read());
      Serial.print("\n");
    }
    //moveDeltaDistance(a, M1A);
    if (DEBUG){
      Serial.printf("\nMotor 1A: "); 
      Serial.print((int) enc1A.read());
    }
  }
}
