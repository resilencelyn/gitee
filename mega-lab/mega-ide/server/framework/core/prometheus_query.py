
class Prometheus_query():
    query_type = {
            'memory_usage': 'sum(node_memory_MemTotal_bytes{nodename}) / (1024^3)',
            'memory_percent_usage': 'sum(node_memory_MemAvailable_bytes{nodename}) / sum(node_memory_MemTotal_bytes{nodename})',
            'disk_write': 'sum(max(rate(node_disk_written_bytes_total{nodename}[1m]) / 1024) by (instance))',
            'disk_read': 'sum(max(rate(node_disk_read_bytes_total{nodename}[1m]) / 1024) by (instance))',
            'network_receive': 'sum(max(rate(node_network_receive_bytes_total{nodename}[1m])*8/1024) by (instance))',
            'network_transmit': 'sum(max(rate(node_network_transmit_bytes_total{nodename}[1m])*8/1024) by (instance))',
            'cpu_cores': "count(node_cpu_seconds_total{mode=~'system', nodename})"
    }


query_type = Prometheus_query.query_type