#include <Arduino.h>
#include <stdio.h>

#define PWM_PIN A4

int valeur_CO2;
int valeur_tVOC;

void captureCO2tVOC() {
  pinMode(PWM_PIN, INPUT);

  int pwm_value;
  int pwm_percent;
  int startTime = millis();
  
  while (startTime - millis() < 500)
  {
    pwm_value = pulseIn(PWM_PIN, HIGH);
    pwm_percent = pwm_value * 100 / 33333;
    if (pwm_percent >= 55) {
      valeur_tVOC = 40 * pwm_percent - 1800;
    }
    if (pwm_percent >= 5 && pwm_percent <= 45) {
      valeur_CO2 = 25 * pwm_percent - 125;
    }
  }
}

int getCO2() {
  captureCO2tVOC();
  return valeur_CO2;
}

int gettVOC() {
  captureCO2tVOC();
  return valeur_tVOC;
}