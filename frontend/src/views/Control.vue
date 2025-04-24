<template>
  <div class="column-ui">
    <div class="row-ui">
      <div class="svg-wrapper">
        <img src="@/assets/Chambers.svg" class="robot-svg" />

        <!-- Chamber buttons A -->
        <button
          v-for="i in 24"
          :key="'A' + i"
          class="chamber-btn"
          :style="{
            top: i <= 12 ? '44.9%' : '50.7%',
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
            top: i <= 12 ? '23.1%' : '29%',
            left: `${30 + ((i - 1) % 12) * (66 / 11)}%`,
            transform: 'translate(-50%, -50%)'
          }"
          @click="handleChamberClick(`B${i}`)"
        >
          B{{ i }}
        </button>

        <!-- Additional chamber-like buttons -->
        <button class="chamber-btn" style="top: 23.1%; left: 4.5%; transform: translate(-50%, -50%);" @click="handleChamberClick('C1')">C1</button>
        <button class="chamber-btn" style="top: 29%; left: 4.5%; transform: translate(-50%, -50%);" @click="handleChamberClick('C2')">C2</button>
        <button class="chamber-btn" style="top: 5.2%; left: 10.8%; transform: translate(-50%, -50%);" @click="handleChamberClick('P')">P</button>
      </div>

      <!-- Panels -->
      <div class="panel-stack">
        <div class="next-action-panel">
          <div class="panel-header">Commands</div>
          <div class="btn-display">
            <button class="panel-btn" style="background-color: #ff0000; color: white; height: 5.5vw; font-size: 2.7vw;" @click="logCommand('STOP')">STOP</button>
            <button class="panel-btn" style="background-color: #f9cb00; color: white; height: 3vw;" @click="logCommand('RESET')">RESET</button>
            <button class="panel-btn" style="background-color: #4cd000; color: white; height: 3vw;" @click="logCommand('RESUME')">RESUME</button>
            <button class="panel-btn" style="background-color: #004073; color: white; height: 3vw;" @click="runNextAction">RUN NEXT</button>
          </div>
        </div>

        <div class="next-action-panel">
          <div class="panel-header">Next action</div>
          <div class="action-display">{{ nextAction || '—' }}</div>
          <div class="btn-display">
            <button class="panel-btn" @click="clearNextAction">CLEAR</button>
            <button class="panel-btn" @click="handleChamberClick('AUTO A')">AUTO A</button>
            <button class="panel-btn" @click="handleChamberClick('AUTO B')">AUTO B</button>
          </div>
        </div>
      </div>
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
</template>

<script setup>
import { ref, watch, nextTick } from 'vue'
import axios from 'axios';

const logs = ref([])
const logBox = ref(null)

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
  // No log generation here
}

function clearNextAction() {
  nextAction.value = null
  selectedChambers.value = []
}

async function runNextAction() {
  if (nextAction.value) {
    log(`Running: ${nextAction.value}`, 'info');

    try {
      const res = await axios.post('http://localhost:3000/api/arduino/command', {
        command: "TOGGLE" // nextAction.value
      });

    } catch (err) {
      const msg = err.response?.data?.error || err.message || 'Unknown error';
    }

    clearNextAction()
  }
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
  console.log(`[${type.toUpperCase()}] ${message}`)
}

function confirmClearLog() {
  if (confirm("Are you sure you want to clear the log?")) {
    logs.value = []
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
  width: 100%;
  height: 100%;
  display: flex;
  flex-direction: column;
  align-items:flex-start; /* Align to the left */
  justify-content: flex-start; /* Align to the top */
  margin: 0;
  padding: 0;
  /*background-color: yellow;*/
}

.row-ui {
  width: 100%;
  height: 75%;
  display: flex;
  flex-direction: row;
  align-items: flex-start; /* Align to the left */
  justify-content: flex-start; /* Align to the top */
  margin: 0;
  padding: 0;
  /*background-color: rgb(55, 0, 255);*/
}

.robot-svg {
  position: absolute; /* Absolute position relative to .svg-wrapper */
  top: 0; /* Adjust this to place SVG as needed */
  left: 0; /* Align left of the SVG with the left of the container */
  width: 100%; /* Ensure it fills the container width */
  height: auto; /* Maintain aspect ratio */
  object-fit: contain; /* Keeps the SVG inside the container without stretching */
  pointer-events: none;
  /*background-color: red;  /* Added for debugging */
}


.svg-wrapper {
  position: relative; /* Make sure this is relative to provide context for absolute positioning */
  top: 0.5rem;
  left: 0.5rem;
  width: 65%; /* Adjust width as needed */
  aspect-ratio: 1 / 1; /* Keeps the container a square */
  margin-top: 0; /* Ensure no margin at the top */
  /*background-color: blue; /* Added for debugging */

}
/*
.chamber-btn {
  position: absolute;
  z-index: 10;
  padding: 0.7% 1.2%;
  font-size:100%;
  background-color: #fbfbfb;
  color: rgb(0, 0, 0);
  border: 2px solid black;
  border-radius: 10px;
  cursor: pointer;
}*/
.chamber-btn {
  position: absolute;
  z-index: 10;
  font-size: 1.2vw; /* Scales with viewport width */
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
  /*box-shadow: 0px 0px 8px #3ba7ff; /* Blue glow */
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
  /*top:65%;*/
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
  padding-top: 2vw;
  flex-direction: column;
  gap: 2vw; /* Optional spacing between the two panels */
  position: relative;
  top: 0%;
  left: 2vw; /* Adjust as needed */
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
  height: 3.2vw;
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
  transform: scale(1.05) /*translate(-80%, -20%)*/;
  /*box-shadow: 0px 4px 8px #3ba7ff; /* Blue glow */
  color: rgb(255, 255, 255);
}

.log {
  position: relative;
  background-color: #f7f7f7;
  width: 60vw;
  height: 11vw;
  font-size: 1.1vw;
  padding: 0;
  border-radius: 0.5em;
  overflow: hidden;
  box-shadow: 0px 4px 12px rgba(0, 0, 0, 0.15);
  font-family: monospace;
  display: flex;
  flex-direction: column;
}

.log-header {
  color: white;
  background-color: #004073;
  font-weight: bold;
  border: 0.05em solid #004073;

  padding: 0.1em 0.8em;
  font-size: 1.1em;
  flex-shrink: 0;
}

.log-box {
  flex-grow: 1;
  overflow-y: scroll;
  padding: 0.5em;
}

/* Prevent horizontal scroll */
.log-box::-webkit-scrollbar:horizontal {
  display: none;
}


.log-entry {
  font-weight: 500;
  margin-bottom: 0.3em;
  /*white-space: nowrap; /* Prevents wrapping, optional */
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

/* Firefox support */


/* WebKit-based browsers (Chrome, Edge, Safari) */
.log::-webkit-scrollbar {
  width: 10px;
}

.log::-webkit-scrollbar-track {
  background: #e0e0e0;
  border-radius: 10px;
}

.log::-webkit-scrollbar-thumb {
  background: #004073;
  border-radius: 10px;
  border: 2px solid #e0e0e0;
}

.log::-webkit-scrollbar-thumb:hover {
  background: #3ba7ff;
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
