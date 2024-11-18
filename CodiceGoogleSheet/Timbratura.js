//registrazione dipendenti nel database e registrazione orario ingresso / uscita

function doPost(e) {
  var params = JSON.parse(e.postData.contents);
  var action = params.action;

  // Apri il foglio di calcolo
  var sheet = SpreadsheetApp.getActiveSpreadsheet();

  if (action === "registra_dipendente") {
    // Dati per registrare un dipendente
    var uid = params.uid;
    var nome = params.nome;
    var cognome = params.cognome;

    // Aggiungi alla scheda "Lista Dipendenti"
    var dipendentiSheet = sheet.getSheetByName("Lista Dipendenti");
    dipendentiSheet.appendRow([uid, nome, cognome]);

    return ContentService.createTextOutput("Dipendente registrato con successo.");
  }

  if (action === "registra_timbro") {
    // Dati per registrare un timbro
    var uid = params.uid;
    var nome = params.nome;
    var cognome = params.cognome;
    var azione = params.azione;

    // Genera timestamp completo
    var timestamp = Utilities.formatDate(new Date(), "GMT+1", "yyyy-MM-dd HH:mm:ss");

    // Aggiungi alla scheda "Timbrature"
    var timbratureSheet = sheet.getSheetByName("Timbrature");
    timbratureSheet.appendRow([uid, nome, cognome, azione, timestamp]);

    return ContentService.createTextOutput("Timbro registrato con successo.");
  }

  return ContentService.createTextOutput("Azione non riconosciuta.");
}
