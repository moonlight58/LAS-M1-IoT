const express = require("express");
const http = require("http");
const net = require("net");
const path = require("path");

// Importation de nos modules
const sensorRoutes = require("./routes/sensorRoutes");
const sensorController = require("./controllers/sensorController");
const wsService = require("./services/websocketService");

const app = express();
const server = http.createServer(app);

// Middlewares
app.use(express.json());
app.use(express.static(path.join(__dirname, "public")));

// Initialisation des WebSockets
wsService.init(server);

// Montage des routes HTTP
app.use("/", sensorRoutes);

// Création du serveur TCP (ESP32)
const tcpServer = net.createServer((socket) => {
  socket.on("data", (data) => {
    const payload = data.toString().trim();
    sensorController.handleEsp32DataTcp(payload);
  });

  socket.on("error", (err) => console.error("Erreur TCP:", err.message));
});

// Lancement des serveurs
tcpServer.listen(8080, () =>
  console.log("Serveur TCP (ESP32) sur le port 8080"),
);
server.listen(3001, () => console.log("Serveur HTTP & WS sur le port 3000"));
