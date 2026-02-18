USE [PBOption]
/***************************************
* UPDATE MAP INFO
***************************************/
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 1) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(1, N'TD_PORTAKABA', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 1) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_PORTAKABA', MapState = 1 WHERE Idx = 1
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 2) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(2, N'TD_REDROCK', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 2) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_REDROCK', MapState = 1 WHERE Idx = 2
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 3) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(3, N'TD_LIBRARY', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 3) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_LIBRARY', MapState = 1 WHERE Idx = 3
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 4) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(4, N'TD_MSTATION', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 4) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_MSTATION', MapState = 1 WHERE Idx = 4
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 5) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(5, N'TD_MIDNIGHTZONE', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 5) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_MIDNIGHTZONE', MapState = 1 WHERE Idx = 5
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 6) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(6, N'TD_UPTOWN', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 6) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_UPTOWN', MapState = 1 WHERE Idx = 6
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 7) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(7, N'TD_BURNINGHALL', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 7) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_BURNINGHALL', MapState = 1 WHERE Idx = 7
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 8) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(8, N'TD_DSQUAD', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 8) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_DSQUAD', MapState = 1 WHERE Idx = 8
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 9) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(9, N'TD_CRACKDOWN', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 9) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_CRACKDOWN', MapState = 1 WHERE Idx = 9
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 10) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(10, N'TD_SAINTMANSION', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 10) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_SAINTMANSION', MapState = 1 WHERE Idx = 10
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 11) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(11, N'TD_EASTERNROAD', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 11) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_EASTERNROAD', MapState = 1 WHERE Idx = 11
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 12) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(12, N'TD_DOWNTOWN', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 12) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_DOWNTOWN', MapState = 1 WHERE Idx = 12
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 13) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(13, N'TD_LUXVILLE', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 13) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_LUXVILLE', MapState = 1 WHERE Idx = 13
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 14) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(14, N'TD_BLOWCITY', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 14) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_BLOWCITY', MapState = 1 WHERE Idx = 14
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 15) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(15, N'TD_STORMTUBE', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 15) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_STORMTUBE', MapState = 1 WHERE Idx = 15
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 16) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(16, N'TD_GIRAN2', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 16) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_GIRAN2', MapState = 1 WHERE Idx = 16
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 17) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(17, N'TD_BREAKDOWN', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 17) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_BREAKDOWN', MapState = 1 WHERE Idx = 17
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 18) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(18, N'TD_TRAININGCAMP', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 18) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_TRAININGCAMP', MapState = 1 WHERE Idx = 18
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 19) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(19, N'TD_SENTRYBASE', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 19) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_SENTRYBASE', MapState = 1 WHERE Idx = 19
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 20) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(20, N'TD_DESERTCAMP', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 20) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_DESERTCAMP', MapState = 1 WHERE Idx = 20
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 21) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(21, N'TD_KICKPOINT', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 21) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_KICKPOINT', MapState = 1 WHERE Idx = 21
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 22) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(22, N'TD_FACEROCK', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 22) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_FACEROCK', MapState = 1 WHERE Idx = 22
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 23) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(23, N'TD_SUPPLYBASE', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 23) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_SUPPLYBASE', MapState = 1 WHERE Idx = 23
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 24) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(24, N'TD_SANDSTORM', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 24) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_SANDSTORM', MapState = 1 WHERE Idx = 24
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 25) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(25, N'BB_DOWNTOWN', 2)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 25) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'BB_DOWNTOWN', MapState = 2 WHERE Idx = 25
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 26) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(26, N'BB_LUXVILLE', 2)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 26) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'BB_LUXVILLE', MapState = 2 WHERE Idx = 26
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 27) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(27, N'BB_OUTPOST', 2)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 27) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'BB_OUTPOST', MapState = 2 WHERE Idx = 27
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 28) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(28, N'BB_BLOWCITY', 2)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 28) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'BB_BLOWCITY', MapState = 2 WHERE Idx = 28
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 29) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(29, N'BB_STORMTUBE', 2)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 29) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'BB_STORMTUBE', MapState = 2 WHERE Idx = 29
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 30) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(30, N'BB_SENTRYBASE', 2)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 30) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'BB_SENTRYBASE', MapState = 2 WHERE Idx = 30
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 31) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(31, N'BB_HOSPITAL', 2)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 31) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'BB_HOSPITAL', MapState = 2 WHERE Idx = 31
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 32) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(32, N'BB_DOWNTOWN2', 2)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 32) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'BB_DOWNTOWN2', MapState = 2 WHERE Idx = 32
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 33) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(33, N'BB_SHOPPINGCENTER', 2)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 33) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'BB_SHOPPINGCENTER', MapState = 2 WHERE Idx = 33
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 34) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(34, N'BB_SANDSTORM', 2)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 34) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'BB_SANDSTORM', MapState = 2 WHERE Idx = 34
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 35) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(35, N'DS_BREAKDOWN', 3)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 35) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'DS_BREAKDOWN', MapState = 3 WHERE Idx = 35
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 36) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(36, N'DS_GIRAN', 3)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 36) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'DS_GIRAN', MapState = 3 WHERE Idx = 36
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 37) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(37, N'DS_GIRAN2', 3)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 37) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'DS_GIRAN2', MapState = 3 WHERE Idx = 37
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 38) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(38, N'DS_HELISPOT', 3)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 38) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'DS_HELISPOT', MapState = 3 WHERE Idx = 38
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 39) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(39, N'DF_BLACKPANTHER', 5)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 39) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'DF_BLACKPANTHER', MapState = 5 WHERE Idx = 39
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 40) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(40, N'ES_BREEDINGNEST', 7)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 40) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'ES_BREEDINGNEST', MapState = 7 WHERE Idx = 40
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 41) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(41, N'ES_DUPTOWN', 7)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 41) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'ES_DUPTOWN', MapState = 7 WHERE Idx = 41
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 42) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(42, N'ES_DINOBREAKDOWN', 7)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 42) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'ES_DINOBREAKDOWN', MapState = 7 WHERE Idx = 42
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 43) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(43, N'DM_1FACTORYRUINS', 11)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 43) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'DM_1FACTORYRUINS', MapState = 11 WHERE Idx = 43
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 44) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(44, N'TUTORIAL', 10)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 44) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TUTORIAL', MapState = 10 WHERE Idx = 44
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 45) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(45, N'TD_SHOPPINGCENTER', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 45) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_SHOPPINGCENTER', MapState = 1 WHERE Idx = 45
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 46) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(46, N'TD_SAFARI', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 46) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_SAFARI', MapState = 1 WHERE Idx = 46
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 47) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(47, N'TD_DRAGONALLEY', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 47) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_DRAGONALLEY', MapState = 1 WHERE Idx = 47
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 48) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(48, N'TD_MACHUPICHU', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 48) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_MACHUPICHU', MapState = 1 WHERE Idx = 48
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 49) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(49, N'BB_CARGOSHIP', 2)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 49) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'BB_CARGOSHIP', MapState = 2 WHERE Idx = 49
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 50) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(50, N'TD_TWOTOWERS', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 50) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_TWOTOWERS', MapState = 1 WHERE Idx = 50
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 51) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(51, N'CC_CRACKDOWN', 12)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 51) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'CC_CRACKDOWN', MapState = 12 WHERE Idx = 51
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 52) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(52, N'CC_SAINTMANSION', 12)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 52) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'CC_SAINTMANSION', MapState = 12 WHERE Idx = 52
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 53) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(53, N'CC_DSQUAD', 12)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 53) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'CC_DSQUAD', MapState = 12 WHERE Idx = 53
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 54) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(54, N'CC_PORTAKABA', 12)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 54) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'CC_PORTAKABA', MapState = 12 WHERE Idx = 54
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 55) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(55, N'CC_MSTATION', 12)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 55) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'CC_MSTATION', MapState = 12 WHERE Idx = 55
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 56) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(56, N'CC_TRAININGCAMP', 12)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 56) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'CC_TRAININGCAMP', MapState = 12 WHERE Idx = 56
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 57) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(57, N'TD_ANGKORRUINS', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 57) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_ANGKORRUINS', MapState = 1 WHERE Idx = 57
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 58) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(58, N'CC_REDROCK', 12)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 58) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'CC_REDROCK', MapState = 12 WHERE Idx = 58
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 59) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(59, N'CC_SAFARI', 12)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 59) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'CC_SAFARI', MapState = 12 WHERE Idx = 59
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 60) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(60, N'TD_GHOSTTOWN', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 60) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_GHOSTTOWN', MapState = 1 WHERE Idx = 60
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 61) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(61, N'TD_GRANDBAZAAR', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 61) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_GRANDBAZAAR', MapState = 1 WHERE Idx = 61
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 62) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(62, N'CC_DINOLAB', 12)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 62) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'CC_DINOLAB', MapState = 12 WHERE Idx = 62
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 63) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(63, N'TD_UNDER23', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 63) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_UNDER23', MapState = 1 WHERE Idx = 63
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 64) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(64, N'TD_TAIPEICITYMALL', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 64) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_TAIPEICITYMALL', MapState = 1 WHERE Idx = 64
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 65) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(65, N'TD_SAFARI2', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 65) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_SAFARI2', MapState = 1 WHERE Idx = 65
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 66) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(66, N'BB_AIRPORT', 2)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 66) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'BB_AIRPORT', MapState = 2 WHERE Idx = 66
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 67) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(67, N'CC_SAFARI2', 12)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 67) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'CC_SAFARI2', MapState = 12 WHERE Idx = 67
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 68) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(68, N'TD_METRO', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 68) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_METRO', MapState = 1 WHERE Idx = 68
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 69) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(69, N'DM_2SEWERS', 11)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 69) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'DM_2SEWERS', MapState = 11 WHERE Idx = 69
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 70) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(70, N'DM_3MILITARYBASE', 11)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 70) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'DM_3MILITARYBASE', MapState = 11 WHERE Idx = 70
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 71) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(71, N'DM_4DINOLAB', 11)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 71) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'DM_4DINOLAB', MapState = 11 WHERE Idx = 71
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 72) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(72, N'DM_5DOWNTOWNSQUARE', 11)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 72) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'DM_5DOWNTOWNSQUARE', MapState = 11 WHERE Idx = 72
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 73) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(73, N'DM_6STONYDESERT', 11)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 73) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'DM_6STONYDESERT', MapState = 11 WHERE Idx = 73
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 74) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(74, N'DM_0RANDOMSTAGE', 11)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 74) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'DM_0RANDOMSTAGE', MapState = 11 WHERE Idx = 74
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 75) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(75, N'DM_7DRAGONNEST', 11)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 75) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'DM_7DRAGONNEST', MapState = 11 WHERE Idx = 75
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 76) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(76, N'DM_8UNKNOWN', 11)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 76) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'DM_8UNKNOWN', MapState = 11 WHERE Idx = 76
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 77) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(77, N'TD_RUSH_HOUR', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 77) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_RUSH_HOUR', MapState = 1 WHERE Idx = 77
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 78) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(78, N'TD_CARGO_PORT', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 78) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_CARGO_PORT', MapState = 1 WHERE Idx = 78
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 79) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(79, N'TD_BLACKMAMBA', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 79) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_BLACKMAMBA', MapState = 1 WHERE Idx = 79
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 80) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(80, N'BB_SAFEHOUSE', 2)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 80) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'BB_SAFEHOUSE', MapState = 2 WHERE Idx = 80
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 81) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(81, N'TD_HOLIDAY', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 81) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_HOLIDAY', MapState = 1 WHERE Idx = 81
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 82) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(82, N'TD_WESTSTATION', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 82) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_WESTSTATION', MapState = 1 WHERE Idx = 82
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 83) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(83, N'BB_HARDROCK', 2)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 83) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'BB_HARDROCK', MapState = 2 WHERE Idx = 83
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 84) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(84, N'TD_HOUSEMUSEUM', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 84) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_HOUSEMUSEUM', MapState = 1 WHERE Idx = 84
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 85) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(85, N'TD_PUMPKINHOLLOW', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 85) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_PUMPKINHOLLOW', MapState = 1 WHERE Idx = 85
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 86) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(86, N'TD_ROTHENVILLAGE', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 86) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_ROTHENVILLAGE', MapState = 1 WHERE Idx = 86
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 87) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(87, N'UU_HOLIDAY', 13)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 87) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'UU_HOLIDAY', MapState = 13 WHERE Idx = 87
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 88) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(88, N'TD_WATERCOMPLEX', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 88) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_WATERCOMPLEX', MapState = 1 WHERE Idx = 88
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 89) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(89, N'TD_HOTELCAMOUFLAGE', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 89) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_HOTELCAMOUFLAGE', MapState = 1 WHERE Idx = 89
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 90) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(90, N'TD_BATTLESHIP', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 90) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_BATTLESHIP', MapState = 1 WHERE Idx = 90
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 91) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(91, N'BB_RAMPARTTOWN', 2)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 91) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'BB_RAMPARTTOWN', MapState = 2 WHERE Idx = 91
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 92) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(92, N'BB_BALLISTIC', 2)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 92) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'BB_BALLISTIC', MapState = 2 WHERE Idx = 92
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 93) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(93, N'TD_RAMPARTTOWN', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 93) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_RAMPARTTOWN', MapState = 1 WHERE Idx = 93
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 94) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(94, N'BB_FALLUCITY', 2)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 94) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'BB_FALLUCITY', MapState = 2 WHERE Idx = 94
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 95) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(95, N'TD_TESTMAP', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 95) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_TESTMAP', MapState = 1 WHERE Idx = 95
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 96) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(96, N'EH_HOLIDAY', 13)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 96) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'EH_HOLIDAY', MapState = 13 WHERE Idx = 96
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 97) = 0 ) INSERT INTO dbo.TPBF_Map (Idx, MapName, MapState) VALUES(97, N'TD_TEST', 1)
IF ( (SELECT COUNT(*) FROM dbo.TPBF_Map WHERE Idx = 97) != 0 ) UPDATE dbo.TPBF_Map SET MapName = N'TD_TEST', MapState = 1 WHERE Idx = 97
