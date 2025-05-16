// routes/arduino.js
const express = require('express');
const router = express.Router();
const { sendToArduino } = require('../services/arduinoController');

const position_reference = {
  B1_pickup: [10000, 5000],
  RESET: "RESET"
}

router.post('/command', async (req, res) => {
  const { id, command } = req.body;

  if (!command || typeof command !== 'string') {
    return res.status(400).json({ error: 'Command must be a non-empty string' });
  }

  try {
    const commands = position_reference[command];

    if (Array.isArray(commands)) {
      for (const cmd of commands) {
        console.log(cmd)
        await sendToArduino(1, cmd);
      }
    } else if (typeof commands === 'string') {
      await sendToArduino(1, commands);
    } else {
      throw new Error(`Invalid command format for key: ${command}`);
    }
    res.json({ status: 'success', sent: command, arduinoResponse: response });
  } catch (err) {
    res.status(500).json({ error: 'alskjdfFailed to send command', details: err.message });
  }
});

module.exports = router;

