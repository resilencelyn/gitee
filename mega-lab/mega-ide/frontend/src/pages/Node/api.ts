import { request } from 'umi';
import { NodeEntity } from './entities';

export async function listNode() {
  return await request<NodeEntity>(`/api/cloud_ide/system_info/list_node_info`, {
    method: 'GET',
  });
}

export async function getMetric(nodeName: string[], metricNames: string[]) {
  return await request(`/api/cloud_ide/system_info/query_metrics`, {
    method: 'GET',
    params: {
      node_lists: nodeName.join(','),
      metric_names: metricNames.join(',')
    },
    skipErrorHandler: true,
  });
}