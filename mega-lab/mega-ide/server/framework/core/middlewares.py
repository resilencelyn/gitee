import re
import typing
from typing import Callable
import time
from starlette.authentication import AuthenticationError
from starlette.requests import HTTPConnection
from starlette.responses import JSONResponse, Response
from starlette.types import ASGIApp, Receive, Scope, Send
import logging
from keycloak import KeycloakOpenID
from fastapi import FastAPI, Request, Response

from server.framework.core.logger import logger
from server.framework.core.settings import settings
from starlette.middleware.base import BaseHTTPMiddleware


class Authenticator:
    key_pattern: typing.Optional[str] = None
    keycloak_openid = KeycloakOpenID(server_url=settings.keycloak_url,
                                     client_id=settings.keycloak_client_id,
                                     realm_name=settings.keycloak_realm_name,
                                     client_secret_key="secret")

    def authenticate(self, conn: HTTPConnection) -> bool:
        if "token" not in conn.cookies:
            raise AuthenticationError("请求未认证")

        token = conn.cookies["token"]
        userinfo = self.keycloak_openid.userinfo(token)
        conn.state.user_id = userinfo['sub']
        conn.state.user_name = userinfo['preferred_username']
        return True


class AuthorizerMiddleware(Authenticator):
    def __init__(
            self,
            app: ASGIApp,
            on_error: typing.Callable[
                [HTTPConnection, AuthenticationError], Response
            ] = None,
    ) -> None:
        self.app = app
        self.on_error: typing.Callable[
            [HTTPConnection, AuthenticationError], Response
        ] = (on_error if on_error is not None else self.default_on_error)

    async def __call__(self, scope: Scope, receive: Receive, send: Send) -> None:
        conn = HTTPConnection(scope)
        if settings.env == 'PROD':
            if '/api' in scope["path"] and scope["path"] != '/api/cloud_ide/ide/ide_auth':
                try:
                    auth_result = self.authenticate(conn)
                except AuthenticationError as e:
                    logger.error(e)
                    response = self.on_error(conn, e)
                    await response(scope, receive, send)
                    return

                if auth_result:
                    await self.app(scope, receive, send)
                    return
                else:
                    logger.info(f'用户认证失败,请求路径[{scope["path"]}]')
                    response = self.on_error(conn, None)
                    await response(scope, receive, send)
                    return
            else:
                await self.app(scope, receive, send)
                return
        else:
            conn.state.user_id = 'mock'
            conn.state.user_name = 'mock'
            await self.app(scope, receive, send)
            return

    def default_on_error(self, conn: HTTPConnection, e: Exception) -> Response:
        return JSONResponse({"detail": str(e)}, status_code=401)
