/*
 Navicat Premium Data Transfer

 Source Server         : localhost_3306
 Source Server Type    : MySQL
 Source Server Version : 50734
 Source Host           : localhost:3306
 Source Schema         : flink-admin

 Target Server Type    : MySQL
 Target Server Version : 50734
 File Encoding         : 65001

 Date: 14/02/2022 17:52:24
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for cdc_job
-- ----------------------------
DROP TABLE IF EXISTS `cdc_job`;
CREATE TABLE `cdc_job`  (
  `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `job_name` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci  DEFAULT '' COMMENT '任务名称',
  `job_type` tinyint(4) NULL DEFAULT NULL COMMENT '任务类型',
  `source_info` varchar(1024) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '源信息',
  `target_db_id` int(11) NULL DEFAULT NULL COMMENT '目标数据库id',
  `target_table_id` int(11) NULL DEFAULT NULL COMMENT '目标表id',
  `column_association` varchar(3072) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '列对应信息',
  `primary_column` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '数据更新对应索引',
  `scan_startup_mode` varchar(60) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '扫描启动模式',
  `skipped_operations` varchar(52) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci  DEFAULT '' COMMENT '跳过记录，c表示插入/创建，u表示更新，d表示删除',
  `job_id` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT 'flink任务id',
  `parallelism` int(11) NULL DEFAULT NULL,
  `savepoint_path` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL,
  `status` tinyint(4) NULL DEFAULT NULL,
  `enable_flag` tinyint(4) NULL DEFAULT NULL,
  `flink_colony_id` bigint(20) NULL DEFAULT NULL COMMENT '任务运行集群id',
  `remark` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL,
  `create_time` datetime(0) NULL DEFAULT NULL,
  `create_user_id` int(11) NULL DEFAULT NULL,
  `update_time` datetime(0) NULL DEFAULT NULL,
  `update_user_id` int(11) NULL DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for cdc_job_run_config
-- ----------------------------
DROP TABLE IF EXISTS `cdc_job_run_config`;
CREATE TABLE `cdc_job_run_config`  (
  `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `job_type` varchar(30) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '任务类型',
  `entry_class` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '运行类',
  `jar_id` int(11) NULL DEFAULT NULL COMMENT 'jarId',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 3 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;


INSERT INTO `cdc_job_run_config`(`id`, `job_type`, `entry_class`, `jar_id`) VALUES (1, 'POINT_TO_POINT_SQL', 'cn.chongho.inf.flink.job.FlinkCdcSqlTemplateJob', 1);
INSERT INTO `cdc_job_run_config`(`id`, `job_type`, `entry_class`, `jar_id`) VALUES (2, 'CONFLUENCE_SQL', 'cn.chongho.inf.flink.job.FlinkCdcSqlTemplateJob', 1);
-- ----------------------------
-- Table structure for check_point_info
-- ----------------------------
DROP TABLE IF EXISTS `check_point_info`;
CREATE TABLE `check_point_info`  (
  `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `job_id` int(11) NULL DEFAULT NULL COMMENT '任务id',
  `job_form` tinyint(4) NULL DEFAULT NULL COMMENT '1job,2cdc-job',
  `point_type` tinyint(4) NULL DEFAULT NULL COMMENT '类型，1check,2save',
  `trigger_id` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL,
  `location` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL,
  `create_time` datetime(0) NULL DEFAULT NULL,
  `create_user_id` int(11) NULL DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 35 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for cluster
-- ----------------------------
DROP TABLE IF EXISTS `cluster`;
CREATE TABLE `cluster`  (
  `id` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT,
  `name` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '名称',
  `url` varchar(300) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '集群地址',
  `enable_flag` tinyint(4) NULL DEFAULT NULL COMMENT '状态',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 3 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of cluster
-- ----------------------------
INSERT INTO `cluster` VALUES (1, 'Flink集群(Local)', 'http://localhost:8081', 1);

-- ----------------------------
-- Table structure for db_source
-- ----------------------------
DROP TABLE IF EXISTS `db_source`;
CREATE TABLE `db_source`  (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `name` varchar(20) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `provider_type` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL COMMENT '数据库类型',
  `url` varchar(300) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `username` varchar(50) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `password` varchar(50) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `remark` varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `create_user_id` int(11) NULL DEFAULT NULL,
  `create_time` datetime(0) NULL DEFAULT NULL,
  `update_user_id` int(11) NULL DEFAULT NULL,
  `update_time` datetime(0) NULL DEFAULT NULL,
  `enable_flag` tinyint(1) NULL DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE INDEX `dbconnection_id_uindex`(`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 2 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Dynamic;


-- ----------------------------
-- Table structure for db_table
-- ----------------------------
DROP TABLE IF EXISTS `db_table`;
CREATE TABLE `db_table`  (
  `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `db_id` int(11) NULL DEFAULT NULL COMMENT '对应数据源id',
  `table_name` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '表名',
  `table_comment` varchar(512) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '备注',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 16 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for db_table_column
-- ----------------------------
DROP TABLE IF EXISTS `db_table_column`;
CREATE TABLE `db_table_column`  (
  `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `table_id` int(11) NULL DEFAULT NULL COMMENT '表id',
  `column_name` varchar(1024) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '列名',
  `column_type` varchar(126) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '数据类型',
  `column_key` varchar(30) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '索引类型',
  `column_comment` varchar(512) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '列描述',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 131 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for jar
-- ----------------------------
DROP TABLE IF EXISTS `jar`;
CREATE TABLE `jar`  (
  `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `file_name` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '上传文件名称',
  `job_key` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT 'finlk系统中文件id',
  `enable_flag` tinyint(4) NULL DEFAULT NULL,
  `create_time` datetime(0) NULL DEFAULT NULL,
  `create_user_id` int(11) NULL DEFAULT NULL,
  `update_time` datetime(0) NULL DEFAULT NULL,
  `update_user_id` int(11) NULL DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 2 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;


-- ----------------------------
-- Table structure for job
-- ----------------------------
DROP TABLE IF EXISTS `job`;
CREATE TABLE `job`  (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `job_name` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL,
  `jar_id` int(11) NULL DEFAULT NULL,
  `entry_class` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL,
  `args` varchar(1024) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL,
  `parallelism` int(11) NULL DEFAULT NULL,
  `target_db_id` bigint(20) NULL DEFAULT NULL COMMENT '落库数据库',
  `savepoint_path` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL,
  `job_id` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL,
  `status` int(11) NULL DEFAULT 0,
  `enable_flag` tinyint(4) NULL DEFAULT 1,
  `flink_colony_id` bigint(20) NULL DEFAULT NULL COMMENT '任务运行集群id',
  `remark` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL,
  `create_time` datetime(0) NULL DEFAULT NULL,
  `create_user_id` int(11) NULL DEFAULT NULL,
  `update_time` datetime(0) NULL DEFAULT NULL,
  `update_user_id` int(11) NULL DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 2 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for menu
-- ----------------------------
DROP TABLE IF EXISTS `menu`;
CREATE TABLE `menu`  (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '主键ID',
  `name` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '菜单名称',
  `url` varchar(500) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '网址',
  `icon` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '显示的图标',
  `menutype` enum('0','1','2') CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '0' COMMENT '类型，0 菜单，1 连接网址,2 隐藏连接',
  `display` int(11) NOT NULL DEFAULT 1 COMMENT '显示排序',
  `parentid` int(11) NOT NULL DEFAULT 0 COMMENT '父级的id，引用本表id字段',
  `creator` int(11) NOT NULL DEFAULT 0 COMMENT '创建者id，0为超级管理员',
  `createtime` timestamp(0) NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  `updateuser` int(11) NULL DEFAULT NULL COMMENT '更新者id',
  `updatetime` timestamp(0) NULL DEFAULT NULL COMMENT '更新时间',
  `flag` enum('0','1') CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '1' COMMENT '是否启用，0 禁用，1启用',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 45 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of menu
-- ----------------------------
INSERT INTO `menu` VALUES (1, '系统首页', '/admin/index', NULL, '2', 1, 0, 0, '2022-02-14 00:00:00', 0, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (2, '修改密码', '/admin/user/updatepass', 'fa-wrench', '2', 0, 0, 0, '2022-02-14 00:00:00', 0, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (3, '系统配置', '12', 'fa-wrench', '0', 1, 0, 0, '2022-02-14 00:00:00', 0, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (4, '菜单配置', '/admin/menu', 'fa-list', '0', 1, 3, 0, '2022-02-14 00:00:00', 0, '2022-02-14 00:00:00', '1');
-- INSERT INTO `menu` VALUES (6, '角色权限', '/admin/role/menu', NULL, '2', 0, 3, 0, '2022-02-14 00:00:00', 0, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (7, '用户管理', '/admin/user', NULL, '1', 2, 3, 0, '2022-02-14 00:00:00', 0, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (8, '新增菜单', '/admin/menu/edit', NULL, '2', 0, 4, 0, '2022-02-14 00:00:00', 0, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (9, '删除菜单', '/admin/menu/delete', NULL, '2', 0, 4, 0, '2022-02-14 00:00:00', 0, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (10, '编辑角色', '/admin/role/edit', NULL, '2', 0, 5, 0, '2022-02-14 00:00:00', 0, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (11, '删除角色', '/admin/role/delete', NULL, '2', 0, 5, 0, '2022-02-14 00:00:00', 0, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (12, '角色资源管理', '/admin/role/menu', NULL, '2', 0, 5, 0, '2022-02-14 00:00:00', 0, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (13, '编辑用户', '/admin/user/edit', NULL, '2', 0, 7, 0, '2022-02-14 00:00:00', 0, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (14, '删除用户', '/admin/user/delete', NULL, '2', 0, 7, 0, '2022-02-14 00:00:00', 0, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (15, '用户角色管理', '/admin/user/role', NULL, '2', 0, 7, 0, '2022-02-14 00:00:00', 0, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (16, '菜单配置', '', NULL, '2', 0, 7, 20, '2022-02-14 00:00:00', 0, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (17, '作业管理', '', 'fa-file-o', '0', 0, 0, -1, '2022-02-14 00:00:00', -1, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (18, '作业列表', '/job/list', 'fa-list', '0', 0, 17, -1, '2022-02-14 00:00:00', -1, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (19, '编辑作业', '/job/edit', '', '2', 0, 17, -1, '2022-02-14 00:00:00', NULL, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (20, '删除作业', '/job/delete', '', '2', 0, 17, -1, '2022-02-14 00:00:00', NULL, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (21, '作业启动', '/job/run', '', '2', 0, 17, -1, '2022-02-14 00:00:00', -1, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (22, '资源列表', '/jars/list', 'fa-tag', '0', 0, 17, -1, '2022-02-14 00:00:00', -1, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (23, 'jar资源删除', '/jars/delete', '', '2', 0, 17, -1, '2022-02-14 00:00:00', -1, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (24, '数据同步管理', '', 'fa-object-group', '0', 0, 0, -1, '2022-02-14 00:00:00', -1, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (25, '数据源列表', '/dbSource/list', '', '0', 0, 24, -1, '2022-02-14 00:00:00', NULL, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (26, '数据源编辑', '/dbSource/edit', '', '2', 0, 24, -1, '2022-02-14 00:00:00', NULL, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (27, '数据源删除', '/dbSource/delete', '', '2', 0, 24, -1, '2022-02-14 00:00:00', NULL, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (30, '同步列表', '/cdcjob/list', '', '0', 0, 24, -1, '2022-02-14 00:00:00', -1, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (31, '作业编辑', '/cdcjob/edit', '', '2', 0, 24, -1, '2022-02-14 00:00:00', -1, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (32, '运行任务', '/cdcjob/run', '', '2', 0, 24, -1, '2022-02-14 00:00:00', -1, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (33, '同步数据源表信息', '/dbSource/syncDbInfo', '', '2', 0, 24, -1, '2022-02-14 00:00:00', NULL, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (34, '手动触发保存点', '/cdcjob/savepoint', '', '2', 0, 24, -1, '2022-02-14 00:00:00', NULL, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (35, '删除cdc任务', '/cdcjob/delete', '', '2', 0, 24, -1, '2022-02-14 00:00:00', NULL, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (36, '数据库表管理', '/dbSource/addTable', '', '2', 0, 24, -1, '2022-02-14 00:00:00', -1, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (37, 'flink集群管理', '', 'fa-list', '0', 1, 0, -1, '2022-02-14 00:00:00', -1, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (38, '集群列表', '/cluster/list', '', '1', 0, 37, -1, '2022-02-14 00:00:00', -1, '2022-02-14 15:15:01', '1');
INSERT INTO `menu` VALUES (39, '新增修改集群', '/cluster/edit', '', '2', 0, 37, -1, '2022-02-14 00:00:00', -1, '2022-02-14 15:15:23', '1');
INSERT INTO `menu` VALUES (40, '删除集群', '/cluster/delete', '', '2', 0, 37, -1, '2022-02-14 00:00:00', -1, '2022-02-14 15:15:35', '1');
INSERT INTO `menu` VALUES (41, '作业更新操作', '/job/update', '', '2', 0, 17, -1, '2022-02-14 00:00:00', NULL, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (42, '作业更新', '/cdcjob/update', '', '2', 0, 24, -1, '2022-02-14 00:00:00', NULL, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (43, '数据源更新', '/dbSource/update', '', '2', 0, 24, -1, '2022-02-14 00:00:00', NULL, '2022-02-14 00:00:00', '1');
INSERT INTO `menu` VALUES (44, '集群更新操作', '/cluster/update', '', '2', 0, 37, -1, '2022-02-14 00:00:00', -1, '2022-02-14 15:15:43', '1');

-- ----------------------------
-- Table structure for role
-- ----------------------------
DROP TABLE IF EXISTS `role`;
CREATE TABLE `role`  (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '角色表主键',
  `name` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '角色名称',
  `createtime` timestamp(0) NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  `creator` int(11) NULL DEFAULT 0 COMMENT '用户id，0为角色，有关联时则为用户的单独权限',
  `description` varchar(200) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '角色描述',
  `updateuser` int(11) NULL DEFAULT NULL COMMENT '更新者id',
  `updatetime` timestamp(0) NULL DEFAULT NULL COMMENT '更新时间',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 2 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of role
-- ----------------------------
INSERT INTO `role` VALUES (1, '超级用户', '2012-02-14 23:00:43', -1, '所有权限', -1, '2022-02-14 09:00:44');

-- ----------------------------
-- Table structure for role_menu
-- ----------------------------
DROP TABLE IF EXISTS `role_menu`;
CREATE TABLE `role_menu`  (
  `roleid` int(11) NOT NULL COMMENT '角色id',
  `menuid` int(11) NOT NULL COMMENT '菜单id',
  `flag` int(1) NOT NULL DEFAULT 1 COMMENT '1为有权限，0为没有权限（防止以后会出现角色有权限但是个人没有权限的情况）',
  `creator` int(11) NOT NULL COMMENT '创建人，0为初始化',
  `createtime` timestamp(0) NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  PRIMARY KEY (`menuid`, `roleid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for user_role
-- ----------------------------
DROP TABLE IF EXISTS `user_role`;
CREATE TABLE `user_role`  (
  `userid` int(11) NOT NULL COMMENT '用户id',
  `roleid` int(11) NOT NULL COMMENT '角色id',
  `creator` int(11) NOT NULL COMMENT '创建人，0为初始化',
  `createtime` timestamp(0) NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  PRIMARY KEY (`userid`, `roleid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of user_role
-- ----------------------------
INSERT INTO `user_role` VALUES (-1, 1, -1, '2022-02-14 11:37:04');

-- ----------------------------
-- Table structure for users
-- ----------------------------
DROP TABLE IF EXISTS `users`;
CREATE TABLE `users`  (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '用户表主键',
  `tenantid` int(11) NOT NULL DEFAULT 0 COMMENT '租户id，0为系统用户',
  `name` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '用户名',
  `psw` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '用户密码MD5加密',
  `email` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '用户邮箱',
  `creator` int(11) NOT NULL COMMENT '创建人，0为初始化',
  `createtime` timestamp(0) NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP(0) COMMENT '创建时间',
  `flag` int(1) NOT NULL DEFAULT 1 COMMENT '用户状态，1启用，0禁用',
  `logintime` timestamp(0) NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '最后登录时间',
  `updateuser` int(11) NULL DEFAULT NULL COMMENT '更新者id',
  `updatetime` timestamp(0) NULL DEFAULT NULL COMMENT '更新时间',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of users
-- ----------------------------
INSERT INTO `users` VALUES (-1, 0, 'admin', 'E10ADC3949BA59ABBE56E057F20F883E', 'admin@chongho.net', 0, '2022-02-14 14:59:01', 1, '2022-02-14 14:57:45', -1, '2022-02-08 12:00:17');


SET FOREIGN_KEY_CHECKS = 1;
