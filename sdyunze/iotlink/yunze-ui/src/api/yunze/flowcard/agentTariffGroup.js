import request from '@/utils/request'

// 查询资费组列表
export function listagentTariffGroup(query) {
  return request({
    url: '/yunze/agentTariffGroup/list',
    method: 'post',
    data: query
  })
}


// 新增 资费组
export function addagentTariffGroup(query) {
  return request({
    url: '/yunze/agentTariffGroup/add',
    method: 'post',
    data: query
  })
}


// 详情 资费组
export function findagentTariffGroup(query) {
  return request({
    url: '/yunze/agentTariffGroup/find',
    method: 'post',
    data: query
  })
}


// 编辑 资费组
export function editagentTariffGroup(query) {
  return request({
    url: '/yunze/agentTariffGroup/edit',
    method: 'post',
    data: query
  })
}


