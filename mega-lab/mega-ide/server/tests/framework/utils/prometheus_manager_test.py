import unittest

from server.framework.utils.prometheus_manager import PrometheusManager

p = PrometheusManager()
query_type = p.query_type
NodeList = ["10.11.25.48", "10.11.25.49", "10.11.25.50"]

class PrometheusManagerTest(unittest.TestCase):
    def test_query(self):
        exp = 'sum(node_memory_MemTotal_bytes - node_memory_MemAvailable_bytes)'
        res = p.query(exp)
        print(res)

    def test_查询内存(self):
        res0 = p.query_metrics(None, query_type['memory_usage'])
        res1 = p.query_metrics(NodeList, query_type['memory_usage'])
        print('---------------总内存GB--------------\n', p.get_value(res0))
        print('---------------各节点内存--------------\n', p.get_value(res1))

    def test_查询内存使用率(self):
        res0 = p.query_metrics(None, query_type['memory_percent_usage'])
        res1 = p.query_metrics(NodeList, query_type['memory_percent_usage'])
        print('---------------总内存使用率--------------\n', p.get_value(res0))
        print('---------------各节点内存使用率--------------\n', p.get_value(res1))

    def test_查询cpu核心数(self):
        res0 = p.query_metrics(None, query_type['cpu_cores'])
        res1 = p.query_metrics(NodeList, query_type['cpu_cores'])
        print('---------------CPU核心总数--------------\n', p.get_value(res0))
        print('---------------各节点CPU核心数--------------\n', p.get_value(res1))

    def test_查询cpu使用率(self):
        res0 = p.query_metrics(None, query_type['cpu_percent_usage'])
        res1 = p.query_metrics(NodeList, query_type['cpu_percent_usage'])
        print('---------------总CPU使用率--------------\n', p.get_value(res0))
        print('---------------各节点CPU使用率--------------\n', p.get_value(res1))

    def test_5分钟负载(self):
        res0 = p.query_metrics(None, query_type['node_load5'])
        res1 = p.query_metrics(NodeList, query_type['node_load5'])
        print('---------------总负载--------------\n', p.get_value(res0))
        print('---------------各节点负载--------------\n', p.get_value(res1))

    def test_查询磁盘写入(self):
        res0 = p.query_metrics(None, query_type['disk_write'])
        res1 = p.query_metrics(NodeList, query_type['disk_write'])
        print('---------------节点总磁盘写入(kb/s)--------------\n', p.get_value(res0))
        print('---------------各节点磁盘写入(kb/s)--------------\n', p.get_value(res1))

    def test_查询磁盘读取(self):
        res0 = p.query_metrics(None, query_type['disk_read'])
        res1 = p.query_metrics(NodeList, query_type['disk_read'])
        print('---------------节点总磁盘写入(kb/s)--------------\n', p.get_value(res0))
        print('---------------各节点磁盘写入(kb/s)--------------\n', p.get_value(res1))

    def test_查询下载带宽(self):
        res0 = p.query_metrics(None, query_type['network_receive'])
        res1 = p.query_metrics(NodeList, query_type['network_receive'])
        print('---------------节点总下载带宽(kb/s)--------------\n', p.get_value(res0))
        print('---------------各节点下载带宽(kb/s)--------------\n', p.get_value(res1))

    def test_查询上传带宽(self):
        res0 = p.query_metrics(None, query_type['network_transmit'])
        res1 = p.query_metrics(NodeList, query_type['network_transmit'])
        print('---------------节点总上传带宽(kb/s)--------------\n', p.get_value(res0))
        print('---------------各节点上传带宽(kb/s)--------------\n', p.get_value(res1))

    def test_查询磁盘使用量(self):
        res0 = p.query_metrics(None, query_type['disk_usage'])
        res1 = p.query_metrics(NodeList, query_type['disk_usage'])
        print('---------------节点总磁盘使用量--------------\n', p.get_value(res0))
        print('---------------各节点磁盘使用量--------------\n', p.get_value(res1))

    def test_查询磁盘大小(self):
        res0 = p.query_metrics(None, query_type['disk_size'])
        res1 = p.query_metrics(NodeList, query_type['disk_size'])
        print('---------------节点总磁盘大小--------------\n', p.get_value(res0))
        print('---------------各节点磁盘大小--------------\n', p.get_value(res1))

    def test_查询磁盘使用百分比(self):
        res0 = p.query_metrics(None, query_type['disk_percent_usage'])
        res1 = p.query_metrics(NodeList, query_type['disk_percent_usage'])
        print('---------------节点总磁盘使用百分比--------------\n', p.get_value(res0))
        print('---------------各节点磁盘使用百分比--------------\n', p.get_value(res1))


if __name__ == '__main__':
    unittest.main()
