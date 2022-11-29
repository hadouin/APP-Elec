#include <Arduino.h>
#include <stdio.h>

#define CARDIO_PIN PD_0

int valeurPrecedente = 0;
int lastTime = 0;
int valeurSeuil = 150;

int getCardio() {
  int i = 0;
  int log[9];
  int somme = 0;
  int startTime = millis();
  do {
    int readValue = analogRead(CARDIO_PIN);
    int detectTime = millis();
    if (readValue > valeurSeuil && readValue > valeurPrecedente){
      if (detectTime > lastTime + 100) {
        int avgBPM = 30 * 1000 / (detectTime - lastTime);
        log[i] = avgBPM;
        somme = somme + avgBPM;
        i++;
      }
      lastTime = detectTime;
    }
    valeurPrecedente = readValue;
    if (startTime - millis() > 30000) {
      return 0;
    }
  } while (i<11);

  return somme / 10;
}