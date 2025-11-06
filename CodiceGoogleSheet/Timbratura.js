function doPost(e) {
  const spreadsheetId = "ID_FOGLIO";  // Sostituisci con l'ID del tuo foglio
  const ss = SpreadsheetApp.openById(spreadsheetId);
  const foglioDipendenti = ss.getSheetByName("Dipendenti");
  const foglioOrari = ss.getSheetByName("Orari");

  const dati = JSON.parse(e.postData.contents);
  const azione = dati.azione; // "registra" o "registraOrario"
  
  if (azione === "registra") {
    const badgeID = dati.badgeID;
    const nomeCognome = dati.nomeCognome;

    // Verifica se il badge esiste già
    const righe = foglioDipendenti.getDataRange().getValues();
    for (let i = 1; i < righe.length; i++) {
      if (righe[i][0] === badgeID) {
        return ContentService.createTextOutput("Badge già registrato").setMimeType(ContentService.MimeType.TEXT);
      }
    }

    // Aggiungi il nuovo dipendente
    foglioDipendenti.appendRow([badgeID, nomeCognome.split(" ")[0], nomeCognome.split(" ")[1]]);
    return ContentService.createTextOutput("Dipendente registrato").setMimeType(ContentService.MimeType.TEXT);

  } else if (azione === "registraOrario") {
    const badgeID = dati.badgeID;
    const tipo = dati.tipo;
    const nomeCognome = dati.nomeCognome; // Aggiungi il nome completo

    const data = new Date();
    const dataFormattata = Utilities.formatDate(data, Session.getScriptTimeZone(), "yyyy-MM-dd");
    const oraFormattata = Utilities.formatDate(data, Session.getScriptTimeZone(), "HH:mm:ss");

    // Aggiungi al foglio Orari anche il nome e cognome
    foglioOrari.appendRow([dataFormattata, oraFormattata, badgeID, nomeCognome, tipo]);
    return ContentService.createTextOutput("Orario registrato").setMimeType(ContentService.MimeType.TEXT);
  }
  
  return ContentService.createTextOutput("Azione non valida").setMimeType(ContentService.MimeType.TEXT);
}
