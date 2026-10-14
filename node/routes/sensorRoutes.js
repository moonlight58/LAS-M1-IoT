const express = require("express");
const router = express.Router();
const sensorController = require("../controllers/sensorController");

// Réception des données (Arduino Nano)
router.post("/api/arduino-data", sensorController.handleNanoDataHttp);

// API pour l'historique des graphiques
router.get("/nano_data", sensorController.getNanoData);
router.get("/esp_data", sensorController.getEspData);

module.exports = router;
