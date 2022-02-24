import json
import re

import requests

from server.framework.core.settings import settings


class PrometheusManager:

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
            url += f'&start={end}'
        if step is not None:
            url += f'&start={step}'
        try:
            # print(url)
            return json.loads(requests.get(url=url).content.decode('utf8', 'ignore'))
        except Exception as e:
            print(e)
            return {}

    def query_metrics(self, instances = None , query: str = ''):
        """
        输入查询语句，返回查询结果
        """
        instance_res = []
        patt = r'nodename'

        if instances is None:
            query = re.sub(patt, '', query)
            instance_res.append(self.query(query))

        else:
            print(query)
            for instance in instances:
                query_instance = re.sub(patt, 'nodename=~"' + instance + '"', query)
                instance_res.append(self.query(query_instance))
        print(query)
        return instance_res

    def get_value(self, datas):
        """
        prometheus返回的信息存放在数组字典，解析数组并获取数值
        :param datas: prometheus指标数组字典
        :return:
        """
        values = []
        for i in datas:
            values.append(i['data']['result'][0]['value'][1])
        return values