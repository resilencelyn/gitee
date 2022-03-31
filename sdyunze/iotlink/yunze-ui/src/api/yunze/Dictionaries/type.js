import request from '@/utils/request'

// 出库入库
export function Administration(query) {
  return request({
    url: '/yunze/Administration/type',
    method: 'post',
    data: query
  })
}

// 插入数据库信息
export function Warehousingadd(query) {
  return request({
    url: '/yunze/Warehousing/information',
    method: 'post',
    data: query
  })
}
