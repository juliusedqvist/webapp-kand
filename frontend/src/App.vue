<script setup>
</script>

<template>
  <main>
    <nav>
      <router-link to="/">Home</router-link>
      <router-link to="/control">Control-center</router-link>
      <router-link to="/docs">Documentation</router-link>
    </nav>
    <router-view></router-view>
  </main>
</template>

<style scoped>

@media (min-width: 1024px) {

  header .wrapper {
    display: flex;
    place-items: flex-start;
    flex-wrap: wrap;
  }
}
</style>
