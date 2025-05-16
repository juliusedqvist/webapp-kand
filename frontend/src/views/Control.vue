<template>
  <div class="row-ui">
    <!-- Chambers, buttons, and log -->
    <div class="column-ui">
      <div class="svg-wrapper">
        <img src="@/assets/Chambers.svg" class="robot-svg" />

        <!-- Chamber buttons A -->
        <button
          v-for="i in 24"
          :key="'A' + i"
          class="chamber-btn"
          :style="{
            top: i <= 12 ? '82.6%' : '93.3%',
            left: `${6.3 + ((i - 1) % 12) * (66 / 11)}%`,
            transform: 'translate(-50%, -50%)'
          }"
          @click="handleChamberClick(`A${i}`)"
        >
          A{{ i }}
        </button>

        <!-- Chamber buttons B -->
        <button
          v-for="i in 24"
          :key="'B' + i"
          class="chamber-btn"
          :style="{
            top: i <= 12 ? '42.5%' : '53.4%',
            left: `${30 + ((i - 1) % 12) * (66 / 11)}%`,
            transform: 'translate(-50%, -50%)'
          }"
          @click="handleChamberClick(`B${i}`)"
        >
          B{{ i }}
        </button>

        <!-- Additional chamber-like buttons -->
        <button class="chamber-btn" style="top: 42.5%; left: 4.5%; transform: translate(-50%, -50%);" @click="handleChamberClick('C1')">C1</button>
        <button class="chamber-btn" style="top: 53.4%; left: 4.5%; transform: translate(-50%, -50%);" @click="handleChamberClick('C2')">C2</button>
        <button class="chamber-btn" style="top: 9.6%; left: 10.8%; transform: translate(-50%, -50%);" @click="handleChamberClick('P')">P</button>
      </div>

        <!-- Log Section -->
      <div class="log" style="top: 0%; left: 2.25%;">
        <div class="log-header">
          <span>Log</span>
          <button class="clear-log-btn" @click="confirmClearLog">Clear</button>
        </div>
        <div class="log-box" ref="logBox">
          <div
            v-for="(entry, index) in logs"
            :key="index"
            class="log-entry"
            :class="entry.type"
          >
            {{ entry.message }}
          </div>
        </div>
      </div>
      
    </div>
    <!-- Command and action panels -->
    <div class="panel-stack">
      <div class="next-action-panel">
        <div class="panel-header">Commands</div>
        <div class="btn-display">
          <button class="panel-btn" style="background-color: #ff0000; color: white; height: 5.5vw; font-size: 2.7vw;" @click="logCommand('STOP')">STOP</button>
          <button class="panel-btn" style="background-color: #f9cb00; color: white;" @click="logCommand('RESET')">RESET</button>
          <button class="panel-btn" style="background-color: #4cd000; color: white;" @click="logCommand('RESUME')">RESUME</button>
          <button class="panel-btn" style="background-color: #004073; color: white;" @click="runNextAction">RUN NEXT</button>
        </div>
      </div>
      <div class="next-action-panel">
        <div class="panel-header">Next action</div>
        <div class="action-display">{{ nextAction || '—' }}</div>
        <div class="btn-display">
          <button class="panel-btn" @click="clearNextAction">CLEAR</button>
        </div>
      </div>
      

      </div>
    
  </div>
</template>

<script setup>
import { ref, watch, nextTick } from 'vue'
import axios from 'axios';

const logs = ref([])
const storedLogs = localStorage.getItem('logs')
if (storedLogs) {
  try {
    logs.value = JSON.parse(storedLogs)
  } catch (e) {
    console.warn('Could not parse stored logs:', e)
    logs.value = []
  }
}


const logBox = ref(null)
const maxLogEntries = 2000


const nextAction = ref(null)
const selectedChambers = ref([])

function handleChamberClick(label) {
  if (selectedChambers.value.length === 0) {
    selectedChambers.value.push(label)
    nextAction.value = `${label}`
  } else if (selectedChambers.value.length === 1) {
    selectedChambers.value.push(label)
    nextAction.value = `${selectedChambers.value[0]} ⇒ ${selectedChambers.value[1]}`
  }
}

function clearNextAction() {
  nextAction.value = null
  selectedChambers.value = []
}

async function runNextAction() {
  const tempActionValue = nextAction.value;
  clearNextAction();

  if (!tempActionValue) return;

  log(`Running: ${tempActionValue}`, 'info');

  const commandArray = tempActionValue.split(" ");
  const fromLocation = commandArray[0];
  const toLocation = commandArray.length > 1 ? commandArray[commandArray.length - 1] : null;

  try {
    if (fromLocation === toLocation) {
      await sendCommand(`${toLocation}_leave`);
      await sendCommand(`RESET`); 
    } else if (!toLocation) {
      await sendCommand(`${fromLocation}_pickup`);
      await sendCommand(`RESET`);
    } else {
      await sendCommand(`${fromLocation}_pickup`);
      await sendCommand(`${toLocation}_leave`);
      await sendCommand(`RESET`);
    }
  } catch (err) {
    const msg = err.response?.data?.error || err.message || 'Unknown error';
    console.error("Command error:", msg);
  }
}

// Reusable helper for sending commands
async function sendCommand(command) {
  console.log("Sending command:", command);
  await axios.post('http://localhost:3000/api/arduino/command', { command }, timeout: 10000);
}


async function logCommand(command) {
  log(command, 'info')
  try {
      const res = await axios.post('http://localhost:3000/api/arduino/command', {
        command: nextAction.value
      });

    } catch (err) {
      const msg = err.response?.data?.error || err.message || 'Unknown error';
    }
}

function log(message, type = 'info') {
  const now = new Date()
  const time = `${now.toLocaleDateString("sv-SE")} ${now.toLocaleTimeString("sv-SE")}`
  logs.value.push({ message: `[${time}] ${message}`, type })

  // Trim if over max
  if (logs.value.length > maxLogEntries) {
    logs.value.splice(0, logs.value.length - maxLogEntries)
  }

  // Persist
  localStorage.setItem('logs', JSON.stringify(logs.value))

  console.log(`[${type.toUpperCase()}] ${message}`)
}


function confirmClearLog() {
  if (confirm("Are you sure you want to clear the log?")) {
    logs.value = []
    localStorage.removeItem('logs')
  }
}


watch(
  logs,
  async () => {
    await nextTick()
    if (logBox.value) {
      logBox.value.scrollTop = logBox.value.scrollHeight
    }
  },
  { deep: true }
)
</script>



<style scoped>
.column-ui {
  width: 70%;
  height: 100%;
  display: flex;
  flex-direction: column;
  align-items:flex-start; 
  justify-content: flex-start; 
  margin: 0;
  padding: 1em;  
}

.row-ui {
  width: 100%;
  height: 100%;
  display: flex;
  flex-direction: row;
  align-items: flex-start; 
  justify-content: flex-start; 
  margin: 0;
  padding: 0;
}

.robot-svg {
  position: absolute;
  top: 0; 
  left: 0; 
  width: 100%; 
  height: auto; 
  object-fit: contain;
  pointer-events: none;
}


.svg-wrapper {
  position: relative; 
  width: 100%;
  aspect-ratio: 1.84 / 1; 
  margin-top: 0; 

}

.chamber-btn {
  position: absolute;
  z-index: 10;
  font-size: 1.2vw; 
  width: 3vw;
  height: 2.7vw;
  background-color: #fbfbfb;
  color: rgb(0, 0, 0);
  border: 0.1em solid black;
  border-radius: 0.7em;
  cursor: pointer;
  transition: all 0.15s ease-in-out;
  display: flex;
  align-items: center;
  justify-content: center;
  text-align: center;
}

.chamber-btn:hover {
  background-color: #004073;
  color: rgb(255, 255, 255);
}

.commands{
  position: relative;
  background-color: #00731b;
  width: 3em;
  height: 3em;
  font-size: 1vw;

}

.next-action-panel {
  position: relative;
  width: 15vw;
  height: auto;
  background-color: #004073;
  border: 0.1em solid #004073;
  color: #fbfbfb;
  padding: 0.2em;
  border-radius: 0.5em;
  display: flex;
  flex-direction: column;
  align-items: center;
  font-family: Helvetica;
  gap: 0.4em;
}

.panel-stack {
  display: flex;
  padding-top: 7vw;
  flex-direction: column;
  gap: 2vw; 
  position: relative;
  top: 0%;
  left: 2vw; 
}


.panel-header {
  font-size: 1.6vw;
  margin-bottom: -0.4em;
  margin-top: -0.3em;
  width: 100%;
  height: 10%;
  font-family: Helvetica;
  text-align: center;
  
}

.action-display {
  background-color: #fbfbfb;
  color: black;
  width: 100%;
  margin-top: 0em;
  padding: 0.4vw;
  font-size: 1.5vw;
  text-align: center;
  border: 0.em solid #004073;
  border-radius: 0.3em;
}

.btn-display {
  background-color: #fbfbfb;
  color: black;
  width: 100%;
  margin-top: 0em;
  padding: 0.5vw;
  text-align: center;
  border: 0em solid #fbfbfb;
  border-radius: 0.3em;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 0.5vw;
}

.panel-btn {
  width: 100%;
  height: 3.4vw;
  font-weight: bold;
  background-color: #fbfbfb;
  color: black;
  font-size: 1.1vw;
  color: rgb(0, 0, 0);
  border: 0.2vw solid black;
  border-radius: 0.6vw;
  cursor: pointer;
  transition: all 0.15s ease-in-out;
  display: flex;
  align-items: center;
  justify-content: center;
  text-align: center;
}

.panel-btn:hover{
  background-color: #004073;
  transform: scale(1.05);
  color: rgb(255, 255, 255);
}

.log {
  position: relative;
  background-color: #f7f7f7;
  width: 60vw;
  height: 25vh;
  max-height: 20vw;
  margin-left: 0.15em;
  font-size: 1.1vw;
  margin-top: 0.5em;
  border-radius: 0.5em;
  overflow: hidden;
  font-family: monospace;
  display: flex;
  flex-direction: column;
}

.log-header {
  color: white;
  background-color: #004073;
  font-weight: bold;
  padding: 0.1em 0.8em;
  font-size: 1.1em;
  flex-shrink: 0;
}

.log-box {
  flex-grow: 1;
  overflow-y: scroll;
  padding: 0.5em;
  border-left: 0.25vw solid #004073;
  border-right: 0.25vw solid #004073;
  border-bottom: 0.25vw solid #004073;
  border-bottom-right-radius: 0.5em;
  border-bottom-left-radius: 0.5em;
}

.log-box::-webkit-scrollbar:horizontal {
  display: none;
}


.log-entry {
  font-weight: 500;
  margin-bottom: 0.3em;
}

.log-entry.info {
  color: #004073;
}

.log-entry.warning {
  color: #b8860b;
}

.log-entry.error {
  color: #b00020;
}

.log-box::-webkit-scrollbar {
  width: 0.8em;
  
}

.log-box::-webkit-scrollbar-track {
  background: #dedede;
  border-bottom-right-radius: 0.25em;
}

.log-box::-webkit-scrollbar-thumb {
  background: #004073;
  border-radius: 10px;
  border: 2px solid #e0e0e0;
}

.log-box::-webkit-scrollbar-thumb:hover {
  background: #0389f7;
}

.clear-log-btn {
  background-color: transparent;
  border: 1px solid white;
  color: white;
  padding: 0.15em 0.5em;
  margin-left: 1em;
  font-size: 0.9em;
  cursor: pointer;
  border-radius: 0.3em;
  transition: background-color 0.2s, color 0.2s;
}

.clear-log-btn:hover {
  background-color: white;
  color: #004073;
}

</style>
