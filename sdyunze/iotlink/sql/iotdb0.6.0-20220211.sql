# 0.5.1 版本 sql

# 新增 首页数据表 [2022-02-11][zf]
CREATE TABLE `yz_indexpage` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `downCount` int(11) NOT NULL DEFAULT '0' COMMENT '已停机数量',
  `overdoseCount` int(11) NOT NULL DEFAULT '0' COMMENT '超用量数量',
  `expiringSoonCount` int(11) NOT NULL DEFAULT '0' COMMENT '快到期数量',
  `thresholdReachedCount` int(11) NOT NULL DEFAULT '0' COMMENT '已达阈值数量',
  `simCardCount` int(11) NOT NULL DEFAULT '0' COMMENT 'Sim卡总数',
  `simCardNewCount` int(11) NOT NULL DEFAULT '0' COMMENT 'Sim卡本月新增数量',
  `currentMonth` double(13,2) NOT NULL DEFAULT '0.00' COMMENT '本月数据用量GB',
  `simActivity` double(6,2) NOT NULL DEFAULT '0.00' COMMENT 'SIm卡活跃度',
  `lifeCycleDistribution` longblob NOT NULL COMMENT '生命周期分布',
  `businessVolumeFlow` longblob NOT NULL COMMENT '业务使用情况-数据',
  `businessVolumeVoice` longblob COMMENT '业务使用情况-语音',
  `businessVolumeMessage` longblob COMMENT '业务使用情况-短信',
  `customerCount` int(11) NOT NULL DEFAULT '0' COMMENT '客户总数',
  `customerNewCount` int(11) NOT NULL DEFAULT '0' COMMENT '本月新增客户',
  `salesContractCount` int(11) NOT NULL DEFAULT '0' COMMENT '本月销售合同总数',
  `salesContractDepositCount` double(13,2) NOT NULL DEFAULT '0.00' COMMENT '本月销售合同入款金额',
  `weChatOrderCollection` double(13,2) NOT NULL DEFAULT '0.00' COMMENT '微信订单收款',
  `orderAmount` double(13,2) NOT NULL DEFAULT '0.00' COMMENT '本月订单总金额',
  `systemCliqueCount` int(11) NOT NULL DEFAULT '0' COMMENT '系统企业总数',
  `systemUserCount` int(11) NOT NULL DEFAULT '0' COMMENT '系统用户总数',
  `activationCardCount` int(11) NOT NULL DEFAULT '0' COMMENT '本月激活卡数',
  `shipCard` int(11) NOT NULL DEFAULT '0' COMMENT '本月发货卡数',
  `logInIp` int(11) NOT NULL DEFAULT '0' COMMENT '本月登录ip总数',
  `create_date` datetime NOT NULL COMMENT '创建时间',
  `update_date` datetime NOT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '修改时间',
  `record_date` date NOT NULL COMMENT '记录日期',
  `dept_id` int(11) NOT NULL COMMENT '所属用户id',
  `requestMap` longblob NOT NULL COMMENT '请求参数',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=70 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='首页数据表';

# 执行任务表新增 首页任务生成 [2022-02-11][zf]

INSERT INTO `iotdb`.`sys_job`(`job_id`, `job_name`, `job_group`, `invoke_target`, `cron_expression`, `misfire_policy`, `concurrent`, `status`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (30, '首页数据生成', 'SYSTEM', 'indexpageGenerateTask.GenerateTask(60,\'100\')', '0 0 0 * * ?', '1', '1', '0', 'admin', '2022-01-25 11:35:11', 'admin', '2022-01-28 08:01:01', '');


# ‘集团’ 字眼 变更为 ‘企业’

UPDATE `iotdb`.`sys_menu` SET `menu_name` = '企业管理' WHERE `menu_id` = 103;
UPDATE `iotdb`.`sys_menu` SET `menu_name` = '企业列表' WHERE `menu_id` = 2028;
UPDATE `iotdb`.`sys_menu` SET `menu_name` = '系统企业总数' WHERE `menu_id` = 2192;



# 新增菜单

INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2172, '首页加载', 0, 27, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:findToDay', '#', 'admin', '2022-01-26 10:15:09', '', NULL, '');
INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2173, '已停机数量', 2172, 1, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:downCount', '#', 'admin', '2022-01-26 10:16:47', '', NULL, '');
INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2175, '超用量数量', 2172, 2, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:overdoseCount', '#', 'admin', '2022-01-26 10:16:47', '', NULL, '');
INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2176, '快到期数量', 2172, 3, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:expiringSoonCount', '#', 'admin', '2022-01-26 10:16:47', '', NULL, '');
INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2177, '已达阈值数量', 2172, 4, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:thresholdReachedCount', '#', 'admin', '2022-01-26 10:16:47', '', NULL, '');
INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2178, 'Sim卡总数', 2172, 5, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:simCardCount', '#', 'admin', '2022-01-26 10:16:47', '', NULL, '');
INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2179, 'Sim卡本月新增数量', 2172, 6, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:simCardNewCount', '#', 'admin', '2022-01-26 10:16:47', '', NULL, '');
INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2180, '本月数据用量GB', 2172, 7, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:currentMonth', '#', 'admin', '2022-01-26 10:16:47', '', NULL, '');
INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2181, 'SIm卡活跃度', 2172, 8, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:simActivity', '#', 'admin', '2022-01-26 10:16:47', '', NULL, '');
INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2182, '生命周期分布', 2172, 9, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:lifeCycleDistribution', '#', 'admin', '2022-01-26 10:16:47', '', NULL, '');
INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2183, '业务使用情况-数据', 2172, 10, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:businessVolumeFlow', '#', 'admin', '2022-01-26 10:16:47', '', NULL, '');
INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2184, '业务使用情况-语音', 2172, 11, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:businessVolumeVoice', '#', 'admin', '2022-01-26 10:16:47', '', NULL, '');
INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2185, '业务使用情况-短信', 2172, 12, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:businessVolumeMessage', '#', 'admin', '2022-01-26 10:16:47', '', NULL, '');
INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2190, '微信订单收款', 2172, 17, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:weChatOrderCollection', '#', 'admin', '2022-01-26 10:16:47', '', NULL, '');
INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2191, '本月订单总金额', 2172, 18, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:orderAmount', '#', 'admin', '2022-01-26 10:16:47', '', NULL, '');
INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2192, '系统集团总数', 2172, 19, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:systemCliqueCount', '#', 'admin', '2022-01-26 10:16:47', '', NULL, '');
INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2193, '系统用户总数', 2172, 20, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:systemUserCount', '#', 'admin', '2022-01-26 10:16:47', '', NULL, '');
INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2194, '本月激活卡数', 2172, 21, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:activationCardCount', '#', 'admin', '2022-01-26 10:16:47', '', NULL, '');
INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2195, '本月发货卡数', 2172, 22, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:shipCard', '#', 'admin', '2022-01-26 10:16:47', '', NULL, '');
INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2196, '本月登录ip总数', 2172, 23, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:logInIp', '#', 'admin', '2022-01-26 10:16:47', '', NULL, '');
INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2197, '在线用户', 2172, 24, '', NULL, 1, 0, 'F', '0', '0', 'yunze:index:onlineCount', '#', 'admin', '2022-01-26 16:37:47', '', NULL, '');

INSERT INTO `iotdb`.`sys_menu`(`menu_id`, `menu_name`, `parent_id`, `order_num`, `path`, `component`, `is_frame`, `is_cache`, `menu_type`, `visible`, `status`, `perms`, `icon`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (2198, '关于IoTLink', 0, 2, 'iotlinkindex', 'Iotlinkindex', 1, 0, 'C', '0', '0', 'Iotlinkindex', 'button', 'admin', '2022-02-14 13:50:56', '', NULL, '');


# 新增菜单权限

INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (4, 2172);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (4, 2173);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (4, 2175);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (4, 2176);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (4, 2177);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (4, 2178);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (4, 2179);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (4, 2180);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (4, 2181);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (4, 2182);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (4, 2183);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (4, 2190);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (4, 2191);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (4, 2192);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (4, 2193);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (4, 2194);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (4, 2195);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2172);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2173);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2175);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2176);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2177);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2178);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2179);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2180);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2181);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2182);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2183);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2184);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2185);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2190);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2191);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2192);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2193);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2194);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2195);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2196);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (5, 2197);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (6, 2172);
INSERT INTO `iotdb`.`sys_role_menu`(`role_id`, `menu_id`) VALUES (7, 2172);





# 系统参数

INSERT INTO `iotdb`.`sys_config`(`config_id`, `config_name`, `config_key`, `config_value`, `config_type`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (14, '快到期', 'card.expiringSoonCount', '45', 'Y', 'admin', '2022-01-24 13:55:41', '', NULL, '45天到期卡号，用于首页判断相差多少天到期');
INSERT INTO `iotdb`.`sys_config`(`config_id`, `config_name`, `config_key`, `config_value`, `config_type`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (15, '首页数据缓存时间', 'IndexpageCacheTime', '15', 'Y', 'admin', '2022-01-25 16:59:01', '', NULL, '15 = 15分钟');


# 字典新增

INSERT INTO `iotdb`.`sys_dict_type`(`dict_id`, `dict_name`, `dict_type`, `status`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (228, '维度条件', 'yunze_card_dimensionType', '0', 'admin', '2022-01-24 09:08:42', 'admin', '2022-01-24 09:22:32', NULL);
INSERT INTO `iotdb`.`sys_dict_type`(`dict_id`, `dict_name`, `dict_type`, `status`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (229, '维度字段', 'yunze_card_dimensionField', '0', 'admin', '2022-01-24 09:09:12', 'admin', '2022-01-24 09:14:14', NULL);



INSERT INTO `iotdb`.`sys_dict_data`(`dict_code`, `dict_sort`, `dict_label`, `dict_value`, `dict_type`, `css_class`, `list_class`, `is_default`, `status`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (564, 1, '>=', '1', 'yunze_card_dimensionType', NULL, NULL, 'N', '0', 'admin', '2022-01-24 09:16:18', 'admin', '2022-01-24 09:34:59', NULL);
INSERT INTO `iotdb`.`sys_dict_data`(`dict_code`, `dict_sort`, `dict_label`, `dict_value`, `dict_type`, `css_class`, `list_class`, `is_default`, `status`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (565, 2, '>', '2', 'yunze_card_dimensionType', NULL, NULL, 'N', '0', 'admin', '2022-01-24 09:16:33', '', NULL, NULL);
INSERT INTO `iotdb`.`sys_dict_data`(`dict_code`, `dict_sort`, `dict_label`, `dict_value`, `dict_type`, `css_class`, `list_class`, `is_default`, `status`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (566, 3, '=', '3', 'yunze_card_dimensionType', NULL, NULL, 'N', '0', 'admin', '2022-01-24 09:16:42', '', NULL, NULL);
INSERT INTO `iotdb`.`sys_dict_data`(`dict_code`, `dict_sort`, `dict_label`, `dict_value`, `dict_type`, `css_class`, `list_class`, `is_default`, `status`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (567, 4, '<', '4', 'yunze_card_dimensionType', NULL, NULL, 'N', '0', 'admin', '2022-01-24 09:16:51', '', NULL, NULL);
INSERT INTO `iotdb`.`sys_dict_data`(`dict_code`, `dict_sort`, `dict_label`, `dict_value`, `dict_type`, `css_class`, `list_class`, `is_default`, `status`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (568, 5, '<=', '5', 'yunze_card_dimensionType', NULL, NULL, 'N', '0', 'admin', '2022-01-24 09:17:04', '', NULL, NULL);
INSERT INTO `iotdb`.`sys_dict_data`(`dict_code`, `dict_sort`, `dict_label`, `dict_value`, `dict_type`, `css_class`, `list_class`, `is_default`, `status`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (569, 6, '!=', '6', 'yunze_card_dimensionType', NULL, NULL, 'N', '0', 'admin', '2022-01-24 09:17:46', '', NULL, NULL);
INSERT INTO `iotdb`.`sys_dict_data`(`dict_code`, `dict_sort`, `dict_label`, `dict_value`, `dict_type`, `css_class`, `list_class`, `is_default`, `status`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (570, 1, '账期用量', '1', 'yunze_card_dimensionField', NULL, NULL, 'N', '0', 'admin', '2022-01-24 09:18:59', 'admin', '2022-01-24 09:19:23', NULL);
INSERT INTO `iotdb`.`sys_dict_data`(`dict_code`, `dict_sort`, `dict_label`, `dict_value`, `dict_type`, `css_class`, `list_class`, `is_default`, `status`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (571, 2, '剩余用量', '2', 'yunze_card_dimensionField', NULL, NULL, 'N', '0', 'admin', '2022-01-24 09:19:11', '', NULL, NULL);
INSERT INTO `iotdb`.`sys_dict_data`(`dict_code`, `dict_sort`, `dict_label`, `dict_value`, `dict_type`, `css_class`, `list_class`, `is_default`, `status`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (572, 5, '到期日期', '5', 'yunze_card_sel_timetype', NULL, NULL, 'N', '0', 'admin', '2022-01-24 09:51:52', '', NULL, NULL);
INSERT INTO `iotdb`.`sys_dict_data`(`dict_code`, `dict_sort`, `dict_label`, `dict_value`, `dict_type`, `css_class`, `list_class`, `is_default`, `status`, `create_by`, `create_time`, `update_by`, `update_time`, `remark`) VALUES (573, 6, '创建时间', '6', 'yunze_card_sel_timetype', NULL, NULL, 'N', '0', 'admin', '2022-01-24 14:27:27', '', NULL, NULL);






