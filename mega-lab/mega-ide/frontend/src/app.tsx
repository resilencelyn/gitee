import React from 'react';
import {
  BasicLayoutProps,
  Settings as LayoutSettings,
} from '@ant-design/pro-layout';
import Keycloak from 'keycloak-js';
import { RequestConfig } from 'umi';

import { ReactKeycloakProvider, useKeycloak } from '@react-keycloak/web';

const base_domain = process.env.MEGAIDE_ENV === 'dev' ? '' : 'lixulife.com';
const keycloak = Keycloak({
  'realm': 'dev',
  'url': 'http://keycloak.lixulife.com/auth',
  'clientId': 'megalab',
},
);

export const layout = ({
  initialState,
}: {
  initialState: { settings?: LayoutSettings; };
}): BasicLayoutProps => {
  return {
    menuHeaderRender: undefined,
    ...initialState?.settings,
  };
};

export async function getInitialState() {
  return {};
}

export function rootContainer(container: any) {
  return React.createElement(ReactKeycloakProvider, {
    authClient: keycloak, autoRefreshToken: true,
    initOptions: { onLoad: 'login-required', checkLoginIframe: false, enableLogging: true },
    onEvent: (event, error) => {
      let domain = '';
      if (base_domain) {
        domain = `domain=${base_domain};`
      }
      if (event === 'onAuthSuccess') {
        document.cookie = `token=${keycloak.token};${domain}`
      } else if (event === 'onAuthLogout') {
        document.cookie = `token=null;${domain}`
      }
    }
  }, container);
}

export const request: RequestConfig = {
  timeout: 10000,
  credentials: 'include',
  errorConfig: {},
  middlewares: [],
  requestInterceptors: [(url, options) => {
    return {
      url: `${url}`,
      options: {
        ...options,
        interceptors: true
      },
    };
  }],
  responseInterceptors: [],
};
