// routes/arduino.js
const express = require('express');
const router = express.Router();
const { sendToArduino } = require('../services/arduinoController');

router.get('/test', (req, res) => {
  res.send('Hello, world!');
});

// e.g., POST /api/arduino/led/on
router.post('/led/:state', (req, res) => {
  const state = req.params.state;
  if (state !== 'on' && state !== 'off') {
    return res.status(400).send('Invalid state');
  }

  sendToArduino(state === 'on' ? 'LED_ON' : 'LED_OFF');
  res.json({ status: 'success', command: state });
});

router.post('/command', (req, res) => {
  const { command } = req.body;

  if (!command || typeof command !== 'string') {
    return res.status(400).json({ error: 'Command must be a non-empty string' });
  }

  sendToArduino(command);
  res.json({ status: 'success', sent: command });
});

module.exports = router;

