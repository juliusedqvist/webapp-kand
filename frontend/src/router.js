import { createRouter, createWebHistory } from 'vue-router';
import Home from '@/views/Home.vue';
import ControlOld from '@/views/ControlOld.vue';
import Control from '@/views/Control.vue';
import Docs from '@/views/Docs.vue';
import NotFound from '@/views/NotFound.vue';

const routes = [
  { path: '/', component: Home },
  { path: '/control', component: Control},
  { path: '/controlold', component: ControlOld},
  { path: '/docs', component: Docs },
  { path: '/:pathMatch(.*)*', component: NotFound }  // 404-sida
];

const router = createRouter({
  history: createWebHistory(),
  routes
});

export default router;

