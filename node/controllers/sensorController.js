const SensorModel = require("../models/sensorModel");
const wsService = require("../services/websocketService");

const SensorController = {
  // ---- Data Reception ----

  handleNanoDataHttp: (req, res) => {
    const { x, y, z } = req.body;

    SensorModel.insertNanoData(x, y, z, function (err) {
      if (err) return res.status(500).json({ error: err.message });

      wsService.broadcast({ type: "nano", payload: { x, y, z } });
      res.status(200).json({ status: "success" });
    });
  },

  handleEsp32DataTcp: (payload) => {
    const [temp, hum] = payload.split(",");
    if (temp && hum) {
      const temperature = parseFloat(temp);
      const humidity = parseFloat(hum);

      SensorModel.insertEspData(temperature, humidity, (err) => {
        if (err) console.error("Erreur insertion ESP32 :", err.message);
      });

      wsService.broadcast({
        type: "esp32",
        payload: { temperature, humidity },
      });
    }
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
