import request from '@/utils/request'

// 查询资费组列表
export function listTariffGroup(query) {
  return request({
    url: '/yunze/tariffGroup/list',
    method: 'post',
    data: query
  })
}


// 新增 资费组
export function addtariffGroup(query) {
  return request({
    url: '/yunze/tariffGroup/add',
    method: 'post',
    data: query
  })
}


// 详情 资费组
export function findTariffGroup(query) {
  return request({
    url: '/yunze/tariffGroup/find',
    method: 'post',
    data: query
  })
}


// 编辑 资费组
export function editTariffGroup(query) {
  return request({
    url: '/yunze/tariffGroup/edit',
    method: 'post',
    data: query
  })
}


