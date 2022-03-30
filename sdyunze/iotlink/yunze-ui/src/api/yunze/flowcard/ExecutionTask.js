import request from '@/utils/request'

// 查询API通道列表
export function listExecutionTask(query) {
  return request({
    url: '/yunze/ExecutionTask/list',
    method: 'post',
    data: query
  })
}
