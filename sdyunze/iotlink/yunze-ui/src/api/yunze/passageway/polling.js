import request from '@/utils/request'

// 查询API通道列表
export function  listSysLog(query) {
  return request({
    url: '/yunze/polling/list',
    method: 'post',
    data: query
  })
}
