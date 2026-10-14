const db = require("../config/database");

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
      "SELECT timestamp AS datetime, x, y, z FROM nano_data ORDER BY timestamp DESC LIMIT ?",
      [limit],
      callback,
    );
  },

  // ESP32 data retrieval
  getEspData: (limit, callback) => {
    db.all(
      "SELECT timestamp AS datetime, temperature, humidity FROM esp_data ORDER BY timestamp DESC LIMIT ?",
      [limit],
      callback,
    );
  },
};

module.exports = SensorModel;
