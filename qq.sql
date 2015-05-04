-- phpMyAdmin SQL Dump
-- version 4.1.8
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: 2014-06-26 08:08:26
-- 服务器版本： 5.5.33-MariaDB
-- PHP Version: 5.4.20

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `qq`
--

-- --------------------------------------------------------

--
-- 表的结构 `usr_friendslist`
--

CREATE TABLE IF NOT EXISTS `usr_friendslist` (
  `uid` varchar(10) NOT NULL,
  `fid` varchar(10) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- 转存表中的数据 `usr_friendslist`
--

INSERT INTO `usr_friendslist` (`uid`, `fid`) VALUES
('307390184', '228401807'),
('228401807', '307390184'),
('123', '307390184'),
('123', '228401807'),
('307390184', '123'),
('228401807', '123'),
('123', '4'),
('4', '123'),
('4', '228401807');

-- --------------------------------------------------------

--
-- 表的结构 `usr_head_portrait`
--

CREATE TABLE IF NOT EXISTS `usr_head_portrait` (
  `uid` varchar(10) NOT NULL,
  `head_portrait_location` varchar(20) NOT NULL,
  PRIMARY KEY (`uid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- 转存表中的数据 `usr_head_portrait`
--

INSERT INTO `usr_head_portrait` (`uid`, `head_portrait_location`) VALUES
('123', './images/123.jpg'),
('228401807', './images/head4.jpg'),
('307390184', './images/head2.jpg'),
('4', './images/head1.jpg'),
('5', './images/head1.jpg');

-- --------------------------------------------------------

--
-- 表的结构 `usr_info`
--

CREATE TABLE IF NOT EXISTS `usr_info` (
  `uid` varchar(10) NOT NULL,
  `pwd` varchar(15) NOT NULL,
  `nickname` varchar(20) NOT NULL,
  PRIMARY KEY (`uid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- 转存表中的数据 `usr_info`
--

INSERT INTO `usr_info` (`uid`, `pwd`, `nickname`) VALUES
('123', '12', 'feng'),
('228401807', '576801807', 'Cheng Chen'),
('307390184', '941956', 'Huang Tao'),
('4', '12', 'hi'),
('5', '123', 'ho');

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
