import request from '@/utils/request'
import { praseStrEmpty } from "@/utils/ruoyi";


// 客户信息 列表
export function getList(query) {
  return request({
    url: '/yunze/XsglCustomer/list',
    method: 'post',
    data: query
  })
}

// 客户信息 新增
export function add(query) {
  return request({
    url: '/yunze/XsglCustomer/add',
    method: 'post',
    data: query
  })
}

// 客户信息 查询单条
export function find(query) {
  return request({
    url: '/yunze/XsglCustomer/find',
    method: 'post',
    data: query
  })
}

// 客户信息 修改
export function upd(query) {
  return request({
    url: '/yunze/XsglCustomer/upd',
    method: 'post',
    data: query
  })
}


// 客户信息 高级修改
export function updInfo(query) {
  return request({
    url: '/yunze/XsglCustomer/updInfo',
    method: 'post',
    data: query
  })
}


// 查询所属客户 合伙人
export function findSalesPartner(query) {
  return request({
    url: '/yunze/XsglCustomer/findSalesPartner',
    method: 'post',
    data: query
  })
}



// 查询 总管理 或 非总管理 下  客户
export function CustomerDividefind(query) {
  return request({
    url: '/yunze/XsglCustomer/CustomerDividefind',
    method: 'post',
    data: query
  })
}



// 划分 客户
export function CustomerDivide(query) {
  return request({
    url: '/yunze/XsglCustomer/CustomerDivide',
    method: 'post',
    data: query
  })
}


// 费率设置 客户
export function CustomerRate(query) {
  return request({
    url: '/yunze/XsglCustomer/CustomerRate',
    method: 'post',
    data: query
  })
}



// 客户 简要信息查询
export function findCustomerArr(query) {
  return request({
    url: '/yunze/XsglCustomer/findCustomerArr',
    method: 'post',
    data: query
  })
}

// 查询 客户地址简要信息
export function fnidAccountArr(query) {
  return request({
    url: '/yunze/XsglCustomer/fnidAccountArr',
    method: 'post',
    data: query
  })
}

// 查询  简要联系人信息
export function fnidPeopleArr(query) {
  return request({
    url: '/yunze/XsglCustomer/fnidPeopleArr',
    method: 'post',
    data: query
  })
}

// 联系人 编辑
export function ContactsList(query) {
  return request({
    url: '/yunze/XsglCustomer/contacts',
    method: 'post',
    data: query
  })
}

// 查询 客户 加载图片信息
export function urlPictureArr(query) {
  return request({
    url: '/yunze/XsglCustomer/urlPicture',
    method: 'post',
    data: query
  })
}

// 查询 客户图片列表
export function contactsListA(query) {
  return request({
    url: '/yunze/XsglCustomer/contactsList',
    method: 'post',
    data: query
  })
}


// 删除单个图片
export function DeleSingle(query) {
  return request({
    url: '/yunze/XsglCustomer/single',
    method: 'post',
    data: query
  })
}


// 修改 图片状态 无效
export function UpdStatus(query) {
  return request({
    url: '/yunze/XsglCustomer/switchStatus',
    method: 'post',
    data: query
  })
}

// 修改 图片状态 有效
export function EffectiveStatus(query) {
  return request({
    url: '/yunze/XsglCustomer/effective',
    method: 'post',
    data: query
  })
}

// 查询 联系人图片列表
export function LiaisonList(query) {
  return request({
    url: '/yunze/XsglCustomer/liaison',
    method: 'post',
    data: query
  })
}

// 删除联系人单个图片
export function Deurl(query) {
  return request({
    url: '/yunze/XsglCustomer/deurl',
    method: 'post',
    data: query
  })
}

// 修改 联系人 无效状态
export function Invalid(query) {
  return request({
    url: '/yunze/XsglCustomer/invalid',
    method: 'post',
    data: query
  })
}

// 修改 联系人 有效状态
export function Effectivesta(query) {
  return request({
    url: '/yunze/XsglCustomer/effectivesta',
    method: 'post',
    data: query
  })
}






