import { ProFormDigit, ProFormGroup, ProFormSelect, ProFormSwitch, ProFormText } from '@ant-design/pro-form';
import React, { useEffect, useState } from 'react';
import { listIdeRegistryOptions } from '@/pages/IdeRegistry/api';
import { listIdeImageOptions } from '@/pages/IdeImage/api';

export const formChidren = (currentEntity: any) => {
  const [ideImageOptions, setIdeImageOptions] = useState([]);

  const handleIdeRegistryChange = async (value: any) => {
    let options = await listIdeImageOptions(value);
    setIdeImageOptions(options);
  };


  return (<div>
    <ProFormText name='name' label='环境名称'
                 rules={[
                   { required: true, message: '请填写环境名称' },
                 ]} />
    <ProFormGroup>
      <ProFormDigit label='CPU(核)' name='max_cpu' width='sm' min={1} max={10} />

      <ProFormDigit label='内存(G)' name='max_memory' width='sm' min={1} max={10} />

      <ProFormDigit label='硬盘(G)' name='max_disk' width='sm' min={1} max={10} />
    </ProFormGroup>
    <ProFormSwitch fieldProps={{ defaultChecked: true }} name='status' label='启用' />
  </div>);
};
