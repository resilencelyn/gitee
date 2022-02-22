import { ProFormDependency, ProFormSelect, ProFormText } from '@ant-design/pro-form';
import { listIdeEnvironmentOptions } from '@/pages/IdeEnvironment/api';
import { listIdeImageOptions } from '../IdeImage/api';
import { listIdeRegistryOptions } from '../IdeRegistry/api';
import { Form } from 'antd';

export const formChildren = () => {
  return (
    <div>
      <Form.Item noStyle shouldUpdate>
        {(form) => {
          return (
            <ProFormSelect
              name='ide_registry_id'
              label='镜像源'
              rules={[
                { required: true, message: '请选择镜像源' },
              ]}
              request={listIdeRegistryOptions}
              fieldProps={{
                onChange: () => form.setFieldsValue({ ide_image_id: undefined }),
              }}
            />
          );
        }}
      </Form.Item>
      <ProFormDependency name={['ide_registry_id']}>
        {({ ide_registry_id }) => {
          return (
            <ProFormSelect
              name='ide_image_id'
              label='镜像'
              rules={[
                { required: true, message: '请选择镜像' },
              ]}
              params={{ ide_registry_id }}
              request={() => ide_registry_id ? listIdeImageOptions(ide_registry_id) : new Promise((resolve) => resolve([]))}
            />
          )
        }}
      </ProFormDependency>
      <ProFormSelect
        name='ide_environment_id'
        label='环境'
        rules={[
          { required: true, message: '请选择环境' },
        ]}
        request={listIdeEnvironmentOptions}
      />
      <ProFormText
        name='name'
        label='名称'
        rules={[
          { required: true, message: '请选择IDE名称' },
        ]}
      />
    </div>
  );
};
