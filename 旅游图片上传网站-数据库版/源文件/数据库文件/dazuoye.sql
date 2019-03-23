/*
Navicat MySQL Data Transfer

Source Server         : localhost_3306
Source Server Version : 50717
Source Host           : localhost:3306
Source Database       : dazuoye

Target Server Type    : MYSQL
Target Server Version : 50717
File Encoding         : 65001

Date: 2018-06-24 21:26:37
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for rec
-- ----------------------------
DROP TABLE IF EXISTS `rec`;
CREATE TABLE `rec` (
  `rec_id` int(3) NOT NULL AUTO_INCREMENT,
  `rec_src` varchar(255) DEFAULT NULL,
  `rec_user_name` varchar(255) DEFAULT NULL,
  `rec_text` varchar(255) DEFAULT NULL,
  `rec_date` varchar(255) DEFAULT NULL,
  `rec_count` int(1) DEFAULT NULL,
  `rec_tag` varchar(255) DEFAULT NULL,
  `rec_class` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`rec_id`)
) ENGINE=InnoDB AUTO_INCREMENT=22 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of rec
-- ----------------------------
INSERT INTO `rec` VALUES ('1', './file/test1.png', '田甘迅', '昔人已乘黄鹤去，此地空余黄鹤楼。\r\n黄鹤一去不复返，白云千载空悠悠。', '06-15 22:13', '11', '黄鹤楼', '景物');
INSERT INTO `rec` VALUES ('2', './file/test2.png', '皮卡丘', '白日依山尽，黄河入海流。\r\n欲穷千里目，更上一层楼。', '06-15 23:03', '9', '鹳雀楼', '景物');
INSERT INTO `rec` VALUES ('13', './file/20180619211436u=2629069553,2461131198&fm=173&s=D0220FF18832128E9420FD200300F053&w=640&h=396&img.jpg', '小仙女', '月落乌啼霜满天，江枫渔火对愁眠。', '06月19日 21:14', '0', '枫桥', '景物');
INSERT INTO `rec` VALUES ('14', './file/20180619211927u=3173019512,1951630311&fm=173&s=B31050CC0988276E50A1751C0300D090&w=500&h=260&img.jpg', '小恐龙', '明月出天山，苍茫云海间。', '06月19日 21:19', '0', '关山', '景物');
INSERT INTO `rec` VALUES ('16', './file/20180619212215u=78628266,3684078830&fm=173&s=9FE6DE01DE9B545F909D158A0100E090&w=598&h=488&img.jpg', '小可爱', '日照香炉生紫烟， 遥看瀑布挂前川。', '06月19日 21:22', '0', '庐山', '景物');
INSERT INTO `rec` VALUES ('17', './file/20180619212450u=3402065109,2104570946&fm=173&s=9035CF345823AE8C9C9FF9CD0300B0A8&w=640&h=446&img.jpg', '小生气', '峨眉山月半轮秋，影入平羌江水流。', '06月19日 21:24', '1', '峨眉山', '景物');
INSERT INTO `rec` VALUES ('18', './file/20180619213011201410221834859.jpg', '小恐龙', '作为风靡热带海南岛的特色冰爽甜品，传统的海南清补凉主要是以红豆(绿豆)、薏米、、花生、空心粉等做成', '06月19日 21:30', '0', '清凉补', '小吃');
INSERT INTO `rec` VALUES ('19', '', '小猫猫', '描述', '06月24日 19:57', '0', '标签', '类别');
INSERT INTO `rec` VALUES ('20', '', '', '', '06月24日 19:59', '0', '', '');
INSERT INTO `rec` VALUES ('21', '', '小猫猫', '描述', '06月24日 19:59', '0', '标签', '类别');

-- ----------------------------
-- Table structure for rec1
-- ----------------------------
DROP TABLE IF EXISTS `rec1`;
CREATE TABLE `rec1` (
  `rec_id` int(3) NOT NULL AUTO_INCREMENT,
  `rec_src` varchar(255) DEFAULT NULL,
  `rec_user_name` varchar(255) DEFAULT NULL,
  `rec_text` varchar(255) DEFAULT NULL,
  `rec_date` varchar(255) DEFAULT NULL,
  `rec_count` int(1) DEFAULT NULL,
  `rec_tag` varchar(255) DEFAULT NULL,
  `rec_class` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`rec_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of rec1
-- ----------------------------
INSERT INTO `rec1` VALUES ('1', '22', '11', '11', '11', '11', '11', '11');

-- ----------------------------
-- Table structure for rec2
-- ----------------------------
DROP TABLE IF EXISTS `rec2`;
CREATE TABLE `rec2` (
  `rec_id` int(3) NOT NULL AUTO_INCREMENT,
  `rec_src` varchar(255) DEFAULT NULL,
  `rec_user_name` varchar(255) DEFAULT NULL,
  `rec_text` varchar(255) DEFAULT NULL,
  `rec_date` varchar(255) DEFAULT NULL,
  `rec_count` int(1) DEFAULT NULL,
  `rec_tag` varchar(255) DEFAULT NULL,
  `rec_class` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`rec_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of rec2
-- ----------------------------

-- ----------------------------
-- Table structure for user_info
-- ----------------------------
DROP TABLE IF EXISTS `user_info`;
CREATE TABLE `user_info` (
  `id` int(3) NOT NULL AUTO_INCREMENT,
  `username` varchar(255) NOT NULL,
  `password` varchar(255) NOT NULL,
  `访问次数` int(3) unsigned zerofill DEFAULT NULL,
  `上次访问时间` varchar(255) DEFAULT NULL,
  `推荐数` int(3) DEFAULT NULL,
  `点赞数` int(3) DEFAULT NULL,
  `被点赞数` int(3) DEFAULT NULL,
  `被推荐数` int(3) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=18 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of user_info
-- ----------------------------
INSERT INTO `user_info` VALUES ('1', '田甘迅', '123', '042', '06月20日 12:25', '10', '100', '0', '0');
INSERT INTO `user_info` VALUES ('2', '皮卡丘', '456', '009', '2018-06-19 08:55:38', '10', '100', '0', '0');
INSERT INTO `user_info` VALUES ('3', '小老鼠', '1234', '002', '2018-06-14 13:41:57', '10', '100', '0', '0');
INSERT INTO `user_info` VALUES ('4', '小老虎', '234', '003', '2018-06-14 13:51:36', '10', '100', '0', '0');
INSERT INTO `user_info` VALUES ('5', '小太阳', '123', '002', '2018-06-14 13:51:55', '10', '100', '0', '0');
INSERT INTO `user_info` VALUES ('6', '小兔子', '123', '001', '2018-06-14 13:49:10', '10', '100', '0', '0');
INSERT INTO `user_info` VALUES ('7', '小猫咪', '111', '001', '2018-06-15 15:58:56', '10', '100', '0', '0');
INSERT INTO `user_info` VALUES ('8', '小凶许', '111', '011', '2018-06-15 23:50:38', '10', '100', '0', '0');
INSERT INTO `user_info` VALUES ('9', '小青蛙', '123', '001', '2018-06-19 00:21:00', '10', '100', '0', '0');
INSERT INTO `user_info` VALUES ('10', '贾真慧', '123', '006', '2018-06-19 20:36:04', '10', '100', '0', '0');
INSERT INTO `user_info` VALUES ('11', '小月亮', '123', '002', '2018-06-19 20:53:01', '10', '100', '0', '0');
INSERT INTO `user_info` VALUES ('12', '小仙女', '123', '002', '06月19日 21:12', '10', '100', '0', '0');
INSERT INTO `user_info` VALUES ('13', '小恐龙', '123', '003', '06月19日 21:29', '10', '100', '0', '0');
INSERT INTO `user_info` VALUES ('14', '小可爱', '123', '003', '06月19日 21:50', '10', '100', '0', '0');
INSERT INTO `user_info` VALUES ('15', '小生气', '123', '017', '06月20日 20:57', '10', '100', '0', '0');
INSERT INTO `user_info` VALUES ('16', '', '', '001', '06月22日 20:49', '10', '100', '0', '0');
INSERT INTO `user_info` VALUES ('17', '小猫猫', '123', '005', '06月24日 19:59', '10', '100', '0', '0');
SET FOREIGN_KEY_CHECKS=1;
