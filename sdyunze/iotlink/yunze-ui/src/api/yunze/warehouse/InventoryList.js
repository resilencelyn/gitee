import request from '@/utils/request'

// 查询
export function findlist(query) {
  return request({
    url: '/yunze/InventoryList/list',
    method: 'post',
    data: query
  })
}

// 修改
export function updInfo(query) {
  return request({
    url: '/yunze/InventoryList/upd',
    method: 'post',
    data: query
  })
}

