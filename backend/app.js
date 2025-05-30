// app.js
const express = require('express');
const cors = require('cors');

const app = express();
const port = 3000;

app.use(cors());
app.use(express.json());

const arduinoRoutes = require('./routes/arduino');
app.use('/api/arduino', arduinoRoutes);

app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}`);
});

