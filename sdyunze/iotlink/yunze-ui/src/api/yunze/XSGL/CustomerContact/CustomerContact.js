import request from '@/utils/request'

// 客户联系记录 新增
export function NewCustomerAdd(query) {
  return request({
    url: '/yunze/CustomerContact/NewCustomer',
    method: 'post',
    data: query
  })
}
// 客户联系记录 主表
export function getList(query) {
  return request({
    url: '/yunze/CustomerContact/list',
    method: 'post',
    data: query
  })
}

// 删除 客户联系记录
export function DeleteID(query) {
  return request({
    url: '/yunze/CustomerContact/delId',
    method: 'post',
    data: query
  })
}

// 修改 客户联系记录
export function UpdateId(query) {
  return request({
    url: '/yunze/CustomerContact/updateId',
    method: 'post',
    data: query
  })
}

// 赋值 企业名称 管理员
export function AssignmentID(query) {
  return request({
    url: '/yunze/CustomerContact/assignment',
    method: 'post',
    data: query
  })
}
