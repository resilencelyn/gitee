import json
import re

import requests

from server.framework.core.settings import settings


class PrometheusManager:
    query_type = {
        'memory_size': 'sum(node_memory_MemTotal_bytes{nodename})',
        'memory_usage': 'sum(node_memory_MemTotal_bytes{nodename} - node_memory_MemAvailable_bytes{nodename})',
        'memory_percent_usage': 'sum(node_memory_MemAvailable_bytes{nodename}) / sum(node_memory_MemTotal_bytes{nodename})',
        'disk_write': 'sum(max(rate(node_disk_written_bytes_total{nodename}[1m])) by (instance))',
        'disk_read': 'sum(max(rate(node_disk_read_bytes_total{nodename}[1m])) by (instance))',
        'network_receive': 'sum(max(rate(node_network_receive_bytes_total{nodename}[1m])*8) by (instance))',
        'network_transmit': 'sum(max(rate(node_network_transmit_bytes_total{nodename}[1m])*8) by (instance))',
        'cpu_cores': 'count(node_cpu_seconds_total{mode=~"system", nodename})',
        'cpu_percent_usage': 'avg(1-avg(rate(node_cpu_seconds_total{mode="idle", nodename}[1m])))',
        'cpu_usage': '(count(node_cpu_seconds_total{mode=~"system", nodename})) * (avg(1-avg(rate(node_cpu_seconds_total{mode="idle", nodename}[1m]))))',
        'disk_usage': 'sum(node_filesystem_size_bytes{fstype=~"xfs|ext.*", mountpoint="/", nodename} - node_filesystem_free_bytes{fstype=~"xfs|ext.*", mountpoint="/", nodename})',
        'disk_size': 'sum(node_filesystem_size_bytes{fstype=~"ext4|xfs", mountpoint="/", nodename} - 0)',
        'disk_percent_usage': '(sum(node_filesystem_size_bytes{fstype=~"xfs|ext.*", mountpoint="/", nodename} - node_filesystem_free_bytes{fstype=~"xfs|ext.*", mountpoint="/", nodename})) / (sum(node_filesystem_size_bytes{fstype=~"ext4|xfs", mountpoint="/", nodename} - 0) )',
        'node_load5': 'node_load5{nodename}'
    }

    def query(self, expr, start=None, end=None, step=None):
        """
        向prometheus发送查询语句
        :param expr: 查询语句
        :param start:
        :param end:
        :param step:
        :return:
        """
        url = settings.prometheus_address + '/api/v1/query?query=' + expr
        if start is not None:
            url += f'&start={start}'
        if end is not None:
            url += f'&end={end}'
        if step is not None:
            url += f'&step={step}'
        try:
            print(url)
            return json.loads(requests.get(url=url).content.decode('utf8', 'ignore'))
        except Exception as e:
            print(e)
            return {}

    def query_metrics(self, instances=None, query: str = '',
                      start=None, end=None, step=None):
        """
        输入查询语句，根据输入节点信息返回查询结果
        """
        instance_res = []
        patt = r'nodename'

        if instances is None:
            query = re.sub(patt, '', query)
            instance_res.append(self.query(expr=query, start=start, end=end, step=step))

        else:
            for instance in instances:
                query_instance = re.sub(patt, 'nodename=~"' + instance + '"', query)
                instance_res.append(self.query(expr=query_instance, start=start, end=end, step=step))
        return instance_res

    def get_value(self, datas):
        """
        prometheus返回的信息存放在数组字典，解析数组并获取数值
        :param datas: prometheus指标数组字典
        :return:
        """
        values = []
        try:
            for i in datas:
                values.append(i['data']['result'][0]['value'][1])
        except:
            pass
        return values
