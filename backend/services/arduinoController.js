const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

let port;
let parser;
let isConnected = false;

// Create and open the serial port ONCE
port = new SerialPort({ path: '/dev/ttyUSB0', baudRate: 9600 });

port.on('open', () => {
    isConnected = true;
    console.log('Connected to Arduino');

    parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

    parser.on('data', data => {
        console.log('Received from Arduino:', data.trim());
    });
});

port.on('error', (err) => {
    console.error('Error connecting to Arduino:', err.message);
});

// Exported function: just writes to the existing port
function sendToArduino(command) {
    if (!isConnected || !port.writable) {
        return console.error('Port not ready');
    }

    console.log(command);

    port.write(command + '\n', (err) => {
        if (err) {
            console.error('Error writing to Arduino:', err.message);
        } else {
            console.log('Command sent to Arduino:', command);
        }
    });
}

module.exports = {
    sendToArduino,
};
