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

function sendBroadcastCommand(command) {
  return new Promise((resolve, reject) => {
    const responses = {};
    const expectedIds = Object.keys(idToPort);
    let receivedCount = 0;

    if (expectedIds.length === 0) {
      return resolve([]);
    }

    for (const id of expectedIds) {
      const port = idToPort[id];
      const parser = portToParser[port.path];

      if (!port.writable || !parser) {
        console.warn(`Skipping ID ${id}: port not writable or parser missing`);
        continue;
      }

      const onData = (data) => {
        const trimmed = data.trim();
        console.log(`Response from ID ${id}: ${trimmed}`);

        // Avoid multiple triggers
        parser.off('data', onData);

        responses[id] = trimmed;
        receivedCount++;

        if (receivedCount === expectedIds.length) {
          // Sort responses by numeric ID order
          const ordered = expectedIds
            .sort((a, b) => a - b)
            .map((id) => responses[id]);
          resolve(ordered);
        }
      };

      parser.on('data', onData);

      port.write(command + '\n', (err) => {
        if (err) {
          parser.off('data', onData);
          console.error(`Error writing to ID ${id}:`, err.message);
          responses[id] = `Error: ${err.message}`;
          receivedCount++;

          if (receivedCount === expectedIds.length) {
            const ordered = expectedIds
              .sort((a, b) => a - b)
              .map((id) => responses[id]);
            resolve(ordered);
          }
        } else {
          console.log(`Sent to ID ${id}: ${command}`);
        }
      });
    }
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
      
      if (trimmed === 'done' || trimmed.includes('error')) {
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
  sendBroadcastCommand
};
