from fastapi import APIRouter, Query

from server.framework.core.http_schemas import CommonResponseSchema
from server.framework.utils.kubernetes_manager import KubernetesManager
from server.framework.utils.prometheus_manager import PrometheusManager

system_info_api = APIRouter()


@system_info_api.get("/list_node_info", name='获取节点信息', response_model=CommonResponseSchema)
async def list_node_info() -> CommonResponseSchema:
    kubernetes_manager = KubernetesManager()
    node_info = kubernetes_manager.list_node_info()
    return CommonResponseSchema(data=node_info)


@system_info_api.get("/query_metrics", name='获取指标信息', response_model=CommonResponseSchema)
async def query_metrics(node_lists: str = Query(None, description='节点列表'),
                        metric_name: str = Query(None, description='指标名称')) -> CommonResponseSchema:
    prometheus_manager = PrometheusManager()
    if node_lists is not None:
        node_lists = node_lists.split(',')
    if metric_name in prometheus_manager.query_type:
        results = prometheus_manager.query_metrics(instances=node_lists,
                                                   query=prometheus_manager.query_type[metric_name])
        return CommonResponseSchema(data=results)
    else:
        return CommonResponseSchema(success=False, message='指标不存在')
