import React from 'react';
import {
  BasicLayoutProps,
  Settings as LayoutSettings,
} from '@ant-design/pro-layout';
import Keycloak from 'keycloak-js';
import {RequestConfig} from 'umi';

import {ReactKeycloakProvider, useKeycloak} from '@react-keycloak/web';

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
    initOptions: {onLoad: 'login-required', checkLoginIframe: false, enableLogging: true},
  }, container);
}


export const request: RequestConfig = {
  timeout: 10000,
  errorConfig: {},
  middlewares: [],
  // @ts-ignore
  requestInterceptors: [(url, options) => {
    let headers = {'token': keycloak.token}
    return {
      url: `${url}`,
      options: {
        ...options, headers: headers, interceptors: true
      },
    };
  }],
  responseInterceptors: [],
};
