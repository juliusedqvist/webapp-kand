const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

function sendToArduino(command) {
    let port;
    let parser;
    let isConnected = false;

    port = new SerialPort({ path: '/dev/ttyUSB0', baudRate: 9600 });

    port.on('open', () => {
        isConnected = true;
        parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

        parser.on('data', data => {
            console.log('Received from Arduino:', data.trim());
        });

        console.log('Connected to Arduino');
    });
    console.log(command);
    port.write("TOGGLE" + '\n', (err) => {
        if (err) {
            return console.error('Error writing to Arduino:', err.message);
        }
        console.log('Command sent to Arduino:', command);
    });

    port.on('error', (err) => {
        console.error('Error connecting to Arduino:', err.message);
    });
}



module.exports = {
    sendToArduino
};
