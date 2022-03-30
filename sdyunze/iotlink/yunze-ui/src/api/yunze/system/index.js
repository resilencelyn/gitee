import request from '@/utils/request'

// 查询首页信息
export function  findToDay(query) {
  return request({
    url: '/yunze/index/findToDay',
    method: 'post',
    data: query
  })
}

