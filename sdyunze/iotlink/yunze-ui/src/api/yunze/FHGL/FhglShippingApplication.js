import request from '@/utils/request'


// 订单合同发货 列表
export function getList(query) {
  return request({
    url: '/yunze/ShippingApplication/list',
    method: 'post',
    data: query
  })
}


// 发货申请 新增
export function add(query) {
  return request({
    url: '/yunze/ShippingApplication/add',
    method: 'post',
    data: query
  })
}


// 查询商品已发货数量
export function getQuantityShipped(query) {
  return request({
    url: '/yunze/ShippingApplication/sel_SUM_Sent_Quantity',
    method: 'post',
    data: query
  })
}

// 发货申请查询 合同 数据
export function findContractData(query) {
  return request({
    url: '/yunze/XsglContract/find_FH_data',
    method: 'post',
    data: query
  })
}


// 查询 发货申请详情
export function findData(query) {
  return request({
    url: '/yunze/ShippingApplication/findData',
    method: 'post',
    data: query
  })
}

// 修改 发货申请
export function Updinfo(query) {
  return request({
    url: '/yunze/ShippingApplication/Updinfo',
    method: 'post',
    data: query
  })
}

// 删除 发货申请
export function DelInfo(query) {
  return request({
    url: '/yunze/ShippingApplication/DelInfo',
    method: 'post',
    data: query
  })
}


// 新增 发货登记
export function Register(query) {
  return request({
    url: '/yunze/ExpressInquiry/Register',
    method: 'post',
    data: query
  })
}

// 查询 单号
export function OddNumbers(query) {
  return request({
    url: '/yunze/ShippingApplication/oddNumbers',
    method: 'post',
    data: query
  })
}


// 查询 出库 合同发货 && 普通发货
export function findOut(query) {
  return request({
    url: '/yunze/ShippingApplication/findOut',
    method: 'post',
    data: query
  })
}


