// routes/arduino.js
const express = require('express');
const router = express.Router();
const { sendToArduino } = require('../services/arduinoController');

const position_reference = {
  B1_pickup: [[10000, 1], [5000, 0]],
  A1_leave: [[20000, 1], [5000, 0]],
  RESET: [["RESET", 0], ["RESET", 1]],
  STOP: "STOP",
  RESUME: "RESUME"
}

router.post('/command', async (req, res) => {
  const { id, command } = req.body;
  console.log(req.body);

  if (!command || typeof command !== 'string') {
    return res.status(400).json({ error: 'Command must be a non-empty string' });
  }

  try {
    const commands = position_reference[command];

    console.log(commands)
    if (Array.isArray(commands)) {
      for (const cmd of commands) {
        console.log(cmd)
        await sendToArduino(cmd[1], cmd[0]);
      }
    } else if (typeof commands === 'string') {
      sendToArduino(0, commands);
      sendToArduino(1, commands);
      // sendToArduino(2, commands);
    } else {
      throw new Error(`Invalid command format for key: ${command}`);
    }
    res.json({ status: 'success', sent: command });
  } catch (err) {
    console.log(err)
    res.status(500).json({ error: 'Failed to send command', details: err.message });
  }
});

module.exports = router;

