import React, { FC, useState } from 'react';
import ProTable, { ActionType, ProColumns } from '@ant-design/pro-table';
import { Button } from 'antd';
import { PlusOutlined } from '@ant-design/icons';
import { handleRemove } from '@/utils/actions';

type MegaTableProps = {
  title: string,
  columns: ProColumns[],
  actionRef: React.MutableRefObject<ActionType | undefined>,
  handleCreateModalVisible: React.Dispatch<React.SetStateAction<boolean>>,
  deleteRowAction: any,
  tableRequest: any,
  polling?: number,
}

const MegaTable: FC<MegaTableProps> = (
  props,
) => {
  const {
    columns, actionRef, title,
    handleCreateModalVisible, deleteRowAction,
    tableRequest,
    polling,
  } = props;

  const [selectedRowsState, setSelectedRows] = useState<BaseTableEntity[]>([]);

  return (
    <ProTable<BaseTableEntity>
      headerTitle={title}
      columns={columns}
      actionRef={actionRef}
      request={tableRequest}
      rowKey='id'
      polling={polling}
      rowSelection={{
        onChange: (_, selectedRows) => {
          setSelectedRows(selectedRows);
        },
      }}
      search={{
        labelWidth: 'auto',
      }}
      pagination={{
        pageSize: 10,
      }}
      dateFormatter='string'
      toolBarRender={() => [
        <Button
          type='primary'
          key='primary'
          onClick={() => {
            handleCreateModalVisible(true);
          }}
        >
          <PlusOutlined /> 新增
        </Button>,
        <Button
          onClick={async () => {
            let ids = selectedRowsState.map(e => {
              return e.id;
            }).join(',');
            await handleRemove(ids, deleteRowAction);
            actionRef.current?.reloadAndRest?.();
          }}
        >
          批量删除
        </Button>,
      ]}
    />);
};
export default MegaTable;
