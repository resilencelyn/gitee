import request from '@/utils/request'
import { praseStrEmpty } from "@/utils/ruoyi";


// 订单合同发货 列表
export function getList(query) {
  return request({
    url: '/yunze/FhglOrderDelivery/list',
    method: 'post',
    data: query
  })
}





// 查询未完成的销售合同ID
export function getUndoneContract(query) {
  return request({
    url: '/yunze/FhglOrderDelivery/sel_CT_ID',
    method: 'post',
    data: query
  })
}

// 合同发货信息 ID
export function getODYContractID(query) {
  return request({
    url: '/yunze/FhglOrderDelivery/Sle_ODYContractID',
    method: 'post',
    data: query
  })
}


