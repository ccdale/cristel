-- MySQL dump 10.15  Distrib 10.0.12-MariaDB, for Linux (x86_64)
--
-- Host: localhost    Database: tv
-- ------------------------------------------------------
-- Server version	10.0.12-MariaDB-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `actor`
--

DROP TABLE IF EXISTS `actor`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `actor` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `actorname` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `actormap`
--

DROP TABLE IF EXISTS `actormap`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `actormap` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `actorid` int(11) NOT NULL,
  `scheduleid` int(11) NOT NULL,
  `chr` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `actorid` (`actorid`),
  KEY `scheduleid` (`scheduleid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `channel`
--

DROP TABLE IF EXISTS `channel`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `channel` (
  `rtid` int(11) NOT NULL,
  `freeviewid` int(11) NOT NULL,
  `channelname` varchar(50) NOT NULL,
  `favourite` tinyint(1) DEFAULT '0',
  `getdata` tinyint(1) DEFAULT '0',
  `favgroup` tinyint(2) DEFAULT '0',
  `sortorder` int(8) DEFAULT '0',
  `hasplusone` int(11) DEFAULT '0',
  `mux` int(11) DEFAULT NULL,
  PRIMARY KEY (`rtid`),
  KEY `channelname` (`channelname`),
  KEY `freeviewid` (`freeviewid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `epg`
--

DROP TABLE IF EXISTS `epg`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `epg` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `networkid` varchar(14) NOT NULL,
  `starttime` int(11) NOT NULL,
  `endtime` int(11) NOT NULL,
  `title` varchar(40) NOT NULL,
  `description` varchar(255) DEFAULT NULL,
  `content` varchar(40) DEFAULT NULL,
  `series` varchar(40) DEFAULT NULL,
  `eventid` varchar(20) NOT NULL,
  `fromrtid` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `freeview`
--

DROP TABLE IF EXISTS `freeview`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `freeview` (
  `id` int(11) NOT NULL,
  `conditionalaccess` tinyint(1) NOT NULL,
  `networkid` varchar(14) NOT NULL,
  `mux` int(11) NOT NULL,
  `source` varchar(10) NOT NULL,
  `defaultauth` varchar(20) NOT NULL,
  `pmtpid` varchar(10) NOT NULL,
  `channelname` varchar(30) NOT NULL,
  `type` enum('r','t','d') NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `genre`
--

DROP TABLE IF EXISTS `genre`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `genre` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `genrename` varchar(25) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `genrename` (`genrename`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `previousrecord`
--

DROP TABLE IF EXISTS `previousrecord`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `previousrecord` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `content` varchar(40) NOT NULL,
  `filename` varchar(255) NOT NULL,
  `title` varchar(255) DEFAULT NULL,
  `subtitle` varchar(255) DEFAULT NULL,
  `episode` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `schedule`
--

DROP TABLE IF EXISTS `schedule`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `schedule` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `channel` int(11) NOT NULL,
  `title` varchar(255) NOT NULL,
  `subtitle` varchar(255) DEFAULT NULL,
  `episode` varchar(255) DEFAULT NULL,
  `year` int(11) DEFAULT NULL,
  `director` int(11) DEFAULT NULL,
  `premiere` tinyint(1) DEFAULT NULL,
  `film` tinyint(1) DEFAULT NULL,
  `isrepeat` tinyint(1) DEFAULT NULL,
  `subtitles` tinyint(1) DEFAULT NULL,
  `widescreen` tinyint(1) DEFAULT NULL,
  `newseries` tinyint(1) DEFAULT NULL,
  `deafsigned` tinyint(1) DEFAULT NULL,
  `blackandWhite` tinyint(1) DEFAULT NULL,
  `star` tinyint(4) DEFAULT NULL,
  `certificate` tinyint(4) DEFAULT NULL,
  `description` text,
  `genre` int(11) DEFAULT NULL,
  `choice` tinyint(1) DEFAULT NULL,
  `starttime` int(11) NOT NULL,
  `endtime` int(11) NOT NULL,
  `duration` int(11) NOT NULL,
  `programid` varchar(128) DEFAULT NULL,
  `seriesid` varchar(128) DEFAULT NULL,
  `record` enum('c','l','n','p','y') DEFAULT 'n',
  PRIMARY KEY (`id`),
  KEY `title` (`title`),
  KEY `description` (`description`(255)),
  KEY `channel` (`channel`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `seriesrecord`
--

DROP TABLE IF EXISTS `seriesrecord`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `seriesrecord` (
  `id` varchar(40) NOT NULL,
  `title` varchar(40) NOT NULL,
  `channel` int(11) DEFAULT NULL,
  `priority` int(11) DEFAULT '10',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `system`
--

DROP TABLE IF EXISTS `system`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `system` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `confname` varchar(255) NOT NULL,
  `confval` text NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2014-07-26  2:16:25
