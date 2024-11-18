function doPost(e) {
  // Recupera i dati dal payload
  var params = JSON.parse(e.postData.contents);
  var uid = params.uid;
  var nome = params.nome;
  var cognome = params.cognome;

  // Apri il foglio di calcolo e seleziona il primo foglio
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();

  // Aggiungi una nuova riga
  sheet.appendRow([uid, nome, cognome]);

  return ContentService.createTextOutput("Dati aggiunti con successo.");
}
