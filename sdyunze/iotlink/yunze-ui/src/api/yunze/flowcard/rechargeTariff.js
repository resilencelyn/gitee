import request from '@/utils/request'




// 查询单条 资费计划  简要
export function FindPacket(query) {
  return request({
    url: '/yunze/cardFlow/FindPacket',
    method: 'post',
    data: query
  })
}






