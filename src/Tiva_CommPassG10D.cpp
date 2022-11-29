#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>

#include "readCardio.h"
#include "readCO2.h"

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
        }
        else 
        { 
            Serial.print("0");
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

void wait_for_dht11()
{
delay(2000);
}

void start_signal(uint8_t dht11_pin)
{
pinMode(dht11_pin, OUTPUT);
digitalWrite(dht11_pin, LOW); 
delay(18);
digitalWrite(dht11_pin, HIGH);
pinMode(dht11_pin, INPUT);
digitalWrite(dht11_pin, HIGH); 
}

  uint8_t humi;
  uint8_t humd;
  uint8_t tempi;
  uint8_t tempd; 

void read_dht11(uint8_t dht11_pin)
{
  uint16_t rawHumidity = 0;
  uint16_t rawTemperature = 0;
  uint8_t checkSum = 0;
  uint16_t data = 0;

  unsigned long startTime;
  
  for ( int8_t i = -3 ; i < 80; i++ ) 
  { 
    byte live;
    startTime = micros();

    do {
      live = (unsigned long)(micros() - startTime);
      if ( live > 90 ) {
        Serial.println("ERROR_TIMEOUT");
        return;
      }
    } while ( digitalRead(dht11_pin) == (i & 1) ? HIGH : LOW );

    if ( i >= 0 && (i & 1) ) {
      data <<= 1;

      // TON of bit 0 is maximum 30 usecs and of bit 1 is at least 68 usecs.
      if ( live > 30 ) {
        data |= 1; // we got a one
      }
    }

    switch ( i ) {
      case 31:
        rawHumidity = data;
        break;
      case 63:
        rawTemperature = data;
      case 79: 
        checkSum = data;
        data = 0;
        break;
    }
  }

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


//------------------------------------------------------------------------
// Comm_passerelle
// Programme de test de la Communication avec la passerelle
//------------------------------------------------------------------------

char  TrameEnvoi[20];
char  TrameRecep[20];

void setup() 
//------------------------------------------------------------------------
//------------------------------------------------------------------------
{
// initialiser ici les ports en entrée ou sortie:

	//pinMode(..., ...);

// initialiser les deux serial ports:
	Serial.begin(9600);			// port serie terminal Energia
	Serial1.begin(9600);		// port serie BlueTooth APP

// initialiser les champs fixes du buffer d'envoi
  TrameEnvoi[0] = '1';			// type trame = trame courante
// le champ objet : mettre le numero de groupe
	TrameEnvoi[1] = 'G';
	TrameEnvoi[2] = '1';
	TrameEnvoi[3] = '0';
	TrameEnvoi[4] = 'D';		// votre numero
// le champ "requete" : envoi Objet -> passerelle = requete en ecriture
	TrameEnvoi[5] = '1';		//	 requete en ecriture
// le champ "type" : type de capteur
	//TrameEnvoi[6] = x;		// ce champ est variable
// le champ "numero" : numero du capteur
	TrameEnvoi[7] = '0';		// on va laisser le numero '01' pour
	TrameEnvoi[8] = '1';		// tous les capteurs qu'on utilise
// le champ "valeur" : la valeur du capteur a envoyer au site web
	//TrameEnvoi[] = x;		// ce champ est variable octets 9,10,11,12
	//TrameEnvoi[] = x;
	//TrameEnvoi[] = x;
	//TrameEnvoi[] = x;
// le champ "time stamp" : ce champ n'est pas utilise par la passerelle
	TrameEnvoi[13] = 'F';	// valeur quelconque
	TrameEnvoi[14] = 'F';
	TrameEnvoi[15] = 'F';
	TrameEnvoi[16] = 'F';
// le champ "checksum" : ce champ est variable
	//TrameEnvoi[17] = '0';
	//TrameEnvoi[18] = '0';		// 19e et dernier octet de la trame
}

void   Send_Trame(char typeCapt, uint16_t valeurCapt);
void   Recep_Trame(void);
void   Analyse_Trame(void);
char   Conv_hexToAsc(int digit);

// define for C02 and tVOC not found on datasheet of gateway
#define		CAPT_TEMP			0x33	/* type de capteur de temperature			*/
#define		CAPT_HUM			0x34  /* type de capteur d' humidité				*/
#define		CAPT_SONMIC		0x37	/* type de capteur de son/microphone	*/
#define		CAPT_CARDIO		0x39	/* type de capteur de freq cardiaque	*/

void loop() 
//------------------------------------------------------------------------
//------------------------------------------------------------------------
{
  wait_for_dht11();
  start_signal(PA_5);
  read_dht11(PA_5);
  int U = tempi % 10;
  int D = tempi / 10;
  int DU = (D << 4) | U;
  uint16_t valeur_Temp = (DU << 4) | tempd;
  Send_Trame(CAPT_TEMP, valeur_Temp); 

}

void  Recep_Trame(void)
//------------------------------------------------------------------------
//------------------------------------------------------------------------
{
	int nbrecu, len;	char digRecu;

	Serial.print("Trame Reçue = ");
	nbrecu = 0;
	//
	// TODO: ajouter ici le code pour recevoir les octets et les mettre dans TrameRecep[]
	// repeter autant de fois que necessaire
		// tester si un octet est dans le buffer de reception de Serial1 (BlueTooth)
		// si oui, lire l'octet et le mettre dans TrameRecep
	//
	Serial.println();		// retour à la ligne sur la console
}

void  Analyse_Trame(void)
//------------------------------------------------------------------------
//------------------------------------------------------------------------
{
	Serial.println(">>>  Debut Analyse");
	// Analyser la trame reçue dans le buffer TrameRecep[]
	// Si c'est un "écho" de la trame envoyée, alors ne rien faire
	// Si c'est un message venant du site web, alors decider de l'action à faire
	Serial.println("<<<  Fin Analyse");
	Serial.println();		// retour à la ligne sur la console
}

char  Conv_hexToAsc(int digit)
//------------------------------------------------------------------------
// convertir un digit hexa (0..F) en caractère Ascii
//------------------------------------------------------------------------
{	char valAsc;

	// garder que les 4 bits de poid faible = 1 chiffre hexa (0 à 15)
	digit &= 0x0F;
	valAsc = digit + 0x30;
	if (digit > 9)
		valAsc += 0x07;
	return valAsc;
}

#define		SIZE_ENVOI	19
#define		SIZE_RECEP	15

void  Send_Trame(char typeCapt, uint16_t valeurCapt)
//------------------------------------------------------------------------
//------------------------------------------------------------------------
{
	int  digit, n;		char CheckSum, digAsc;

	TrameEnvoi[6] = typeCapt;	// le champ "type" : type de capteur

	// convertir la valeur du capteur en 4 chiffres ASCII (poid fort en premier)
	// conversion du 1er chiffre (poid fort)
	digit = (valeurCapt >> 12) & 0x0F;
	TrameEnvoi[9]  = Conv_hexToAsc(digit);
	// conversion du 2e chiffre
	digit = (valeurCapt >> 8) & 0x0F;
	TrameEnvoi[10] = Conv_hexToAsc(digit);
	// conversion du 3e chiffre
	digit = (valeurCapt >> 4) & 0x0F;
	TrameEnvoi[11] = Conv_hexToAsc(digit);
	// conversion du 4e chiffre (poid faible)
	digit = valeurCapt & 0x0F;			// pas besoin de décalage.
	TrameEnvoi[12] = Conv_hexToAsc(digit);

	// lire les octets de TrameEnvoi un par un et envoyer sur les deux lignes serie
	Serial.print("Trame Envoyée = ");
	// boucle pour envoyer une trame vers la passerelle
	CheckSum = 0;
	// envoi des 'SIZE_ENVOI-2' premiers octets
	for (n = 0; n < SIZE_ENVOI-2; n++) {
		Serial.print(TrameEnvoi[n]);	// afficher sur la console
		Serial1.print(TrameEnvoi[n]);	// envoyer au BlueTooth = passerelle
		CheckSum = CheckSum + TrameEnvoi[n];
	}
	digit  = (CheckSum >> 4) & 0x0F;	// poid fort du CheckSum
	digAsc = Conv_hexToAsc(digit);
	Serial.print(digAsc);				// envoi du poid fort
	Serial1.print(digAsc);
	digit  = CheckSum & 0x0F;			// poid faible du CheckSum
	digAsc = Conv_hexToAsc(digit);
	Serial.print(digAsc);				// envoi du poid faible
	Serial1.print(digAsc);
	Serial.println();		// retour à la ligne sur la console
}
