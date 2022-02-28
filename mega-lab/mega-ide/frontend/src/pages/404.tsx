import { Button, Result } from 'antd';
import { history } from 'umi';

export default () => (
  <Result
    style={{ background: '#fff' }}
    status="404"
    title="404"
    subTitle="抱歉，您访问的页面不存在"
    extra={
      <Button type="primary" onClick={() => history.push('/')}>
        返回主页
      </Button>
    }
  />
);
