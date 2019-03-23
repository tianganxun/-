/*
Navicat MySQL Data Transfer

Source Server         : localhost_3306
Source Server Version : 50717
Source Host           : localhost:3306
Source Database       : 作业布置

Target Server Type    : MYSQL
Target Server Version : 50717
File Encoding         : 65001

Date: 2018-01-20 19:06:56
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for login1
-- ----------------------------
DROP TABLE IF EXISTS `login1`;
CREATE TABLE `login1` (
  `username` varchar(255) DEFAULT NULL,
  `pwd` varchar(255) DEFAULT NULL,
  `leixing` tinyint(4) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of login1
-- ----------------------------
INSERT INTO `login1` VALUES ('1', '1', '1');
INSERT INTO `login1` VALUES ('2', '2', '0');
INSERT INTO `login1` VALUES ('田甘迅', '123', '1');
INSERT INTO `login1` VALUES ('刘备', '22', '0');
INSERT INTO `login1` VALUES ('关羽', '33', '0');
INSERT INTO `login1` VALUES ('张飞', '44', '0');
INSERT INTO `login1` VALUES ('曹操', '55', '0');

-- ----------------------------
-- Table structure for zuoye
-- ----------------------------
DROP TABLE IF EXISTS `zuoye`;
CREATE TABLE `zuoye` (
  `作业序号` varchar(255) DEFAULT NULL,
  `banji` varchar(255) DEFAULT NULL,
  `作业科目` varchar(255) DEFAULT NULL,
  `作业概述` varchar(255) DEFAULT NULL,
  `截止日期` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

-- ----------------------------
-- Records of zuoye
-- ----------------------------
INSERT INTO `zuoye` VALUES ('2', '2', '英语', '写一篇关于马的英语作文', '2018-1-31');
INSERT INTO `zuoye` VALUES ('9', '1', '语文', '抄写《赤壁赋》', '2018-2-8');
INSERT INTO `zuoye` VALUES ('19', '2', '模式识别', '第三章习题', '2018-1-31');
INSERT INTO `zuoye` VALUES ('1', '3', '数学', '第四章习题', '2018-1-31');
INSERT INTO `zuoye` VALUES ('3', '2', '操作系统', '第四章习题', '2018-1-31');
INSERT INTO `zuoye` VALUES ('4', '1', '编译原理', '第四章习题', '2018-1-31');
INSERT INTO `zuoye` VALUES ('5', '3', '系统结构', '第四章习题', '2018-1-31');
SET FOREIGN_KEY_CHECKS=1;
