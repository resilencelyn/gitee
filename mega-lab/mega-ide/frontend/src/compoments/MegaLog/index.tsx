import React, { useState, useRef, useEffect, useImperativeHandle, forwardRef } from 'react';
import { DrawerForm } from '@ant-design/pro-form';
import { RequestResponse } from 'umi-request';
import { Spin } from 'antd';
import styles from './index.less';

export default forwardRef((props: {
  id: string;
  status: number;
  action: (id: string) => Promise<RequestResponse<string>>;
}, ref: any) => {
  const { id, status, action } = props;
  const [visible, setVisible] = useState<boolean>(false);

  const logsRef = useRef<HTMLDivElement>(null);
  const [scrollTop, setScrollTop] = useState<number>(0);
  const [lockScroll, setLockScroll] = useState<boolean>(false);

  const [buildInfo, setBuildInfo] = useState<string[]>([]);
  const [timerId, setTimerId] = useState<NodeJS.Timer>();

  const injectBuildInfo = async (id: string) => {
    const buildResult = await action(id);
    const rows = buildResult.data.split('\n').filter((row: string) => !!row);
    setBuildInfo(rows);
  }

  const showBuildInfo = async () => {
    setVisible(true);
    injectBuildInfo(id);
  };

  const onScroll = () => {
    if (logsRef && logsRef.current) {
      const dom = logsRef.current;
      if (dom.scrollTop < dom.scrollHeight) {
        setLockScroll(false);
      } else {
        setLockScroll(true);
      }
    }
  };

  const scrollToBottom = () => {
    if (logsRef && logsRef.current) {
      const dom = logsRef.current;
      if (dom.scrollHeight !== scrollTop && !lockScroll) {
        dom.scrollTop = dom.scrollHeight;
        setScrollTop(dom.scrollHeight);
      }
    }
  };

  useImperativeHandle(ref, () => ({
    show: showBuildInfo
  }))

  useEffect(() => {
    if (visible) {
      const newTimerId = setInterval(() => {
        return injectBuildInfo(id)
      }, 2000)
      setTimerId(newTimerId);
    } else {
      if (timerId) clearInterval(timerId)
    }
  }, [visible]);

  useEffect(() => {
    if ([2, 3].includes(status) && timerId) {
      clearInterval(timerId)
    }
  }, [status])

  useEffect(() => {
    scrollToBottom();
  }, [buildInfo.length]);

  return (
    <DrawerForm
      title="构建详情"
      visible={visible}
      submitter={false}
      onVisibleChange={setVisible}
      drawerProps={{ bodyStyle: { padding: 0 } }}
    >
      <div
        ref={logsRef}
        className={styles.logs}
        onScroll={onScroll}
      >
        {
          buildInfo.map((row, index) => (
            <p className={styles.row} key={index}>{row}</p>
          ))
        }
        { status === 1 && <Spin spinning size='small'/>}
      </div>
    </DrawerForm>
  );
})

