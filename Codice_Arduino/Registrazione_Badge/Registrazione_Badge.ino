//registrazione badge e invio dati su google sheet
#include <SPI.h>
#include <MFRC522.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

#define SS_PIN 10      // Pin SS del modulo RFID
#define RST_PIN 9      // Pin RST del modulo RFID

// Wi-Fi
const char* ssid = "Tuo_SSID";         // Rete Wi-Fi
const char* password = "Tua_Password"; // Password Wi-Fi

// Webhook Google Apps Script
const char* server = "script.google.com";
const String webhookPath = "/macros/s/TUO_WEBHOOK_ID/exec"; // Sostituisci con il tuo webhook

// Inizializzazione RFID
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Client HTTP
WiFiClient wifiClient;
HttpClient client = HttpClient(wifiClient, server, 443);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Inizializzazione Wi-Fi
  Serial.print("Connettendo a Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConnesso al Wi-Fi!");

  // Inizializzazione RFID
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Sistema pronto. Avvicina un badge per registrarlo.");
}

void loop() {
  // Controlla se un badge RFID è stato avvicinato
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }

  // Ottieni UID del badge
  String rfid_uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    rfid_uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println("Badge rilevato. UID: " + rfid_uid);

  // Richiedi nome e cognome tramite seriale
  Serial.println("Inserisci il nome del dipendente:");
  while (!Serial.available());
  String nome = Serial.readStringUntil('\n');
  nome.trim();

  Serial.println("Inserisci il cognome del dipendente:");
  while (!Serial.available());
  String cognome = Serial.readStringUntil('\n');
  cognome.trim();

  // Invia i dati al webhook
  String jsonPayload = "{\"action\":\"registra_dipendente\",\"uid\":\"" + rfid_uid + "\",\"nome\":\"" + nome + "\",\"cognome\":\"" + cognome + "\"}";

  Serial.println("Invio dati a Google Sheets...");
  client.beginRequest();
  client.post(webhookPath);
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", jsonPayload.length());
  client.beginBody();
  client.print(jsonPayload);
  client.endRequest();

  // Leggi la risposta
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  if (statusCode == 200) {
    Serial.println("Dipendente registrato con successo: " + response);
  } else {
    Serial.println("Errore nell'invio: " + String(statusCode));
  }
}
