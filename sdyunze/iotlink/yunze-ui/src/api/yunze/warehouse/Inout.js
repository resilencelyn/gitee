import request from '@/utils/request'

// 商品多选框查询
export function Commodity(query) {
  return request({
    url: '/yunze/commodity/list',
    method: 'post',
    data: query
  })
}
// 载入查询
export function Queryloading(query) {
  return request({
    url: '/yunze/commodity/queryloading',
    method: 'post',
    data: query
  })
}
// 查询列表
export function listStorage(query) {
  return request({
    url: '/yunze/Warehousing/liststorage',
    method: 'post',
    data: query
  })
}

// 查询列表
export function DetailedID(query) {
  return request({
    url: '/yunze/Warehousing/detailed',
    method: 'post',
    data: query
  })
}
