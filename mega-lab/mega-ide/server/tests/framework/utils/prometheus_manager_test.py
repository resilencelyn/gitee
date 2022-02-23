import unittest
import json
from server.framework.core.settings import settings
import requests
from server.framework.utils.prometheus_manager import PrometheusManager

p = PrometheusManager()

class PrometheusManagerTest(unittest.TestCase):
    def test_query(self):
        exp = 'sum(node_memory_MemTotal_bytes - node_memory_MemAvailable_bytes)'
        res = p.query(exp)
        print(res)

    def test_查询内存使用(self):
        res = p.query_memory_usage(None)
        print(res)


if __name__ == '__main__':
    unittest.main()
