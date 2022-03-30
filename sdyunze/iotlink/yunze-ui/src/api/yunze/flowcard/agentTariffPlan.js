import request from '@/utils/request'



// 新增 资费计划
export function AddagentTariffPlan(query) {
  return request({
    url: '/yunze/agentTariffPlan/add',
    method: 'post',
    data: query
  })
}




// 详情 资费计划
export function findagentTariffPlan(query) {
  return request({
    url: '/yunze/agentTariffPlan/find',
    method: 'post',
    data: query
  })
}

// 编辑 资费计划
export function editagentTariffPlan(query) {
  return request({
    url: '/yunze/agentTariffPlan/edit',
    method: 'post',
    data: query
  })
}
