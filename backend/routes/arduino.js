// routes/arduino.js
const express = require('express');
const router = express.Router();
const { sendToArduino } = require('../services/arduinoController');

router.post('/command', async (req, res) => {
  const { id, command } = req.body;

  if (!command || typeof command !== 'string') {
    return res.status(400).json({ error: 'Command must be a non-empty string' });
  }

  try {
    const response = await sendToArduino(1, 10000);
    res.json({ status: 'success', sent: command, arduinoResponse: response });
  } catch (err) {
    res.status(500).json({ error: 'Failed to send command', details: err.message });
  }
});

module.exports = router;

