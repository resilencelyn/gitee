import request from '@/utils/request'

// 查询
export function listOrder(query) {
  return request({
    url: '/yunze/order/list',
    method: 'post',
    data: query
  })
}


// 查询 资费计划 信息
export function queryPacketSimple(query) {
  return request({
    url: '/yunze/order/queryPacketSimple',
    method: 'post',
    data: query
  })
}

// 查询 订单详情
export function findOrder(query) {
  return request({
    url: '/yunze/order/findOrder',
    method: 'post',
    data: query
  })
}


// 获取商品 名称
export function findName(query) {
  return request({
    url: '/yunze/productInfo/findName',
    method: 'post',
    data: query
  })
}


//  查询发货订单
export function findWxOrder(query) {
  return request({
    url: '/yunze/WxByOrder/findWxOrder',
    method: 'post',
    data: query
  })
}

// 全部导出
export function Exportallorders(map) {
  return request({
    url: '/yunze/order/outOrder',
    method: 'post',
    data: map
  })
}

//  购物订单导出
export function Shoppingexport(query) {
  return request({
    url: '/yunze/WxByOrder/shoppingexport',
    method: 'post',
    data: query
  })
}




