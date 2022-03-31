import request from '@/utils/request'

// 查询连接管理
export function listCard(query) {
  return request({
    url: '/yunze/card/list',
    method: 'post',
    data: query
  })
}

// 查询卡板详细
export function getCard(map) {
  return request({
    url: '/yunze/card/find',
    method: 'post',
    data: map
  })
}


// 查询当前套餐简要信息
export function queryflowSimple(map) {
  return request({
    url: '/yunze/cardFlow/queryflowSimple' ,
    method: 'post',
    data: map
  })
}


// 查询在线信息
export function queryOnlineStatus(map) {
  return request({
    url: '/yunze/card/queryOnlineStatus' ,
    method: 'post',
    data: map
  })
}



// 获取资费组名称
export function queryPackageSimple() {
  return request({
    url: '/yunze/cardFlow/queryPackageSimple',
    method: 'post'
  })
}



// 获取公司名称
export function getDeptName(data) {
  return request({
    url: '/yunze/card/getDeptName',
    method: 'post',
    data: data
  })
}

// 获取套餐信息
export function queryflowData(map) {
  return request({
    url: '/yunze/cardFlow/queryflow' ,
    method: 'post',
    data: map
  })
}

// 导出卡板信息
export function exportCard(map) {
  return request({
    url: '/yunze/card/exportData',
    method: 'post',
    data: map
  })
}

// 查询 企业下所属人员
export function findDeptUser(map) {
  return request({
    url: '/yunze/card/findDeptUser',
    method: 'post',
    data: map
  })
}

export function DivideCard(map) {
  return request({
    url: '/yunze/card/divide',
    method: 'post',
    data: map
  })
}



// 查询简要 通道信息
export function findSp(map) {
  return request({
    url: '/yunze/cardRoute/findSp',
    method: 'post',
    data: map
  })
}


// 查询 通道简要信息  状态为 正常 划分通道用
export function findCr(map) {
  return request({
    url: '/yunze/cardRoute/findCr',
    method: 'post',
    data: map
  })
}



// 下载卡板导入模板
export function importTemplate() {
  return request({
    url: '/yunze/card/importTemplate',
    method: 'get'
  })
}



// 查询卡板详细
export function SynCarStatus(map) {
  return request({
    url: '/yunze/card/SynStatus',
    method: 'post',
    data: map
  })
}


// 查询卡板详细
export function SynCarFlow(map) {
  return request({
    url: '/yunze/card/SynFlow',
    method: 'post',
    data: map
  })
}


// 机卡解绑
export function CarUntie(map) {
  return request({
    url: '/yunze/card/CardBinding',
    method: 'post',
    data: map
  })
}

// 获取 所属代理下 卡分组
export function getCardGrouping(map) {
  return request({
    url: '/yunze/card/getCardGrouping',
    method: 'post',
    data: map
  })
}

// 修改 备注 分组
export function UpdateFill(map) {
  return request({
    url: '/yunze/card/updatefill',
    method: 'post',
    data: map
  })
}

// 同步 激活时间
export function SynCardActivateDate(map) {
  return request({
    url: '/yunze/card/SynActivateDate',
    method: 'post',
    data: map
  })
}

// 批量 【停机】
export function StoppedArr(map) {
  return request({
    url: '/yunze/card/stopped',
    method: 'post',
    data: map
  })
}
// 批量 【复机】
export function MachineArr(map) {
  return request({
    url: '/yunze/card/machine',
    method: 'post',
    data: map
  })
}
// 批量 【断网】
export function DisconnectNetworkArr(map) {
  return request({
    url: '/yunze/card/disconnectNetwork',
    method: 'post',
    data: map
  })
}
// 批量 【开网】
export function OpenNetworkArr(map) {
  return request({
    url: '/yunze/card/openNetwork',
    method: 'post',
    data: map
  })
}
// 批量 【同步用量】
export function ConsumptionArr(map) {
  return request({
    url: '/yunze/card/consumption',
    method: 'post',
    data: map
  })
}
// 批量 【同步状态】
export function PublicMethodArr(map) {
  return request({
    url: '/yunze/card/publicmethod',
    method: 'post',
    data: map
  })
}
// 批量 【同步状态和用量】
export function ConsumptionAndStateArr(map) {
  return request({
    url: '/yunze/card/consumptionandstate',
    method: 'post',
    data: map
  })
}



// 获取iccid
export function getIccid(map) {
  return request({
    url: '/yunze/card/getIccid',
    method: 'post',
    data: map
  })
}





// 智能匹对
export function cardMatch(map) {
  return request({
    url: '/yunze/card/cardMatch',
    method: 'post',
    data: map
  })
}




// 单卡停机原因查询
export function simStopReason(map) {
  return request({
    url: '/yunze/card/simStopReason',
    method: 'post',
    data: map
  })
}

// 单卡开关机状态实时查询
export function onOffStatus(map) {
  return request({
    url: '/yunze/card/onOffStatus',
    method: 'post',
    data: map
  })
}

// 单卡已开通APN信息查询
export function apnInfo(map) {
  return request({
    url: '/yunze/card/apnInfo',
    method: 'post',
    data: map
  })
}

// 物联卡机卡分离状态查询
export function cardBindStatus(map) {
  return request({
    url: '/yunze/card/cardBindStatus',
    method: 'post',
    data: map
  })
}

// 单卡状态变更历史查询
export function simChangeHistory(map) {
  return request({
    url: '/yunze/card/simChangeHistory',
    method: 'post',
    data: map
  })
}












































