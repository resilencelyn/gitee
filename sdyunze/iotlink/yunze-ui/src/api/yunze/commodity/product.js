import request from '@/utils/request'

// 查询API通道列表
export function listProduct(query) {
  return request({
    url: '/yunze/productInfo/list',
    method: 'post',
    data: query
  })
}



// 获取 商品分类
export function getCategory(query) {
  return request({
    url: '/yunze/prdCategory/getCategory',
    method: 'post',
    data: query
  })
}




// 新增
export function save(query) {
  return request({
    url: '/yunze/productInfo/add',
    method: 'post',
    data: query
  })
}

// 编辑
export function edit(query) {
  return request({
    url: '/yunze/productInfo/edit',
    method: 'post',
    data: query
  })
}



// 详情
export function findData(query) {
  return request({
    url: '/yunze/productInfo/findData',
    method: 'post',
    data: query
  })
}



// 获取商品 代理详情
export function findAgentList(query) {
  return request({
    url: '/yunze/productAgent/list',
    method: 'post',
    data: query
})
}
// 商品代理 编辑
export function AgentEdit(query) {
  return request({
    url: '/yunze/productAgent/edit',
    method: 'post',
    data: query
})
}


// 商品代理 编辑
export function PrdImgfindList(query) {
  return request({
    url: '/yunze/productPic/findList',
    method: 'post',
    data: query
  })
}

// 图片下载
export function readImage(query) {
  return request({
    url: '/yunze/productPic/readImage',
    method: 'get',
    data: query
  })
}


