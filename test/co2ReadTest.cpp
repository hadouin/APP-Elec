#include <Arduino.h>
#include <stdio.h>
byte PWM_PIN = A4;
 
int pwm_value;
int pwm_percent; 

void setup() {
  pinMode(PWM_PIN, INPUT);
  Serial.begin(9600);
}
 
void loop() {
  pwm_value = pulseIn(PWM_PIN, HIGH);
  pwm_percent = pwm_value * 100 / 33333;
  if (pwm_percent >= 55) {
    int y = 40 * pwm_percent - 1800;
    Serial.print("value of CO2=");
    Serial.println(y);
  }
  if (pwm_percent >= 5 && pwm_percent <= 45) {
    int y = 25 * pwm_percent - 125;
    Serial.print("value of tVOC=");
    Serial.println(y);
  }
}