import {ProFormSelect, ProFormRadio, ProFormText} from '@ant-design/pro-form';
import React from 'react';
import {listIdeRegistryOptions} from '@/pages/IdeRegistry/api';

export const formChidren = () => {
  return (<div>
    <ProFormSelect name='ide_registry_id' label='镜像源'
                   rules={[
                     {required: true, message: '请选择镜像源'},
                   ]}
                   request={listIdeRegistryOptions}></ProFormSelect>
    <ProFormText name='name' label='镜像名称'
                 rules={[
                   {required: true, message: '镜像名称'},
                 ]}/>
    <ProFormText name='version' label='镜像版本'
                 rules={[
                   {required: true, message: '请填写镜像版本'},
                 ]}/>
    <ProFormRadio.Group
      name="ide_type"
      label="镜像类型"
      options={[
        {
          label: 'jupyter',
          value: 0,
        },
        {
          label: 'vscode',
          value: 1,
        },
      ]}
    />
    <ProFormText name='git_url' label='Git地址'/>
    <ProFormText name='note' label='描述'/>
  </div>);
};
