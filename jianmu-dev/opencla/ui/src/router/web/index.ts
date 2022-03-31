import { createRouter, createWebHistory, Router } from 'vue-router';

const router = createRouter({
  history: createWebHistory(),
  routes: [
    // pc端
    {
      path: '',
      name: 'web',
      redirect: '',
      component: () => import('@/layout/web/index.vue'),
      children: [
        {
          path: '',
          name: 'sign',
          component: () => import('@/views/web/sign.vue'),
        },
        {
          path: 'agreement',
          name: 'agreement',
          props: ({ query: { code, error_description, email, returnUrl } }) => ({
            code,
            errorDescription: error_description,
            email,
            returnUrl,
          }),
          component: () => import('@/views/web/agreement.vue'),
        },
        {
          path: 'check',
          name: 'check',
          props: ({ query: { email } }) => ({ email }),
          component: () => import('@/views/web/check.vue'),
        },
      ],
    },
  ],
}) as Router;

export default router;
