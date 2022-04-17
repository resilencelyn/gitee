import Connect from "../Connect";

/**
 * 渲染器接口
 */
export default interface IParser{
    /**
     * 当连接成功时调用的方法
     */
    onConnected():void
}

