import { createRouter, createWebHistory, Router } from 'vue-router';

const router = createRouter({
  history: createWebHistory(),
  routes: [
    //  移动端
    {
      path: '',
      name: 'wap',
      component: () => import('@/layout/wap/index.vue'),
      redirect: '',
      children: [
        {
          path: '',
          name: 'sign',
          component: () => import('@/views/wap/sign.vue'),
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
          component: () => import('@/views/wap/agreement.vue'),
        },
        {
          path: 'check',
          component: () => import('@/layout/wap/index.vue'),
          children: [
            {
              path: '',
              name: 'check',
              props: ({ query: { email } }) => ({ email }),
              component: () => import('@/views/wap/check.vue'),
            },
            {
              path: '',
              name: 'result',
              props: ({ query: { email } }) => ({ email }),
              component: () => import('@/views/wap/result.vue'),
            },
          ],
        },
      ],
    },
  ],
}) as Router;

export default router;
