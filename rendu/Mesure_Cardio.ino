#define PORT PD_0

int valeurPrecedente = 0;
int lastTime = 0;
int valeurSeuil = 150;

void setup() {
  Serial.begin(9600);
  Serial.println("init");
}

void loop() {
  int i = 0;
  int log[9];
  int somme = 0;
  Serial.print('[');
  do {
    int readValue = analogRead(PORT);
    int detectTime = millis();
    if (readValue > valeurSeuil && readValue > valeurPrecedente){
      if (detectTime > lastTime + 100) {
        int avgBPM = 60 * 1000 / (detectTime - lastTime);
        log[i] = avgBPM;
        Serial.print('=');
        somme = somme + avgBPM;

        i++;
      }
      lastTime = detectTime;
    }
    valeurPrecedente = readValue;
  } while (i<11);
  Serial.print("] ");
  Serial.println(somme / 10);
  for (size_t i = 0; i < 11; i++)
  {
    Serial.print(log[i]);
    Serial.print(' ');
  }
  Serial.println("");
}