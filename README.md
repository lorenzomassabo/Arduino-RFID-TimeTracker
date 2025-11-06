# MES 5G – Sistema di Timbratura con Arduino Nano 33 IoT

## Progetto realizzato dagli Studenti della classe 5^G 2024/2025 Dell'Istituto Natta Deambrosis
## Descrizione del progetto
Questo progetto realizza un sistema di timbratura per il controllo degli orari di presenza dei dipendenti.  
Tramite un badge RFID ogni utente può registrare l’ingresso e l’uscita.  
I dati vengono inviati in tempo reale a un Foglio Google Sheets tramite una richiesta HTTP POST.  
Il sistema può essere utilizzato in aziende, scuole o laboratori.

## Obiettivi del sistema
- Identificare univocamente ogni dipendente tramite UID RFID
- Registrare automaticamente l’orario
- Trasmettere i dati in rete via Wi-Fi
- Archiviare le timbrature in cloud per una facile consultazione

## Componenti utilizzati
- Arduino Nano 33 IoT
- Lettore RFID 13.56MHz (RC522 o compatibile)
- Connessione Wi-Fi
- Google Sheets + Google Apps Script per la gestione server-side

## Funzionamento generale
1. Il dipendente avvicina il proprio badge RFID al lettore
2. Arduino legge l’UID del tag
3. Viene determinato automaticamente se l’azione è:
   - ingresso
   - uscita
4. Arduino invia i dati al server in formato JSON tramite richiesta HTTP POST
5. Google Apps Script riceve le informazioni e le registra nel foglio Google
6. I dati sono immediatamente disponibili per consultazione

## Comunicazione e architettura
| Livello | Tecnologia |
|--------|------------|
| Hardware | Arduino Nano 33 IoT + RFID |
| Connessione | Wi-Fi |
| Protocollo | HTTP POST |
| Backend | Google Apps Script |
| Database | Foglio Google Sheets |

## Dati trasmessi
Il sistema invia al server i seguenti parametri:

| Campo | Descrizione |
|------|-------------|
| UID | Identificativo badge RFID |
| Stato | ingresso oppure uscita |
| Timestamp | generato dal server |
| Dispositivo | nome postazione configurabile |

Esempio di payload:
```json
{
  "id": "A1 B2 C3 D4",
  "stato": "ingresso",
  "nome_dispositivo": "MES01"
}
