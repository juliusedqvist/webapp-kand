import { createRouter, createWebHistory } from 'vue-router';
import Home from '@/views/Home.vue';
import RobotControl from '@/views/RobotControl.vue';
import Documentation from '@/views/Documentation.vue';
import NotFound from '@/views/NotFound.vue';

const routes = [
  { path: '/', component: Home },
  { path: '/robot', component: RobotControl },
  { path: '/docs', component: Documentation },
  { path: '/:pathMatch(.*)*', component: NotFound }  // 404-sida
];

const router = createRouter({
  history: createWebHistory(),
  routes
});

export default router;

