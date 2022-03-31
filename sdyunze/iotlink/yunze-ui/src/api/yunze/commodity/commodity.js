import request from '@/utils/request'

// 查询
export function SelMap(query) {
  return request({
    url: '/yunze/commodity/SelMap',
    method: 'post',
    data: query
  })
}



// 新增
export function add(query) {
  return request({
    url: '/yunze/commodity/add',
    method: 'post',
    data: query
  })
}

// 编辑
export function edit(query) {
  return request({
    url: '/yunze/commodity/upd',
    method: 'post',
    data: query
  })
}



// 查询单条数据
export function findData(query) {
  return request({
    url: '/yunze/commodity/find',
    method: 'post',
    data: query
  })
}

