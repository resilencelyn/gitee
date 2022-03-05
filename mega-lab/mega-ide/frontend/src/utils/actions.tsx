import { message } from 'antd';
import React from 'react';

export async function handleRemove(ids: string, action: any) {
  const hide = message.loading('正在删除');
  try {
    await action(ids);
    hide();
    message.success('删除成功');
  } catch (error) {
    hide();
    message.error('删除失败');
  }
}

export async function handleAdd(fields: any, action: any) {
  const hide = message.loading('正在添加');
  try {
    await action({ ...fields });
    hide();
    message.success('新增成功');
    return true;
  } catch (error) {
    hide();
    message.error('新增失败');
    return false;
  }
};

export async function handleUpdate(id: string | undefined, fields: any, action: any) {
  const hide = message.loading('正在更新');
  try {
    await action({ id, ...fields });
    hide();
    message.success('更新成功');
    return true;
  } catch (error) {
    hide();
    message.error('更新失败');
    return false;
  }
};
