// Template for communicating with the arduino
// Possibly switch with a python script

const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

// Replace with your actual port and baud rate
// This will depend on how many arduinos and the number will vary etc
//
// const port = new SerialPort({ path: '/dev/ttyUSB0', baudRate: 9600 });
// const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

// Template communication code

// parser.on('data', data => {
//   console.log('Received from Arduino:', data);
// });
//
// function sendToArduino(command) {
//   port.write(command + '\n', err => {
//     if (err) {
//       console.error('Error on write:', err.message);
//     } else {
//       console.log('Command sent:', command);
//     }
//   });
// }

function sendToArduino(command) {
  console.log(`[MOCK] Would send to Arduino: ${command}`);
}

module.exports = { sendToArduino };

