import React, {useEffect, useRef, useState} from 'react';
import {message, Tag} from 'antd';
import type {ActionType} from '@ant-design/pro-table';
import {ProColumns} from '@ant-design/pro-table';
import {listIdeRegistryOptions} from '@/pages/IdeRegistry/api';
import {IdeImageEntity} from '@/pages/IdeImage/entities';
import {
  addIdeImage,
  buildIdeImage,
  deleteIdeImage,
  getBuildInfo,
  getIdeImage,
  listIdeImage,
  updateIdeImage,
} from '@/pages/IdeImage/api';
import {handleAdd, handleRemove, handleUpdate} from '@/utils/actions';
import {
  CheckCircleOutlined,
  SyncOutlined,
  CloseCircleOutlined,
  MinusCircleOutlined,
} from '@ant-design/icons';
import MegaCreateForm from '@/compoments/MegaCreateForm';
import {formChidren} from '@/pages/IdeImage/form';
import MegaUpdateForm from '@/compoments/MegaUpdateForm';
import MegaTable from '@/compoments/MegaTable';
import {DrawerForm, ModalForm, ProFormInstance} from '@ant-design/pro-form';

export default function RegistryIndexPage() {
  const actionRef = useRef<ActionType>();

  const [createModalVisible, handleCreateModalVisible] =
    useState<boolean>(false);
  const [updateModalVisible, handleUpdateModalVisible] =
    useState<boolean>(false);
  const [buildInfoModalVisible, handleBuildInfoModalVisible] =
    useState<boolean>(false);

  const [currentEntity, setCurrentEntity] = useState<BaseTableEntity>();
  const [ideRegistryOptions, setIdeRegistryOptions] = useState([]);

  const [buildInfo, setBuildInfo] = useState('');
  const [timerId, setTimerId] = useState();
  useEffect(() => {
    if (buildInfoModalVisible == false) {
      clearInterval(timerId)
    } else {
      let id = setInterval(() => {
        let entity = currentEntity as IdeImageEntity
        return injectBuildInfo(entity.id)
      }, 5000)
      // @ts-ignore
      setTimerId(id)
    }
  }, [buildInfoModalVisible]);

  const injectBuildInfo = async (id: string) => {
    let buildResult = await getBuildInfo(id);
    let data = buildResult.data.replaceAll('\n', '<br>');
    setBuildInfo(data);
  }
  const showBuildInfo = async (record: IdeImageEntity) => {
    handleBuildInfoModalVisible(true);
    injectBuildInfo(record.id)
    setCurrentEntity(record)

  };

  const buildStatus = (record: IdeImageEntity) => {
    if (record.status == 0) {
      return (
        <Tag icon={<MinusCircleOutlined/>} color="default">
          未构建
        </Tag>
      );
    }
    if (record.status == 1) {
      return (
        <a
          onClick={() => {
            showBuildInfo(record);
          }}
        >
          <Tag icon={<SyncOutlined spin/>} color="processing">
            构建中
          </Tag>
        </a>
      );
    }
    if (record.status == 2) {
      return (
        <a
          onClick={() => {
            showBuildInfo(record);
          }}
        >
          <Tag icon={<CloseCircleOutlined/>} color="error">
            构建失败
          </Tag>
        </a>
      );
    }
    if (record.status == 3) {
      return (
        <a
          onClick={() => {
            showBuildInfo(record);
          }}
        >
          <Tag icon={<CheckCircleOutlined/>} color="success">
            构建成功
          </Tag>
        </a>
      );
    }
  };

  useEffect(() => {
    async function initData() {
      let options = await listIdeRegistryOptions();
      setIdeRegistryOptions(options);
    }

    initData().then(() => {
    });
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
        children={formChidren()}
        addEntityAction={addIdeImage}
      ></MegaCreateForm>
      <MegaUpdateForm
        title={'编辑镜像'}
        handleModalVisible={handleUpdateModalVisible}
        actionRef={actionRef}
        modalVisible={updateModalVisible}
        children={formChidren()}
        currentEntity={currentEntity}
        getEntityAction={getIdeImage}
        updateEntityAction={updateIdeImage}
      ></MegaUpdateForm>

      <DrawerForm
        title="构建详情"
        visible={buildInfoModalVisible}
        submitter={false}
        onVisibleChange={handleBuildInfoModalVisible}
      >
        <div dangerouslySetInnerHTML={{__html: buildInfo}}></div>
      </DrawerForm>
    </div>
  );
}
