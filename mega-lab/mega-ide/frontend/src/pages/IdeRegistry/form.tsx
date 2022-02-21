import { ProFormSwitch, ProFormText } from '@ant-design/pro-form';
import React from 'react';

export const formChildren = () => {
  return (<div>
    <ProFormText name='name' label='镜像源名称'
                 rules={[
                   { required: true, message: '请填写镜像源名称' },
                 ]} />
    <ProFormText name='registry' label='镜像源地址'
                 rules={[
                   { required: true, message: '请填写镜像源地址' },
                 ]} />
    <ProFormSwitch fieldProps={{ defaultChecked: true }} name='enable' label='启用' />
  </div>);
};
