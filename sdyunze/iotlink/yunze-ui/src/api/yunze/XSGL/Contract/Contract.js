import request from '@/utils/request'
import { praseStrEmpty } from "@/utils/ruoyi";


// 销售合同 列表
export function getList(query) {
  return request({
    url: '/yunze/XsglContract/list',
    method: 'post',
    data: query
  })
}

// 销售合同 新增
export function add(query) {
  return request({
    url: '/yunze/XsglContract/add',
    method: 'post',
    data: query
  })
}

// 销售合同 查询单条
export function find(query) {
  return request({
    url: '/yunze/XsglContract/find',
    method: 'post',
    data: query
  })
}

// 销售合同 修改
export function upd(query) {
  return request({
    url: '/yunze/XsglContract/upd',
    method: 'post',
    data: query
  })
}






