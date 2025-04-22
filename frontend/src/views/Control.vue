<template>
  <div class="column-ui">
    <div class="row-ui">
      <div class="svg-wrapper">
        <img src="@/assets/Chambers.svg" class="robot-svg" />

        <!-- Chamber buttons -->
        <button
          v-for="i in 24"
          :key="i"
          class="chamber-btn"
          :style="{
            top: i <= 12 ? '44.9%' : '50.7%', // First row top, second row lower
            left: `${6.3 + ((i - 1) % 12) * (66 / 11)}%`, // 12 buttons across a ~50% span
            transform: 'translate(-50%, -50%)'
          }"
          @click="trigger(`A${i}`)"
        >
          A{{ i }}
        </button>
        <button
          v-for="i in 24"
          :key="i"
          class="chamber-btn"
          :style="{
            top: i <= 12 ? '23.1%' : '29%', // First row top, second row lower
            left: `${30 + ((i - 1) % 12) * (66 / 11)}%`, // 12 buttons across a ~50% span
            transform: 'translate(-50%, -50%)'
          }"
          @click="trigger(`B${i}`)"
        >
          B{{ i }}
        </button>
        
        <button
          class="chamber-btn"
          style="top: 23.1%; left: 4.5%; transform: translate(-50%, -50%);"
          @click="trigger('C1')"
        >
          C1
        </button>
        <button
          class="chamber-btn"
          style="top: 29%; left: 4.5%; transform: translate(-50%, -50%);"
          @click="trigger('C2')"
        >
          C2
        </button>
        <button
          class="chamber-btn"
          style="top: 5.2%; left: 10.8%; transform: translate(-50%, -50%);"
          @click="trigger('P')"
        >
          P
        </button>
      </div>
      <div class="commands"
      style="top: 80%; left: 60%;">

      
      
      </div>
      <div class="next-action-panel">
      <div class="panel-header">Next action</div>
      <div class="action-display">{{ nextAction || '—' }}</div>

      <button class="panel-btn" @click="clearNextAction">CLEAR</button>
      <button class="panel-btn" @click="setNextActionFrom('A')">PICK FROM A</button>
      <button class="panel-btn" @click="setNextActionTo('A')">PLACE IN A</button>
      <button class="panel-btn" @click="setNextActionFrom('B')">PICK FROM B</button>
      <button class="panel-btn" @click="setNextActionTo('B')">PLACE IN B</button>
      </div>
    </div>
    <div class="log" style="top: 0%; left: 2.25%;">
      <div class="log-header">
  <span>Log</span>
  <button class="clear-log-btn" @click="confirmClearLog">Clear</button></div>
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

const logs = ref([])
const logBox = ref(null)

const nextAction = ref(null)
function setNextActionFrom(source) {
  if (triggeredCommands.value.length > 0) {
    const last = triggeredCommands.value.at(-1)
    nextAction.value = `${last} ⇒ ${source}`
  }
}

function setNextActionTo(target) {
  if (triggeredCommands.value.length > 0) {
    const last = triggeredCommands.value.at(-1)
    nextAction.value = `${last} ⇒ ${target}`
  }
}

function clearNextAction() {
  nextAction.value = null
}

function trigger(button) {
  const now = new Date()
  const time = `${now.toLocaleDateString("sv-SE")} ${now.toLocaleTimeString("sv-SE")}`
  const type = getRandomType() // Replace or set to 'info' based on real logic
  const message = `[${time}] ${button} clicked`

  logs.value.push({ message, type })
  console.log(`[${type.toUpperCase()}] ${message}`)
}

function getRandomType() {
  const types = ['info', 'warning', 'error']
  return types[Math.floor(Math.random() * types.length)]
}

function confirmClearLog() {
  if (confirm("Are you sure you want to clear the log?")) {
    logs.value = []
  }
}

// Scroll to bottom on log change
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
  align-items: flex-start; /* Align to the left */
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
  transform: translate(-50%, -50%) scale(1.15);
  /*box-shadow: 0px 4px 8px #3ba7ff; /* Blue glow */
  color: rgb(255, 255, 255);
}

.commands{
  position: relative;
  background-color: #00731b;
  width: 3em;
  height: 3em;
  font-size: 1vw;

}
.log {
  position: relative;
  background-color: #f7f7f7;
  width: 60em;
  height: 11em;
  font-size: 1vw;
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

.next-action-panel {
  position: relative;
  width: 10em;
  background-color: #fbfbfb;
  border: 4px solid #004073;
  color: #fbfbfb;
  padding: 0.2em;
  border-radius: 0.5em;
  display: flex;
  flex-direction: column;
  align-items: center;
  font-family: sans-serif;
  gap: 0.4em;
}

.panel-header {
  font-size: 1.2em;
  font-weight: bold;
}

.action-display {
  background-color: #fbfbfb;
  color: black;
  width: 100%;
  padding: 0.5em;
  font-size: 1.2em;
  text-align: center;
  border-radius: 0.3em;
}

.panel-btn {
  width: 100%;
  background-color: white;
  color: black;
  border: 2px solid black;
  font-weight: bold;
  padding: 0.5em;
  border-radius: 0.3em;
  cursor: pointer;
  transition: all 0.2s ease;
}

.panel-btn:hover {
  background-color: #ddeeff;
}





</style>
