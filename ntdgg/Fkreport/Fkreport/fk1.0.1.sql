/*
Navicat MySQL Data Transfer

Source Server         : 本地数据
Source Server Version : 80025
Source Host           : 127.0.0.1:3306
Source Database       : fk

Target Server Type    : MYSQL
Target Server Version : 80025
File Encoding         : 65001

Date: 2022-04-12 23:16:40
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for fk_demo
-- ----------------------------
DROP TABLE IF EXISTS `fk_demo`;
CREATE TABLE `fk_demo` (
  `id` int NOT NULL AUTO_INCREMENT,
  `value` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT NULL,
  `type` int DEFAULT NULL,
  `data` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=utf8mb3 ROW_FORMAT=DYNAMIC;

-- ----------------------------
-- Table structure for fk_function
-- ----------------------------
DROP TABLE IF EXISTS `fk_function`;
CREATE TABLE `fk_function` (
  `id` int NOT NULL AUTO_INCREMENT COMMENT '主键',
  `bill` varchar(255) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '编号',
  `title` varchar(255) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '标题',
  `fun_name` varchar(255) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '函数名',
  `function` longtext CHARACTER SET utf8 COLLATE utf8_unicode_ci COMMENT '函数SQL',
  `add_user` varchar(255) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '添加人',
  `add_time` int DEFAULT NULL COMMENT '添加时间',
  `status` int NOT NULL DEFAULT '0' COMMENT '0编辑中，1启用',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8mb3 COLLATE=utf8_unicode_ci ROW_FORMAT=DYNAMIC COMMENT='函数方法表';

-- ----------------------------
-- Table structure for fk_imags
-- ----------------------------
DROP TABLE IF EXISTS `fk_imags`;
CREATE TABLE `fk_imags` (
  `id` int NOT NULL AUTO_INCREMENT,
  `title` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT NULL,
  `src` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT NULL,
  `ext` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=27 DEFAULT CHARSET=utf8mb3 ROW_FORMAT=DYNAMIC;

-- ----------------------------
-- Table structure for fk_main
-- ----------------------------
DROP TABLE IF EXISTS `fk_main`;
CREATE TABLE `fk_main` (
  `id` int NOT NULL AUTO_INCREMENT COMMENT '主键',
  `title` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT NULL COMMENT '标题',
  `type` smallint DEFAULT NULL COMMENT '类别',
  `file` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT NULL COMMENT '附件',
  `add_time` datetime DEFAULT NULL COMMENT '时间',
  `data` longtext CHARACTER SET utf8 COLLATE utf8_general_ci,
  `height` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT NULL,
  `width` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT NULL,
  `update_time` int DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=MyISAM AUTO_INCREMENT=7 DEFAULT CHARSET=utf8mb3 ROW_FORMAT=DYNAMIC;
