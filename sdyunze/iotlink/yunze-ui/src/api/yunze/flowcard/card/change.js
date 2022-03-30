import request from '@/utils/request'

// 查询API通道列表
export function  listChange(query) {
  return request({
    url: '/yunze/change/list',
    method: 'post',
    data: query
  })
}
export function  getselTime(query) {
  return request({
    url: 'yunze/change/info',
    method: 'post',
    data: query
  })
}
