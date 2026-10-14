const SensorModel = require("../models/sensorModel");
const wsService = require("../services/websocketService");

const isNum = (v) => typeof v === "number" && Number.isFinite(v);

const SensorController = {
  // ---- Data Reception ----

  handleNanoDataHttp: (req, res) => {
    const { x, y, z } = req.body || {};

    if (!isNum(x) || !isNum(y) || !isNum(z)) {
      return res
        .status(400)
        .json({ error: "x, y et z doivent être des nombres" });
    }

    SensorModel.insertNanoData(x, y, z, function (err) {
      if (err) return res.status(500).json({ error: err.message });

      wsService.broadcast({ type: "nano", payload: { x, y, z } });
      res.status(200).json({ status: "success" });
    });
  },

  // Reçoit UNE ligne JSON : {"temperature":22.5,"humidity":48.0}
  handleEsp32DataTcp: (line) => {
    let data;
    try {
      data = JSON.parse(line);
    } catch {
      return console.error("JSON invalide (ESP32) :", line);
    }

    const { temperature, humidity } = data;
    if (!isNum(temperature) || !isNum(humidity)) {
      return console.error("Données ESP32 invalides :", line);
    }

    SensorModel.insertEspData(temperature, humidity, (err) => {
      if (err) return console.error("Erreur insertion ESP32 :", err.message);

      wsService.broadcast({
        type: "esp32",
        payload: { temperature, humidity },
      });
    });
  },

  // ---- Data Retrieval ----

  getNanoData: (req, res) => {
    SensorModel.getNanoData(20, (err, rows) => {
      if (err) return res.status(500).json({ error: err.message });
      res.json(rows.reverse());
    });
  },

  getEspData: (req, res) => {
    SensorModel.getEspData(20, (err, rows) => {
      if (err) return res.status(500).json({ error: err.message });
      res.json({ data: rows.reverse() });
    });
  },
};

module.exports = SensorController;
