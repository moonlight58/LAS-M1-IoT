const sqlite3 = require("sqlite3").verbose();
const path = require("path");

const dbPath = path.resolve(__dirname, "../iot_data.db");
const db = new sqlite3.Database(dbPath, (err) => {
  if (err) console.error("Err SQLite :", err.message);
  else {
    console.log("Connected to db");
    initDB();
  }
});

function initDB() {
  db.serialize(() => {
    //Données ESP32
    db.run(`CREATE TABLE IF NOT EXISTS esp_data (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            temperature REAL NOT NULL,
            humidity REAL NOT NULL,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
        )`);

    //Arduino Nano
    db.run(`CREATE TABLE IF NOT EXISTS nano_data (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            x REAL NOT NULL,
            y REAL NOT NULL,
            z REAL NOT NULL,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
        )`);
  });
}

module.exports = db;
