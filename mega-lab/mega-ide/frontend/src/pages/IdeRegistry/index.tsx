import React, { useRef, useState } from 'react';
import type { ActionType } from '@ant-design/pro-table';
import { ProColumns } from '@ant-design/pro-table';
import { IdeRegistryEntity } from '@/pages/IdeRegistry/entities';
import {
  addIdeRegistry,
  deleteIdeRegistry,
  getIdeRegistry,
  listIdeRegistry,
  updateIdeRegistry,
} from '@/pages/IdeRegistry/api';
import { handleRemove } from '@/utils/actions';
import MegaCreateForm from '@/compoments/MegaCreateForm';
import { formChildren } from '@/pages/IdeRegistry/form';
import MegaUpdateForm from '@/compoments/MegaUpdateForm';
import MegaTable from '@/compoments/MegaTable';
import { renderEnableStatue } from '@/pages/IdeRegistry/render';

export default function RegistryIndexPage() {
  const tableActionRef = useRef<ActionType>();

  const [createModalVisible, handleCreateModalVisible] =
    useState<boolean>(false);
  const [updateModalVisible, handleUpdateModalVisible] =
    useState<boolean>(false);

  const [currentEntity, setCurrentEntity] = useState<BaseTableEntity>();

  const tableColumns: ProColumns<IdeRegistryEntity>[] = [
    {
      title: '镜像源名称',
      dataIndex: 'name',
    },
    {
      title: '镜像源地址',
      dataIndex: 'registry',
      search: false,
    },
    {
      title: '是否启用',
      dataIndex: 'enable',
      valueType: 'select',
      valueEnum: {
        0: {
          text: '禁用',
        },
        1: {
          text: '启用',
        },
      },
      render: (_, record) => renderEnableStatue(record.enable),
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
        <a
          key="edit"
          onClick={() => {
            setCurrentEntity(record);
            handleUpdateModalVisible(true);
          }}
        >
          编辑
        </a>,
        <a
          key="delete"
          onClick={async () => {
            await handleRemove(record.id, deleteIdeRegistry);
            tableActionRef.current?.reloadAndRest?.();
          }}
        >
          删除
        </a>,
      ],
    },
  ];

  return (
    <div>
      <MegaTable
        polling={undefined}
        handleCreateModalVisible={handleCreateModalVisible}
        columns={tableColumns}
        actionRef={tableActionRef}
        title={'镜像源'}
        tableRequest={listIdeRegistry}
        deleteRowAction={deleteIdeRegistry}
      ></MegaTable>

      <MegaCreateForm
        handleModalVisible={handleCreateModalVisible}
        actionRef={tableActionRef}
        modalVisible={createModalVisible}
        title={'新增镜像源'}
        children={formChildren()}
        addEntityAction={addIdeRegistry}
      ></MegaCreateForm>

      <MegaUpdateForm
        handleModalVisible={handleUpdateModalVisible}
        actionRef={tableActionRef}
        modalVisible={updateModalVisible}
        currentEntity={currentEntity}
        title={'编辑镜像源'}
        children={formChildren()}
        getEntityAction={getIdeRegistry}
        updateEntityAction={updateIdeRegistry}
      ></MegaUpdateForm>
    </div>
  );
}
