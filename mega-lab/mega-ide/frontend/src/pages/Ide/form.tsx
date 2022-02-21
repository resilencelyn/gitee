import { ProFormSelect, ProFormText } from '@ant-design/pro-form';
import React from 'react';
import { listIdeEnvironmentOptions } from '@/pages/IdeEnvironment/api';

export const formChildren = () => {
  return (<div>
    <ProFormSelect name='ide_environment_id' label='环境'
                   rules={[
                     { required: true, message: '请选择环境' },
                   ]}
                   request={listIdeEnvironmentOptions}></ProFormSelect>
    <ProFormText name='name' label='名称'
                 rules={[
                   { required: true, message: '请选择IDE名称' },
                 ]}></ProFormText>
  </div>);
};
