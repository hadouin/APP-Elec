#include <Arduino.h>
#include <stdio.h>

#include "readTemperature.h"
#include "readCardio.h"
#include "readCO2.h"

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

void   Send_Trame(char typeCapt, int valeurCapt);
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
//---Temperature---
	int valeur_Temp;
	valeur_Temp = getTemperature();
	Send_Trame(CAPT_TEMP, valeur_Temp);
		// Lire la trame de réponse
	//Recep_Trame();
		// analyser la trame reçue	
		// afficher la trame reçue sur le terminal de Energia
	//Analyse_Trame();
delay(2000);
//---Humidité---
	int valeur_Hum;
	valeur_Hum = getHumidite();
	Send_Trame(CAPT_HUM, valeur_Hum);
	//Recep_Trame();
	//Analyse_Trame();
delay(2000);
//---Cardiaque---
	int valeur_cardio;
	valeur_cardio = getCardio();
	Send_Trame(CAPT_CARDIO, valeur_cardio);
	//Recep_Trame();
	//Analyse_Trame();
delay(2000);
//---C02---
	int valeur_C02;
	valeur_C02 = getCO2();
delay(2000);
//---tVOC---
	int valeur_tVOC;
	valeur_tVOC = gettVOC();
	// TODO: analyse when button pressed on PA7 or PC7 // Temporisation de 2 à 10 secondes
delay(2000);
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
