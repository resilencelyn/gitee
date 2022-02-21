import { Tag } from 'antd';
import React from 'react';

export function renderEnableStatue(status: number) {
  if (status == 0) {
    return (
      <Tag color='default'>
        禁用
      </Tag>
    );
  }
  if (status == 1) {
    return (
      <Tag color='success'>
        启用
      </Tag>
    );
  }
}
