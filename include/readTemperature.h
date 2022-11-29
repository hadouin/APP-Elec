#include <Arduino.h>
#include <string.h>
#include <stdlib.h>

#define DHT11_PIN PA_5
// Function to concatenate
// two integers into one
int concat(int a, int b)
{
 
    char s1[20];
    char s2[20];
 
    // Convert both the integers to string
    sprintf(s1, "%d", a);
    sprintf(s2, "%d", b);
 
    // Concatenate both strings
    strcat(s1, s2);
 
    // Convert the concatenated string
    // to integer
    int c = atoi(s1);
 
    // return the formed integer
    return c;
}

void dec2bin(int n)
{
    int c, k;

   for (c = 15; c >= 0; c--)
    {
        k = n >> c;

        if (k & 1) 
        {
            Serial.print("1");
            Serial1.print("1");
        }
        else 
        { 
            Serial.print("0");
            Serial1.print("0");
        }
    }
}

void dec2bin8(int n)
{
    int c, k;

   for (c = 7; c >= 0; c--)
    {
        k = n >> c;

        if (k & 1) {

            Serial.print("1");
        }
        else {
             Serial.print("0");
        }
    }
}

uint16_t rawHumidity = 0;
uint16_t rawTemperature = 0;
uint8_t checkSum = 0;
uint16_t data = 0;

uint8_t humi;
uint8_t humd;
uint8_t tempi;
uint8_t tempd; 

void start_signal(uint8_t dht11_pin)
{
  pinMode(dht11_pin, OUTPUT);
  digitalWrite(dht11_pin, LOW); 
  delay(18);
  digitalWrite(dht11_pin, HIGH);
  pinMode(dht11_pin, INPUT);
  digitalWrite(dht11_pin, HIGH);
}
void read_dht11(uint8_t dht11_pin)
{

  unsigned long startTime;
  
  for ( int8_t i = -3 ; i < 80; i++ ) 
  { 
    data = 0;
    byte live;
    startTime = micros();

    // Wait for state to change
    do {
      live = (unsigned long)(micros() - startTime);
      if ( live > 90 ) {
        Serial.println("ERROR_TIMEOUT");
        return;
      }
    } while ( digitalRead(dht11_pin) == (i & 1) ? HIGH : LOW );

    // choose 1 or 0 in function of the duration of the pulse
    if ( i >= 0 && (i & 1) ) {
      data <<= 1;
      // TON of bit 0 is maximum 30 usecs and of bit 1 is at least 68 usecs.
      if ( live > 30 ) {
        data |= 1; // we got a one
      }
    }

    // assign data read to corresponding value 
    switch ( i ) {
      case 31:
        rawHumidity = data;
        break;
      case 63:
        rawTemperature = data;
        break;
      case 79: 
        checkSum = data;
        break;
    }
  }

  // humi = rawHumidity >> 8;
  // rawHumidity <<= 8;
  // humd = rawHumidity >> 8;

  // tempi = rawTemperature >> 8;
  // rawTemperature <<= 8;
  // tempd = rawTemperature >> 8;

  Serial.println("Humidity: ");
  dec2bin(rawHumidity);
  Serial.print("\t");
  humi = rawHumidity >> 8;
  dec2bin8(humi);
  Serial.print("\t");
  rawHumidity = rawHumidity << 8;
  humd = rawHumidity >> 8;
  dec2bin8(humd);
  Serial.print("\t");
  Serial.print(humi);
  Serial.print(".");
  Serial.print(humd);
  Serial.print("%");
  Serial.println("");

  Serial.println("Temperature Degree Celcius: ");
  dec2bin(rawTemperature);
  Serial.print("\t");
  tempi = rawTemperature >> 8;
  dec2bin8(tempi);
  Serial.print("\t");
  rawTemperature = rawTemperature << 8;
  tempd = rawTemperature >> 8;
  //tempd = (byte)rawTemperature;
  dec2bin8(tempd);
  Serial.print("\t");
  Serial.print(tempi);
  Serial.print(".");
  Serial.print(tempd);
  Serial.print("C");
  Serial.println("");

  Serial.println("Checksum Byte: ");
  dec2bin8(checkSum);
  Serial.println("");
  dec2bin8(tempi + tempd + humi + humd);
  Serial.println("");
  if ((byte)checkSum == (byte)(tempi + tempd + humi + humd))
  {
    Serial.print("CHECKSUM_OK");
  } 
  else 
  {
    Serial.print("CHECKSUM_ERROR");
  }
  Serial.println("");
  Serial.println("");
  Serial.println("");

}

void captureTempHum(uint8_t dht11_pin){
  start_signal(dht11_pin);
  read_dht11(dht11_pin);
}

int getTemperature() {
  captureTempHum(DHT11_PIN);
  return (tempi << 8) | tempd;
}

int getHumidite() {
  captureTempHum(DHT11_PIN);
  return (humi << 8) | humd;
}
