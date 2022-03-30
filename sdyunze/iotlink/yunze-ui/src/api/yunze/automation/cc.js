import request from '@/utils/request'

// 查询API通道列表
export function  listCc(query) {
  return request({
    url: '/yunze/cc/list',
    method: 'post',
    data: query
  })
}
export function  AddCc(query) {
  return request({
    url: '/yunze/cc/add',
    method: 'post',
    data: query
  })
}

// 查询
export function NameAdd(map) {
  return request({
    url: '/yunze/cc/nameadd',
    method: 'post',
    data: map
  })
}
// 修改
export function  UpdateCc(query) {
  return request({
    url: '/yunze/cc/ccupdate',
    method: 'post',
    data: query
  })
}










