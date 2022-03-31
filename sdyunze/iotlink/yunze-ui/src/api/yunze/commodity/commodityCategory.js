import request from '@/utils/request'

// 查询API通道列表
export function listCategory(query) {
  return request({
    url: '/yunze/prdCategory/list',
    method: 'post',
    data: query
  })
}




// 新增
export function save(query) {
  return request({
    url: '/yunze/prdCategory/save',
    method: 'post',
    data: query
  })
}




// 修改
export function upd(query) {
  return request({
    url: '/yunze/prdCategory/upd',
    method: 'post',
    data: query
  })
}


