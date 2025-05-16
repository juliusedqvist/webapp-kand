// routes/arduino.js
const express = require('express');
const router = express.Router();
const { sendToArduino } = require('../services/arduinoController');

router.post('/command', (req, res) => {
  const { command } = req.body;

  if (!command || typeof command !== 'string') {
    return res.status(400).json({ error: 'Command must be a non-empty string' });
  }

  sendToArduino(10000, 1);
  res.json({ status: 'success', sent: command });
});

module.exports = router;

