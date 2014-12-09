#include <DRV8835MotorShield.h>
#include <Encoder.h>
#include <PID_v1.h>

/*
 * This example uses the DRV8835MotorShield library to drive each motor with the
 * Pololu DRV8835 Dual Motor Driver Shield for Arduino forward, then backward. 
 * The yellow user LED is on when a motor is set to a positive speed and off when
 * a motor is set to a negative speed.
 */

#define LED_PIN 13
#define MARGIN 0

double Setpoint, Input, Output;
boolean Flipped;

DRV8835MotorShield motors;
Encoder myEnc(2,3);
PID myPID(&Input, &Output, &Setpoint, 2, 5, 3, DIRECT);

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Input = (double) myEnc.read();
  Setpoint = Input;
  myPID.SetMode(AUTOMATIC);
  myPID.SetSampleTime(2);
}

void moveToAngle(double a){
  Setpoint = a;
  Input = (double) myEnc.read(); 
  if (a - Input > MARGIN){
    motors.flipM1(true);
    while (a - Input > MARGIN){
      myPID.Compute();
      motors.setM1Speed(Output);
      //Serial.println("speed=255");
      Input = (double) myEnc.read();
    }
  } else if (Input - a > MARGIN) {
    motors.flipM1(false);
    while (Input - a > MARGIN){
      Input = Setpoint;
      Setpoint = (double) myEnc.read();
      myPID.Compute();
      motors.setM1Speed(Output);
      Input = (double) myEnc.read();
    }
  }
  Serial.println("speed=0");
  motors.setM1Speed(0);
  Serial.println(myEnc.read());
}

void loop()
{
  float a = Serial.parseFloat();
  if (Serial.read() == '\n'){
    moveToAngle((double) a);
  }
}
