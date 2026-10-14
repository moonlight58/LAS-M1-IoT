const db = require("../config/database");

// SQLite stocke CURRENT_TIMESTAMP en UTC ("2026-10-14 12:00:00", sans fuseau).
// strftime le convertit en ISO 8601 avec "Z" pour que le navigateur
// l'affiche correctement en heure locale (comme dans l'exemple du sujet).
const ISO_TS = `strftime('%Y-%m-%dT%H:%M:%SZ', timestamp)`;

const SensorModel = {
  // Insertion of Arduino Nano data
  insertNanoData: (x, y, z, callback) => {
    const sql = `INSERT INTO nano_data (x, y, z) VALUES (?, ?, ?)`;
    db.run(sql, [x, y, z], callback);
  },

  // Insertion of ESP32 data
  insertEspData: (temperature, humidity, callback) => {
    const sql = `INSERT INTO esp_data (temperature, humidity) VALUES (?, ?)`;
    db.run(sql, [temperature, humidity], callback);
  },

  // Nano data retrieval
  getNanoData: (limit, callback) => {
    db.all(
      `SELECT ${ISO_TS} AS datetime, x, y, z FROM nano_data ORDER BY id DESC LIMIT ?`,
      [limit],
      callback,
    );
  },

  // ESP32 data retrieval
  getEspData: (limit, callback) => {
    db.all(
      `SELECT ${ISO_TS} AS datetime, temperature, humidity FROM esp_data ORDER BY id DESC LIMIT ?`,
      [limit],
      callback,
    );
  },
};

module.exports = SensorModel;
