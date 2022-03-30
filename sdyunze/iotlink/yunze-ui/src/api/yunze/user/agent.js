import request from '@/utils/request'
import { praseStrEmpty } from "@/utils/ruoyi";


// 新增 资费计划
export function AddtariffPlan(query) {
  return request({
    url: '/yunze/tariffPlan/add',
    method: 'post',
    data: query
  })
}



// 划分资费组查询 简要
export function findPackage(query) {
  return request({
    url: '/yunze/tariffGroup/findPackage',
    method: 'post',
    data: query
  })
}




// 划分资费计划查询 简要
export function findPacket(query) {
  return request({
    url: '/yunze/tariffPlan/findPacket',
    method: 'post',
    data: query
  })
}


// 平台资费组 划分
export function tariffDivision(query) {
  return request({
    url: '/yunze/tariffGroup/tariffDivision',
    method: 'post',
    data: query
  })
}


// 企业 资费组 划分
export function agentTariffDivision(query) {
  return request({
    url: '/yunze/tariffGroup/agentTariffDivision',
    method: 'post',
    data: query
  })
}


// 查询用户列表
export function listUser(query) {
  return request({
    url: '/yunze/agent/list',
    method: 'post',
    data: query
  })
}

// 查询用户详细
export function getUser(userId) {
  return request({
    url: '/yunze/agent/' + praseStrEmpty(userId),
    method: 'get'
  })
}

// 新增用户
export function addUser(data) {
  return request({
    url: '/yunze/agent',
    method: 'post',
    data: data
  })
}

// 修改用户
export function updateUser(data) {
  return request({
    url: '/yunze/agent',
    method: 'put',
    data: data
  })
}

// 删除用户
export function delUser(userId) {
  return request({
    url: '/yunze/agent/' + userId,
    method: 'delete'
  })
}

// 导出用户
export function exportUser(query) {
  return request({
    url: '/yunze/agent/export',
    method: 'get',
    params: query
  })
}

// 用户密码重置
export function resetUserPwd(userId, password) {
  const data = {
    userId,
    password
  }
  return request({
    url: '/yunze/agent/resetPwd',
    method: 'put',
    data: data
  })
}

// 用户状态修改
export function changeUserStatus(userId, status) {
  const data = {
    userId,
    status
  }
  return request({
    url: '/yunze/agent/changeStatus',
    method: 'put',
    data: data
  })
}

// 查询用户个人信息
export function getUserProfile() {
  return request({
    url: '/yunze/agent/profile',
    method: 'get'
  })
}

// 修改用户个人信息
export function updateUserProfile(data) {
  return request({
    url: '/yunze/agent/profile',
    method: 'put',
    data: data
  })
}

// 用户密码重置
export function updateUserPwd(oldPassword, newPassword) {
  const data = {
    oldPassword,
    newPassword
  }
  return request({
    url: '/yunze/agent/profile/updatePwd',
    method: 'put',
    params: data
  })
}

// 用户头像上传
export function uploadAvatar(data) {
  return request({
    url: '/yunze/agent/profile/avatar',
    method: 'post',
    data: data
  })
}

// 下载用户导入模板
export function importTemplate() {
  return request({
    url: '/yunze/agent/importTemplate',
    method: 'get'
  })
}
