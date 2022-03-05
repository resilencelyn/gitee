from fastapi import APIRouter

from server.app.cloud_ide.api.urls import cloud_ide_router

api_router = APIRouter()
api_router.include_router(cloud_ide_router, prefix="/cloud_ide")
