import request from '@/utils/request'


// 入款记录 列表
export function listRemittance(query) {
  return request({
    url: '/yunze/remittance/list',
    method: 'post',
    data: query
  })
}
// 新增 入款信息
export function AddRemittance(query) {
  return request({
    url: '/yunze/remittance/addIncome',
    method: 'post',
    data: query
  })
}

// 查询 入款信息详情表
export function SleIncome(query) {
  return request({
    url: '/yunze/remittance/slefind',
    method: 'post',
    data: query
  })
}
// 新增 入款信息详情表
export function AddIncome(query) {
  return request({
    url: '/yunze/remittance/addfind',
    method: 'post',
    data: query
  })
}
// 删除 入款信息详情表
export function DeleteIncome(query) {
  return request({
    url: '/yunze/remittance/deletefind',
    method: 'post',
    data: query
  })
}
// 查询本年入款总计和为入款
export function TotalfortIncome(query) {
  return request({
    url: '/yunze/remittance/Totalfortheyear',
    method: 'post',
    data: query
  })
}
// 详情数据查看
export function DeteilsIncome(query) {
  return request({
    url: '/yunze/remittance/sledeteils',
    method: 'post',
    data: query
  })
}
// 查询月款总计和为入款
export function AmountOfMoneyList(query) {
  return request({
    url: '/yunze/remittance/amountofmoney',
    method: 'post',
    data: query
  })
}
// 入款信息导出
export function ExportRemittance(map) {
  return request({
    url: '/yunze/remittance/ExportRemittance',
    method: 'post',
    data: map
  })
}
// 入款详情导出
export function ExportIncome(map) {
  return request({
    url: '/yunze/remittance/ExportIncome',
    method: 'post',
    data: map
  })
}
