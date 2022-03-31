## 微信web端 依赖sql 可选择性执行非升级必须执行sql [2022-03-31] [zf]


CREATE TABLE `yz_wx_config` (
  `id` char(50) NOT NULL COMMENT '\r\nid',
  `app_id` varchar(50) NOT NULL COMMENT '公众号id',
  `app_secret` varchar(50) NOT NULL COMMENT '公众号密钥',
  `app_token` varchar(100) NOT NULL COMMENT '接入token',
  `origin_id` varchar(30) DEFAULT NULL COMMENT '原始id',
  `mch_id` varchar(30) DEFAULT NULL COMMENT '商户号',
  `paterner_key` varchar(50) DEFAULT NULL COMMENT '商户密钥',
  `index_url` varchar(500) DEFAULT NULL COMMENT '接入路径',
  `back_url` varchar(500) DEFAULT NULL COMMENT '回调路径',
  `status` int(2) DEFAULT '0' COMMENT '开启状态：0未开启-1开启',
  `create_time` datetime DEFAULT NULL COMMENT '创建时间',
  `update_time` datetime DEFAULT NULL COMMENT '更新时间',
  `create_user` varchar(30) NOT NULL COMMENT '更新人',
  `company_id` varchar(50) DEFAULT NULL COMMENT '区分公众号',
  `agent_id` varchar(500) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT NULL COMMENT '所属企业id',
  `native_url` varchar(100) DEFAULT NULL COMMENT '扫码回调',
  `auth_txt` varchar(80) DEFAULT NULL COMMENT '授权文件名',
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE KEY `app_id` (`app_id`) USING BTREE,
  UNIQUE KEY `app_secret` (`app_secret`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='微信公众号配置';



CREATE TABLE `yz_wx_menu` (
  `id` char(50) NOT NULL COMMENT 'id',
  `menu_id` int(10) NOT NULL COMMENT '菜单id',
  `menu_name` varchar(30) NOT NULL COMMENT '菜单名',
  `menu_pid` int(10) DEFAULT '0' COMMENT '父级ID，一级菜单默认0',
  `menu_url` varchar(300) DEFAULT '0' COMMENT '菜单：一级菜单默认0',
  `menu_status` int(2) DEFAULT '0' COMMENT '状态：0未开启-1开启',
  `create_time` datetime DEFAULT NULL COMMENT '创建时间',
  `update_time` datetime DEFAULT NULL COMMENT '更新时间',
  `create_user` varchar(30) DEFAULT NULL COMMENT '更新人',
  `app_id` varchar(100) DEFAULT NULL COMMENT '所属微信公众号',
  `menu_type` varchar(50) DEFAULT NULL,
  `media_id` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='微信菜单表';





CREATE TABLE `yz_wechat_user` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `user_name` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `openid` varchar(40) NOT NULL,
  `head_image` varchar(300) DEFAULT NULL,
  `iccid` varchar(30) DEFAULT NULL,
  `sex` int(2) DEFAULT '33',
  `create_time` datetime DEFAULT NULL,
  `city` varchar(40) DEFAULT NULL,
  `province` varchar(40) DEFAULT NULL,
  `country` varchar(30) DEFAULT NULL,
  `app_id` varchar(50) DEFAULT NULL,
  `status` int(2) DEFAULT NULL,
  `bind_status` int(2) DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=469 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;




CREATE TABLE `yz_wx_autoreply` (
  `id` int(10) NOT NULL AUTO_INCREMENT COMMENT 'id',
  `auto_id` int(3) NOT NULL COMMENT '回复ID，区分关注(100)，关键词(200)，默认（1）',
  `keywords` varchar(30) DEFAULT NULL COMMENT '关键字',
  `content` varchar(500) DEFAULT NULL COMMENT '内容',
  `create_time` datetime DEFAULT NULL COMMENT '创建时间',
  `create_user` varchar(30) DEFAULT NULL COMMENT '更新人',
  `app_id` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='微信自动回复';


## 微信端购物界面依赖表

CREATE TABLE `yz_wx_by_brand_info` (
  `brand_id` smallint(5) unsigned NOT NULL AUTO_INCREMENT COMMENT '品牌ID',
  `brand_name` varchar(50) NOT NULL COMMENT '品牌名称',
  `telephone` varchar(50) NOT NULL COMMENT '联系电话',
  `brand_web` varchar(100) DEFAULT NULL COMMENT '品牌网络',
  `brand_logo` varchar(100) DEFAULT NULL COMMENT '品牌logo URL',
  `brand_desc` varchar(150) DEFAULT NULL COMMENT '品牌描述',
  `brand_status` tinyint(4) NOT NULL DEFAULT '0' COMMENT '品牌状态,0禁用,1启用',
  `brand_order` tinyint(4) NOT NULL DEFAULT '0' COMMENT '排序',
  `modified_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '最后修改时间',
  PRIMARY KEY (`brand_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='品牌信息表';





CREATE TABLE `yz_wx_by_product_agent` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `dept_id` int(11) DEFAULT NULL COMMENT '代理编号 与 agent_id 一致 但需要用于系统结构 数据验证',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=103 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='商品购买代理关联表';


CREATE TABLE `yz_wx_by_product_category` (
  `category_id` smallint(5) unsigned NOT NULL AUTO_INCREMENT COMMENT '分类ID',
  `category_name` varchar(10) NOT NULL COMMENT '分类名称',
  `category_code` varchar(10) NOT NULL COMMENT '分类编码',
  `parent_id` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT '父分类ID',
  `category_level` tinyint(4) NOT NULL DEFAULT '1' COMMENT '分类层级',
  `category_status` tinyint(4) NOT NULL DEFAULT '1' COMMENT '分类状态',
  `modified_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '最后修改时间',
  PRIMARY KEY (`category_id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='商品分类表';


CREATE TABLE `yz_wx_by_product_comment` (
  `comment_id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '评论ID',
  `product_id` int(10) unsigned NOT NULL COMMENT '商品ID',
  `order_id` bigint(20) unsigned NOT NULL COMMENT '订单ID',
  `customer_id` int(10) unsigned NOT NULL COMMENT '用户ID',
  `title` varchar(50) NOT NULL COMMENT '评论标题',
  `content` varchar(300) NOT NULL COMMENT '评论内容',
  `audit_status` tinyint(4) NOT NULL COMMENT '审核状态：0未审核，1已审核',
  `audit_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '评论时间',
  `modified_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '最后修改时间',
  PRIMARY KEY (`comment_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='商品评论表';


CREATE TABLE `yz_wx_by_product_info` (
  `product_id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '商品ID',
  `product_core` char(16) NOT NULL COMMENT '商品编码',
  `product_name` varchar(20) NOT NULL COMMENT '商品名称',
  `bar_code` varchar(50) DEFAULT NULL COMMENT '国条码',
  `brand_id` int(10) unsigned DEFAULT NULL COMMENT '品牌表的ID',
  `one_category_id` smallint(5) unsigned NOT NULL COMMENT '一级分类ID',
  `two_category_id` smallint(5) unsigned DEFAULT NULL COMMENT '二级分类ID',
  `three_category_id` smallint(5) unsigned DEFAULT NULL COMMENT '三级分类ID',
  `supplier_id` int(10) unsigned DEFAULT NULL COMMENT '商品的供应商ID',
  `price` decimal(8,2) NOT NULL COMMENT '商品销售价格',
  `average_cost` decimal(18,2) DEFAULT NULL COMMENT '商品加权平均成本',
  `publish_status` tinyint(4) NOT NULL DEFAULT '0' COMMENT '上下架状态：0下架1上架',
  `audit_status` tinyint(4) NOT NULL DEFAULT '0' COMMENT '审核状态：0未审核，1已审核',
  `weight` float DEFAULT NULL COMMENT '商品重量',
  `length` float DEFAULT NULL COMMENT '商品长度',
  `height` float DEFAULT NULL COMMENT '商品高度',
  `width` float DEFAULT NULL COMMENT '商品宽度',
  `color_type` enum('红','黄','蓝','黑') DEFAULT NULL,
  `production_date` datetime DEFAULT NULL COMMENT '生产日期',
  `shelf_life` int(11) DEFAULT NULL COMMENT '商品有效期',
  `descript` text COMMENT '商品描述',
  `indate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '商品录入时间',
  `modified_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '最后修改时间',
  PRIMARY KEY (`product_id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='商品信息表';


CREATE TABLE `yz_wx_by_product_pic_info` (
  `product_pic_id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '商品图片ID',
  `product_id` int(10) unsigned NOT NULL COMMENT '商品ID',
  `pic_desc` varchar(50) DEFAULT NULL COMMENT '图片描述',
  `pic_url` varchar(200) NOT NULL COMMENT '图片URL',
  `is_master` tinyint(4) NOT NULL DEFAULT '0' COMMENT '是否主图：0.非主图1.主图',
  `pic_order` tinyint(4) NOT NULL DEFAULT '0' COMMENT '图片排序',
  `pic_status` tinyint(4) NOT NULL DEFAULT '1' COMMENT '图片是否有效：0无效 1有效',
  `modified_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '最后修改时间',
  PRIMARY KEY (`product_pic_id`)
) ENGINE=InnoDB AUTO_INCREMENT=54 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='商品图片信息表';


CREATE TABLE `yz_wx_by_supplier_info` (
  `supplier_id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '供应商ID',
  `supplier_code` char(8) NOT NULL COMMENT '供应商编码',
  `supplier_name` char(50) NOT NULL COMMENT '供应商名称',
  `supplier_type` tinyint(4) NOT NULL COMMENT '供应商类型：1.自营，2.平台',
  `link_man` varchar(10) NOT NULL COMMENT '供应商联系人',
  `phone_number` varchar(50) NOT NULL COMMENT '联系电话',
  `bank_name` varchar(50) NOT NULL COMMENT '供应商开户银行名称',
  `bank_account` varchar(50) NOT NULL COMMENT '银行账号',
  `address` varchar(200) NOT NULL COMMENT '供应商地址',
  `supplier_status` tinyint(4) NOT NULL DEFAULT '0' COMMENT '状态：0禁止，1启用',
  `modified_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '最后修改时间',
  PRIMARY KEY (`supplier_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='供应商信息表';






