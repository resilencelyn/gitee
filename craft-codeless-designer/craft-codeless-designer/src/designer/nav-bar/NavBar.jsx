import {
  DeleteOutlined,
  DownloadOutlined,
  EyeOutlined,
  GithubOutlined,
  QuestionCircleOutlined,
  RedoOutlined,
  SaveOutlined,
  UndoOutlined,
} from '@ant-design/icons';
import { useEditor } from '@craftjs/core';
import { Button, Col, message, Row } from 'antd';
import styled from 'styled-components';
import CCDIcon from '../../assets/ccd-icon.svg';

const NavBarWrapper = styled.div`
  border: none;
  height: 44px;
  overflow: hidden;
  width: 100vw;
  line-height: 44px;
  padding: 0px 15px;
  background: #261c2c;
  color: #f7f7f7;

  a {
    img {
      width: 60px;
      margin-top: -8px;
      margin-left: -15px;
    }
  }

  span {
    font-size: 18px;
    color: #fff;
  }

  .versionText {
    position: absolute;
    font-size: 10px;
    padding: 2px;
    border-radius: 0;
    left: 246px;
    top: 17px;
    height: 16px;
    line-height: 12px;
    text-align: center;
    font-weight: bold;
  }

  .buttonGroup {
    button {
      background: #3e2c41;
      color: #fff;
      border: none;
      border-radius: 0;
    }
  }
`;

/**
 * @class NavBar
 *
 * The navigation bar at the top of the designer.
 *
 * @author 大漠穷秋<damoqiongqiu@126.com>
 */
export const NavBar = props => {
  const { actions, query } = useEditor();

  return (
    <NavBarWrapper>
      <Row>
        <Col span={12} style={{ textAlign: 'left' }}>
          <a href="#">
            <img src={CCDIcon}></img>
          </a>
          <span>Craft Codeless Designer</span>
          <div className="versionText">v0.20.0</div>
        </Col>
        <Col span={12} style={{ textAlign: 'end' }}>
          <Button.Group className="buttonGroup">
            <Button title="Preview the page." icon={<EyeOutlined style={{ color: '#fff' }} />}></Button>
            <Button title="Delete, hit Delete key." icon={<DeleteOutlined style={{ color: '#fff' }} />} />
            <Button title="Undo" icon={<UndoOutlined style={{ color: '#fff' }} />}></Button>
            <Button title="Redo" icon={<RedoOutlined style={{ color: '#fff' }} />}></Button>
            <Button
              title="Save"
              icon={<SaveOutlined style={{ color: '#fff' }} />}
              onClick={evt => {
                try {
                  const json = query.serialize();
                  console.log(json);
                  window.localStorage.setItem('test-data', JSON.stringify(json));
                  message.success('Data saved to window.localStorage.');
                } catch (error) {
                  console.error(error);
                  message.error('Save data failed.');
                }
              }}
            ></Button>
            <Button
              title="Load"
              icon={<DownloadOutlined style={{ color: '#fff' }} />}
              onClick={evt => {
                try {
                  const data = props.onLoadData && props.onLoadData(evt);
                  console.log(data);
                  data && actions.deserialize(data);
                  message.success('Load data success.');
                } catch (error) {
                  console.error(error);
                  message.error('Load data failed.');
                }
              }}
            ></Button>
            <Button title="Help" icon={<QuestionCircleOutlined style={{ color: '#fff' }} />}></Button>
            <Button
              title="Github"
              icon={<GithubOutlined style={{ color: '#fff' }} />}
              onClick={evt => {
                window.open('https://github.com/craft-codeless-designer/craft-codeless-designer', '_blank');
              }}
            ></Button>
          </Button.Group>
        </Col>
      </Row>
    </NavBarWrapper>
  );
};