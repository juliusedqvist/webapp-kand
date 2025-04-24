<template>
  <div>
    <button @click="sendHelloWorld('1')">Toggle LED 1</button>
  </div>
  <div>
    <button @click="sendHelloWorld('2')">Toggle LED 2</button>
  </div>
</template>

<script setup>
import { ref } from 'vue';
import axios from 'axios';

// Function to send "Hello World" message to the API when button is clicked
async function sendHelloWorld(toggle) {
  try {
    const response = await axios.post('http://localhost:3000/api/arduino/command', {
      command: toggle
    });
    console.log('Response:', response.data);
  } catch (error) {
    console.error('Error:', error.message || 'Unknown error');
  }
}
</script>

<style scoped>
button {
  padding: 10px 20px;
  font-size: 16px;
  background-color: #4CAF50;
  border: none;
  color: white;
  border-radius: 4px;
  cursor: pointer;
}

button:hover {
  background-color: #45a049;
}
</style>

