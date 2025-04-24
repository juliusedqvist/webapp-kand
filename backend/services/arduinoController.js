const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

let port1;
let port2;
let parser;
let isConnected = false;

// Create and open the serial port ONCE
port1 = new SerialPort({ path: '/dev/ttyUSB0', baudRate: 9600 });
port2 = new SerialPort({ path: '/dev/ttyUSB1', baudRate: 9600 });

port1.on('open', () => {
    isConnected = true;
    console.log('Connected to Arduino');

    parser = port1.pipe(new ReadlineParser({ delimiter: '\n' }));

    parser.on('data', data => {
        console.log('Received from Arduino:', data.trim());
    });
});
port2.on('open', () => {
    isConnected = true;
    console.log('Connected to Arduino');

    parser = port2.pipe(new ReadlineParser({ delimiter: '\n' }));

    parser.on('data', data => {
        console.log('Received from Arduino:', data.trim());
    });
});

port1.on('error', (err) => {
    console.error('Error connecting to Arduino:', err.message);
});
port2.on('error', (err) => {
    console.error('Error connecting to Arduino:', err.message);
});

// Exported function: just writes to the existing port
function sendToArduino(command) {

    if (command === '1') {
        if (!isConnected || !port1.writable) {
            return console.error('port1 not ready');
        }

        port1.write(command + '\n', (err) => {
            if (err) {
                console.error('Error writing to Arduino:', err.message);
            } else {
                console.log('Command sent to Arduino:', command);
            }
        });
    }
    if (command === '2') {
        if (!isConnected || !port2.writable) {
            return console.error('port2 not ready');
        }

        port2.write(command + '\n', (err) => {
            if (err) {
                console.error('Error writing to Arduino:', err.message);
            } else {
                console.log('Command sent to Arduino:', command);
            }
        });
    }
}

module.exports = {
    sendToArduino,
};
