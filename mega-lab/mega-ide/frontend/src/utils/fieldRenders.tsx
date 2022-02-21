import { Tag } from 'antd';
import {
  CheckCircleOutlined,
  MinusCircleOutlined,
  SyncOutlined,
} from '@ant-design/icons';
import React from 'react';

export function renderRunningStatue(status: number) {
  if (status == 0) {
    return <Tag color="default">未运行</Tag>;
  }
  if (status == 1) {
    return (
      <Tag icon={<SyncOutlined spin />} color="processing">
        启动中
      </Tag>
    );
  }
  if (status == 2) {
    return <Tag color="success">运行中</Tag>;
  }
  if (status == 3) {
    return (
      <Tag icon={<SyncOutlined spin />} color="processing">
        停止中
      </Tag>
    );
  }
}

export function renderAllowStatue(status: number) {
  if (status == 0) {
    return <Tag color="default">不允许</Tag>;
  }
  if (status == 1) {
    return <Tag color="success">允许</Tag>;
  }
}
