// services/arduinoController.js
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

let ports = {};
let idToPort = {};
let portToParser = {};

const candidates = ['/dev/ttyUSB0', '/dev/ttyUSB1', '/dev/ttyUSB2']; // Or use serial/by-id for stability

for (const path of candidates) {
  const port = new SerialPort({ path, baudRate: 9600 });

  port.on('open', () => {
    console.log(`Port opened: ${path}`);
    const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));
    portToParser[path] = parser;

    parser.on('data', data => {
      const trimmed = data.trim();
      console.log(`Data from ${path}: ${trimmed}`);

      // If it reports ID like "ID:1"
      if (trimmed.startsWith('ID:')) {
        const id = trimmed.split(':')[1];
        idToPort[id] = port;
        ports[path] = port;
        console.log(`Mapped Arduino ID ${id} to ${path}`);
      }
    });
  });

  port.on('error', (err) => {
    console.error(`Error on ${path}:`, err.message);
  });
}

function sendToArduino(id, command) {
  return new Promise((resolve, reject) => {
    const port = idToPort[id];

    if (!port || !port.writable) {
      return reject(new Error(`No writable port for ID ${id}`));
    }

    const parser = portToParser[port.path];
    if (!parser) {
      return reject(new Error(`No parser found for port ${port.path}`));
    }

    const onData = (data) => {
      const trimmed = data.trim().toLowerCase();
      console.log(`Response from Arduino ID ${id}: ${trimmed}`);

      if (trimmed === 'done' || trimmed === 'fuck') {
        parser.off('data', onData);
        resolve(trimmed);
      }
    };

    parser.on('data', onData);

    port.write(command + '\n', (err) => {
      if (err) {
        parser.off('data', onData);
        return reject(err);
      } else {
        console.log(`Command sent to Arduino ID ${id}: ${command}`);
      }
    });
  });
}

module.exports = {
  sendToArduino,
};
