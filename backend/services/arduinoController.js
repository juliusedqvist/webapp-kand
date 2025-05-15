const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

let ports = {};
let idToPort = {};

const candidates = ['/dev/ttyUSB0', '/dev/ttyUSB1']; // or use serial/by-id for stability

for (const path of candidates) {
  const port = new SerialPort({ path, baudRate: 9600 });

  port.on('open', () => {
    console.log(`Port opened: ${path}`);
    const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

    parser.on('data', data => {
      const trimmed = data.trim();
      console.log(`Data from ${path}: ${trimmed}`);

      // If it reports ID like "ID:1"
      if (trimmed.startsWith('ID:')) {
        const id = trimmed.split(':')[1];

        // Store mapping
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


// Exported function: just writes to the existing port


function sendToArduino(id, command) {
  const port = idToPort[id];

  if (!port || !port.writable) {
    console.error(`No writable port for ID ${id}`);
    return;
  }

  port.write(command + '\n', (err) => {
    if (err) {
      console.error(`Error writing to Arduino ID ${id}:`, err.message);
    } else {
      console.log(`Command sent to Arduino ID ${id}:`, command);
    }
  });
}



module.exports = {
    sendToArduino,
};
