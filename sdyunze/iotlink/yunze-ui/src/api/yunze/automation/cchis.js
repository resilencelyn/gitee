import request from '@/utils/request'

// 查询API通道列表
export function  listCcHis(query) {
  return request({
    url: '/yunze/cchis/list',
    method: 'post',
    data: query
  })
}
