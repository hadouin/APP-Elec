#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>

#include "oled.h"
#include "IsepScreen.h"

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

  // Serial.println("Humidity: ");
  // dec2bin(rawHumidity);
  // Serial.print("\t");
  humi = rawHumidity >> 8;
  // dec2bin8(humi);
  // Serial.print("\t");
  rawHumidity = rawHumidity << 8;
  humd = rawHumidity >> 8;
  // dec2bin8(humd);
  // Serial.print("\t");
  // Serial.print(humi);
  // Serial.print(".");
  // Serial.print(humd);
  // Serial.print("%");
  // Serial.println("");

  // Serial.println("Temperature Degree Celcius: ");
  // dec2bin(rawTemperature);
  // Serial.print("\t");
  tempi = rawTemperature >> 8;
  // dec2bin8(tempi);
  // Serial.print("\t");
  rawTemperature = rawTemperature << 8;
  tempd = rawTemperature >> 8;
  //tempd = (byte)rawTemperature;
  // dec2bin8(tempd);
  // Serial.print("\t");
  // Serial.print(tempi);
  // Serial.print(".");
  // Serial.print(tempd);
  // Serial.print("C");
  // Serial.println("");

  // Serial.println("Checksum Byte: ");
  // dec2bin8(checkSum);
  // Serial.println("");
  // dec2bin8(tempi + tempd + humi + humd);
  // Serial.println("");
  if ((byte)checkSum == (byte)(tempi + tempd + humi + humd))
  {
    // Serial.print("CHECKSUM_OK");
  } 
  else 
  {
    // Serial.print("CHECKSUM_ERROR");
  }
  // Serial.println("");
  // Serial.println("");
  // Serial.println("");
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

// Init logo
  InitI2C();
  InitScreen();
  Display(EmiScreenWhite);

}

void   Send_Trame(char typeCapt, uint16_t valeurCapt, char typeReq);
void   Recep_Trame(void);
void   Analyse_Trame(void);
char   Conv_hexToAsc(int digit);

// define request type
#define		REQ_ECRITURE		0x31		/* requete en ecriture						*/
#define		REQ_LECTURE			0x32  	/* requete en lecture						*/


// define for C02 and tVOC not found on datasheet of gateway
#define   CAPT_OLED     0x31  /* type de capteur d' OLED display   */

#define		CAPT_TEMP			0x33	/* type de capteur de temperature			*/
#define		CAPT_HUM			0x34  /* type de capteur d' humidité				*/
#define		CAPT_SONMIC		0x37	/* type de capteur de son/microphone	*/
#define		CAPT_CARDIO		0x39	/* type de capteur de freq cardiaque	*/

// define ports 
#define PORT_CARDIO PD_0
#define PORT_TEMP PA_5

// valeur pour cardio
int valeurPrecedente = 0;
int lastTime = 0;
int valeurSeuil = 150;

void loop() 
//------------------------------------------------------------------------
//------------------------------------------------------------------------
{
  // Send_Trame(CAPT_OLED, 0, REQ_LECTURE);
  // delay(100);
  Recep_Trame();
  Analyse_Trame();
  
  // Captation temperature
  
  wait_for_dht11();
  start_signal(PORT_TEMP);
  read_dht11(PORT_TEMP);

  int U = tempi % 10;
  int D = tempi / 10;
  int DU = (D << 4) | U;
  uint16_t valeur_Temp = (DU << 4) | tempd;
  Send_Trame(CAPT_TEMP, valeur_Temp, REQ_ECRITURE);
  delay(100);
  Recep_Trame();

  // Captation humidite
  int U_H = humi % 10;
  int D_H = humi / 10;
  int DU_H = (D_H << 4) | U_H;
  uint16_t valeur_Humi = (DU_H << 4) | humd;
  Send_Trame(CAPT_HUM, valeur_Humi, REQ_ECRITURE);
  delay(100);
  Recep_Trame();

  delay(200);



  // Captation cardiaque
  // int i = 0;
  // int log[9];
  // int somme = 0;
  // Serial.print('[');
  // do {
  //   int readValue = analogRead(PORT_CARDIO);
  //   int detectTime = millis();
  //   if (readValue > valeurSeuil && readValue > valeurPrecedente){
  //     if (detectTime > lastTime + 100) {
  //       int avgBPM = 60 * 1000 / (detectTime - lastTime);
  //       log[i] = avgBPM;
  //       Serial.print('=');
  //       somme = somme + avgBPM;

  //       i++;
  //     }
  //     lastTime = detectTime;
  //   }
  //   valeurPrecedente = readValue;
  // } while (i<11);
  // Serial.print("] ");
  // uint16_t moyenne = round(somme / 10);

  // Serial.println(moyenne);
  //   for (size_t i = 0; i < 11; i++)
  // {
  //   Serial.print(log[i]);
  //   Serial.print(' ');
  // }
  // Serial.println("");
  // Send_Trame(CAPT_CARDIO, moyenne);

}

void  Recep_Trame(void)
//------------------------------------------------------------------------
//------------------------------------------------------------------------
{
	int nbrecu;
  nbrecu = 0;
	//
	// TODO: ajouter ici le code pour recevoir les octets et les mettre dans TrameRecep[]
	// repeter autant de fois que necessaire
  // tester si un octet est dans le buffer de reception de Serial1 (BlueTooth)
  // si oui, lire l'octet et le mettre dans TrameRecep
  while (Serial1.available()) {
    TrameRecep[nbrecu] = Serial1.read();
    nbrecu++;
  }

}

void  Analyse_Trame(void)
//------------------------------------------------------------------------
//------------------------------------------------------------------------
{
	Serial.println(">>>  Debut Analyse");
	// Analyser la trame reçue dans le buffer TrameRecep[]
	// Si c'est un "écho" de la trame envoyée, alors ne rien faire
	if ( TrameRecep[0] == TrameEnvoi[0] 
    && TrameRecep[1] == TrameEnvoi[1] 
    && TrameRecep[2] == TrameEnvoi[2] 
    && TrameRecep[3] == TrameEnvoi[3] 
    && TrameRecep[4] == TrameEnvoi[4] 
    && TrameRecep[5] == TrameEnvoi[5] 
    && TrameRecep[6] == TrameEnvoi[6] 
    && TrameRecep[7] == TrameEnvoi[7] 
    && TrameRecep[8] == TrameEnvoi[8] 
    && TrameRecep[9] == TrameEnvoi[9] 
    && TrameRecep[10] == TrameEnvoi[10] 
    && TrameRecep[11] == TrameEnvoi[11] 
    && TrameRecep[12] == TrameEnvoi[12]
  ) {
    Serial.println("Trame recu == Trame envoi");
  } else {
      Serial.print("Trame recue : ");
    for (size_t i = 0; i < 12; i++)
    {
      Serial.print(TrameRecep[i]);
    }
  }
  Serial.println("");
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

void  Send_Trame(char typeCapt, uint16_t valeurCapt, char typeRequest)
//------------------------------------------------------------------------
//------------------------------------------------------------------------
{
	int  digit, n;		char CheckSum, digAsc;

  TrameEnvoi[5] = typeRequest;	// le champ "type" : type de requete

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
