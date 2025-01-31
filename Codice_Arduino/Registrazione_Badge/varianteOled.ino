#include <SPI.h>
#include <MFRC522.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definisci le credenziali Wi-Fi.
const char* ssid = "SSID";
const char* password = "PASSWD";

// Pin RFID
#define RST_PIN 9
#define SS_PIN 10
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Display OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// URL per inviare i dati a Google Sheets
const char* googleScriptUrl = "script.google.com";  // Sostituisci con il tuo URL del Web App
const int httpsPort = 443;

// Array per tenere traccia dello stato (entrata/uscita) dei dipendenti
char badgeIDs[16][20];   // Array per memorizzare fino a 10 ID di badge
bool statoDipendenti[16]; // Array per tenere traccia dello stato di ingresso/uscita (true = ingresso, false = uscita)
int numDipendenti = 1;  // Numero attuale di dipendenti registrati

// Variabili per gestire i badge RFID
char idBadge[20]; // Array per l'ID del badge
char nomeDipendente[30]; // Array per il nome del dipendente
unsigned long lastRead = 0;

// Funzione per disegnare l'icona del Wi-Fi

void setup() {
  // Inizializzazione del display OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Inizializzazione...");
  display.display();

  // Avvia la comunicazione seriale
  Serial.begin(9600);

  // Connessione Wi-Fi
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Connessione WiFi...");
  display.display();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connessione WiFi in corso...");
  }
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("WiFi connesso!");
  display.display();
  Serial.println("WiFi connesso!");

  // Inizializza il modulo RFID
  SPI.begin();
  mfrc522.PCD_Init();
  display.clearDisplay();
  display.setTextSize(2); // Dimensione del testo normale per "Ingresso" o "Uscita"
  display.setCursor(20,0);
  display.println("Sistema");
  display.setCursor(20,20);
  display.println("pronto!");
  display.display();
  Serial.println("Sistema di presenza pronto!");
}

void loop() {
  // Controlla se un tag RFID è stato letto
  if (millis() - lastRead > 2000) {
    if (mfrc522.PICC_IsNewCardPresent()) {
      if (mfrc522.PICC_ReadCardSerial()) {
        // Ottieni l'ID del badge
        snprintf(idBadge, sizeof(idBadge), "%02X%02X%02X%02X",
                 mfrc522.uid.uidByte[0], mfrc522.uid.uidByte[1],
                 mfrc522.uid.uidByte[2], mfrc522.uid.uidByte[3]);

        // Aggiungi logica per associare l'ID del badge a un nome
        if (getNomeDipendente(idBadge, nomeDipendente)) {
          Serial.print("Dipendente: ");
          Serial.println(nomeDipendente);
          display.clearDisplay();
          display.setCursor(25, 20);
          display.println("Dipendente:");
          display.setCursor(20, 30);
          display.println(nomeDipendente);
          display.display();

          // Trova l'indice dell'ID del badge nell'array badgeIDs
          int index = findBadgeIndex(idBadge);
          if (index == -1) {
            // Se il badge non è stato registrato, lo registriamo
            if (numDipendenti < 16) {
              strncpy(badgeIDs[numDipendenti], idBadge, sizeof(idBadge));
              statoDipendenti[numDipendenti] = true;  // Ingresso
              numDipendenti++;
            }
            // Registra l'ingresso
            registraPresenza(nomeDipendente, idBadge, "ingresso");
          } else {
            // Se il badge è stato già registrato, controlliamo lo stato
            const char* tipo = (statoDipendenti[index]) ? "uscita" : "ingresso";
            // Registra l'uscita o ingresso
            registraPresenza(nomeDipendente, idBadge, tipo);
            // Cambia lo stato del dipendente
            statoDipendenti[index] = !statoDipendenti[index];
          }
        } else {
          Serial.println("Badge non riconosciuto.");
          display.clearDisplay();
          display.setCursor(20, 0);
          display.println("Badge non riconosciuto.");
          display.display();
        }

        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1(); // Ferma la crittografia
        lastRead = millis();
      }
    }
  }
}
bool getNomeDipendente(char* id, char* nome) {
  // Esegui il mapping tra ID e nome (puoi sostituire questa logica con un array o un altro metodo)
  if (strcmp(id, "5A5D0A3") == 0) {
    strcpy(nome, "Lorenzo Massabo");
    return true;
  }
  // Aggiungi altri ID qui
  return false;
}

// Funzione per trovare l'indice di un badgeID nell'array
int findBadgeIndex(char* badgeID) {
  for (int i = 0; i < numDipendenti; i++) {
    if (strcmp(badgeIDs[i], badgeID) == 0) {
      return i;  // Restituisce l'indice del badge trovato
    }
  }
  return -1;  // Restituisce -1 se il badge non è trovato
}

void registraPresenza(char* nome, char* badgeID, const char* tipo) {
  // Prepara i dati da inviare come JSON
  String data = "{\"azione\":\"registraOrario\",\"badgeID\":\"" + String(badgeID) + "\",\"tipo\":\"" + String(tipo) + "\",\"nomeCognome\":\"" + String(nome) + "\"}";

  // Crea una connessione SSL con il server usando WiFiClient
  WiFiClient client;
  
  // Connetti al server usando SSL
  if (client.connectSSL(googleScriptUrl, httpsPort)) {
    Serial.println("Connessione SSL riuscita!");
    display.clearDisplay();
    display.setCursor(20,0);
    display.println("Connessione SSL riuscita!");
    display.display();

    // Crea una richiesta POST
    String postRequest = "POST /macros/s/AKfycbxsywjz2Nsl9ItJldMhc9owjhwO2z4xz4ZHI8U5HLnshwXhX6CloWTx6GnswVyCeHEOGg/exec HTTP/1.1\r\n";  // Sostituisci con il tuo ID
    postRequest += "Host: " + String(googleScriptUrl) + "\r\n";
    postRequest += "Content-Type: application/json\r\n";
    postRequest += "Content-Length: " + String(data.length()) + "\r\n";
    postRequest += "Connection: close\r\n\r\n";
    postRequest += data; // I dati JSON da inviare

    // Invia la richiesta
    client.print(postRequest);
    Serial.println("Dati inviati:");
    display.clearDisplay();
    display.setCursor(20,0);
    display.println("Dati inviati:");
    display.display();

    // Leggi la risposta
    String response = "";
    while (client.available()) {
      response = client.readStringUntil('\r');
      Serial.print(response);
    }

    Serial.println("\nRisposta ricevuta!");
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2); // Dimensione del testo normale per "Ingresso" o "Uscita"
    display.setCursor(15, 20); // Posiziona la scritta sotto "OK!"
    display.println(tipo); // Mostra "ingresso" o "uscita"
    display.display();
    delay(2000); // Mantiene la scritta per 2 secondi
    client.stop();  // Chiudi la connessione SSL

    display.clearDisplay();
    display.setTextSize(2); // Dimensione del testo
    display.setCursor(20,0);
    display.println("Sistema");
    display.setCursor(20,20);
    display.println("pronto!");
    display.display();

  } else {
    Serial.println("Connessione SSL fallita");
    display.clearDisplay();
    display.setCursor(20,0);
    display.println("Connessione SSL fallita");   
    display.setCursor(20,20); 
    display.println("fallita");
    display.display();

    display.clearDisplay();
    display.setTextSize(2); // Dimensione del testo
    display.setCursor(20,0);
    display.println("Sistema");
    display.setCursor(20,20);
    display.println("pronto!");
    display.display();
  }
}
