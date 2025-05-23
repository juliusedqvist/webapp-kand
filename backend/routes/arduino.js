// routes/arduino.js
const express = require('express');
const router = express.Router();
const { sendBroadcastCommand, sendToArduino } = require('../services/arduinoController');

const position_reference = {
  B1_pickup: [[7200, 1], [47000, 2], [3842, 0], [70000, 2], ["RESET", 0], ["RESET", 1]],
  A1_leave: [[20000, 1], [4000, 0], [30000, 2], ["RESET", 0]],
  B2_pickup: [[7150, 1], [39723, 0], [10000, 2]],
  B3_pickup: [[10000, 2]],
  B4_pickup: [[10000, 1], [4000, 0], ["RESET", 0], ["RESET", 1]],
  RESET: [["RESET", 0], ["RESET", 1], ["RESET", 2]],
  STOP: "STOP",
  RESUME: "RESUME",
  // REQUEST_POS: [["REQUEST_POS", 0], ["REQUEST_POS", 1], ["REQUEST_POS", 2]]
  REQUEST_POS: "REQUEST_POS"
}

router.post('/command', async (req, res) => {
  const { id, command } = req.body;
  console.log(req.body);

  if (!command || typeof command !== 'string') {
    return res.status(400).json({ error: 'Command must be a non-empty string' });
  }

  try {
    const commands = position_reference[command];
    let response = ""; // <-- use let so we can reassign

    console.log(commands);
    if (Array.isArray(commands)) {
      const responses = [];
      for (const cmd of commands) {
        console.log(cmd);
        const resData = await sendToArduino(cmd[1], cmd[0]);
        responses.push({ id: cmd[1], command: cmd[0], response: resData });
        if (resData.includes("fuck")) {
          break;
        }
      }
      response = responses;
    } else if (typeof commands === 'string') {
      const broadcastResponses = await sendBroadcastCommand(commands);
      response = broadcastResponses.map((res, i) => ({
        id: i,
        command: commands,
        response: res
      }));
    } else {
      throw new Error(`Invalid command format for key: ${command}`);
    }
    res.json({ status: 'success', sent: command, received: response });
    console.log(res.json);
  } catch (err) {
    console.log(err);
    res.status(500).json({ error: 'Failed to send command', details: err.message });
  }
});

module.exports = router;

