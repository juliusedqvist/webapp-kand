<script setup>
</script>

<template>
  <div id="app">
    <nav>
      <router-link to="/">Home</router-link>
      <router-link to="/control">Control-center</router-link>
      <router-link to="/controlold">Control-center (old)</router-link>
      <router-link to="/docs">Documentation</router-link>
    </nav>
    <main>
      <router-view />
    </main>
  </div>
</template>

<style>
html, body, #app {
  margin: 0;
  padding: 0;
  height: 100%;
  width: 100%;
  overflow: hidden; /* disables all scrollbars */
}

#app {
  display: flex;
  flex-direction: column;
  height: 100%;
}

nav {
  background-color: #004073;
  color: white;
  padding: 10px;
  display: flex;
  gap: 10px;
  flex-shrink: 0;
}

main {
  flex: 1;
  padding: 0; /* Remove unnecessary padding */
  display: flex;
  flex-direction: column;
  justify-content: flex-start; /* Align to top of the main */
  align-items: flex-start; /* Align to the left of the main */
  overflow: hidden; /* ensures no scroll in content */
}
</style>
