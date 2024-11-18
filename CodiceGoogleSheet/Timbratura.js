function doPost(e) {
  var params = JSON.parse(e.postData.contents);
  var action = params.action;

  // Apri il foglio di calcolo
  var sheet = SpreadsheetApp.getActiveSpreadsheet();

  if (action === "registra_dipendente") {
    // **Registrazione Dipendente**
    var uid = params.uid;
    var nome = params.nome;
    var cognome = params.cognome;

    // Verifica se UID esiste già nella scheda "Lista Dipendenti"
    var dipendentiSheet = sheet.getSheetByName("Lista Dipendenti");
    var data = dipendentiSheet.getDataRange().getValues();

    for (var i = 1; i < data.length; i++) {
      if (data[i][0] == uid) {
        return ContentService.createTextOutput("Errore: UID già registrato.");
      }
    }

    // Aggiungi il dipendente alla scheda "Lista Dipendenti"
    dipendentiSheet.appendRow([uid, nome, cognome]);

    return ContentService.createTextOutput("Dipendente registrato con successo.");
  }

  if (action === "registra_timbro") {
    // **Registrazione Timbro**
    var uid = params.uid;
    var azione = params.azione;

    // Trova nome e cognome del dipendente dalla scheda "Lista Dipendenti"
    var dipendentiSheet = sheet.getSheetByName("Lista Dipendenti");
    var data = dipendentiSheet.getDataRange().getValues();
    var nome = "";
    var cognome = "";

    for (var i = 1; i < data.length; i++) {
      if (data[i][0] == uid) {
        nome = data[i][1];
        cognome = data[i][2];
        break;
      }
    }

    if (nome === "" || cognome === "") {
      return ContentService.createTextOutput("Errore: UID non trovato.");
    }

    // Genera timestamp
    var timestamp = Utilities.formatDate(new Date(), "GMT+1", "yyyy-MM-dd HH:mm:ss");

    // Aggiungi i dati alla scheda "Timbrature"
    var timbratureSheet = sheet.getSheetByName("Timbrature");
    timbratureSheet.appendRow([uid, nome, cognome, azione, timestamp]);

    return ContentService.createTextOutput("Timbro registrato con successo.");
  }

  return ContentService.createTextOutput("Errore: Azione non riconosciuta.");
}
