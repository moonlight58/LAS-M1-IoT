const WebSocket = require("ws");

let wss;

const WebsocketService = {
  init: (server) => {
    wss = new WebSocket.Server({ server });
    wss.on("connection", (ws) => {
      console.log("Nouveau client WebSocket connecté");
    });
  },

  broadcast: (data) => {
    if (!wss) return;
    wss.clients.forEach((client) => {
      if (client.readyState === WebSocket.OPEN) {
        client.send(JSON.stringify(data));
      }
    });
  },
};

module.exports = WebsocketService;
