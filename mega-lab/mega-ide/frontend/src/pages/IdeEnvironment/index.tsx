import React, { useEffect, useRef, useState } from 'react';
import type { ActionType } from '@ant-design/pro-table';
import ProTable, { ProColumns } from '@ant-design/pro-table';
import { handleRemove } from '@/utils/actions';
import MegaCreateForm from '@/compoments/MegaCreateForm';
import { formChidren } from '@/pages/IdeEnvironment/form';
import MegaUpdateForm from '@/compoments/MegaUpdateForm';
import {
  addIdeEnvironment, deleteIdeEnvironment,
  getIdeEnvironment,
  listIdeEnvironment,
  updateIdeEnvironment,
} from '@/pages/IdeEnvironment/api';
import { IdeEnvironmentEntity } from '@/pages/IdeEnvironment/entities';
import MegaTable from '@/compoments/MegaTable';
import { listIdeRegistryOptions } from '@/pages/IdeRegistry/api';
import { renderEnableStatue } from '@/pages/IdeEnvironment/render';

export default function RegistryIndexPage() {
  const actionRef = useRef<ActionType>();

  const [createModalVisible, handleCreateModalVisible] = useState<boolean>(false);
  const [updateModalVisible, handleUpdateModalVisible] = useState<boolean>(false);
  const [currentEntity, setCurrentEntity] = useState<BaseTableEntity>();
  const [ideRegistryOptions, setIdeRegistryOptions] = useState([]);

  useEffect(() => {
    async function initData() {
      let options = await listIdeRegistryOptions();
      setIdeRegistryOptions(options);
    }

    initData().then(() => {
    });
  }, []);

  const IdeEnvironmentColumns: ProColumns<IdeEnvironmentEntity>[] = [
    {
      title: '环境名称',
      dataIndex: 'name',
    },
    {
      title: 'CPU(核)',
      dataIndex: 'max_cpu',
      hideInSearch: true,
    },
    {
      title: '内存(G)',
      dataIndex: 'max_memory',
      hideInSearch: true,
    },
    {
      title: '硬盘(G)',
      dataIndex: 'max_disk',
      hideInSearch: true,
    },
    {
      title: '状态',
      dataIndex: 'status',
      hideInSearch: true,
      render: (_, record) => (
        renderEnableStatue(record.status)
      ),
    },
    {
      title: '创建时间',
      key: 'created',
      dataIndex: 'created',
      valueType: 'dateTime',
      hideInSearch: true,
    },
    {
      title: '操作',
      valueType: 'option',
      render: (text, record, _, action) => [
        <a key='edit'
           onClick={() => {
             setCurrentEntity(record);
             handleUpdateModalVisible(true);
           }}>
          编辑
        </a>,
        <a
          key='delete'
          onClick={async () => {
            await handleRemove(record.id, deleteIdeEnvironment);
            actionRef.current?.reloadAndRest?.();
          }}>
          删除
        </a>,
      ],
    },
  ];

  return (
    <div>
      <MegaTable title={'资源配置'} columns={IdeEnvironmentColumns} actionRef={actionRef}
                 handleCreateModalVisible={handleCreateModalVisible}
                 deleteRowAction={deleteIdeEnvironment} tableRequest={listIdeEnvironment}>
      </MegaTable>

      <MegaCreateForm title={'新增资源配置'} handleModalVisible={handleCreateModalVisible} actionRef={actionRef}
                      modalVisible={createModalVisible} children={formChidren(undefined)}
                      addEntityAction={addIdeEnvironment}></MegaCreateForm>
      <MegaUpdateForm title={'编辑资源配置'} modalVisible={updateModalVisible} handleModalVisible={handleUpdateModalVisible}
                      actionRef={actionRef} getEntityAction={getIdeEnvironment}
                      children={formChidren(currentEntity)}
                      updateEntityAction={updateIdeEnvironment} currentEntity={currentEntity}></MegaUpdateForm>


    </div>
  );
}

