// routes/arduino.js
const express = require('express');
const router = express.Router();
const { sendToArduino } = require('../services/arduinoController');

const position_reference = {
  B1_pickup: 10000,
  RESET: "RESET"
}

router.post('/command', async (req, res) => {
  const { id, command } = req.body;

  if (!command || typeof command !== 'string') {
    return res.status(400).json({ error: 'Command must be a non-empty string' });
  }

  try {
    console.log(position_reference[command]);
    const response = await sendToArduino(0, position_reference[command]);
    res.json({ status: 'success', sent: command, arduinoResponse: response });
  } catch (err) {
    res.status(500).json({ error: 'Failed to send command', details: err.message });
  }
});

module.exports = router;

