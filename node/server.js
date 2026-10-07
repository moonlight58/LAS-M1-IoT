const net = require('net');
const express = require('express');
const sqlite3 = require('sqlite3').verbose();
const WebSocket = require('ws');

// Ports configuration
const HTTP_PORT = 3000; // For Arduino Nano 33 IoT
const TCP_PORT = 8080;  // For ESP32

// ==========================================
// 1. HTTP Server setup (Arduino Nano 33 IoT)
// ==========================================
const app = express();
app.use(express.json());
app.post('/api/arduino-data', (req, res) => {
    const sensorData = req.body;
    console.log('Received HTTP data from Arduino:', sensorData);
    
    // TODO: Insert into SQLite database here
    
    res.status(200).json({ status: 'success', message: 'Data logged successfully' });
});

app.listen(HTTP_PORT, () => {
    console.log(`HTTP Server listening on port ${HTTP_PORT}`);
});

// ==========================================
// 2. TCP Server setup (ESP32)
// ==========================================
const tcpServer = net.createServer((socket) => {
    console.log(`ESP32 connected from ${socket.remoteAddress}:${socket.remotePort}`);

    socket.on('data', (data) => {
        const payload = data.toString().trim();
        console.log('Received TCP data from ESP32:', payload);
        
        // TODO: Parse payload and insert into SQLite database here
    });

    socket.on('end', () => {
        console.log('ESP32 disconnected');
    });

    socket.on('error', (err) => {
        console.error('TCP Socket Error:', err.message);
    });
});

tcpServer.listen(TCP_PORT, '0.0.0.0', () => {
    console.log(`TCP Server listening on port ${TCP_PORT}`);
});

// ==========================================
// 3. Database Initialization (Placeholder)
// ==========================================

const db = new sqlite3.Database('./iot_data.db', (err) => {
    if (err) {
        console.error('Error connecting to SQLite:', err.message);
    } else {
        console.log('Connected to the SQLite database.');
        initializeDatabase();
    }
});


function initializeDatabase() {
    db.serialize(() => {
        // Table for Temperature Data
        db.run(`
            CREATE TABLE IF NOT EXISTS temperature (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                value REAL NOT NULL,
                timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        `, (err) => {
            if (err) console.error("Error creating temperature table:", err.message);
        });

        // Table for Humidity Data
        db.run(`
            CREATE TABLE IF NOT EXISTS humidity (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                value REAL NOT NULL,
                timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        `, (err) => {
            if (err) console.error("Error creating humidity table:", err.message);
        });

        // Table for Accelerometer Data (X, Y, Z axes)
        db.run(`
            CREATE TABLE IF NOT EXISTS accelerometer (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                x REAL NOT NULL,
                y REAL NOT NULL,
                z REAL NOT NULL,
                timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        `, (err) => {
            if (err) console.error("Error creating accelerometer table:", err.message);
            else console.log("All database tables initialized successfully.");
        });
    });
}
