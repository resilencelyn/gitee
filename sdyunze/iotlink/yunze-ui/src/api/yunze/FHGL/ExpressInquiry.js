import request from '@/utils/request'
import { praseStrEmpty } from "@/utils/ruoyi";


// 快递查询 列表
export function getList(query) {
  return request({
    url: '/yunze/ExpressInquiry/list',
    method: 'post',
    data: query
  })
}


// 快递查询 详情
export function FindInfo(query) {
  return request({
    url: '/yunze/ExpressInquiry/FindInfo',
    method: 'post',
    data: query
  })
}


// 快递查询 修改
export function UpdInfo(query) {
  return request({
    url: '/yunze/ExpressInquiry/UpdInfo',
    method: 'post',
    data: query
  })
}


