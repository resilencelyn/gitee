import React, { useState } from 'react';
import { Statistic } from 'antd';
import ProCard from '@ant-design/pro-card';
import RcResizeObserver from 'rc-resize-observer';
import { PageContainer } from '@ant-design/pro-layout';
import { Chart } from '@antv/g2';

const { Divider } = ProCard;
import { useKeycloak } from '@react-keycloak/web';

export default () => {
  const [responsive, setResponsive] = useState(false);
  const { keycloak, initialized } = useKeycloak();
  return (
    <RcResizeObserver
      key='resize-observer'
      onResize={(offset) => {
        setResponsive(offset.width < 596);
      }}
    >
      <ProCard.Group title='系统概览' direction={responsive ? 'column' : 'row'}>
        <ProCard>
          <Statistic title='CPU总数' value={79.0} precision={2} />
        </ProCard>
        <Divider type={responsive ? 'horizontal' : 'vertical'} />
        <ProCard>
          <Statistic title='内存总数' value={112893.0} precision={2} />
        </ProCard>
        <Divider type={responsive ? 'horizontal' : 'vertical'} />
        <ProCard>
          <Statistic title='磁盘总容量' value={93} suffix='/ 100' />
        </ProCard>
      </ProCard.Group>

      <ProCard.Group title='资源概览' direction={responsive ? 'column' : 'row'}>
        <ProCard>
          <Statistic title='CPU使用率' value={79.0} precision={2} />
          <div id='container' />
        </ProCard>
        <Divider type={responsive ? 'horizontal' : 'vertical'} />
        <ProCard>
          <Statistic title='内存使用率' value={112893.0} precision={2} />
        </ProCard>
        <Divider type={responsive ? 'horizontal' : 'vertical'} />
        <ProCard>
          <Statistic title='磁盘使用率' value={93} suffix='/ 100' />
        </ProCard>
      </ProCard.Group>
    </RcResizeObserver>
  );
};
