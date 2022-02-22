from fastapi import APIRouter, Query

from server.framework.core.http_schemas import CommonResponseSchema
from server.framework.utils.kubernetes_manager import KubernetesManager

system_info_api = APIRouter()


@system_info_api.get("/list_node_info", name='获取单个镜像源', response_model=CommonResponseSchema)
async def list_node_info() -> CommonResponseSchema:
    kubernetes_manager = KubernetesManager()
    node_info = kubernetes_manager.list_node_info()
    return CommonResponseSchema(data=node_info)
