import request from '@/utils/request'

// 查询API通道列表
export function  listCcUrl(query) {
  return request({
    url: '/yunze/ccurl/list',
    method: 'post',
    data: query
  })
}

export function  AddCcUrl(query) {
  return request({
    url: '/yunze/ccurl/add',
    method: 'post',
    data: query
  })
}

export function  UpdateCcUrl(query) {
  return request({
    url: '/yunze/ccurl/update',
    method: 'post',
    data: query
  })
}

export function  deleteUrl(query) {
  return request({
    url: '/yunze/ccurl/deleteurl',
    method: 'post',
    data: query
  })
}
















