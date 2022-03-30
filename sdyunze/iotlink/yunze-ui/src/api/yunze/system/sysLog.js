import request from '@/utils/request'

// 查询API通道列表
export function  listSysLog(query) {
  return request({
    url: '/yunze/sysLog/list',
    method: 'post',
    data: query
  })
}
export function  getByIds(query) {
  return request({
    url: 'yunze/sysLog/findSys',
    method: 'post',
    data: query
  })
}
