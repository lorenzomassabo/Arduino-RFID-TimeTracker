//registra il l'orario di ingresso e di uscita e lo carica su Google Sheets

void loop() {
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

  // Determina l'azione (Ingresso/Uscita)
  String azione = "Ingresso"; // Puoi modificare o chiedere tramite Serial
  Serial.println("Azione: " + azione);

  // Invia i dati al webhook
  String jsonPayload = "{\"action\":\"registra_timbro\",\"uid\":\"" + rfid_uid + "\",\"azione\":\"" + azione + "\"}";

  client.beginRequest();
  client.post(webhookPath);
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", jsonPayload.length());
  client.beginBody();
  client.print(jsonPayload);
  client.endRequest();

  int statusCode = client.responseStatusCode();
  if (statusCode == 200) {
    Serial.println("Timbro registrato con successo.");
  } else {
    Serial.println("Errore: " + String(statusCode));
  }
}
