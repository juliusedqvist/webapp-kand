const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

// Initialize the serial port with correct path and baud rate
const port = new SerialPort({ path: '/dev/ttyUSB0', baudRate: 9600 });

// Set up parser to read lines ending with newline character
const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

// Event listener for incoming data from Arduino
parser.on('data', data => {
  console.log('Received from Arduino:', data.trim());
});

// Function to send commands to the Arduino
function sendToArduino(command) {
  port.write(command + '\n', err => {
    if (err) {
      console.error('Error on write:', err.message);
    } else {
      console.log('Command sent:', command);
    }
  });
}

module.exports = { sendToArduino };

