import request from '@/utils/request'

// 查询列表
export function Inventorydetails(query) {
  return request({
    url: '/yunze/inventorydetails/list',
    method: 'post',
    data: query
  })
}

// 库存明细
export function ExportInventorydetails(map) {
  return request({
    url: '/yunze/inventorydetails/Exportdetails',
    method: 'post',
    data: map
  })
}


