import request from '@/utils/request'

// 查询API通道列表
export function CategoryList(query) {
  return request({
    url: '/yunze/shop/prdCategory',
    method: 'post',
    data: query
  })
}

