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

double Setpoint1A, Input1A, Output1A, Setpoint1B, Input1B, Output1B;
boolean Flipped;
boolean FingerDown;
boolean Homed;

DRV8835MotorShield driver1;
DRV8835MotorShield driver2;
Encoder enc1A(2,3);
Encoder enc1B(4,5);
double K_p = 2.0;
double K_i = 5.0;
double K_d = 3.0;
PID pid1A(&Input1A, &Output1A, &Setpoint1A, K_p, K_i, K_d, DIRECT);
PID pid1B(&Input1B, &Output1B, &Setpoint1B, K_p, K_i, K_d, DIRECT);

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
  if (DEBUG){
    Serial.println("done setting up");
  }
}

void goHome(){
  moveToAngle(180.0, M1A);
  moveToAngle(180.0, M1B); 
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
      Serial.println(enc1B.read());
      break; 
  }
  
  
  
}

void testCommandSpeed(int sp){
  /**if (sp > 0){
    motors.flipM1(true);
    motors.setM1Speed(sp);
  } else if (sp < 0){
    motors.flipM1(false);
    motors.setM1Speed(-sp); 
  } **/
  if (DEBUG){
    Serial.println(sp);
  }
  driver1.setM1Speed(sp);
  delay(1000);
  driver1.setM1Speed(0);
}

void loop()
{
  if (!Homed){
    //goHome();
    Homed = true; 
  }
  FingerDown = true;
  if (Serial.available()){
    Serial.println("receiving");
    if (DEBUG){
      Serial.println("Receiving serial");
    }
    char f = Serial.read();
    if (DEBUG){
      Serial.println(f);
    }
    FingerDown = f!='f';
    int m = Serial.parseInt();
    if (DEBUG){
      Serial.println(m);
    }
    int a;
    char s = Serial.read();
    if (DEBUG){
      Serial.println(s);
    }
    if (s == '-'){
      a = -(double) Serial.parseInt();
    } else if (s == '+'){
      a = (double) Serial.parseInt();
    }
    if (DEBUG){
      Serial.println(a);
    }
    if (FingerDown){
      //moveToAngle(a, m);
      moveDeltaDistance(a, m);
      if (DEBUG){
        Serial.println(a);
        Serial.printf("\nMotor 1A: "); 
        Serial.print((int) enc1A.read());
        Serial.printf("\nMotor 1B: ");
        Serial.print((int) enc1B.read());
      }
    } else if (!FingerDown) {
      //goHome();
    }
  }
}
