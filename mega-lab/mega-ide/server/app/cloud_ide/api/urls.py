from fastapi import APIRouter

from server.app.cloud_ide.api.ide_api import ide_api
from server.app.cloud_ide.api.ide_environment_api import ide_environment_api
from server.app.cloud_ide.api.ide_image_api import ide_image_api
from server.app.cloud_ide.api.ide_registry_api import ide_registry_api
from server.app.cloud_ide.api.system_info import system_info_api

cloud_ide_router = APIRouter()
cloud_ide_router.include_router(ide_registry_api, prefix='/ide_registry', tags=['IDE镜像源管理'])
cloud_ide_router.include_router(ide_image_api, prefix='/ide_image', tags=['IDE镜像管理'])
cloud_ide_router.include_router(ide_environment_api, prefix='/ide_environment', tags=["IDE环境管理"])
cloud_ide_router.include_router(ide_api, prefix='/ide', tags=["IDE管理"])
cloud_ide_router.include_router(system_info_api, prefix='/system_info', tags=["系统管理"])
