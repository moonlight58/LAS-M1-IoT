const express = require("express");
const http = require("http");
const net = require("net");
const path = require("path");

// Importation de nos modules
const sensorRoutes = require("./routes/sensorRoutes");
const sensorController = require("./controllers/sensorController");
const wsService = require("./services/websocketService");

const HTTP_PORT = 3000;
const TCP_PORT = 8080;

const app = express();
const server = http.createServer(app);

// Middlewares
app.use(express.json());
app.use(express.static(path.join(__dirname, "public")));

// Initialisation des WebSockets
wsService.init(server);

// Montage des routes HTTP
app.use("/", sensorRoutes);

// Serveur TCP (ESP32) : un message JSON par ligne ('\n' = séparateur).
// TCP est un flux : un "data" peut contenir plusieurs lignes ou une ligne
// coupée en deux, d'où le buffer par connexion.
const tcpServer = net.createServer((socket) => {
  console.log("ESP32 connecté :", socket.remoteAddress);
  let buffer = "";

  socket.on("data", (chunk) => {
    buffer += chunk.toString();
    const lines = buffer.split("\n");
    buffer = lines.pop(); // dernier morceau, possiblement incomplet

    lines
      .map((l) => l.trim())
      .filter(Boolean)
      .forEach((l) => sensorController.handleEsp32DataTcp(l));
  });

  socket.on("close", () => console.log("ESP32 déconnecté"));
  socket.on("error", (err) => console.error("Erreur TCP:", err.message));
});

// Lancement des serveurs
tcpServer.listen(TCP_PORT, () =>
  console.log(`Serveur TCP (ESP32) sur le port ${TCP_PORT}`),
);
server.listen(HTTP_PORT, () =>
  console.log(`Serveur HTTP & WS sur le port ${HTTP_PORT}`),
);
