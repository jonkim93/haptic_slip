#include <DRV8835MotorShield.h>

/*
 * This example uses the DRV8835MotorShield library to drive each motor with the
 * Pololu DRV8835 Dual Motor Driver Shield for Arduino forward, then backward. 
 * The yellow user LED is on when a motor is set to a positive speed and off when
 * a motor is set to a negative speed.
 */

#define LED_PIN 13

DRV8835MotorShield motors;

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  
  // uncomment one or both of the following lines if your motors' directions need to be flipped
  //motors.flipM1(true);
  //motors.flipM2(true);
}

void loop()
{
  // run M1 motor with positive speed
  digitalWrite(LED_PIN, HIGH);
  motors.flipM1(false);
  motors.setM1Speed(400);
  delay(1000);
  motors.setM1Speed(0);
  delay(1000);
  
  // run M1 motor with negative speed
  digitalWrite(LED_PIN, LOW);
  motors.flipM1(true);
  motors.setM1Speed(400);
  delay(1000);
  motors.setM1Speed(0);
  delay(1000);
}
