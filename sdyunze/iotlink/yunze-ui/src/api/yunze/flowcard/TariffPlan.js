import request from '@/utils/request'



// 新增 资费计划
export function AddtariffPlan(query) {
  return request({
    url: '/yunze/tariffPlan/add',
    method: 'post',
    data: query
  })
}




// 详情 资费计划
export function findTariffPlan(query) {
  return request({
    url: '/yunze/tariffPlan/find',
    method: 'post',
    data: query
  })
}

// 编辑 资费计划
export function editTariffPlan(query) {
  return request({
    url: '/yunze/tariffPlan/edit',
    method: 'post',
    data: query
  })
}
