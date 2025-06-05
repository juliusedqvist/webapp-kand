<template>
  <div class="page-container">
    <div class="content">
      <!-- <h1>Documentation for the control system</h1> -->
      <iframe
        class="pdf-viewer"
        src="Documentation.pdf"
        title="PDF Viewer">
      </iframe>
    </div>
  </div>
</template>

<script setup>
// No script needed
</script>

<style scoped>
.page-container {
  display: flex;
  justify-content: center; /* Horizontally center content */
  padding: 0.5rem;
}

.content {
  max-width: 1000px;
  width: 60vw;
  text-align: center;
}

h1 {
  color: #333;
  margin-bottom: 1rem;
}

.pdf-viewer {
  width: 100%;
  height: 90vh;
  border: none;
  box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
}
</style>
