import json

import requests

from server.framework.core.settings import settings


class PrometheusManager:

    def query(self, expr, start=None, end=None, step=None):
        url = settings.prometheus_address + '/api/v1/query?query=' + expr
        if start is not None:
            url += f'&start={start}'
        if end is not None:
            url += f'&start={end}'
        if step is not None:
            url += f'&start={step}'
        try:
            print(url)
            return json.loads(requests.get(url=url).content.decode('utf8', 'ignore'))
        except Exception as e:
            print(e)
            return {}

    def query_memory_usage(self, instances):
        query = ''
        if instances is None:
            query = 'sum(node_memory_MemTotal_bytes - node_memory_MemAvailable_bytes)'
            print(query)
            self.query(query)
        else:
            for instance in instances:
                query += 'sum(node_memory_MemTotal_bytes{nodename=~"' + instance + '.*"} - node_memory_MemAvailable_bytes{nodename=~"' + instance + '.*"})+'
            query = query[:-1]
            self.query(query)

