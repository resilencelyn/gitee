import uvicorn
from fastapi import FastAPI

from fastapi.middleware.cors import CORSMiddleware
from fastapi.staticfiles import StaticFiles
from fastapi.middleware.gzip import GZipMiddleware
from starlette.exceptions import HTTPException as StarletteHTTPException
from server.app.routers import api_router
from server.framework.core.handlers import not_found_handlers
from server.framework.core.middlewares import AuthorizerMiddleware
from server.framework.core.settings import settings


def show_banner():
    with open('./asserts/banner.txt') as f:
        print(f.read())


def build_app():
    if settings.env == 'PROD':
        application = FastAPI(docs_url=None, redoc_url=None)
    else:
        application = FastAPI()

    return application


def inject_middleware(application: FastAPI):
    origins = [
        '*'
    ]
    application.add_middleware(
        CORSMiddleware,
        allow_origins=origins,
        allow_credentials=True,
        allow_methods=["*"],
        allow_headers=["*"],
    )
    application.add_middleware(AuthorizerMiddleware)
    application.add_middleware(GZipMiddleware, minimum_size=1000)

    application.add_exception_handler(StarletteHTTPException, not_found_handlers)


def init_router(application: FastAPI):
    application.include_router(api_router, prefix='/api')
    application.mount("/", StaticFiles(directory="public"), name="public")


show_banner()
app = build_app()
inject_middleware(app)
init_router(app)

if __name__ == '__main__':
    uvicorn.run(app="main:app", host=settings.app_host, port=settings.app_port, reload=True, debug=True)
