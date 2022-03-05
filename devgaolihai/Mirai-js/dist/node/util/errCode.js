"use strict";

let errCodeMap = {
  1: '错误的 auth key',
  2: '指定的 Bot 不存在',
  3: 'Session 失效或不存在',
  4: 'Session未认证(未激活)',
  5: '发送消息目标不存在(指定对象不存在)',
  6: '指定文件不存在，出现于发送本地图片',
  10: '无操作权限，指 Bot 没有对应操作的限权',
  20: 'Bot 被禁言，指 Bot 当前无法向指定群发送消息',
  30: '消息过长',
  400: '错误的访问，可能是参数错误'
};
let errCodeEnum = {
  get BOT_NOT_FOUND() {
    return 2;
  } // 需要的时候随时加


};
module.exports = {
  errCodeMap,
  errCodeEnum
};