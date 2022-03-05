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
                        metric_names: str = Query(None, description='指标名称，多个指标以逗号分割'),
                        start: str = Query(None, description='开始时间'),
                        end: str = Query(None, description='结束时间'),
                        step: str = Query(None, description='聚合步长')) -> CommonResponseSchema:
    prometheus_manager = PrometheusManager()
    if node_lists is not None:
        node_lists = node_lists.split(',')
    monitor_value = {}
    metrics = metric_names.split(',')
    for metric_name in metrics:
        if metric_name in prometheus_manager.query_type:
            results = prometheus_manager.query_metrics(instances=node_lists,
                                                       query=prometheus_manager.query_type[metric_name],
                                                       start=start,
                                                       end=end,
                                                       step=step)
            monitor_value[metric_name] = prometheus_manager.get_value(results)
    return CommonResponseSchema(success=True, data=monitor_value)
