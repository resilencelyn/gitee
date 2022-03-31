/*
 Navicat Premium Data Transfer

 Source Server         : 本地服务器
 Source Server Type    : MySQL
 Source Server Version : 80025
 Source Host           : localhost:3306
 Source Schema         : fkreport

 Target Server Type    : MySQL
 Target Server Version : 80025
 File Encoding         : 65001

 Date: 11/12/2021 11:04:22
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for fk_demo
-- ----------------------------
DROP TABLE IF EXISTS `fk_demo`;
CREATE TABLE `fk_demo`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `value` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `type` int NULL DEFAULT NULL,
  `data` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 12 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of fk_demo
-- ----------------------------
INSERT INTO `fk_demo` VALUES (1, '1', 1, '一线城市');
INSERT INTO `fk_demo` VALUES (2, '2', 1, '一线城市');
INSERT INTO `fk_demo` VALUES (3, '3', 1, '一线城市');
INSERT INTO `fk_demo` VALUES (4, '22', 2, '二线城市');
INSERT INTO `fk_demo` VALUES (5, '1', 2, '二线城市');
INSERT INTO `fk_demo` VALUES (6, '14', 3, '三线城市');
INSERT INTO `fk_demo` VALUES (7, '19', 4, '四线及以下');
INSERT INTO `fk_demo` VALUES (8, '2', 2, '二线城市');
INSERT INTO `fk_demo` VALUES (9, '1', 1, '一线城市');
INSERT INTO `fk_demo` VALUES (10, '7', 1, '一线城市');
INSERT INTO `fk_demo` VALUES (11, '1', 1, '一线城市');

-- ----------------------------
-- Table structure for fk_function
-- ----------------------------
DROP TABLE IF EXISTS `fk_function`;
CREATE TABLE `fk_function`  (
  `id` int NOT NULL AUTO_INCREMENT COMMENT '主键',
  `bill` varchar(255) CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL DEFAULT NULL COMMENT '编号',
  `title` varchar(255) CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL DEFAULT NULL COMMENT '标题',
  `fun_name` varchar(255) CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL DEFAULT NULL COMMENT '函数名',
  `function` longtext CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL COMMENT '函数SQL',
  `add_user` varchar(255) CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL DEFAULT NULL COMMENT '添加人',
  `add_time` int NULL DEFAULT NULL COMMENT '添加时间',
  `status` int NOT NULL DEFAULT 0 COMMENT '0编辑中，1启用',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 2 CHARACTER SET = utf8 COLLATE = utf8_unicode_ci COMMENT = '函数方法表' ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of fk_function
-- ----------------------------
INSERT INTO `fk_function` VALUES (1, '111', '测试1233', 'get_test', 'select sum(value) as value,type,data from fk_demo GROUP BY type', 'Sys', 1632569551, 0);

-- ----------------------------
-- Table structure for fk_imags
-- ----------------------------
DROP TABLE IF EXISTS `fk_imags`;
CREATE TABLE `fk_imags`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `title` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `src` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `ext` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 27 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of fk_imags
-- ----------------------------
INSERT INTO `fk_imags` VALUES (1, '1.png', 'uploads/20211211/5578a5614017956ad65cacc05a93220d.png', 'png');
INSERT INTO `fk_imags` VALUES (2, 'bg_body.jpg', 'uploads/20211211/d521e7ada5366f184688eb0f7239e6b8.jpg', 'jpg');
INSERT INTO `fk_imags` VALUES (3, 'bg_title.png', 'uploads/20211211/1db7f462c80858a614f2c5380c7946d0.png', 'png');
INSERT INTO `fk_imags` VALUES (4, 'bg_box1.png', 'uploads/20211211/1a798eec7b8edcabb324588586288005.png', 'png');
INSERT INTO `fk_imags` VALUES (5, 'title_icon.png', 'uploads/20211211/5a7eb88f1226e5499fa5376c09ebd3c2.png', 'png');

-- ----------------------------
-- Table structure for fk_main
-- ----------------------------
DROP TABLE IF EXISTS `fk_main`;
CREATE TABLE `fk_main`  (
  `id` int NOT NULL AUTO_INCREMENT COMMENT '主键',
  `title` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL COMMENT '标题',
  `type` smallint NULL DEFAULT NULL COMMENT '类别',
  `file` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL COMMENT '附件',
  `add_time` datetime NULL DEFAULT NULL COMMENT '时间',
  `data` longtext CHARACTER SET utf8 COLLATE utf8_general_ci NULL,
  `height` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `width` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL,
  `update_time` int NULL DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 7 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of fk_main
-- ----------------------------
INSERT INTO `fk_main` VALUES (1, '数据大屏模板', 0, 'uploads/20211211\\5578a5614017956ad65cacc05a93220d.png', '2021-12-11 10:20:18', NULL, '1080', '1920', NULL);
INSERT INTO `fk_main` VALUES (2, '大数据统计大屏', 0, 'uploads/20211211\\d521e7ada5366f184688eb0f7239e6b8.jpg', '2021-12-11 10:23:23', '{\"id\":2,\"f_id\":\"Fk2351640\",\"f_list\":{\"img_2353992\":{\"id\":\"img_2353992\",\"f_title\":\"/storage/uploads/20211211/1db7f462c80858a614f2c5380c7946d0.png\",\"f_color\":\"rgb(45, 109, 204)\",\"f_size\":\"20\",\"f_width\":\"1877\",\"f_height\":\"79\",\"left\":\"25\",\"top\":\"15\",\"zindex\":\"2\",\"d_type\":\"\",\"fundata\":\"\",\"r_data\":\"\",\"f_type\":\"img\"},\"text_3056503\":{\"id\":\"text_3056503\",\"f_title\":\"大数据展示屏幕\",\"f_color\":\"rgba(255, 255, 255, 0.94)\",\"f_size\":\"45\",\"f_width\":\"343\",\"f_height\":\"75\",\"left\":812,\"top\":21,\"zindex\":\"2\",\"d_type\":\"\",\"fundata\":\"\",\"r_data\":\"\",\"f_type\":\"text\"},\"img_3554216\":{\"id\":\"img_3554216\",\"f_title\":\"/storage/uploads/20211211/1a798eec7b8edcabb324588586288005.png\",\"f_color\":\"rgb(45, 109, 204)\",\"f_size\":\"20\",\"f_width\":\"534\",\"f_height\":\"419\",\"left\":\"25\",\"top\":\"108\",\"zindex\":\"2\",\"d_type\":\"\",\"fundata\":\"\",\"r_data\":\"\",\"f_type\":\"img\"},\"img_3648664\":{\"id\":\"img_3648664\",\"f_title\":\"/storage/uploads/20211211/5a7eb88f1226e5499fa5376c09ebd3c2.png\",\"f_color\":\"rgb(45, 109, 204)\",\"f_size\":\"20\",\"f_width\":\"15\",\"f_height\":\"15\",\"left\":\"57\",\"top\":\"130\",\"zindex\":\"2\",\"d_type\":\"\",\"fundata\":\"\",\"r_data\":\"\",\"f_type\":\"img\"},\"text_3744640\":{\"id\":\"text_3744640\",\"f_title\":\"数据统计标题\",\"f_color\":\"rgba(255, 120, 2, 0.94)\",\"f_size\":\"30\",\"f_width\":\"192\",\"f_height\":\"57\",\"left\":\"78\",\"top\":\"117\",\"zindex\":\"2\",\"d_type\":\"\",\"fundata\":\"\",\"r_data\":\"\",\"f_type\":\"text\"},\"bar_3903529\":{\"id\":\"bar_3903529\",\"f_title\":\"大屏文字\",\"f_color\":\"rgb(45, 109, 204)\",\"f_size\":\"20\",\"f_width\":\"494\",\"f_height\":\"328\",\"left\":\"51\",\"top\":\"177\",\"zindex\":\"2\",\"d_type\":\"2\",\"fundata\":\"@get_test\",\"r_data\":\"600\",\"f_type\":\"bar\"},\"img_4503248\":{\"id\":\"img_4503248\",\"f_title\":\"/storage/uploads/20211211/1a798eec7b8edcabb324588586288005.png\",\"f_color\":\"rgb(45, 109, 204)\",\"f_size\":\"20\",\"f_width\":\"813\",\"f_height\":\"627\",\"left\":\"575\",\"top\":\"102\",\"zindex\":\"2\",\"d_type\":\"\",\"fundata\":\"\",\"r_data\":\"\",\"f_type\":\"img\"},\"img_4549448\":{\"id\":\"img_4549448\",\"f_title\":\"/storage/uploads/20211211/1a798eec7b8edcabb324588586288005.png\",\"f_color\":\"rgb(45, 109, 204)\",\"f_size\":\"20\",\"f_width\":\"485\",\"f_height\":\"931\",\"left\":\"1410\",\"top\":\"101\",\"zindex\":\"2\",\"d_type\":\"\",\"fundata\":\"\",\"r_data\":\"\",\"f_type\":\"img\"},\"img_4642752\":{\"id\":\"img_4642752\",\"f_title\":\"/storage/uploads/20211211/1a798eec7b8edcabb324588586288005.png\",\"f_color\":\"rgb(45, 109, 204)\",\"f_size\":\"20\",\"f_width\":\"528\",\"f_height\":\"485\",\"left\":31,\"top\":546,\"zindex\":\"2\",\"d_type\":\"\",\"fundata\":\"\",\"r_data\":\"\",\"f_type\":\"img\"},\"img_4644871\":{\"id\":\"img_4644871\",\"f_title\":\"/storage/uploads/20211211/1a798eec7b8edcabb324588586288005.png\",\"f_color\":\"rgb(45, 109, 204)\",\"f_size\":\"20\",\"f_width\":\"809\",\"f_height\":\"277\",\"left\":578,\"top\":755,\"zindex\":\"2\",\"d_type\":\"\",\"fundata\":\"\",\"r_data\":\"\",\"f_type\":\"img\"},\"gauge_4830160\":{\"id\":\"gauge_4830160\",\"f_title\":\"大屏文字\",\"f_color\":\"rgb(45, 109, 204)\",\"f_size\":\"20\",\"f_width\":\"344\",\"f_height\":\"183\",\"left\":632,\"top\":827,\"zindex\":\"2\",\"d_type\":\"1\",\"fundata\":\"0.65\",\"r_data\":\"\",\"f_type\":\"gauge\"},\"gauge_4903544\":{\"id\":\"gauge_4903544\",\"f_title\":\"大屏文字\",\"f_color\":\"rgb(45, 109, 204)\",\"f_size\":\"20\",\"f_width\":\"345\",\"f_height\":\"194\",\"left\":996,\"top\":817,\"zindex\":\"2\",\"d_type\":\"\",\"fundata\":\"0.95\",\"r_data\":\"\",\"f_type\":\"gauge\"},\"rosePlot_4934320\":{\"id\":\"rosePlot_4934320\",\"f_title\":\"大屏文字\",\"f_color\":\"rgb(45, 109, 204)\",\"f_size\":\"20\",\"f_width\":\"414\",\"f_height\":\"394\",\"left\":\"90\",\"top\":\"597\",\"zindex\":\"2\",\"d_type\":\"2\",\"fundata\":\"@get_test\",\"r_data\":\"300\",\"f_type\":\"rosePlot\"},\"area_5019136\":{\"id\":\"area_5019136\",\"f_title\":\"大屏文字\",\"f_color\":\"rgb(45, 109, 204)\",\"f_size\":\"20\",\"f_width\":\"395\",\"f_height\":\"332\",\"left\":1450,\"top\":656,\"zindex\":\"2\",\"d_type\":\"2\",\"fundata\":\"@get_test\",\"r_data\":\"300\",\"f_type\":\"area\"},\"line_510080\":{\"id\":\"line_510080\",\"f_title\":\"大屏文字\",\"f_color\":\"rgb(45, 109, 204)\",\"f_size\":\"20\",\"f_width\":\"435\",\"f_height\":\"387\",\"left\":1443,\"top\":185,\"zindex\":\"2\",\"d_type\":\"2\",\"fundata\":\"@get_test\",\"r_data\":\"60\",\"f_type\":\"line\"},\"date_5138705\":{\"id\":\"date_5138705\",\"f_title\":\"大屏文字\",\"f_color\":\"rgb(45, 109, 204)\",\"f_size\":\"26\",\"f_width\":\"450\",\"f_height\":\"80\",\"left\":\"763\",\"top\":\"121\",\"zindex\":\"2\",\"d_type\":\"\",\"fundata\":\"\",\"r_data\":\"\",\"f_type\":\"date\"},\"pie_5301249\":{\"id\":\"pie_5301249\",\"f_title\":\"大屏文字\",\"f_color\":\"rgb(45, 109, 204)\",\"f_size\":\"20\",\"f_width\":\"399\",\"f_height\":\"361\",\"left\":611,\"top\":236,\"zindex\":\"2\",\"d_type\":\"2\",\"fundata\":\"@get_test\",\"r_data\":\"60\",\"f_type\":\"pie\"},\"text_5407432\":{\"id\":\"text_5407432\",\"f_title\":\"数据统计标题\",\"f_color\":\"rgba(252, 108, 13, 1)\",\"f_size\":\"30\",\"f_width\":\"190\",\"f_height\":\"54\",\"left\":\"1462\",\"top\":\"598\",\"zindex\":\"2\",\"d_type\":\"\",\"fundata\":\"\",\"r_data\":\"\",\"f_type\":\"text\"},\"text_5410143\":{\"id\":\"text_5410143\",\"f_title\":\"数据统计标题\",\"f_color\":\"rgba(252, 108, 13, 1)\",\"f_size\":\"30\",\"f_width\":\"202\",\"f_height\":\"54\",\"left\":\"1460\",\"top\":\"119\",\"zindex\":\"2\",\"d_type\":\"\",\"fundata\":\"\",\"r_data\":\"\",\"f_type\":\"text\"},\"text_5413119\":{\"id\":\"text_5413119\",\"f_title\":\"数据统计标题\",\"f_color\":\"#fc6c0d\",\"f_size\":\"30\",\"f_width\":\"213\",\"f_height\":\"141\",\"left\":\"76\",\"top\":\"562\",\"zindex\":\"2\",\"d_type\":\"\",\"fundata\":\"\",\"r_data\":\"\",\"f_type\":\"text\"},\"text_5415911\":{\"id\":\"text_5415911\",\"f_title\":\"数据统计标题\",\"f_color\":\"rgba(252, 108, 13, 1)\",\"f_size\":\"30\",\"f_width\":\"190\",\"f_height\":\"61\",\"left\":654,\"top\":777,\"zindex\":\"2\",\"d_type\":\"\",\"fundata\":\"\",\"r_data\":\"\",\"f_type\":\"text\"},\"img_5647392\":{\"id\":\"img_5647392\",\"f_title\":\"/storage/uploads/20211211/5a7eb88f1226e5499fa5376c09ebd3c2.png\",\"f_color\":\"rgb(45, 109, 204)\",\"f_size\":\"20\",\"f_width\":\"15\",\"f_height\":\"15\",\"left\":\"49\",\"top\":\"577\",\"zindex\":\"2\",\"d_type\":\"\",\"fundata\":\"\",\"r_data\":\"\",\"f_type\":\"img\"},\"img_5649800\":{\"id\":\"img_5649800\",\"f_title\":\"/storage/uploads/20211211/5a7eb88f1226e5499fa5376c09ebd3c2.png\",\"f_color\":\"rgb(45, 109, 204)\",\"f_size\":\"20\",\"f_width\":\"15\",\"f_height\":\"15\",\"left\":\"624\",\"top\":\"791\",\"zindex\":\"2\",\"d_type\":\"\",\"fundata\":\"\",\"r_data\":\"\",\"f_type\":\"img\"},\"img_565264\":{\"id\":\"img_565264\",\"f_title\":\"/storage/uploads/20211211/5a7eb88f1226e5499fa5376c09ebd3c2.png\",\"f_color\":\"rgb(45, 109, 204)\",\"f_size\":\"20\",\"f_width\":\"15\",\"f_height\":\"15\",\"left\":1438,\"top\":614,\"zindex\":\"2\",\"d_type\":\"\",\"fundata\":\"\",\"r_data\":\"\",\"f_type\":\"img\"},\"img_5654559\":{\"id\":\"img_5654559\",\"f_title\":\"/storage/uploads/20211211/5a7eb88f1226e5499fa5376c09ebd3c2.png\",\"f_color\":\"rgb(45, 109, 204)\",\"f_size\":\"20\",\"f_width\":\"15\",\"f_height\":\"15\",\"left\":1437,\"top\":134,\"zindex\":\"2\",\"d_type\":\"\",\"fundata\":\"\",\"r_data\":\"\",\"f_type\":\"img\"},\"Liquid_5858361\":{\"id\":\"Liquid_5858361\",\"f_title\":\"大屏文字\",\"f_color\":\"rgb(45, 109, 204)\",\"f_size\":\"20\",\"f_width\":\"226\",\"f_height\":\"294\",\"left\":\"1106\",\"top\":\"261\",\"zindex\":\"2\",\"d_type\":\"\",\"fundata\":\"0.55\",\"r_data\":\"\",\"f_type\":\"Liquid\"}},\"chart\":{\"bar_3903529\":{\"type\":\"2\",\"fun\":\"@get_test\"},\"gauge_4830160\":{\"type\":\"1\",\"fun\":\"0.65\"},\"gauge_4903544\":{\"type\":1,\"fun\":\"0.95\"},\"rosePlot_4934320\":{\"type\":\"2\",\"fun\":\"@get_test\"},\"area_5019136\":{\"type\":\"2\",\"fun\":\"@get_test\"},\"line_510080\":{\"type\":\"2\",\"fun\":\"@get_test\"},\"pie_5301249\":{\"type\":\"2\",\"fun\":\"@get_test\"},\"Liquid_5858361\":{\"type\":1,\"fun\":\"0.55\"}},\"f_ver\":\"v1.0\"}', '1080', '1920', 1639191750);

SET FOREIGN_KEY_CHECKS = 1;
