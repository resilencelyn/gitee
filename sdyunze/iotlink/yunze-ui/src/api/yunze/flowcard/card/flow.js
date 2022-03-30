import request from '@/utils/request'

// 查询API通道列表
export function  listflow(query) {
  return request({
    url: '/yunze/cardFlow/list',
    method: 'post',
    data: query
  })
}
// 订购资费导出
export function  exportFlow(query) {
  return request({
    url: '/yunze/cardFlow/exportflow',
    method: 'post',
    data: query
  })
}
// 订购资费导出
export function ExportCardFlow(map) {
  return request({
    url: '/yunze/cardFlow/exportflow',
    method: 'post',
    data: map
  })
}
