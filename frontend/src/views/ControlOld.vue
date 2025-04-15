
<template>
  <div>
    <h1>Robotkontroll</h1>
    <p>Här kan du styra roboten.</p>

    <button @click="toggleLED">{{ ledOn ? 'Stäng av LED' : 'Tänd LED' }}</button>
  </div>
</template>

<script setup>
import { ref } from 'vue';

const ledOn = ref(false);

function toggleLED() {
  const state = ledOn.value ? 'off' : 'on';

  fetch(`http://localhost:3000/api/arduino/led/${state}`, {
    method: 'POST',
  })
    .then(res => res.json())
    .then(data => {
      console.log('Svar från backend:', data);
      ledOn.value = !ledOn.value;
    })
    .catch(err => {
      console.error('Fel vid POST:', err);
    });
}
</script>

<style scoped>
h1 {
  color: #ff9800;
}

button {
  padding: 10px 20px;
  margin-top: 10px;
  font-size: 16px;
  background-color: #ff9800;
  border: none;
  color: white;
  border-radius: 4px;
  cursor: pointer;
}

button:hover {
  background-color: #fb8c00;
}
</style>

