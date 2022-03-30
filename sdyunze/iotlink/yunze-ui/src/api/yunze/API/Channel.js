import request from '@/utils/request'

// 查询API通道列表
export function listCardRoute(query) {
  return request({
    url: '/yunze/cardRoute/list',
    method: 'post',
    data: query
  })
}

// 查询API通道详细
export function getCardRoute(map) {
  return request({
    url: '/yunze/cardRoute/find',
    method: 'post',
    data: map
  })
}


// 新增 API通道信息
export function addCardRoute(map) {
  return request({
    url: '/yunze/cardRoute/add' ,
    method: 'post',
    data: map
  })
}



// 编辑 API通道信息
export function editCardRoute(data) {
  return request({
    url: '/yunze/cardRoute/edit',
    method: 'post',
    data: data
  })
}

// 编辑 API通道信息
export function queryflowData(map) {
  return request({
    url: '/yunze/cardRouteFlow/queryflow' ,
    method: 'post',
    data: map
  })
}

// 导出API通道信息
export function exportcardRoute(map) {
  return request({
    url: '/yunze/cardRoute/exportData',
    method: 'post',
    data: map
  })
}




// 删除 API通道信息
export function delCardRoute(map) {
  return request({
    url: '/yunze/cardRoute/delRoute',
    method: 'post',
    data: map
  })
}





