#include <Arduino.h>
#include <stdio.h>

#include "oled.h"
#include "IsepScreen.h"


void setup()
{
  // put your setup code here, to run once:

  InitI2C();
  InitScreen();

  Display(EmiScreenWhite);
  Serial1.begin(9600);
  Serial1.println("Bonjour");

  Serial.begin(9600);
  Serial.println("Bonjour");
  pinMode(PE_2, INPUT);

  // void test_fft(); // La fonction test_fft disponible dans le fichier fft.ino montre comment utiliser la fft

}

float tensionFloat(int potentiometre) {
  float result = (float) potentiometre * 3.3 / 4095;  
  return result;
}

void StringAlignedRight(int y, char *val)
{
  int len = strlen(val);
  int x = 128 - len * 6;
  DisplayString(x, y, val);
}

void DisplayPotValue()
{
  // lire la valuer du potentiometre 
  int val = analogRead(PE_2);
  float tension = tensionFloat(val);
  // afficher la tension sur le Serial
  Serial1.print("Tension:");
  Serial1.println(tension);

  // afficher la valuer au Serial1 aka Bluetooth
  Serial1.print("Valeur potentionmetre = ");
  Serial1.println(val);

    // afficher la tension sur le Serial
  Serial.print("Tension:");
  Serial.println(tension);

  // afficher la valuer au Serial1 aka Bluetooth
  Serial.print("Valeur potentionmetre = ");
  Serial.println(val);

  

  // convertir la valeur POT en string 
  char valStr[21]; // define char array with 21 max char to print on screen
  
  float tmpVal = (tension < 0) ? -tension : tension;
  
  int tmpInt1 = tmpVal;                  // Get the integer
  float tmpFrac = tmpVal - tmpInt1;      // Get fraction
  int tmpInt2 = trunc(tmpFrac * 100);  // Turn into integer
  
  sprintf(valStr, "V:%d.%02d pot:%d", tmpInt1, tmpInt2, val);
  StringAlignedRight(7, valStr); // afficher la string sur l'ecran aligné a droite 7 = coordonnées Y sur l'ecran de 0 à 7 (8 lignes)
}

void loop()
{
  if (Serial.available() > 0) {
    // read the incoming byte:
    byte incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
  }
  
  Display(EmiScreenWhite);
  DisplayPotValue();
  delay(1000);
}