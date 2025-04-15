<template>
  <div class="robot-ui">
    <div class="svg-wrapper">
      <img src="@/assets/Chambers.svg" class="robot-svg" />

      <!-- Overlay buttons, absolutely positioned inside wrapper -->
      <button
        class="chamber-btn"
        style="top: 25%; left: 40%;"
        @click="trigger('Chamber A')"
      >
        Chamber A
      </button>
    </div>
  </div>
</template>


<script setup>
function trigger(chamber) {
  console.log(`${chamber} clicked`)
}
</script>

<style scoped>
.robot-ui {
  width: 100%;
  height: 100%;
  display: flex;
  flex-direction: column;
  align-items: flex-start; /* Align to the left */
  justify-content: flex-start; /* Align to the top */
  margin: 0;
  padding: 0;
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

.chamber-btn {
  position: absolute;
  transform: translate(-50%, -50%);
  z-index: 10;
  padding: 6px 10px;
  font-size: 12px;
  background-color: #004073;
  color: white;
  border: none;
  border-radius: 6px;
  cursor: pointer;
}
</style>
