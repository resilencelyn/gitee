import os
import re
import typing

from starlette.authentication import AuthenticationError
from starlette.requests import HTTPConnection
from starlette.responses import JSONResponse, Response
from starlette.types import ASGIApp, Receive, Scope, Send

from keycloak import KeycloakOpenID

from server.framework.core.settings import settings


class Authenticator:
    key_pattern: typing.Optional[str] = None
    keycloak_openid = KeycloakOpenID(server_url=settings.keycloak_url,
                                     client_id=settings.keycloak_client_id,
                                     realm_name=settings.keycloak_realm_name,
                                     client_secret_key="secret")

    def authenticate(self, conn: HTTPConnection) -> bool:
        if "token" not in conn.headers:
            raise AuthenticationError("请求未认证")

        token = conn.headers["token"]
        self.keycloak_openid.userinfo(token)

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
        if settings.env == 'PROD':
            if '/api' in scope["path"]:
                conn = HTTPConnection(scope)
                try:
                    auth_result = self.authenticate(conn)
                except AuthenticationError as e:
                    response = self.on_error(conn, e)
                    await response(scope, receive, send)
                    return

                if auth_result:
                    await self.app(scope, receive, send)
                    return
                else:
                    response = self.on_error(conn, None)
                    await response(scope, receive, send)
                    return
            else:
                await self.app(scope, receive, send)
                return
        else:
            await self.app(scope, receive, send)
            return

    def default_on_error(self, conn: HTTPConnection, e: Exception) -> Response:
        return JSONResponse({"detail": str(e)}, status_code=401)
