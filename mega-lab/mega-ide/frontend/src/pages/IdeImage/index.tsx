import React, { Fragment, useEffect, useRef, useState } from 'react';
import { message, Tag } from 'antd';
import type { ActionType } from '@ant-design/pro-table';
import { ProColumns } from '@ant-design/pro-table';
import { listIdeRegistryOptions } from '@/pages/IdeRegistry/api';
import { IdeImageEntity } from '@/pages/IdeImage/entities';
import {
  addIdeImage,
  buildIdeImage,
  deleteIdeImage,
  getBuildInfo,
  getIdeImage,
  listIdeImage,
  updateIdeImage,
} from '@/pages/IdeImage/api';
import { handleRemove } from '@/utils/actions';
import {
  CheckCircleOutlined,
  SyncOutlined,
  CloseCircleOutlined,
  MinusCircleOutlined,
} from '@ant-design/icons';
import MegaCreateForm from '@/compoments/MegaCreateForm';
import { formChidren } from '@/pages/IdeImage/form';
import MegaUpdateForm from '@/compoments/MegaUpdateForm';
import MegaTable from '@/compoments/MegaTable';
import MegaLog from '@/compoments/MegaLog';

export default function RegistryIndexPage() {
  const actionRef = useRef<ActionType>();

  const [createModalVisible, handleCreateModalVisible] = useState<boolean>(false);
  const [updateModalVisible, handleUpdateModalVisible] = useState<boolean>(false);

  const [currentEntity, setCurrentEntity] = useState<BaseTableEntity>();
  const [ideRegistryOptions, setIdeRegistryOptions] = useState([]);

  const buildStatus = (record: IdeImageEntity) => {
    const logsRef = useRef<{ show: () => void }>(null);
    const STATUSES = [
      {
        icon: <MinusCircleOutlined />,
        color: 'default',
        text: '未构建',
        clickable: false,
      },
      {
        icon: <SyncOutlined spin />,
        color: 'processing',
        text: '构建中',
        clickable: true,
      },
      {
        icon: <CloseCircleOutlined />,
        color: 'error',
        text: '构建失败',
        clickable: true,
      },
      {
        icon: <CheckCircleOutlined />,
        color: 'success',
        text: '构建成功',
        clickable: true,
      }
    ]
    const STATUS = STATUSES[record.status];
    if (!STATUS) return null;
    const { icon, color, text, clickable } = STATUS;
    const tag = (
      <Tag icon={icon} color={color}>
        {text}
      </Tag>
    )
    return clickable ? (
      <Fragment>
        <a onClick={() => {
          if (logsRef.current) logsRef.current.show();
        }}>{tag}</a>
        <MegaLog ref={logsRef} id={record.id} status={record.status} action={getBuildInfo} />
      </Fragment>
    ) : tag;
  };

  async function initIdeRegistryData() {
    const options = await listIdeRegistryOptions();
    setIdeRegistryOptions(options);
  }

  useEffect(() => {
    initIdeRegistryData()
  }, []);

  const tableColumns: ProColumns<IdeImageEntity>[] = [
    {
      title: '镜像源',
      dataIndex: 'ide_registry',
      valueType: 'select',
      fieldProps: {
        options: ideRegistryOptions,
      },
    },
    {
      title: '镜像名称',
      dataIndex: 'name',
    },
    {
      title: '镜像版本',
      dataIndex: 'version',
      hideInSearch: true,
    },
    {
      title: 'Git地址',
      dataIndex: 'git_url',
      hideInSearch: true,
    },
    {
      title: '备注',
      dataIndex: 'note',
      hideInSearch: true,
    },
    {
      title: '状态',
      dataIndex: 'status',
      hideInSearch: true,
      render: (_, record) => buildStatus(record),
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
          key="build"
          onClick={async () => {
            message.success('开始构建');
            await buildIdeImage(record.id);
            actionRef.current?.reloadAndRest?.();
          }}
        >
          构建
        </a>,
        <a
          key="delete"
          onClick={async () => {
            await handleRemove(record.id, deleteIdeImage);
            actionRef.current?.reloadAndRest?.();
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
        title={'镜像'}
        columns={tableColumns}
        actionRef={actionRef}
        polling={5000}
        handleCreateModalVisible={handleCreateModalVisible}
        deleteRowAction={deleteIdeImage}
        tableRequest={listIdeImage}
      ></MegaTable>

      <MegaCreateForm
        title={'新增镜像'}
        handleModalVisible={handleCreateModalVisible}
        actionRef={actionRef}
        modalVisible={createModalVisible}
        addEntityAction={addIdeImage}
      >
        {formChidren()}
      </MegaCreateForm>
      <MegaUpdateForm
        title={'编辑镜像'}
        handleModalVisible={handleUpdateModalVisible}
        actionRef={actionRef}
        modalVisible={updateModalVisible}
        currentEntity={currentEntity}
        getEntityAction={getIdeImage}
        updateEntityAction={updateIdeImage}
      >
        {formChidren()}
      </MegaUpdateForm>
    </div>
  );
}
