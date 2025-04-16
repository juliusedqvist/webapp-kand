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

      This is where the commands will be
      
      </div>
    </div>
    <div class="log"
      style="top: 0%; left: 2.3%;">

      This is where the log will be
      
      </div>
  </div>
  
</template>




<script setup>
function trigger(button) {
  console.log(`${button} clicked`)
}
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
  background-color: yellow;
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
  background-color: rgb(55, 0, 255);
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
.log{
  position: relative;
  background-color: #730049;
  width: 40.2em;
  height: 6em;
  font-size: 1vw;
  padding: 0.2em 0.5em;

}

</style>
