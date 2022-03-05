import ProTable, { ProColumns } from '@ant-design/pro-table';
import { listNode } from './api';
import { NodeEntity } from './entities';
import ColumnRender, {  IORender, PercentRender } from './ColumnRender';
import { changeHardwareSize } from '@/utils/utils';

export default () => {

  const columns: ProColumns<NodeEntity>[] = [
    {
      title: '节点名',
      render: (_, record) => <ColumnRender main={record.node_name} sub={record.ip} />
    },
    {
      title: 'CPU',
      render: (_, record) => (
        <PercentRender
          nodeName={record.node_name}
          metrics={['cpu_percent_usage', 'cpu_usage', 'cpu_cores']}
          suffix={'核'}
        />
      )
    },
    {
      title: '内存',
      render: (_, record) => (
        <PercentRender
          nodeName={record.node_name}
          metrics={['memory_percent_usage', 'memory_usage', 'memory_size']}
          format={(value) => changeHardwareSize(value, 'B', 'GB')}
          suffix={'GB'}
        />
      )
    },
    {
      title: '磁盘',
      render: (_, record) => (
        <PercentRender
          nodeName={record.node_name}
          metrics={['disk_percent_usage', 'disk_usage', 'disk_size']}
          format={(value) => changeHardwareSize(value, 'B', 'GB')}
          suffix={'GB'}
        />
      )
    },
    {
      title: '磁盘读取',
      render: (_, record) => (
        <IORender 
          nodeName={record.node_name}
          metrics={['disk_read']}
        />
      )
    },
    {
      title: '磁盘写入',
      render: (_, record) => (
        <IORender 
          nodeName={record.node_name}
          metrics={['disk_write']}
        />
      )
    },
    {
      title: '下载带宽',
      render: (_, record) => (
        <IORender 
          nodeName={record.node_name}
          metrics={['network_receive']}
        />
      )
    },
    {
      title: '上传带宽',
      render: (_, record) => (
        <IORender 
          nodeName={record.node_name}
          metrics={['network_transmit']}
        />
      )
    },
  ];

  return (
    <div style={{ paddingTop: 16 }}>
      <ProTable
        headerTitle={'节点列表'}
        rowKey='node_name'
        columns={columns}
        search={false}
        request={listNode}
        pagination={{
          pageSize: 10,
        }}
      />
    </div>
  );
}
