import request from '@/utils/request'


// 公司信息 列表
export function getList(query) {
  return request({
    url: '/yunze/CwglFirm/list',
    method: 'post',
    data: query
  })
}

// 公司信息 新增
export function add(query) {
  return request({
    url: '/yunze/CwglFirm/add',
    method: 'post',
    data: query
  })
}

// 公司信息 查询单条
export function find(query) {
  return request({
    url: '/yunze/CwglFirm/find',
    method: 'post',
    data: query
  })
}

// 公司信息 修改
export function upd(query) {
  return request({
    url: '/yunze/CwglFirm/upd',
    method: 'post',
    data: query
  })
}

// 查询简要 公司信息
export function findArr(query) {
  return request({
    url: '/yunze/CwglFirm/findArr',
    method: 'post',
    data: query
  })
}
// 删除 Fat_ID
export function  deleteFat_ID(query) {
  return request({
    url: '/yunze/CwglFirm/deletefatid',
    method: 'post',
    data: query
  })
}
// 真实删除 Fat_ID
export function  deleteFat_IDArr(query) {
  return request({
    url: '/yunze/CwglFirm/deletefatidarr',
    method: 'post',
    data: query
  })
}

// 单独 查询 公司名称
export function  CompanyName(query) {
  return request({
    url: '/yunze/CwglFirm/companyname',
    method: 'post',
    data: query
  })
}
