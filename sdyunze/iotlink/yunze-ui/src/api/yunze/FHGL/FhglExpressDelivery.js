import request from '@/utils/request'
import { praseStrEmpty } from "@/utils/ruoyi";


// 快递公司信息 列表
export function getList(query) {
  return request({
    url: '/yunze/ExpressDelivery/list',
    method: 'post',
    data: query
  })
}


// 查询快递公司简要信息
export function ExpressSelDate(query) {
  return request({
    url: '/yunze/ExpressDelivery/sel_date',
    method: 'post',
    data: query
  })
}



