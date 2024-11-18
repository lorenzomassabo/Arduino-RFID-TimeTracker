//Registrazione Badge

#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>

#define SS_PIN 10      // Pin SS del modulo RFID
#define RST_PIN 9      // Pin RST del modulo RFID

// Inizializzazione RFID
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Struttura per i dati dei dipendenti
struct Dipendente {
  char rfid[12];
  char nome[20];
  char cognome[20];
};

// Numero massimo di dipendenti
const int MAX_DIPENDENTI = 10;
Dipendente dipendenti[MAX_DIPENDENTI];

// Funzioni
void registraDipendente(String rfid, String nome, String cognome);
void caricaDipendenti();
void salvaDipendenti();

void setup() {
  // Configurazione Serial
  Serial.begin(9600);
  while (!Serial);

  // Inizializzazione RFID
  SPI.begin();
  mfrc522.PCD_Init();

  // Carica i dipendenti dalla EEPROM
  caricaDipendenti();

  // Messaggio di avvio
  Serial.println("Modalità registrazione dipendenti.");
  Serial.println("Digita 'registra' per aggiungere un nuovo badge.");
}

void loop() {
  // Controlla comandi da seriale
  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();

    if (comando == "registra") {
      Serial.println("Modalità registrazione attivata. Avvicina un badge RFID.");
      while (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        delay(50);
      }

      // Ottieni UID del badge
      String rfid_uid = "";
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        rfid_uid += String(mfrc522.uid.uidByte[i], HEX);
      }
      Serial.println("Badge rilevato. UID: " + rfid_uid);

      // Richiedi nome e cognome via seriale
      Serial.println("Inserisci il nome del dipendente:");
      while (!Serial.available());
      String nome = Serial.readStringUntil('\n');
      nome.trim();

      Serial.println("Inserisci il cognome del dipendente:");
      while (!Serial.available());
      String cognome = Serial.readStringUntil('\n');
      cognome.trim();

      // Registra il nuovo dipendente
      registraDipendente(rfid_uid, nome, cognome);
      Serial.println("Dipendente registrato con successo!");
    }
  }
}

// Funzione per registrare un nuovo dipendente
void registraDipendente(String rfid, String nome, String cognome) {
  for (int i = 0; i < MAX_DIPENDENTI; i++) {
    if (strlen(dipendenti[i].rfid) == 0) { // Trova il primo slot libero
      strncpy(dipendenti[i].rfid, rfid.c_str(), sizeof(dipendenti[i].rfid));
      strncpy(dipendenti[i].nome, nome.c_str(), sizeof(dipendenti[i].nome));
      strncpy(dipendenti[i].cognome, cognome.c_str(), sizeof(dipendenti[i].cognome));
      salvaDipendenti();
      Serial.println("Nuovo dipendente aggiunto:");
      Serial.println("RFID: " + rfid);
      Serial.println("Nome: " + nome);
      Serial.println("Cognome: " + cognome);
      return;
    }
  }
  Serial.println("Errore: Database dipendenti pieno!");
}

// Funzione per caricare i dipendenti dalla EEPROM
void caricaDipendenti() {
  for (int i = 0; i < MAX_DIPENDENTI; i++) {
    EEPROM.get(i * sizeof(Dipendente), dipendenti[i]);
  }
  Serial.println("Database dipendenti caricato.");
}

// Funzione per salvare i dipendenti nella EEPROM
void salvaDipendenti() {
  for (int i = 0; i < MAX_DIPENDENTI; i++) {
    EEPROM.put(i * sizeof(Dipendente), dipendenti[i]);
  }
  Serial.println("Database dipendenti salvato.");
}
