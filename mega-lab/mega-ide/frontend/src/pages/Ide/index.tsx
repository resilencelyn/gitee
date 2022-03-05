import React, { useEffect, useRef, useState } from 'react';
import { Tag } from 'antd';
import {
  SyncOutlined,
} from '@ant-design/icons';
import type { ActionType } from '@ant-design/pro-table';
import { ProColumns } from '@ant-design/pro-table';
import { handleRemove } from '@/utils/actions';
import MegaCreateForm from '@/compoments/MegaCreateForm';
import MegaUpdateForm from '@/compoments/MegaUpdateForm';
import {
  listIdeEnvironmentOptions,
} from '@/pages/IdeEnvironment/api';
import MegaTable from '@/compoments/MegaTable';
import { IdeEntity } from '@/pages/Ide/entities';
import {
  addIde,
  deleteIde,
  getIde,
  getIdeUrl,
  listIde,
  startIde,
  stopIde,
  updateIde,
} from '@/pages/Ide/api';
import { formChildren } from '@/pages/Ide/form';
import { listIdeRegistryOptions } from '../IdeRegistry/api';

export default function RegistryIndexPage() {
  const actionRef = useRef<ActionType>();

  const [createModalVisible, handleCreateModalVisible] =
    useState<boolean>(false);
  const [updateModalVisible, handleUpdateModalVisible] =
    useState<boolean>(false);
  const [currentEntity, setCurrentEntity] = useState<IdeEntity>();
  const [ideRegistryOptions, setIdeRegistryOptions] = useState([]);
  const [ideEnvironmentOptions, setIdeEnvironmentOptions] = useState([]);

  async function initIdeRegistryData() {
    const options = await listIdeRegistryOptions();
    setIdeRegistryOptions(options);
  }

  async function initIdeEnvironmentData() {
    const options = await listIdeEnvironmentOptions();
    setIdeEnvironmentOptions(options);
  }

  useEffect(() => {
    initIdeEnvironmentData();
    initIdeRegistryData();
  }, []);

  const tableColumns: ProColumns<IdeEntity>[] = [
    {
      title: '镜像源',
      dataIndex: 'ide_registry',
      valueType: 'select',
      fieldProps: {
        options: ideRegistryOptions,
      },
    },
    {
      title: '镜像',
      dataIndex: 'ide_image',
      hideInSearch: true,
    },
    {
      title: '环境',
      dataIndex: 'ide_environment',
      valueType: 'select',
      fieldProps: {
        options: ideEnvironmentOptions,
      },
    },
    {
      title: '名称',
      dataIndex: 'name',
      hideInSearch: true,
    },
    {
      title: '状态',
      dataIndex: 'status',
      hideInSearch: true,
      render: (_, record) => {
        const STATUSES = [
          {
            color: 'default',
            text: '未运行',
            clickable: false,
          },
          {
            icon: <SyncOutlined spin />,
            color: 'processing',
            text: '启动中',
          },
          {
            color: 'success',
            text: '运行中',
          },
          {
            icon: <SyncOutlined spin />,
            color: 'processing',
            text: '停止中',
          },
        ]
        const STATUS = STATUSES[record.status];
        if (!STATUS) return null;
        const { icon, color, text } = STATUS || STATUS[0];
        return <Tag icon={icon} color={color}>{text}</Tag>;
      },
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
      render: (text, record, _, action) => {
        const buttons = [];
        buttons.push(
          <a
            key='edit'
            onClick={() => {
              setCurrentEntity(record);
              handleUpdateModalVisible(true);
            }}
          >
            编辑
          </a>,
        );

        if (record.status == 0) {
          buttons.push(
            <a
              key='start'
              onClick={async () => {
                await startIde(record.id);
                actionRef.current?.reloadAndRest?.();
              }}
            >
              启动
            </a>,
          );
        }


        if (record.status == 2) {
          buttons.push(
            <a
              key='stop'
              onClick={async () => {
                await stopIde(record.id);
                actionRef.current?.reloadAndRest?.();
              }}
            >
              停止
            </a>,
          );
          buttons.push(
            <a
              key='open'
              onClick={async () => {
                let ideData: any = await getIdeUrl(record.id);
                window.open(ideData.data.url);
              }}
            >
              查看
            </a>,
          );
        }

        buttons.push(
          <a
            key='delete'
            onClick={async () => {
              await handleRemove(record.id, deleteIde);
              actionRef.current?.reloadAndRest?.();
            }}
          >
            删除
          </a>,
        );
        return buttons;
      },
    },
  ];

  return (
    <div>
      <MegaTable
        polling={5000}
        title={'开发环境'}
        columns={tableColumns}
        actionRef={actionRef}
        handleCreateModalVisible={handleCreateModalVisible}
        deleteRowAction={deleteIde}
        tableRequest={listIde}
      />
      <MegaCreateForm
        title={'新增开发环境'}
        handleModalVisible={handleCreateModalVisible}
        actionRef={actionRef}
        modalVisible={createModalVisible}
        addEntityAction={addIde}
      >
        {formChildren()}
      </MegaCreateForm>
      <MegaUpdateForm
        title={'编辑开发环境'}
        modalVisible={updateModalVisible}
        handleModalVisible={handleUpdateModalVisible}
        actionRef={actionRef}
        getEntityAction={getIde}
        updateEntityAction={updateIde}
        currentEntity={currentEntity}
      >
        {formChildren()}
      </MegaUpdateForm>
    </div>
  );
}
