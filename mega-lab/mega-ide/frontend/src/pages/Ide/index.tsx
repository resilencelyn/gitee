import React, {useEffect, useRef, useState} from 'react';
import type {ActionType} from '@ant-design/pro-table';
import {ProColumns} from '@ant-design/pro-table';
import {handleRemove} from '@/utils/actions';
import MegaCreateForm from '@/compoments/MegaCreateForm';
import MegaUpdateForm from '@/compoments/MegaUpdateForm';
import {
  deleteIdeEnvironment,
  listIdeEnvironmentOptions,
} from '@/pages/IdeEnvironment/api';
import {renderRunningStatue} from '@/utils/fieldRenders';
import MegaTable from '@/compoments/MegaTable';
import {IdeEntity} from '@/pages/Ide/entities';
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
import {formChildren} from '@/pages/Ide/form';
import {message} from 'antd';


export default function RegistryIndexPage() {
  const actionRef = useRef<ActionType>();

  const [createModalVisible, handleCreateModalVisible] =
    useState<boolean>(false);
  const [updateModalVisible, handleUpdateModalVisible] =
    useState<boolean>(false);
  const [currentEntity, setCurrentEntity] = useState<BaseTableEntity>();
  const [ideEnvironmentOptions, setIdeEnvironmentOptions] = useState([]);


  useEffect(() => {

    async function initData() {
      let options = await listIdeEnvironmentOptions();
      setIdeEnvironmentOptions(options);
    }

    initData().then(() => {
    });

  }, []);

  const tableColumns: ProColumns<IdeEntity>[] = [
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
      render: (_, record) => renderRunningStatue(record.status),
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
        let buttons = [];
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
      ></MegaTable>

      <MegaCreateForm
        title={'新增开发环境'}
        handleModalVisible={handleCreateModalVisible}
        actionRef={actionRef}
        modalVisible={createModalVisible}
        children={formChildren()}
        addEntityAction={addIde}
      ></MegaCreateForm>
      <MegaUpdateForm
        title={'编辑开发环境'}
        modalVisible={updateModalVisible}
        handleModalVisible={handleUpdateModalVisible}
        actionRef={actionRef}
        getEntityAction={getIde}
        children={formChildren()}
        updateEntityAction={updateIde}
        currentEntity={currentEntity}
      ></MegaUpdateForm>
    </div>
  );
}
