import request from '@/utils/request'

// 查询列表
export function listOutku(query) {
  return request({
    url: '/yunze/output/listoutku',
    method: 'post',
    data: query
  })
}

// 新增出库信息
export function OutofWarehouse(query) {
  return request({
    url: '/yunze/output/outofWarehouse',
    method: 'post',
    data: query
  })
}
