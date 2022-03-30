import request from '@/utils/request'

// 查询API通道列表
export function  listSysLog(query) {
  return request({
    url: '/yunze/flowhis/list',
    method: 'post',
    data: query
  })
}
export function  listgetbyid(query) {
  return request({
    url: '/yunze/flowhis/find',
    method: 'post',
    data: query
  })
}

// 用量详情导出 （同步导出 分组 、备注、发货时间）
export function ExportCardFlowHis(map) {
  return request({
    url: '/yunze/flowhis/exportflowhis',
    method: 'post',
    data: map
  })
}
