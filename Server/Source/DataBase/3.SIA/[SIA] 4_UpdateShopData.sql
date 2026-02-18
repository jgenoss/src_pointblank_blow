USE [ShopInventoryAgent]
/***************************************
* UPDATE ITEM INFO 
***************************************/
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'Male Default Set',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'Female Default Set',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'Special General Beret',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'Toy Hat',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'Cangaceiro Hat',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'Anti Flash Bang Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'Silver License',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'Gold License',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS New M4 Supply Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS Rangemaster Supply Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS RB454 Supply Kit 1D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS Dual Uzi Supply Kit 1D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS RM Supply Kit 3D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS RM Supply Kit 7D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS RB454 Supply Kit 3D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS RB454 Supply Kit 7D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS Famas G2 Supply Kit 1D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS Dual Uzi Supply Kit 3D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS Dual Uzi Supply Kit 7D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS Dual Uzi Supply Kit 30D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS Famas G2 Supply Kit 3D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS Famas G2 Supply Kit 7D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS Famas G2 Supply Kit 30D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'1,000 Point',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'10,000 Point',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'30,000 Point',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'500 Point',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'5,000 Point',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS DSR-1 Random Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS Dual K-413 Random Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS New M4 Supply Kit 3D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS New M4 Supply Kit 7D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS New M4 Supply Kit 30D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS AUG A3 Random Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS Garena Premium Gacha C',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS AK Supply Kit Elite',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS M4 Supply Kit Elite',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS Super Lucky Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'Black Gacha AUGA3',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'Black Gacha Kriss',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'Black Gacha L115A1',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS Brazuca Supply Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS 1Day Gacha',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS PSG1 Elite RandomBox',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS Super Box2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS SVU Elite RandomBox',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'DragunovElite SupplyKit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS SPAS Elite RandomBox',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'G36C Elite Supply Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS PBIC 2014 Gacha TR',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS PBIC 2014 Gacha MENA',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS PBIC 2014 Gacha ID',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS NewWeapon Supply Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'CS SPAS Elite Gacha TH',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'Toy series SupplyKit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'2015_B.O.B_SupplyKIT',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'BestofLatin2_Supplykit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (0,0,0,0,0,N'License Sl. 30D 1D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103001,0,0,0,0,N'SG 550 Ext',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103002,0,0,0,0,N'AK47 Ext',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103003,0,0,0,0,N'M4A1 Ext',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103004,0,0,0,0,N'K-2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103005,0,0,0,0,N'F2000 Ext',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103009,0,0,0,0,N'AK-47 G.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103010) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103010,0,0,0,0,N'M4A1 S.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103011) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103011,0,0,0,0,N'K-201 Ext',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103013) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103013,0,0,0,0,N'G36C Ext',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103014) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103014,0,0,0,0,N'SG 550 S.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103015) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103015,0,0,0,0,N'AK SOPMOD',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103017) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103017,0,0,0,0,N'AK47 Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103018) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103018,0,0,0,0,N'SG 550 MB',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103019) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103019,0,0,0,0,N'SG 550 Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103020) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103020,0,0,0,0,N'SG 550 Wh.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103021) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103021,0,0,0,0,N'M4A1 Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103022) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103022,0,0,0,0,N'M4A1 Wh.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103023) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103023,0,0,0,0,N'M4A1 G.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103024) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103024,0,0,0,0,N'M4A1 G.+',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103025) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103025,0,0,0,0,N'AK47 Sl+',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103026) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103026,0,0,0,0,N'SG 550 Metal Blue',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103027) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103027,0,0,0,0,N'G36C Ext. Event',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103028) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103028,0,0,0,0,N'AUG A3 Event',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103029) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103029,0,0,0,0,N'G36C Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103030) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103030,0,0,0,0,N'SG 550 Mb. +20%',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103031) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103031,0,0,0,0,N'G36C S. + 20',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103032) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103032,0,0,0,0,N'F2000 Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103033) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103033,0,0,0,0,N'AK SOPMOD +20',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103034) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103034,0,0,0,0,N'G36C Sl. + 20',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103035) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103035,0,0,0,0,N'SG 550 Sp D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103036) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103036,0,0,0,0,N'AUG A3',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103037) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103037,0,0,0,0,N'AUG A3 G.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103038) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103038,0,0,0,0,N'G36C Ext D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103039) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103039,0,0,0,0,N'AK SOPMOD D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103040) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103040,0,0,0,0,N'AUG A3 D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103041) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103041,0,0,0,0,N'AK SOPMOD CG',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103042) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103042,0,0,0,0,N'M4 SR-16 LV1',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103043) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103043,0,0,0,0,N'M4 SR-16 LV2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103044) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103044,0,0,0,0,N'M4 SR-16 LV3',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103045) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103045,0,0,0,0,N'M4 SR-16 F.C',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103046) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103046,0,0,0,0,N'F2000 Sl D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103048) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103048,0,0,0,0,N'AUG A3 Black',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103049) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103049,0,0,0,0,N'Famas G2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103050) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103050,0,0,0,0,N'Famas G2 Commando',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103051) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103051,0,0,0,0,N'Famas G2 Sniper',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103052) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103052,0,0,0,0,N'Famas G2 M203',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103053) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103053,0,0,0,0,N'SS2-V4 PARA Sniper',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103054) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103054,0,0,0,0,N'AK47 Gold D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103055) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103055,0,0,0,0,N'M4A1 Wh. D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103056) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103056,0,0,0,0,N'SG 550 Sl D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103057) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103057,0,0,0,0,N'VZ. 52',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103058) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103058,0,0,0,0,N'AK47 F.C',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103059) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103059,0,0,0,0,N'AK-47',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103060) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103060,0,0,0,0,N'M4A1',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103061) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103061,0,0,0,0,N'SS2-V4 Para Sniper G',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103062) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103062,0,0,0,0,N'Famas G2 Commando E-Sport',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103063) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103063,0,0,0,0,N'AUG A3 E-Sport',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103064) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103064,0,0,0,0,N'FAMAS G2 Commando G',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103065) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103065,0,0,0,0,N'AUG A3 Blue',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103066) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103066,0,0,0,0,N'SG 550',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103067) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103067,0,0,0,0,N'G36C',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103068) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103068,0,0,0,0,N'AK-47 FC Red',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103069) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103069,0,0,0,0,N'Scar-H CQC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103070) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103070,0,0,0,0,N'Scar-H STD Silencer',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103071) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103071,0,0,0,0,N'AUG A3 PBIC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103072) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103072,0,0,0,0,N'AK47 Old Spec',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103073) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103073,0,0,0,0,N'SG 550 Sp EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103074) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103074,0,0,0,0,N'AK SOPMOD EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103075) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103075,0,0,0,0,N'AUG A3 EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103076) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103076,0,0,0,0,N'AK47 Sl EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103077) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103077,0,0,0,0,N'F2000 Sl EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103078) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103078,0,0,0,0,N'AK47 Ext EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103079) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103079,0,0,0,0,N'M4A1 Ext EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103080) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103080,0,0,0,0,N'M4A1 S. EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103081) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103081,0,0,0,0,N'M4A1 Sl EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103082) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103082,0,0,0,0,N'M4 SR-16 LV1 EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103083) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103083,0,0,0,0,N'M4 SR-16 LV2 EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103084) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103084,0,0,0,0,N'M4 SR-16 LV3 EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103085) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103085,0,0,0,0,N'Scar-H CQC D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103086) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103086,0,0,0,0,N'AK-47 Goddess',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103087) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103087,0,0,0,0,N'Famas G2 Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103088) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103088,0,0,0,0,N'Scar-H LB Sniper',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103089) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103089,0,0,0,0,N'AUG A3 Gold Non',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103090) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103090,0,0,0,0,N'FAMAS G2 Digital',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103091) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103091,0,0,0,0,N'Famas G2 Commando PBTN',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103092) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103092,0,0,0,0,N'AK SOPMOD R',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103093) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103093,0,0,0,0,N'AUG A3 Blaze',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103094) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103094,0,0,0,0,N'Scar-L Carbine',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103095) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103095,0,0,0,0,N'Scar-L Recon',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103096) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103096,0,0,0,0,N'Scar-L F.C',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103097) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103097,0,0,0,0,N'AUG A3 GRS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103098) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103098,0,0,0,0,N'M4A1 GRS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103099) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103099,0,0,0,0,N'AK-SOPMOD GRS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103100) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103100,0,0,0,0,N'Famas G2 GRS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103101) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103101,0,0,0,0,N'AK SOPMOD PBNC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103102) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103102,0,0,0,0,N'HK417',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103103) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103103,0,0,0,0,N'M4A1 FC H-Dunter',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103104) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103104,0,0,0,0,N'AUG A3 GSL',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103105) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103105,0,0,0,0,N'FAMAS G2 Commando GSL',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103106) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103106,0,0,0,0,N'AUG A3 ITALIA',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103107) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103107,0,0,0,0,N'AUG A3 Black(P)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103108) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103108,0,0,0,0,N'AUG A3 GRS EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103109) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103109,0,0,0,0,N'AUG A3 TW1st',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103110) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103110,0,0,0,0,N'AUG A3 TW1st(P)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103111) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103111,0,0,0,0,N'AUG A3 BR Camo',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103112) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103112,0,0,0,0,N'AUG A3 Turkey',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103113) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103113,0,0,0,0,N'Famas G2 GRS EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103114) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103114,0,0,0,0,N'M4A1 Elite',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103115) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103115,0,0,0,0,N'AN-94',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103116) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103116,0,0,0,0,N'F-2000 Reload',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103117) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103117,0,0,0,0,N'SG-550 Reload',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103118) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103118,0,0,0,0,N'SS2-V4 Para Sniper Reload',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103119) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103119,0,0,0,0,N'AK-47 Elite',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103120) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103120,0,0,0,0,N'AUG A3 PBIC2013',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103121) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103121,0,0,0,0,N'M4A1 PBIC2013',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103122) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103122,0,0,0,0,N'AK-47 PBIC2013',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103123) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103123,0,0,0,0,N'TAR-21',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103124) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103124,0,0,0,0,N'AK47 Gold(P)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103125) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103125,0,0,0,0,N'AK47 FC(P)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103126) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103126,0,0,0,0,N'AK SOPMOD KingCobra(P)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103127) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103127,0,0,0,0,N'M4A1 SE(P)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103128) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103128,0,0,0,0,N'AUG A3 Azerbaijan',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103129) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103129,0,0,0,0,N'AUG A3 BLOODY',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103130) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103130,0,0,0,0,N'AUG A3 BLOODY GOLD',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103131) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103131,0,0,0,0,N'AUG A3 Latin3',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103142) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103142,0,0,0,0,N'AUG A3 TH 1Year',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103143) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103143,0,0,0,0,N'FG42',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103144) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103144,0,0,0,0,N'AUG A3 R',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103145) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103145,0,0,0,0,N'FG42 Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103146) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103146,0,0,0,0,N'G36 Elite',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103147) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103147,0,0,0,0,N'AUG A3 Inferno',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103148) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103148,0,0,0,0,N'AUG A3 PBNC5',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103149) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103149,0,0,0,0,N'AUG A3 GSL2014',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103150) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103150,0,0,0,0,N'TAR-21 Mid-Night',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103151) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103151,0,0,0,0,N'TAR-21 BR Camo(+15% Exp)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103152) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103152,0,0,0,0,N'AK SOPMOD BR Camo',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103153) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103153,0,0,0,0,N'SC 2010',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103154) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103154,0,0,0,0,N'SC 2010 Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103155) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103155,0,0,0,0,N'AUG A3 Brazuca',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103156) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103156,0,0,0,0,N'Pindad SS2-V4 Gold GW',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103157) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103157,0,0,0,0,N'AUG A3 Champion',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103158) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103158,0,0,0,0,N'AUG A3 W.O.E',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103159) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103159,0,0,0,0,N'AUG A3 PBIC2014',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103160) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103160,0,0,0,0,N'AUG A3 4th Anniversary',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103161) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103161,0,0,0,0,N'AUG A3 NoLogo Latin3',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103162) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103162,0,0,0,0,N'TAR-21 NoLogo Mid-night',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103163) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103163,0,0,0,0,N'AUG A3 PC-Cafe',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103164) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103164,0,0,0,0,N'AUG A3 G E-Sport',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103165) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103165,0,0,0,0,N'AUG A3 Toy',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103166) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103166,0,0,0,0,N'SC 2010 Q',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103167) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103167,0,0,0,0,N'AN-94 Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103168) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103168,0,0,0,0,N'HK-417 Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103169) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103169,0,0,0,0,N'TAR-21 Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103170) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103170,0,0,0,0,N'SCAR-L Carbine Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103171) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103171,0,0,0,0,N'AUG A3 Latin4',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103172) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103172,0,0,0,0,N'SnowBall Blaster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103173) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103173,0,0,0,0,N'AUG A3 Cangaceiro',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103174) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103174,0,0,0,0,N'XM8',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103175) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103175,0,0,0,0,N'SCAR-L Carbine D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103176) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103176,0,0,0,0,N'SCAR-L Recon D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103177) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103177,0,0,0,0,N'XM8 GOLD',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103178) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103178,0,0,0,0,N'AUG A3 CoupleBreak',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103180) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103180,0,0,0,0,N'AUG A3 CNY 2015',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103181) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103181,0,0,0,0,N'AUG A3 Sheep',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103182) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103182,0,0,0,0,N'Tar-21 Sheep',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103183) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (103183,0,0,0,0,N'AUG A3 GRS2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104001,0,0,0,0,N'MP5K Ext',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104002,0,0,0,0,N'Spectre Ext',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104003,0,0,0,0,N'K-1 Ext',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104004,0,0,0,0,N'MP7 Ext',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104006) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104006,0,0,0,0,N'K-1',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104007,0,0,0,0,N'MP5K G.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104008) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104008,0,0,0,0,N'UMP45 Ext',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104009,0,0,0,0,N'Spectre W.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104010) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104010,0,0,0,0,N'P90 M.C',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104011) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104011,0,0,0,0,N'P90 Ext',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104013) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104013,0,0,0,0,N'Kriss S.V',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104014) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104014,0,0,0,0,N'MP5K Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104015) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104015,0,0,0,0,N'MP5K Wh.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104016) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104016,0,0,0,0,N'Spectre Wh.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104020) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104020,0,0,0,0,N'Spectre Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104021) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104021,0,0,0,0,N'K-1 Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104022) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104022,0,0,0,0,N'MP7 Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104023) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104023,0,0,0,0,N'UMP45 Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104024) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104024,0,0,0,0,N'MP5K G D.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104025) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104025,0,0,0,0,N'Spectre W D.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104026) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104026,0,0,0,0,N'Kriss S.V G.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104027) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104027,0,0,0,0,N'P90 M.C. S.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104029) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104029,0,0,0,0,N'P90 Ext D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104031) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104031,0,0,0,0,N'Kriss S.V D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104032) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104032,0,0,0,0,N'P90 M.C D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104033) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104033,0,0,0,0,N'AKS74U Ext',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104034) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104034,0,0,0,0,N'M4 CQB-R LV1',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104035) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104035,0,0,0,0,N'M4 CQB-R LV2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104036) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104036,0,0,0,0,N'M4 CQB-R LV3',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104038) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104038,0,0,0,0,N'MP7 S.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104039) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104039,0,0,0,0,N'Kriss S.V Black',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104041) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104041,0,0,0,0,N'MP5K Sl D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104042) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104042,0,0,0,0,N'Spectre Sl D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104043) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104043,0,0,0,0,N'SS1-R5 Carbine',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104045) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104045,0,0,0,0,N'MP7 Ext D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104046) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104046,0,0,0,0,N'UMP45 Ext D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104047) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104047,0,0,0,0,N'Kriss S.V Batik D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104049) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104049,0,0,0,0,N'SS1-R5 Carbine G',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104050) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104050,0,0,0,0,N'Kriss S.V E-Sport',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104052) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104052,0,0,0,0,N'Kriss S.V Batik',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104054) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104054,0,0,0,0,N'Kriss S.V Vector',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104056) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104056,0,0,0,0,N'MP7',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104057) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104057,0,0,0,0,N'Spectre',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104058) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104058,0,0,0,0,N'UMP45 Black',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104059) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104059,0,0,0,0,N'MP7 Special2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104060) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104060,0,0,0,0,N'Kriss S.V PBIC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104062) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104062,0,0,0,0,N'Spectre Wh. EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104063) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104063,0,0,0,0,N'MP5K G. EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104064) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104064,0,0,0,0,N'P90 M.C. S. EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104065) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104065,0,0,0,0,N'Kriss S.V EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104066) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104066,0,0,0,0,N'MP7 Ext EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104067) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104067,0,0,0,0,N'MP5K Sl EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104068) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104068,0,0,0,0,N'UMP45 Sl EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104069) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104069,0,0,0,0,N'P90 Ext EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104070) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104070,0,0,0,0,N'M4 CQB-R LV1 EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104071) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104071,0,0,0,0,N'M4 CQB-R LV2 EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104072) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104072,0,0,0,0,N'M4 CQB-R LV3 EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104074) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104074,0,0,0,0,N'Spectre Sl EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104075) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104075,0,0,0,0,N'P90 G',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104076) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104076,0,0,0,0,N'AKS74U 100M',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104077) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104077,0,0,0,0,N'P90 Wh(PBSC)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104078) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104078,0,0,0,0,N'P90 Ext PBTN',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104079) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104079,0,0,0,0,N'Kriss S.V PBTN',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104081) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104081,0,0,0,0,N'MP7 GRS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104082) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104082,0,0,0,0,N'P90 MC GRS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104083) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104083,0,0,0,0,N'Kriss S.V GRS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104085) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104085,0,0,0,0,N'P90 Newborn',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104086) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104086,0,0,0,0,N'P90 M.C PBNC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104087) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104087,0,0,0,0,N'Kriss S.V GSL',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104089) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104089,0,0,0,0,N'P90 GSL',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104090) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104090,0,0,0,0,N'Kriss SV Black(P)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104092) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104092,0,0,0,0,N'UMP45 Black(P)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104093) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104093,0,0,0,0,N'Kriss S.V GRS EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104095) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104095,0,0,0,0,N'P90 MC GRS EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104096) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104096,0,0,0,0,N'AKS74u Reload',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104097) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104097,0,0,0,0,N'UMP-45 ext Reload',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104098) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104098,0,0,0,0,N'Spectre Reload',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104099) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104099,0,0,0,0,N'Pidad SS1-R5 Reload',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104100) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104100,0,0,0,0,N'MP5K Reload',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104101) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104101,0,0,0,0,N'KrissSuperV 4th Anniversary',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104103) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104103,0,0,0,0,N'KrissSV PBIC2013',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104105) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104105,0,0,0,0,N'MP7 GRS EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104106) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104106,0,0,0,0,N'PP19-Bizon',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104107) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104107,0,0,0,0,N'MP9 EXT',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104108) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104108,0,0,0,0,N'Kriss S.V Silence',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104110) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104110,0,0,0,0,N'Kriss S.V Turkey',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104112) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104112,0,0,0,0,N'P90 BLOODY',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104113) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104113,0,0,0,0,N'P90 MC Latin3',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104114) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104114,0,0,0,0,N'P90 BR Camo',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104121) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104121,0,0,0,0,N'Kriss S.V TH 1Year',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104123) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104123,0,0,0,0,N'Kriss S.V R',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104125) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104125,0,0,0,0,N'P90 Newborn 2014',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104126) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104126,0,0,0,0,N'Kriss S.V Inferno',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104128) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104128,0,0,0,0,N'P90 Inferno',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104129) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104129,0,0,0,0,N'P90 Ext PBNC5',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104130) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104130,0,0,0,0,N'Kriss S.V GSL2014',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104132) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104132,0,0,0,0,N'Kriss S.V Mid-Night',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104134) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104134,0,0,0,0,N'OA-93',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104136) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104136,0,0,0,0,N'OA-93 Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104138) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104138,0,0,0,0,N'P90 Brazuca',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104139) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104139,0,0,0,0,N'Kriss S.V Brazuca',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104141) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104141,0,0,0,0,N'P90 Ext 5th Anniversary',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104142) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104142,0,0,0,0,N'Kriss S.V Champion',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104144) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104144,0,0,0,0,N'Kriss S.V W.O.E',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104146) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104146,0,0,0,0,N'P90 Ext PBIC2014',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104147) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104147,0,0,0,0,N'P90 NoLogo PBSC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104148) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104148,0,0,0,0,N'P90 MC NoLogo Latin3',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104149) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104149,0,0,0,0,N'KrissS.V NoLogo M-night',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104151) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104151,0,0,0,0,N'Kriss S.V PC-Cafe',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104153) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104153,0,0,0,0,N'Kriss S.V PBSC2014',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104155) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104155,0,0,0,0,N'Kriss S.V G E-Sport',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104157) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104157,0,0,0,0,N'Kriss S.V Toy',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104159) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104159,0,0,0,0,N'OA-93 Q',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104161) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104161,0,0,0,0,N'MP9 Ext. Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104162) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104162,0,0,0,0,N'PP-19 Bizon Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104163) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104163,0,0,0,0,N'P90 Ext Latin4',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104164) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104164,0,0,0,0,N'MP9 Ext XMAS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104165) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104165,0,0,0,0,N'OA-93 XMAS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104167) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104167,0,0,0,0,N'PP-19 Bizon XMAS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104168) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104168,0,0,0,0,N'Kriss SuperV XMAS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104170) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104170,0,0,0,0,N'Kriss S.V CoupleBreaker',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104172) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104172,0,0,0,0,N'Kriss S.V CNY 2015',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104174) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104174,0,0,0,0,N'P90 M.C Sheep',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104175) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (104175,0,0,0,0,N'Kriss S.V GRS2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105001,0,0,0,0,N'Dragunov',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105002,0,0,0,0,N'PSG1',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105003,0,0,0,0,N'SSG 69',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105004,0,0,0,0,N'SSG-69 S.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105005,0,0,0,0,N'L115A1',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105006) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105006,0,0,0,0,N'Dragunov G.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105007,0,0,0,0,N'PSG1 S.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105008) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105008,0,0,0,0,N'SSG 69 Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105009,0,0,0,0,N'PSG1 Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105010) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105010,0,0,0,0,N'Dragunov Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105011) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105011,0,0,0,0,N'Dragunov CS.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105012) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105012,0,0,0,0,N'Dragunov CG.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105014) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105014,0,0,0,0,N'Dragunov G D.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105015) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105015,0,0,0,0,N'L115A1 Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105016) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105016,0,0,0,0,N'PSG1 S. D.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105017) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105017,0,0,0,0,N'L115A1 D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105018) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105018,0,0,0,0,N'M4 SPR LV1',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105019) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105019,0,0,0,0,N'M4 SPR LV2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105020) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105020,0,0,0,0,N'M4 SPR LV3',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105021) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105021,0,0,0,0,N'Rangemaster .338',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105022) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105022,0,0,0,0,N'Rangemaster 7.62',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105023) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105023,0,0,0,0,N'Rangemaster 7.62 STBY',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105024) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105024,0,0,0,0,N'PSG1 G',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105026) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105026,0,0,0,0,N'L115A1 Black',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105027) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105027,0,0,0,0,N'SSG 69 Sl D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105028) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105028,0,0,0,0,N'Dragunov D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105029) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105029,0,0,0,0,N'VSK94',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105030) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105030,0,0,0,0,N'Cheytac M200',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105031) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105031,0,0,0,0,N'Winchester M70',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105032) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105032,0,0,0,0,N'Barrett M82A1',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105033) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105033,0,0,0,0,N'L115A1 E-Sport',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105034) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105034,0,0,0,0,N'DSR-1',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105035) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105035,0,0,0,0,N'SVU',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105036) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105036,0,0,0,0,N'L115A1 S',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105037) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105037,0,0,0,0,N'Scar-H LB Sniper',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105038) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105038,0,0,0,0,N'PSG1 S. EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105039) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105039,0,0,0,0,N'Dragunov G. EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105040) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105040,0,0,0,0,N'Dragunov Sl EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105041) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105041,0,0,0,0,N'L115A1 EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105042) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105042,0,0,0,0,N'PSG1 Sl EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105043) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105043,0,0,0,0,N'SSG 69 Sl EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105044) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105044,0,0,0,0,N'M4 SPR Lv1 EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105045) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105045,0,0,0,0,N'M4 SPR Lv2 EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105046) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105046,0,0,0,0,N'M4 SPR Lv3 EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105047) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105047,0,0,0,0,N'Dragunov Red',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105048) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105048,0,0,0,0,N'VSK94 100M',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105049) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105049,0,0,0,0,N'Dragunov CG(P)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105050) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105050,0,0,0,0,N'L115A1 PBTN',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105052) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105052,0,0,0,0,N'Cheytec M200 GRS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105053) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105053,0,0,0,0,N'L115A1 PBNC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105054) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105054,0,0,0,0,N'SVU S',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105055) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105055,0,0,0,0,N'CheyTac M200 GSL',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105056) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105056,0,0,0,0,N'L115A1 Black(P)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105057) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105057,0,0,0,0,N'CheyTac M200 Bloody',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105058) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105058,0,0,0,0,N'Cheytac M200 GRS EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105059) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105059,0,0,0,0,N'PSG1 Reload',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105063) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105063,0,0,0,0,N'CheyTac M200 Turkey',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105064) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105064,0,0,0,0,N'L115A1 Latin3',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105065) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105065,0,0,0,0,N'L115A1 BR CAMO',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105068) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105068,0,0,0,0,N'XM2010',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105071) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105071,0,0,0,0,N'Cheytac M200 TH 1Year',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105072) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105072,0,0,0,0,N'Walther',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105073) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105073,0,0,0,0,N'Cheytec M200 R',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105074) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105074,0,0,0,0,N'Walther Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105075) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105075,0,0,0,0,N'PSG1 Elite',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105076) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105076,0,0,0,0,N'Dragunov Elite',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105077) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105077,0,0,0,0,N'SVU Elite',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105078) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105078,0,0,0,0,N'VSK Elite',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105079) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105079,0,0,0,0,N'CheyTac M200 Inferno',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105080) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105080,0,0,0,0,N'Cheytac M200 PBNC5',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105081) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105081,0,0,0,0,N'L115A1 GSL2014',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105082) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105082,0,0,0,0,N'Barrett M82A1(P)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105083) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105083,0,0,0,0,N'Cheytac M200 Brazuca',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105084) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105084,0,0,0,0,N'Cheytac M200 Champion',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105085) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105085,0,0,0,0,N'Dragunov W.O.E',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105086) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105086,0,0,0,0,N'Cheytac M200 W.O.E',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105087) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105087,0,0,0,0,N'Cheytac M200 PBIC2014',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105088) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105088,0,0,0,0,N'L115A1 NoLogo Latin3',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105089) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105089,0,0,0,0,N'L115A1 PC-Cafe',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105090) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105090,0,0,0,0,N'Cheytac M200 Merdeka',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105091) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105091,0,0,0,0,N'Cheytac M200 G E-Sport',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105092) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105092,0,0,0,0,N'L115A1 Toy',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105093) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105093,0,0,0,0,N'VSK94 Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105094) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105094,0,0,0,0,N'SVU Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105095) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105095,0,0,0,0,N'DSR-1 Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105096) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105096,0,0,0,0,N'Cheytac M200 Latin4',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105097) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105097,0,0,0,0,N'Cheytac M200 Cangaceiro',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105098) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105098,0,0,0,0,N'DSR-1 D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105099) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105099,0,0,0,0,N'Cheytac CoupleBreaker',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105100) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (105100,0,0,0,0,N'CheyTac M200 GRS2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106001,0,0,0,0,N'870MCS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106002,0,0,0,0,N'870MCS Event',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106003,0,0,0,0,N'SPAS-15',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106004,0,0,0,0,N'870MCS W.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106005,0,0,0,0,N'M1887',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106006) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106006,0,0,0,0,N'SPAS 15 Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106008) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106008,0,0,0,0,N'870MCS Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106009,0,0,0,0,N'SPAS 15 Sl+',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106010) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106010,0,0,0,0,N'M1887 W.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106011) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106011,0,0,0,0,N'870MCS W. D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106012) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106012,0,0,0,0,N'SPAS-15 D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106013) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106013,0,0,0,0,N'M26 LSS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106014) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106014,0,0,0,0,N'870MCS Sl D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106015) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106015,0,0,0,0,N'870MCS D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106016) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106016,0,0,0,0,N'SPAS 15 Sl D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106017) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106017,0,0,0,0,N'M1887 D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106018) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106018,0,0,0,0,N'SPAS-15 MSC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106019) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106019,0,0,0,0,N'JackHammer',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106020) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106020,0,0,0,0,N'Kel-Tec KSG-15',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106021) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106021,0,0,0,0,N'M1887 Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106022) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106022,0,0,0,0,N'SPAS-15 EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106023) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106023,0,0,0,0,N'870MCS W. EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106024) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106024,0,0,0,0,N'SPAS-15 Sl. EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106026) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106026,0,0,0,0,N'M1887 W GRS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106027) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106027,0,0,0,0,N'M1887 PBNC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106028) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106028,0,0,0,0,N'M1887 W GRS EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106029) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106029,0,0,0,0,N'SPAS-15 PBSC2013',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106030) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106030,0,0,0,0,N'M1887 BLOODY',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106032) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106032,0,0,0,0,N'SPAS-15 Elite',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106033) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106033,0,0,0,0,N'M1887 TH 1Year',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106034) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106034,0,0,0,0,N'M1887 R',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106035) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106035,0,0,0,0,N'M1887 PBNC5',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106036) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106036,0,0,0,0,N'Kel-Tec KSG GSL2014',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106037) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106037,0,0,0,0,N'M1887 Brazuca',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106038) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106038,0,0,0,0,N'M1887 PBIC2014',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106039) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106039,0,0,0,0,N'UTS-15',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106040) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106040,0,0,0,0,N'SPAS-15 NoLogo PBSC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106041) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106041,0,0,0,0,N'M1887 LionHeart',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106042) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106042,0,0,0,0,N'Zombie Slayer',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106043) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106043,0,0,0,0,N'Remington ETA',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106044) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106044,0,0,0,0,N'Remington ETA G.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106045) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106045,0,0,0,0,N'UTS-15 Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106046) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106046,0,0,0,0,N'UTS-15 D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106047) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106047,0,0,0,0,N'Cerberus Shotgun',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106048) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (106048,0,0,0,0,N'UTS-15 G.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (110001,0,0,0,0,N'MK46 Ext',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (110002,0,0,0,0,N'MK.46 Sl.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (110003,0,0,0,0,N'MG RPD',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (110004,0,0,0,0,N'L86 LSW',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (110005,0,0,0,0,N'MK46 Ext EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110006) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (110006,0,0,0,0,N'MG RPD 100M',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (110007,0,0,0,0,N'RPD Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110008) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (110008,0,0,0,0,N'RPD EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (110009,0,0,0,0,N'MK.46',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110010) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (110010,0,0,0,0,N'L86LSW X-MAS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110011) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (110011,0,0,0,0,N'Ultimax 100',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110012) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (110012,0,0,0,0,N'Ultimax 100 Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 118004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (118004,0,0,0,0,N'Dual UZI',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 118005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (118005,0,0,0,0,N'Dual UZI Mini',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 118006) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (118006,0,0,0,0,N'Dual UZI Micro',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 118007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (118007,0,0,0,0,N'Dual UZI Silencer',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 118008) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (118008,0,0,0,0,N'Dual UZI Mini Silenver',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 118009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (118009,0,0,0,0,N'Dual UZI Micro Silencer',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 118011) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (118011,0,0,0,0,N'Dual UZI Micro Silencer Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 118013) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (118013,0,0,0,0,N'Dual UZI Mini Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202001,0,0,0,0,N'Desert Eagle',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202002,0,0,0,0,N'MK23 Ext',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202003,0,0,0,0,N'K-5',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202005,0,0,0,0,N'D-Eagle S.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202007,0,0,0,0,N'C. Python',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202010) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202010,0,0,0,0,N'C. Python G.+',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202011) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202011,0,0,0,0,N'Glock 18',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202012) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202012,0,0,0,0,N'C. Python D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202013) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202013,0,0,0,0,N'R.B 454 SS2M',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202014) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202014,0,0,0,0,N'R.B 454 SS5M',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202015) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202015,0,0,0,0,N'R.B 454 SS8M',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202016) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202016,0,0,0,0,N'R.B 454 SS8M+S',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202017) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202017,0,0,0,0,N'C. Python G',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202018) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202018,0,0,0,0,N'C. Python G D.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202019) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202019,0,0,0,0,N'MK23 Ext D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202020) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202020,0,0,0,0,N'C. Python P',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202021) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202021,0,0,0,0,N'Glock 18 D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202022) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202022,0,0,0,0,N'Colt45',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202023) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202023,0,0,0,0,N'IMI UZI 9MM',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202024) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202024,0,0,0,0,N'Kriss S.V Vector SDP',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202026) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202026,0,0,0,0,N'HK 69',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202027) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202027,0,0,0,0,N'MK.23 SI',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202028) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202028,0,0,0,0,N'M79',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202029) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202029,0,0,0,0,N'GL06',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202030) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202030,0,0,0,0,N'Desert Eagle EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202031) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202031,0,0,0,0,N'Colt45 100M',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202033) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202033,0,0,0,0,N'Glock 18 PBNC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202034) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202034,0,0,0,0,N'R.B 454 SS8M+S PBSC2013',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202035) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202035,0,0,0,0,N'MK23 Reload',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202036) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202036,0,0,0,0,N'Desert Eegle Reload',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202037) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202037,0,0,0,0,N'Glock 18 Turkey',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202039) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202039,0,0,0,0,N'Glock 18 Azerbaijan',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202040) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202040,0,0,0,0,N'C. Python PBNC5',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202047) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202047,0,0,0,0,N'Glock 18 BR Camo',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202049) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202049,0,0,0,0,N'C. Pyhon Brazuca',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202050) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202050,0,0,0,0,N'R.B454 SS8M+PBSC NoLogo',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202052) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202052,0,0,0,0,N'C. Python Toy',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202053) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202053,0,0,0,0,N'MGL-32',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202054) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (202054,0,0,0,0,N'GL-06 CNY 2015',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 213001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (213001,0,0,0,0,N'P99&HAK',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 213002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (213002,0,0,0,0,N'P99&HAK Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 213003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (213003,0,0,0,0,N'P99&HAK D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 213004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (213004,0,0,0,0,N'P99&HAK Sl D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 213005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (213005,0,0,0,0,N'P99&HAK EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 213006) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (213006,0,0,0,0,N'P99&HAK D (GW)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 213007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (213007,0,0,0,0,N'P99&HAK Reload',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (214001,0,0,0,0,N'Dual Handgun',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (214002,0,0,0,0,N'Dual Desert Eagle',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (214004,0,0,0,0,N'Dual Desert Eagle Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (214005,0,0,0,0,N'Dual Handgun D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214006) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (214006,0,0,0,0,N'Dual Desert Eagle D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (214007,0,0,0,0,N'HK45 Dual',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214008) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (214008,0,0,0,0,N'Dual HandGun EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (214009,0,0,0,0,N'Dual D-Eagle EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214010) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (214010,0,0,0,0,N'Dual D-Eagle G (P)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214011) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (214011,0,0,0,0,N'Dual Desert Eagle GRS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214012) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (214012,0,0,0,0,N'HK45 Pumpkin',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214015) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (214015,0,0,0,0,N'Dual D-Eagle BR Camo',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214016) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (214016,0,0,0,0,N'Dual D-Eagle G E-Sport',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 230001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (230001,0,0,0,0,N'SuperShorty 870',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 234001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (234001,0,0,0,0,N'Compound Bow',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 234002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (234002,0,0,0,0,N'Compound Smoke',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 234003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (234003,0,0,0,0,N'Compound Flashbang',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 234004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (234004,0,0,0,0,N'Compound WP smoke',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 234005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (234005,0,0,0,0,N'Compound Grenade',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301001,0,0,0,0,N'M-7',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301004,0,0,0,0,N'Amok Kukri',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301007,0,0,0,0,N'Mini Axe',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301009,0,0,0,0,N'M-7 G.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301011) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301011,0,0,0,0,N'Amok Kukri D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301012) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301012,0,0,0,0,N'Mini Axe D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301014) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301014,0,0,0,0,N'Combat Machete',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301017) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301017,0,0,0,0,N'Fang Blade',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301018) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301018,0,0,0,0,N'Ballistic Knife',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301021) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301021,0,0,0,0,N'Keris',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301023) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301023,0,0,0,0,N'Keris S.',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301024) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301024,0,0,0,0,N'Candy Cane',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301025) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301025,0,0,0,0,N'Field Shovel',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301026) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301026,0,0,0,0,N'Amok Kukri EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301027) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301027,0,0,0,0,N'Mini Axe EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301037) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301037,0,0,0,0,N'Ballistic Knife GRS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301041) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301041,0,0,0,0,N'Aribian Sword',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301042) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301042,0,0,0,0,N'M-9 PBNC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301043) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301043,0,0,0,0,N'Combat Machete Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301044) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301044,0,0,0,0,N'Combat Machete PBSC2013',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301046) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301046,0,0,0,0,N'Amok Kukri Turkey',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301047) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301047,0,0,0,0,N'Keris S. X-MAS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301049) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301049,0,0,0,0,N'Arabian Sword2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301050) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301050,0,0,0,0,N'Fang Blade PBNC5',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301051) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301051,0,0,0,0,N'Fang Blade GSL2014',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301052) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301052,0,0,0,0,N'Fang Blade Brazuca',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301057) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301057,0,0,0,0,N'Fang Blade Inferno',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301058) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301058,0,0,0,0,N'ChineseCleaver',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301059) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301059,0,0,0,0,N'GH5007 NoLogo PBSC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301064) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301064,0,0,0,0,N'BadmintonRacket',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301065) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301065,0,0,0,0,N'Keris G E-Sport',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301066) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301066,0,0,0,0,N'Death Scythe',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301067) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301067,0,0,0,0,N'FangBlade Latin4',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301068) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301068,0,0,0,0,N'Icicle',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301069) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301069,0,0,0,0,N'Ice Fork',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301070) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301070,0,0,0,0,N'GH5007 Cangaceiro',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301071) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301071,0,0,0,0,N'Goat hammer',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301073) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (301073,0,0,0,0,N'Chinese Cleaver CNY',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 315001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (315001,0,0,0,0,N'Dual Knife',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 315002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (315002,0,0,0,0,N'Dual Knife D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 315003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (315003,0,0,0,0,N'Bone Knife',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 315004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (315004,0,0,0,0,N'Dual Knife EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 315005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (315005,0,0,0,0,N'Bone Knife GRS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 315006) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (315006,0,0,0,0,N'Dual Knife PBNC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 315007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (315007,0,0,0,0,N'Boneknife GRS2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 323001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (323001,0,0,0,0,N'Bare Fist',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 323002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (323002,0,0,0,0,N'Black Knuckle',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 323003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (323003,0,0,0,0,N'Brass Knuckle',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 323004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (323004,0,0,0,0,N'Silver Knuckle',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 323005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (323005,0,0,0,0,N'Pumpkin Knuckle',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 323006) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (323006,0,0,0,0,N'Spiked Knuckle',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 323007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (323007,0,0,0,0,N'Ballock Knuckle',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 323009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (323009,0,0,0,0,N'Garena Knuckle',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407001,0,0,0,0,N'K-400',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407002,0,0,0,0,N'C-5',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407003,0,0,0,0,N'C-5 D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407004,0,0,0,0,N'K-413 Dual Granade',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407005,0,0,0,0,N'Chocolate Grenade',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407006) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407006,0,0,0,0,N'C-5 EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407007,0,0,0,0,N'K-413 G',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407008) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407008,0,0,0,0,N'Ketupat Grenade',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407009,0,0,0,0,N'Candy Grenade',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407010) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407010,0,0,0,0,N'Lantern Grenade',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407011) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407011,0,0,0,0,N'Ma`amoul Grenade',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407012) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407012,0,0,0,0,N'Decoy Bomb',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407013) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407013,0,0,0,0,N'M14 Mine',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407014) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407014,0,0,0,0,N'K479',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407015) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407015,0,0,0,0,N'Easter Day Egg',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407016) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407016,0,0,0,0,N'Football Bomb',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407017) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407017,0,0,0,0,N'K-413 T',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407018) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407018,0,0,0,0,N'M14 Mine D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407019) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407019,0,0,0,0,N'K-413 Traditional',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407020) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407020,0,0,0,0,N'Dual K-413 PC-Cafe',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407021) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407021,0,0,0,0,N'Shuttlecock',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407022) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407022,0,0,0,0,N'SepaktakrawBall',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407023) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407023,0,0,0,0,N'M18A1 Claymore',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407024) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407024,0,0,0,0,N'K-413 GW',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407025) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407025,0,0,0,0,N'K-400 Goat',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407044) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407044,0,0,0,0,N'C-5 CNY 2015',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407045) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (407045,0,0,0,0,N'K-413 Chocolate',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 508001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (508001,0,0,0,0,N'Smoke',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 508002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (508002,0,0,0,0,N'Flash Bang',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 508003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (508003,0,0,0,0,N'Smoke Plus',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 508004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (508004,0,0,0,0,N'Flash Bang Plus',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 527001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (527001,0,0,0,0,N'WP Smoke',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 527002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (527002,0,0,0,0,N'WP Smoke D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 527003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (527003,0,0,0,0,N'WP Smoke Plus',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 527004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (527004,0,0,0,0,N'WP Smoke EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 527005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (527005,0,0,0,0,N'WP Smoke Plus P',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 528001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (528001,0,0,0,0,N'Medical Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 528002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (528002,0,0,0,0,N'Halloween Medical Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 528003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (528003,0,0,0,0,N'Chocolate Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 528004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (528004,0,0,0,0,N'Medical Kit Lotus',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 528005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (528005,0,0,0,0,N'Medical Kit Opor Ayam',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 528006) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (528006,0,0,0,0,N'Medicla Kit PBNC5',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 528007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (528007,0,0,0,0,N'Easter Day Medical Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 528008) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (528008,0,0,0,0,N'Medical Kit Lotus2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 528009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (528009,0,0,0,0,N'Medical Kit Kurma',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (601001,0,0,0,0,N'Red Bulls',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (601003,0,0,0,0,N'Tarantula',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (601005,0,0,0,0,N'Normal Viper Red',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (601007,0,0,0,0,N'Normal D-fox',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601010) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (601010,0,0,0,0,N'Rica',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601012) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (601012,0,0,0,0,N'D-fox exp +20%',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601015) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (601015,0,0,0,0,N'Viper Red point +30%',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601016) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (601016,0,0,0,0,N'Reinforced Red Bulls',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601018) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (601018,0,0,0,0,N'Reinforced Tarantula',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601020) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (601020,0,0,0,0,N'Reinforced D-fox',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601022) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (601022,0,0,0,0,N'Reinforced Viper Red',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601024) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (601024,0,0,0,0,N'Reinforced Rica',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601026) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (601026,0,0,0,0,N'Reinforced Combo D-Fox',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601028) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (601028,0,0,0,0,N'Reinforced Combo Viper Red',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601030) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (601030,0,0,0,0,N'REBEL Male GRS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601032) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (601032,0,0,0,0,N'REBEL Female GRS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601037) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (601037,0,0,0,0,N'2014 WC Tarantula',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601038) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (601038,0,0,0,0,N'Viper Red Shadow',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (602002,0,0,0,0,N'Acid Paul',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (602004,0,0,0,0,N'Keen Eyes',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602008) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (602008,0,0,0,0,N'Normal Leopard',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (602009,0,0,0,0,N'Normal Hide',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602011) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (602011,0,0,0,0,N'Chou',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602013) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (602013,0,0,0,0,N'Leopard Exp+20%',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602014) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (602014,0,0,0,0,N'Hide point +30%',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602017) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (602017,0,0,0,0,N'Reinforced Acid Paul',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602019) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (602019,0,0,0,0,N'Reinforced Keen Eyes',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602021) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (602021,0,0,0,0,N'Reinforced Leopard',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602023) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (602023,0,0,0,0,N'Reinforced Hide',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602025) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (602025,0,0,0,0,N'Reinforced Chou',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602027) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (602027,0,0,0,0,N'Reinforced Combo Leopard',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602029) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (602029,0,0,0,0,N'Reinforced Combo Hide',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602031) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (602031,0,0,0,0,N'SWAT Male GRS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602033) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (602033,0,0,0,0,N'SWAT Female GRS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602034) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (602034,0,0,0,0,N'Hide Kopassus',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602035) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (602035,0,0,0,0,N'Leopard Bope',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602036) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (602036,0,0,0,0,N'2014 WC Hide',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602039) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (602039,0,0,0,0,N'Hide Recon',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700002,0,0,0,0,N'Acid Pol Headgear',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700005,0,0,0,0,N'Red Bulls Headgear',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700006) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700006,0,0,0,0,N'Tarantula Headgear',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700017) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700017,0,0,0,0,N'Keen Eyes Headgear',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700018) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700018,0,0,0,0,N'D-Fox Headgear',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700019) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700019,0,0,0,0,N'Leopard Headgear',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700020) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700020,0,0,0,0,N'Viper Red Headgear',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700021) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700021,0,0,0,0,N'Hide Headgear',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700030) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700030,0,0,0,0,N'UDT Boonie hat',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700031) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700031,0,0,0,0,N'Beret Bope',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700032) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700032,0,0,0,0,N'Marine Cap',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700033) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700033,0,0,0,0,N'Pentagram Army Cap',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700035) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700035,0,0,0,0,N'707 Unit Headgear',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700036) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700036,0,0,0,0,N'China Marine Headgear',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700037) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700037,0,0,0,0,N'SanTa Hat',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700038) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700038,0,0,0,0,N'Pumpkin Hat',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700039) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700039,0,0,0,0,N'Mask Tiger',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700040) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700040,0,0,0,0,N'Super Beret',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700047) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700047,0,0,0,0,N'Mask Chinese Hat2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700048) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700048,0,0,0,0,N'Patrol Helmet',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700101) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700101,0,0,0,0,N'Headgear Default',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700102) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700102,0,0,0,0,N'Headgear Normal',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700103) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700103,0,0,0,0,N'Headgear Reinforce',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700104) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700104,0,0,0,0,N'Headgear Target Tracking',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700105) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700105,0,0,0,0,N'Headgear Reinforced Plus',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700106) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700106,0,0,0,0,N'Headgear Super',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700107) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700107,0,0,0,0,N'Angel Ring',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700108) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700108,0,0,0,0,N'Headgear GRS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700152) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700152,0,0,0,0,N'Mask GRS White',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700153) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700153,0,0,0,0,N'Mask GRS Red',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700154) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700154,0,0,0,0,N'Mask GRS Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700155) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700155,0,0,0,0,N'Mask GRS',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700156) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700156,0,0,0,0,N'SASR Cap',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700157) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700157,0,0,0,0,N'Pilot Hat',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700158) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700158,0,0,0,0,N'Toy Hat',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700159) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700159,0,0,0,0,N'Special General Beret',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700160) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700160,0,0,0,0,N'GM Hat',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700161) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (700161,0,0,0,0,N'Cangaceiro Hat',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701109) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (701109,0,0,0,0,N'Red beret(Assault)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701110) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (701110,0,0,0,0,N'Red beret(Submachine)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701111) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (701111,0,0,0,0,N'Red beret(Sniper)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701112) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (701112,0,0,0,0,N'Red beret(Shotgun)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701113) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (701113,0,0,0,0,N'Red beret(handgun)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701114) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (701114,0,0,0,0,N'Beret Black',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701115) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (701115,0,0,0,0,N'Beret RedStar',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701116) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (701116,0,0,0,0,N'Beret YellowStar',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701117) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (701117,0,0,0,0,N'Beret CrossSword',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701118) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (701118,0,0,0,0,N'Beret PBTN Champion',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701119) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (701119,0,0,0,0,N'Beret BlackPB',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701120) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (701120,0,0,0,0,N'Beret Turkey',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701121) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (701121,0,0,0,0,N'Beret Kopassus',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701146) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (701146,0,0,0,0,N'Beret PBNC5',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702122) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (702122,0,0,0,0,N'Mask Skull',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702123) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (702123,0,0,0,0,N'Mask Alien Blue eyes',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702124) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (702124,0,0,0,0,N'Mask Alien Red eyes',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702125) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (702125,0,0,0,0,N'Mask Panda',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702126) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (702126,0,0,0,0,N'Mask Egg',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702128) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (702128,0,0,0,0,N'Mask Red Devil',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702129) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (702129,0,0,0,0,N'Mask Dino Skull',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702130) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (702130,0,0,0,0,N'Headgear Trex',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702131) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (702131,0,0,0,0,N'Mask Gatotkaca',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702132) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (702132,0,0,0,0,N'Mask Gatotkaca Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702135) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (702135,0,0,0,0,N'Mask Sheep',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702136) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (702136,0,0,0,0,N'Mask Zombie',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702143) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (702143,0,0,0,0,N'Mask Azerbaijan',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702144) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (702144,0,0,0,0,N'Mask Latin3',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702145) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (702145,0,0,0,0,N'Mask Latin3 Premium',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702147) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (702147,0,0,0,0,N'Mask Egg KeenEyes',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702148) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (702148,0,0,0,0,N'Mask Egg AcidPol',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702149) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (702149,0,0,0,0,N'Mask TH 1Year',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 703137) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (703137,0,0,0,0,N'Mask - Santa',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 703138) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (703138,0,0,0,0,N'Mask China',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 703139) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (703139,0,0,0,0,N'Mask Bandana',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 703140) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (703140,0,0,0,0,N'Mask Cowboy Hat',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 703141) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (703141,0,0,0,0,N'Mask Bandana (GW)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 703142) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (703142,0,0,0,0,N'Mask Fes Hat',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 703150) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (703150,0,0,0,0,N'Mask Chinese Hat2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 703151) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (703151,0,0,0,0,N'Kopassus Boonie Hat',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800001,0,0,0,0,N'Boeing Sunglass',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800002,0,0,0,0,N'Gas Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800003,0,0,0,0,N'Red Bulls Facegear',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800004,0,0,0,0,N'Viper Red Facegear',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800005,0,0,0,0,N'M15 Gas mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800006) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800006,0,0,0,0,N'Sport Sunglass',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800007,0,0,0,0,N'Skull Warmer',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800008) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800008,0,0,0,0,N'G PB V2 Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800009,0,0,0,0,N'Wiley P Sunglass',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800010) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800010,0,0,0,0,N'PB 6200 Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800011) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800011,0,0,0,0,N'Skull Faceguard',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800012) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800012,0,0,0,0,N'Standard Faceguard',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800013) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800013,0,0,0,0,N'G PB V2 Steel Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800014) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800014,0,0,0,0,N'Joker Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800015) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800015,0,0,0,0,N'Mask Determinado',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800016) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800016,0,0,0,0,N'Mask Fuuu',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800017) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800017,0,0,0,0,N'Mask Troll',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800018) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800018,0,0,0,0,N'Mask China Red',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800019) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800019,0,0,0,0,N'Mask Death',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800020) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800020,0,0,0,0,N'Frail Skull Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800021) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800021,0,0,0,0,N'Mask Frail Skull Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800022) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800022,0,0,0,0,N'Mask Black',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800023) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800023,0,0,0,0,N'Mask Russian Blue Tiger',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800024) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800024,0,0,0,0,N'Mask Korea Marine',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800025) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800025,0,0,0,0,N'Mask France Desert',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800026) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800026,0,0,0,0,N'Mask Flame',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800027) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800027,0,0,0,0,N'Mask Jester BW',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800028) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800028,0,0,0,0,N'Mask Jewel Pink',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800029) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800029,0,0,0,0,N'Mask Two tone',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800030) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800030,0,0,0,0,N'Mask Iron',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800031) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800031,0,0,0,0,N'Mask Jason',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800032) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800032,0,0,0,0,N'Mask Pumpkin',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800033) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800033,0,0,0,0,N'Mask Red Cross',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800034) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800034,0,0,0,0,N'Mask Rorschach',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800035) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800035,0,0,0,0,N'Mask Golden Smile',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800036) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800036,0,0,0,0,N'Mask Target',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800037) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800037,0,0,0,0,N'Mask White',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800038) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800038,0,0,0,0,N'Acid Pol Facegear',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800039) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800039,0,0,0,0,N'Keen Eyes Facegear',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800040) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800040,0,0,0,0,N'Hide Facegear',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800041) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800041,0,0,0,0,N'Leopard Facegear',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800042) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800042,0,0,0,0,N'D-Fox Facegear',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800043) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800043,0,0,0,0,N'Tarantula Facegear',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800084) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800084,0,0,0,0,N'Mask BlackSnake',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800113) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800113,0,0,0,0,N'Mask Pierro',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800117) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800117,0,0,0,0,N'Mask Argentina (WC)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800118) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800118,0,0,0,0,N'Mask Brazil (WC)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800119) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800119,0,0,0,0,N'Mask Egland (WC)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800120) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800120,0,0,0,0,N'Mask France (WC)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800121) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800121,0,0,0,0,N'Mask Germany (WC)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800122) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800122,0,0,0,0,N'Mask Italy (WC)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800123) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800123,0,0,0,0,N'Mask Japan (WC)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800124) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800124,0,0,0,0,N'Mask South Africa (WC)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800125) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800125,0,0,0,0,N'Mask South Korea (WC)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800126) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800126,0,0,0,0,N'Mask Spain',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800129) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800129,0,0,0,0,N'Mask Russia',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800130) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800130,0,0,0,0,N'Mask Ukraine',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800131) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800131,0,0,0,0,N'Mask Belorus',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800132) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800132,0,0,0,0,N'Mask Kazahstan',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800133) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800133,0,0,0,0,N'Mask - Besiktas FC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800134) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800134,0,0,0,0,N'Mask Bursapor FC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800135) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800135,0,0,0,0,N'Mask Fenerbahce FC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800136) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800136,0,0,0,0,N'Mask Galatasaray FC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800137) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800137,0,0,0,0,N'Mask Trabzonspor FC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800138) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800138,0,0,0,0,N'Mask - Frail Skull',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800139) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800139,0,0,0,0,N'Mask Bahamas',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800140) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800140,0,0,0,0,N'Mask Bolivia',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800141) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800141,0,0,0,0,N'Mask Cayman Islands',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800142) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800142,0,0,0,0,N'Mask Chile',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800143) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800143,0,0,0,0,N'Mask Colombia',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800144) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800144,0,0,0,0,N'Mask Costa Rica',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800145) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800145,0,0,0,0,N'Mask Honduras',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800146) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800146,0,0,0,0,N'Mask Jamaica',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800147) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800147,0,0,0,0,N'Mask Mexico',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800148) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800148,0,0,0,0,N'Mask Nicaragua',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800149) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800149,0,0,0,0,N'Mask Panama',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800150) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800150,0,0,0,0,N'Mask Paraguay',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800151) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800151,0,0,0,0,N'Mask Dominican',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800152) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800152,0,0,0,0,N'Mask Ecuador',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800153) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800153,0,0,0,0,N'Mask French Guiana',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800154) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800154,0,0,0,0,N'Mask Guatemala',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800155) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800155,0,0,0,0,N'Mask Guyana',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800156) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800156,0,0,0,0,N'Mask Haiti',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800157) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800157,0,0,0,0,N'Mask Peru',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800158) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800158,0,0,0,0,N'Mask Puerto Rico',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800159) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800159,0,0,0,0,N'Mask Suriname',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800160) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800160,0,0,0,0,N'Mask Trinidad and Tobago',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800161) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800161,0,0,0,0,N'Mask Uruguay',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800162) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800162,0,0,0,0,N'Mask Venezuela',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800163) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800163,0,0,0,0,N'Mask Argentina',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800164) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800164,0,0,0,0,N'Mask Raptr',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800165) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800165,0,0,0,0,N'Mask Canada',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800166) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800166,0,0,0,0,N'Mask United Kingdom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800167) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800167,0,0,0,0,N'Mask USA',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800168) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800168,0,0,0,0,N'Mask Indonesia',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800169) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800169,0,0,0,0,N'Mask PBTN',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800171) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800171,0,0,0,0,N'Mask Reinforce Black',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800172) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800172,0,0,0,0,N'Mask Azzurro_E_Bianco',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800173) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800173,0,0,0,0,N'Mask Azzurro_E_Rossa',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800174) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800174,0,0,0,0,N'Mask Bianca_E_Nera',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800175) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800175,0,0,0,0,N'Mask Bianco_E_Azzurro',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800176) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800176,0,0,0,0,N'Mask Blu_E_Nera',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800177) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800177,0,0,0,0,N'Mask Blu_E_Rossa',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800178) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800178,0,0,0,0,N'Mask Croce_Nera',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800179) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800179,0,0,0,0,N'Mask Giallo_E_Amaranto',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800180) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800180,0,0,0,0,N'Mask-Nera_Bianca_E_Rossa',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800181) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800181,0,0,0,0,N'Mask Nera_E_Bianca',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800182) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800182,0,0,0,0,N'Mask Nera_E_Blu',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800183) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800183,0,0,0,0,N'Mask Nera_E_Rosa',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800184) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800184,0,0,0,0,N'Mask Nera_Gialla_E_Bianca',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800185) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800185,0,0,0,0,N'Mask Rosso_E_Gialla',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800186) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800186,0,0,0,0,N'Mask Stella',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800187) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800187,0,0,0,0,N'Mask Viola',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800191) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800191,0,0,0,0,N'Mask Trojan',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800193) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800193,0,0,0,0,N'Mask Tk FC Besiktas V2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800194) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800194,0,0,0,0,N'Mask Tk FC Bursapor V2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800195) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800195,0,0,0,0,N'Mask Tk FC Fenerbahce V2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800196) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800196,0,0,0,0,N'Mask Tk FC Galatasaray V2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800197) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800197,0,0,0,0,N'Mask Tk FC Trabzonspor V2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800198) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800198,0,0,0,0,N'Mask PBIC 2012',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800199) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800199,0,0,0,0,N'Mask Russian Blue EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800200) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800200,0,0,0,0,N'Mask ROK Marine EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800201) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800201,0,0,0,0,N'Mask French Desert EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800202) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800202,0,0,0,0,N'Mask Black EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800203) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800203,0,0,0,0,N'Mask White EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800204) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800204,0,0,0,0,N'Mask Russian Blue',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800205) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800205,0,0,0,0,N'Mask ROK Marine',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800206) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800206,0,0,0,0,N'Mask French Desert',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800207) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800207,0,0,0,0,N'Mask PBSC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800208) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800208,0,0,0,0,N'Mask PBTN 2012',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800209) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800209,0,0,0,0,N'Mask Death GW',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800210) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800210,0,0,0,0,N'Mask Garena Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800211) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800211,0,0,0,0,N'Mask Garena Red',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800212) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800212,0,0,0,0,N'Mask Garena White',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800213) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800213,0,0,0,0,N'Mask BlackSnake',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800214) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800214,0,0,0,0,N'Mask GSL',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800215) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800215,0,0,0,0,N'Mask PBNC4',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800216) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800216,0,0,0,0,N'Mask PBIC2013',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800217) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800217,0,0,0,0,N'Mask PBSC2013',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800224) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800224,0,0,0,0,N'Mask WC 2014 Aregentina',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800225) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800225,0,0,0,0,N'Mask WC 2014 Brazil',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800226) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800226,0,0,0,0,N'Mask WC 2014 Chile',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800227) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800227,0,0,0,0,N'Mask WC 2014 Colombia',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800228) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800228,0,0,0,0,N'Mask WC 2014 Ecuador',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800229) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800229,0,0,0,0,N'Mask WC 2014 England',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800230) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800230,0,0,0,0,N'Mask WC 2014 France',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800231) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800231,0,0,0,0,N'Mask WC 2014 Germany',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800232) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800232,0,0,0,0,N'Mask WC 2014 Italy',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800233) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800233,0,0,0,0,N'Mask WC 2014 Japan',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800234) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800234,0,0,0,0,N'Mask WC 2014 Korea',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800235) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800235,0,0,0,0,N'Mask WC 2014 Spain',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800236) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800236,0,0,0,0,N'Mask WC 2014 U.S.A',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800237) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800237,0,0,0,0,N'Mask WC 2014 Uruguay',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800238) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800238,0,0,0,0,N'Mask WC 2014 Honduras',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800239) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800239,0,0,0,0,N'Mask WC 2014 Mexico',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800240) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800240,0,0,0,0,N'Mask WC 2014 CostaRica',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800241) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800241,0,0,0,0,N'Mask GSL2014',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800242) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800242,0,0,0,0,N'WC 2014 Greece Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800243) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800243,0,0,0,0,N'WC 2014 Netherlands Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800244) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800244,0,0,0,0,N'WC 2014 Belguim Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800245) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800245,0,0,0,0,N'WC 2014 BaH Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800246) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800246,0,0,0,0,N'WC 2014 Switzerland Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800247) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800247,0,0,0,0,N'WC 2014 Croatia Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800248) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800248,0,0,0,0,N'WC 2014 Portugal Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800249) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800249,0,0,0,0,N'WC 2014 Ghana Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800250) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800250,0,0,0,0,N'WC 2014 Nigeria Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800251) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800251,0,0,0,0,N'WC 2014 Algeria Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800252) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800252,0,0,0,0,N'WC 2014 Cameroon Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800253) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800253,0,0,0,0,N'WC 2014 Cote dIvoire Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800254) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800254,0,0,0,0,N'WC 2014 Iran Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800255) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800255,0,0,0,0,N'WC 2014 Australia Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800256) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800256,0,0,0,0,N'WC 2014 Russia Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800257) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800257,0,0,0,0,N'Mask Mid-Night',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800258) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800258,0,0,0,0,N'Brazuca Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800259) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800259,0,0,0,0,N'Mask Sol',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800260) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800260,0,0,0,0,N'Mask Sol Premium',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800261) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800261,0,0,0,0,N'Mask PBIC2014',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800262) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800262,0,0,0,0,N'Mask Unicorn',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800263) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800263,0,0,0,0,N'Mask Soccer Eagle',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800264) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800264,0,0,0,0,N'Mask Soccer Storm',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800265) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800265,0,0,0,0,N'Mask Soccer Lion',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800266) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800266,0,0,0,0,N'Mask Soccer Canary',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800267) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800267,0,0,0,0,N'Mask Indonesia GW',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800268) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800268,0,0,0,0,N'Mask NoLogo PBSC',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800269) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800269,0,0,0,0,N'Mask NoLogo Latin3',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800270) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800270,0,0,0,0,N'Mask NoLogo Latin3 Pre',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800271) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800271,0,0,0,0,N'Mask NoLogo Mid-night',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800272) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800272,0,0,0,0,N'Mask GSL 2013',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800273) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800273,0,0,0,0,N'Mask Latin4',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800274) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800274,0,0,0,0,N'Latin4 Premium Mask',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800275) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (800275,0,0,0,0,N'Mask GRS2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900002,0,0,0,0,N'Man Half-sleeve Shirt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900004,0,0,0,0,N'Red Bulls Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900005,0,0,0,0,N'Acid Pol Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900007,0,0,0,0,N'Keen Eyes Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900008) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900008,0,0,0,0,N'Tarantula Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900012) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900012,0,0,0,0,N'UDT Unit Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900014) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900014,0,0,0,0,N'Bope Unit Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900015) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900015,0,0,0,0,N'D-Fox Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900016) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900016,0,0,0,0,N'Leopard Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900017) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900017,0,0,0,0,N'Viper Red Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900018) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900018,0,0,0,0,N'Hide Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900028) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900028,0,0,0,0,N'Kopassus Unit Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900034) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900034,0,0,0,0,N'Digital Pattern Jacket F',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900035) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900035,0,0,0,0,N'Gray Spy Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900036) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900036,0,0,0,0,N'707 Unit Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900037) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900037,0,0,0,0,N'Digital Pattern Jacket M',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900038) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900038,0,0,0,0,N'Air-force Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900039) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900039,0,0,0,0,N'Spets Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900040) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900040,0,0,0,0,N'SAS Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900041) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900041,0,0,0,0,N'China Marine Top(Female)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900042) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900042,0,0,0,0,N'SASR Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900043) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900043,0,0,0,0,N'China Marine Top(Male)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900044) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900044,0,0,0,0,N'STF Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900045) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900045,0,0,0,0,N'Maneuver Force Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900046) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900046,0,0,0,0,N'FC.PB Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900047) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900047,0,0,0,0,N'CT-Force SF Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900048) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900048,0,0,0,0,N'SWAT SF Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900049) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900049,0,0,0,0,N'Feebie Unit Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900050) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900050,0,0,0,0,N'Devgru Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900051) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900051,0,0,0,0,N'Black Condor Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900052) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900052,0,0,0,0,N'OMON Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900053) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900053,0,0,0,0,N'Spearhead Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900054) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900054,0,0,0,0,N'CSI Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900055) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (900055,0,0,0,0,N'Omega Top',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000001,0,0,0,0,N'Jeans Pants',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000002,0,0,0,0,N'Acid Pol Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000004,0,0,0,0,N'Digital pattern Bottom F',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000005,0,0,0,0,N'Red Bulls Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000006) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000006,0,0,0,0,N'Keen Eyes Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000007,0,0,0,0,N'Tarantula Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000009,0,0,0,0,N'Digital pattern Bottom M',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000010) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000010,0,0,0,0,N'Bope Unit Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000012) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000012,0,0,0,0,N'D-Fox Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000013) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000013,0,0,0,0,N'Leopard Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000014) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000014,0,0,0,0,N'Viper Red Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000015) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000015,0,0,0,0,N'Hide Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000016) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000016,0,0,0,0,N'UDT Unit Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000023) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000023,0,0,0,0,N'Kopassus Unit Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000027) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000027,0,0,0,0,N'Gray Spy Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000028) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000028,0,0,0,0,N'707 Unit Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000029) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000029,0,0,0,0,N'Air-force Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000030) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000030,0,0,0,0,N'Spets Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000031) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000031,0,0,0,0,N'SAS Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000032) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000032,0,0,0,0,N'China Marine Bottom(Female)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000033) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000033,0,0,0,0,N'SASR Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000034) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000034,0,0,0,0,N'China Marine Bottom(Male)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000035) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000035,0,0,0,0,N'STF Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000036) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000036,0,0,0,0,N'Maneuver Force Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000037) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000037,0,0,0,0,N'FC.PB Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000038) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000038,0,0,0,0,N'CT-Force SF Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000039) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000039,0,0,0,0,N'SWAT SF Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000040) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000040,0,0,0,0,N'Feebie Unit Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000041) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000041,0,0,0,0,N'Devgru Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000042) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000042,0,0,0,0,N'Black Condor Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000043) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000043,0,0,0,0,N'OMON Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000044) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000044,0,0,0,0,N'Spearhead Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000045) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000045,0,0,0,0,N'CSI Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000046) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1000046,0,0,0,0,N'Omega Bottom',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100001,0,0,0,0,N'Man Basic Glove',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100002,0,0,0,0,N'Acid Pol Glove',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100003,0,0,0,0,N'Tarantula Glove',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100004,0,0,0,0,N'Woman Basic Glove',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100006) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100006,0,0,0,0,N'Aquaharbinger',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100008) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100008,0,0,0,0,N'Keen Eyes Glove',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100010) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100010,0,0,0,0,N'D-Fox Glove',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100011) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100011,0,0,0,0,N'Leopard Glove',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100012) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100012,0,0,0,0,N'Viper Red Glove',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100013) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100013,0,0,0,0,N'Hide Glove',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100014) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100014,0,0,0,0,N'Red Bulls Glove',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100015) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100015,0,0,0,0,N'Factory Glove',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100016) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100016,0,0,0,0,N'Red Digital Glove F',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100017) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100017,0,0,0,0,N'Pinkglow Glove F',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100018) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100018,0,0,0,0,N'Red Digital Glove M',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100019) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100019,0,0,0,0,N'Digital Combat Glove',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100020) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100020,0,0,0,0,N'Pinkglow Glove M',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100025) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100025,0,0,0,0,N'Kopassus Unit Glove',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100026) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100026,0,0,0,0,N'Aqua Harbinger',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100027) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100027,0,0,0,0,N'Factory Glove(Female)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100028) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100028,0,0,0,0,N'Skeleton Glove(Male)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100029) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100029,0,0,0,0,N'Skeleton Glove(Female)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100030) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100030,0,0,0,0,N'Digital Glove',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100031) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100031,0,0,0,0,N'F Blue Skeleton Glove',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100032) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1100032,0,0,0,0,N'M Blue Skeleton Glove',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1200001,0,0,0,0,N'General Belt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1200002,0,0,0,0,N'Water Bottle Belt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1200003,0,0,0,0,N'Acid Pol Belt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1200004,0,0,0,0,N'Keen Eyes Belt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1200005,0,0,0,0,N'Red Bulls Belt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200006) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1200006,0,0,0,0,N'Tarantula Belt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1200007,0,0,0,0,N'D-Fox Belt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200008) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1200008,0,0,0,0,N'Hide Belt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1200009,0,0,0,0,N'Leopard Belt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200010) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1200010,0,0,0,0,N'ViperRed Belt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200011) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1200011,0,0,0,0,N'Ammo Pocket Belt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200012) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1200012,0,0,0,0,N'Glow Stick Belt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200013) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1200013,0,0,0,0,N'Square Canteen Belt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200014) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1200014,0,0,0,0,N'USGI Canteen Belt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200015) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1200015,0,0,0,0,N'WW2 Canteen Belt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200016) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1200016,0,0,0,0,N'Big Canteen Belt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200017) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1200017,0,0,0,0,N'Warm Bottle Belt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200018) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1200018,0,0,0,0,N'Messkit Belt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200019) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1200019,0,0,0,0,N'Double Eagle Belt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200020) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1200020,0,0,0,0,N'Skull Belt',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300001,0,0,0,0,N'General Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300002,0,0,0,0,N'Brown H Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300003,0,0,0,0,N'Acid Pol Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300004,0,0,0,0,N'Keen Eyes Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300005,0,0,0,0,N'Red Bulls Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300006) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300006,0,0,0,0,N'Tarantula Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300007,0,0,0,0,N'D-Fox Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300008) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300008,0,0,0,0,N'Leopard Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300009,0,0,0,0,N'Viper Red Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300010) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300010,0,0,0,0,N'Hide Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300011) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300011,0,0,0,0,N'Classic leader Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300012) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300012,0,0,0,0,N'Long Gun Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300013) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300013,0,0,0,0,N'Solid thigh Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300014) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300014,0,0,0,0,N'Solid furrow Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300015) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300015,0,0,0,0,N'Hard Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300016) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300016,0,0,0,0,N'Longgun-leather Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300017) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300017,0,0,0,0,N'Velcro Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300018) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300018,0,0,0,0,N'Swat Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300019) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300019,0,0,0,0,N'Croc leather Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300020) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300020,0,0,0,0,N'2 Buckle Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300021) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300021,0,0,0,0,N'1911 Holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300022) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1300022,0,0,0,0,N'Snake leather holster',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1402009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1402009,0,0,0,0,N'Vacance SwimSuit Male',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1402010) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1402010,0,0,0,0,N'Santa male',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1402011) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1402011,0,0,0,0,N'Valentine Male',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1403009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1403009,0,0,0,0,N'Vacance SwimSuit Female',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1403010) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1403010,0,0,0,0,N'Grim Reaper Female',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1403011) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1403011,0,0,0,0,N'Santa Female',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1403012) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1403012,0,0,0,0,N'Sheep Female',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1403013) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1403013,0,0,0,0,N'Valentine Female',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500501) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1500501,0,0,0,0,N'Acid (Escape Dino)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500502) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1500502,0,0,0,0,N'Elite (Escape Dino)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500503) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1500503,0,0,0,0,N'Raptor (Escape Dino)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500504) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1500504,0,0,0,0,N'Sting (Escape Dino)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500506) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1500506,0,0,0,0,N'Tank (Escape Dino)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500511) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1500511,0,0,0,0,N'Raptor (CC Dino)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500512) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1500512,0,0,0,0,N'Sting (CC Dino)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500513) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1500513,0,0,0,0,N'Acid (CC Dino)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500514) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1500514,0,0,0,0,N'Reinforced Raptor (CC Dino)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500515) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1500515,0,0,0,0,N'Reinforced Sting (CC Dino)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500516) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1500516,0,0,0,0,N'Reinforced Acid (CC Dino)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600002,0,0,0,0,N'EXP 130%',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600003,0,0,0,0,N'EXP 150%',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600004,0,0,0,0,N'Point 130%',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600005,0,0,0,0,N'Color Clan Name',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600006) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600006,0,0,0,0,N'Color Nickname',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600007,0,0,0,0,N'Short Respawn 30%',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600008) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600008,0,0,0,0,N'Ammo Up',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600009,0,0,0,0,N'Disguise Rank',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600010) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600010,0,0,0,0,N'Disguise Nick',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600011) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600011,0,0,0,0,N'Free Move(Unit)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600012) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600012,0,0,0,0,N'Clan Exp 150%',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600014) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600014,0,0,0,0,N'Color CrossHair',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600017) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600017,0,0,0,0,N'Get Dropped Weapon',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600018) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600018,0,0,0,0,N'Exp X2(12~15)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600019) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600019,0,0,0,0,N'Exp X2(15~18)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600020) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600020,0,0,0,0,N'Exp X2(18~21)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600021) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600021,0,0,0,0,N'Exp X2(21~00)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600022) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600022,0,0,0,0,N'Point X2 (12~15)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600023) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600023,0,0,0,0,N'Point X2 (15~18)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600024) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600024,0,0,0,0,N'Point X2 (18~21)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600025) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600025,0,0,0,0,N'Point X2 (21~00)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600026) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600026,0,0,0,0,N'Quick Change Weapon',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600027) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600027,0,0,0,0,N'Quick Change Magazine',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600028) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600028,0,0,0,0,N'Mega HP 10%',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600029) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600029,0,0,0,0,N'Increase Invincible Time',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600030) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600030,0,0,0,0,N'Bullet Proof Vest',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600031) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600031,0,0,0,0,N'AP Rounds',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600032) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600032,0,0,0,0,N'Hollow Point Ammo',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600034) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600034,0,0,0,0,N'C4 Speed Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600035) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600035,0,0,0,0,N'Increase Grenade Slot',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600036) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600036,0,0,0,0,N'Jecketed Hollow Point Ammo',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600037) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600037,0,0,0,0,N'Exp 200%',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600038) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600038,0,0,0,0,N'Point 200%',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600040) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600040,0,0,0,0,N'Mega HP 5%',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600044) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600044,0,0,0,0,N'Bullet Proof Vest Plus',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600062) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600062,0,0,0,0,N'Exp 200% (0 ~ 6)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600063) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600063,0,0,0,0,N'Exp 200% (6 ~ 12)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600064) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600064,0,0,0,0,N'Short Respawn 50%',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600065) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600065,0,0,0,0,N'Bullet Proof Vest Plus(GM)',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600077) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600077,0,0,0,0,N'Short Respawn 20%',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600078) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600078,0,0,0,0,N'Hollow Point Ammo Plus',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600079) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600079,0,0,0,0,N'Metal Bullet Proof Vest',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600080) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600080,0,0,0,0,N'Short Respawn 100%',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600119) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600119,0,0,0,0,N'Point 150%',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600159) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600159,0,0,0,0,N'Point 130% EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600160) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600160,0,0,0,0,N'EXP 130% EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600161) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600161,0,0,0,0,N'Clan Exp 150% EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600162) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600162,0,0,0,0,N'Free Move(Period) EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600163) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600163,0,0,0,0,N'Short Respawn 30% EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600164) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600164,0,0,0,0,N'Short Respawn 20% EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600165) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600165,0,0,0,0,N'Quick Change Weapon EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600166) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600166,0,0,0,0,N'Quick Change Magazine EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600167) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600167,0,0,0,0,N'Tactical Insertion EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600168) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600168,0,0,0,0,N'Get Dropped Weapon EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600169) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600169,0,0,0,0,N'Hollow Point Ammo EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600170) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600170,0,0,0,0,N'Full Metal Jacket Ammo EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600172) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600172,0,0,0,0,N'Mig_Reinforce item',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600173) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600173,0,0,0,0,N'Mig_Control Increase',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600174) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600174,0,0,0,0,N'Mig_Penet. Increase',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600175) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600175,0,0,0,0,N'Mig_Accuracy Increase',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600176) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600176,0,0,0,0,N'Mig_Reaction Decrease',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600177) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600177,0,0,0,0,N'Mig_Damage Increase',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600178) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600178,0,0,0,0,N'Mig_Defense Increase',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600180) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600180,0,0,0,0,N'Skill Time Boost X2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600181) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600181,0,0,0,0,N'Captain Armband',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600182) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600182,0,0,0,0,N'Short Respawn 100% EV',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600183) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600183,0,0,0,0,N'Short Respawn 50% GW',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600185) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1600185,0,0,0,0,N'Ammo Up 10% GW',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800039) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800039,0,0,0,0,N'CS Giga Bomb',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800041) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800041,0,0,0,0,N'CS Defcon1 Pack',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800042) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800042,0,0,0,0,N'CS Defcon2 Pack',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800043) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800043,0,0,0,0,N'CS Defcon3 Pack',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800045) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800045,0,0,0,0,N'CS Weapon Crate 2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800046) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800046,0,0,0,0,N'CS Mini Bomb',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800047) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800047,0,0,0,0,N'Change Nickname',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800048) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800048,0,0,0,0,N'Clear History',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800049) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800049,0,0,0,0,N'Clear Score',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800050) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800050,0,0,0,0,N'Reset Escape',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800051) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800051,0,0,0,0,N'Change Clan Name',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800052) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800052,0,0,0,0,N'Change Clan Mark',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800053) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800053,0,0,0,0,N'Clear Clan History',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800055) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800055,0,0,0,0,N'Clan Member +50',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800056) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800056,0,0,0,0,N'Clear Clan Point',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800058) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800058,0,0,0,0,N'CS Mega Bomb',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800059) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800059,0,0,0,0,N'CS Weapon Crate',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800060) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800060,0,0,0,0,N'CS Survival Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800061) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800061,0,0,0,0,N'Mega Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800066) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800066,0,0,0,0,N'CS Point Cachpon1',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800067) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800067,0,0,0,0,N'CS Point Gachpon2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800068) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800068,0,0,0,0,N'CS Giga Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800069) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800069,0,0,0,0,N'CS Swat Kit(Assault) 7D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800070) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800070,0,0,0,0,N'CS Swat Kit(Assault) 30D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800071) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800071,0,0,0,0,N'CS Swat Kit(Support) 30D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800072) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800072,0,0,0,0,N'CS Swat Kit(Support) 7D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800073) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800073,0,0,0,0,N'CS Swat Kit Infiltrat 7D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800074) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800074,0,0,0,0,N'CS Swat kit lnfiltrat 30D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800075) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800075,0,0,0,0,N'CS Swat Kit(Combo) 30D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800076) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800076,0,0,0,0,N'CS Swat Kit(Combo) 7D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800081) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800081,0,0,0,0,N'CS Mega Bomb Russia',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800082) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800082,0,0,0,0,N'CS Item Box1',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800083) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800083,0,0,0,0,N'CS Item Box2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800084) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800084,0,0,0,0,N'CS Angpao',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800085) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800085,0,0,0,0,N'View Other User Info',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800087) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800087,0,0,0,0,N'CS Gift Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800091) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800091,0,0,0,0,N'Ketupat',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800092) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800092,0,0,0,0,N'CS Random Mask Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800101) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800101,0,0,0,0,N'Valentine Chocolate',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800108) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800108,0,0,0,0,N'CS Che Guevara',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800111) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800111,0,0,0,0,N'CS Ready Box Silver Latin',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800112) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800112,0,0,0,0,N'CS Ready Box Gold Latin',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800113) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800113,0,0,0,0,N'CS Ready Box Normal Latin',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800120) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800120,0,0,0,0,N'CS Point Bomb',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800121) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800121,0,0,0,0,N'CS Point Bomb Premium',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800122) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800122,0,0,0,0,N'CS Ready Box Normal Prize',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800123) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800123,0,0,0,0,N'CS Ready Box Silver Prize',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800124) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800124,0,0,0,0,N'CS Ready Box Gold Prize',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800125) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800125,0,0,0,0,N'CS Ready Box Silver',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800126) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800126,0,0,0,0,N'CS Ready Box Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800127) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800127,0,0,0,0,N'CS Ready Box Normal',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800128) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800128,0,0,0,0,N'Prize Money 100',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800129) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800129,0,0,0,0,N'CS Christmas Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800130) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800130,0,0,0,0,N'CS Gift Box North',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800131) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800131,0,0,0,0,N'CS Facebook Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800132) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800132,0,0,0,0,N'CS Raptor Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800133) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800133,0,0,0,0,N'CS X-File Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800171) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800171,0,0,0,0,N'Clan Person Plus 10',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800179) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800179,0,0,0,0,N'Skill Master Lv Ticket',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800183) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800183,0,0,0,0,N'MegaPhone',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800500) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800500,0,0,0,0,N'CS Random Box Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800501) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800501,0,0,0,0,N'CS Random Box Black',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800502) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800502,0,0,0,0,N'CS Gatotkaca Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800503) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800503,0,0,0,0,N'CS Present Box Latin',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800504) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800504,0,0,0,0,N'CS Random Box Black RU',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800505) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800505,0,0,0,0,N'CS Random Box Gold RU',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800507) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800507,0,0,0,0,N'CS Unique Random Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800513) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800513,0,0,0,0,N'CS Tournament Ticket Sl',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800514) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800514,0,0,0,0,N'CS Tournament Ticket G',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800515) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800515,0,0,0,0,N'CS Tournament Ticket On',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800516) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800516,0,0,0,0,N'CS Super Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800517) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800517,0,0,0,0,N'CS PBNC4 Supplykit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800518) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800518,0,0,0,0,N'CS Point Random Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800519) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800519,0,0,0,0,N'CS Garena Premium Gacha A',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800520) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800520,0,0,0,0,N'CS Garena Premium Gacha B',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800521) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800521,0,0,0,0,N'CS Ramadan Random Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800522) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800522,0,0,0,0,N'CS Super Box ID',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800526) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800526,0,0,0,0,N'CS PBIC2012 RandomBox',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800527) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800527,0,0,0,0,N'CS Golden Box TR',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800528) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800528,0,0,0,0,N'CS GRS Supply Kit A',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800529) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800529,0,0,0,0,N'CS GRS Supply Kit B',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800530) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800530,0,0,0,0,N'CS Elite Gacha TH Assault',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800531) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800531,0,0,0,0,N'CS Garena Premium Gacha D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800532) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800532,0,0,0,0,N'CS Crossover Supply Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800533) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800533,0,0,0,0,N'CS Friendship Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800534) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800534,0,0,0,0,N'CS BOB Gacha TH',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800535) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800535,0,0,0,0,N'CS Neon RandomBox',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800536) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800536,0,0,0,0,N'CS Neon RandomBox MENA',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800537) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800537,0,0,0,0,N'CS Vacation Supply Kit 2013',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800538) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800538,0,0,0,0,N'CS BOB Gacha ID',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800539) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800539,0,0,0,0,N'CS PC Cafe Gacha',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800540) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800540,0,0,0,0,N'CS Zepetto Main Supply Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800541) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800541,0,0,0,0,N'CS Zepetto Sec. Supply Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800542) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800542,0,0,0,0,N'CS Zepetto Melee Supply Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800543) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800543,0,0,0,0,N'CS Zepetto Mask Supply Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800544) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800544,0,0,0,0,N'CS Sochi2014 Supply Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800545) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800545,0,0,0,0,N'CS WC 2014 RandomBox A',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800546) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800546,0,0,0,0,N'CS WC 2014 RandomBox B',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800547) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800547,0,0,0,0,N'CS WC 2014 RandomBox C',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800548) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800548,0,0,0,0,N'CS WC 2014 RandomBox D',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800549) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800549,0,0,0,0,N'CS PBNC5 Supply Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800555) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800555,0,0,0,0,N'CS RandomBox Black 1.5',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800558) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800558,0,0,0,0,N'CS WC Ticket',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800559) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800559,0,0,0,0,N'CS WC RedCard',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800560) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800560,0,0,0,0,N'CS WC YellowCard',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800561) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800561,0,0,0,0,N'CS Brazil Brazuca Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800562) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800562,0,0,0,0,N'CS WC 2014 Event Gacha',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800563) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800563,0,0,0,0,N'Sol Festival RandomBox',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800564) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800564,0,0,0,0,N'LAM Ramadan RandomBox',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800566) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800566,0,0,0,0,N'CS PBIC2013 Gachapon',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800568) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800568,0,0,0,0,N'CS PBIC2012 Gachapon PH',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800569) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800569,0,0,0,0,N'CS PBIC2013 Gachapon PH',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800578) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800578,0,0,0,0,N'Assault Rifle Gachapon',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800579) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800579,0,0,0,0,N'Submachine Gun Gachapon',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800580) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800580,0,0,0,0,N'PSG1 Elite Supply Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800581) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800581,0,0,0,0,N'VSK94 Elite Supply Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800582) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800582,0,0,0,0,N'SVU Elite Supply Kit',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800583) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800583,0,0,0,0,N'Garena`s P90 G Gacha',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800584) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800584,0,0,0,0,N'M1887 Lion Heart Gacha',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800585) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800585,0,0,0,0,N'Best of Best 2014 Gacha',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800586) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800586,0,0,0,0,N'Mini Gachapon',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800587) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800587,0,0,0,0,N'CS X-mas RandomBox',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800588) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800588,0,0,0,0,N'CS X-mas GiftBox',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800589) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800589,0,0,0,0,N'CS 2015 NY SupplyKit_ID',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800590) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800590,0,0,0,0,N'Best of 2014 Gacha',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800591) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800591,0,0,0,0,N'Collection X-mas Gacha',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800592) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800592,0,0,0,0,N'E-sport Random Box A',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800593) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800593,0,0,0,0,N'E-sport Random Box B',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800594) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800594,0,0,0,0,N'PBIC2013 Random Box A',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800595) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800595,0,0,0,0,N'PBIC2013 Random Box B',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800596) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800596,0,0,0,0,N'CS X-mas RandomBox RU',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800597) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800597,0,0,0,0,N'CS NY Random Box TR',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800598) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800598,0,0,0,0,N'CS 1st Anniversary Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800602) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800602,0,0,0,0,N'CS sorry random box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800603) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800603,0,0,0,0,N'CS PaysafeRandombox Mini',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800604) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800604,0,0,0,0,N'CS PaysafeRandombox Mega',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800605) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800605,0,0,0,0,N'Sorry RandomBox2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800606) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800606,0,0,0,0,N'Thanks RandomBox 1',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800607) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800607,0,0,0,0,N'Thanks RandomBox 2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800608) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800608,0,0,0,0,N'New Year AUG A3 Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800609) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800609,0,0,0,0,N'CS AUG A3 kasasi Random Box',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800611) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800611,0,0,0,0,N'CS CNY 2015 AUG Gacha',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800612) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (1800612,0,0,0,0,N'CS CNY 2015 Kriss Gacha',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000001,0,0,0,0,N'Point 100',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000002,0,0,0,0,N'Point 200',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000003,0,0,0,0,N'Point 300',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000004,0,0,0,0,N'Point 400',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000005,0,0,0,0,N'Point 500',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000006) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000006,0,0,0,0,N'Point 600',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000007,0,0,0,0,N'Point 700',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000008) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000008,0,0,0,0,N'Point 800',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000009,0,0,0,0,N'Point 900',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000010) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000010,0,0,0,0,N'Point 1,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000015) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000015,0,0,0,0,N'Point 1,500',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000020) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000020,0,0,0,0,N'Point 2,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000025) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000025,0,0,0,0,N'Point 2,500',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000030) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000030,0,0,0,0,N'Point 3,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000032) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000032,0,0,0,0,N'Point 3,200',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000035) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000035,0,0,0,0,N'Point 3,500',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000040) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000040,0,0,0,0,N'Point 4,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000045) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000045,0,0,0,0,N'Point 4,500',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000050) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000050,0,0,0,0,N'Point 5,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000055) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000055,0,0,0,0,N'Point 5,500',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000060) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000060,0,0,0,0,N'Point 6,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000065) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000065,0,0,0,0,N'Point 6,500',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000070) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000070,0,0,0,0,N'Point 7,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000075) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000075,0,0,0,0,N'Point 7,500',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000080) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000080,0,0,0,0,N'Point 8,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000085) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000085,0,0,0,0,N'Point 8,500',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000090) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000090,0,0,0,0,N'Point 9,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000095) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000095,0,0,0,0,N'Point 9,500',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000100) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000100,0,0,0,0,N'Point 10,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000110) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000110,0,0,0,0,N'Point 11,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000120) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000120,0,0,0,0,N'Point 12,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000125) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000125,0,0,0,0,N'Point 12,500',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000130) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000130,0,0,0,0,N'Point 13,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000140) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000140,0,0,0,0,N'Point 14,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000150) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000150,0,0,0,0,N'Point 15,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000160) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000160,0,0,0,0,N'Point 16,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000170) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000170,0,0,0,0,N'Point 17,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000180) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000180,0,0,0,0,N'Point 18,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000190) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000190,0,0,0,0,N'Point 19,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000200) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000200,0,0,0,0,N'Point 20,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000220) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000220,0,0,0,0,N'Point 22,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000240) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000240,0,0,0,0,N'Point 24,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000250) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000250,0,0,0,0,N'Point 25,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000280) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000280,0,0,0,0,N'Point 28,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000300) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000300,0,0,0,0,N'Point 30,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000320) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000320,0,0,0,0,N'Point 32,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000350) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000350,0,0,0,0,N'Point 35,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000360) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000360,0,0,0,0,N'Point 36,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000400) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000400,0,0,0,0,N'Point 40,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000450) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000450,0,0,0,0,N'Point 45,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000500) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000500,0,0,0,0,N'Point 50,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000550) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000550,0,0,0,0,N'Point 55,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000600) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000600,0,0,0,0,N'Point 60,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000700) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000700,0,0,0,0,N'Point 70,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000850) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2000850,0,0,0,0,N'Point 85,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2001000) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2001000,0,0,0,0,N'Point 100,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2002000) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2002000,0,0,0,0,N'Point 200,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2002500) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2002500,0,0,0,0,N'Point 250,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2003000) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2003000,0,0,0,0,N'Point 300,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2005000) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2005000,0,0,0,0,N'Point 500,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2010000) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2010000,0,0,0,0,N'Point 1,000,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2030000) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2030000,0,0,0,0,N'Point 3,000,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2080000) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2080000,0,0,0,0,N'Point 8,000,000',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2200001) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2200001,0,0,0,0,N'RSTab1',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2200002) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2200002,0,0,0,0,N'RSTab2',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2200003) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2200003,0,0,0,0,N'RSTab3',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600004) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2600004,0,0,0,0,N'QC Gold',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600005) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2600005,0,0,0,0,N'QC Assault',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600006) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2600006,0,0,0,0,N'QC BackUp',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600007) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2600007,0,0,0,0,N'QC Infiltration',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600008) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2600008,0,0,0,0,N'QC Special',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600009) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2600009,0,0,0,0,N'QC Defcon',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600010) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2600010,0,0,0,0,N'QC Commissioned O',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600011) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2600011,0,0,0,0,N'QC Company O',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600012) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2600012,0,0,0,0,N'QC Field O',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600014) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2600014,0,0,0,0,N'QC Dino 2ND',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600015) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2600015,0,0,0,0,N'QC Human 2ND',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600016) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2600016,0,0,0,0,N'QC Dino 3RD',0)
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600017) = 0) INSERT INTO PB_BT_SShopItem ( ItemID, AuthType, AuthExpireType, Expire, AutoStart, Name, Restriction ) VALUES (2600017,0,0,0,0,N'QC Human 3RD',0)
GO


/***************************************
* UPDATE MATCHING INFO 
***************************************/
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'Male Default Set')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'Male Default Set' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'Female Default Set')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'Female Default Set' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'Special General Beret')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'Special General Beret' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'Toy Hat')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'Toy Hat' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'Cangaceiro Hat')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'Cangaceiro Hat' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'Anti Flash Bang Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'Anti Flash Bang Mask' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'Silver License')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'Silver License' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'Gold License')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'Gold License' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS New M4 Supply Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS New M4 Supply Kit' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS Rangemaster Supply Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Rangemaster Supply Kit' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS RB454 Supply Kit 1D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS RB454 Supply Kit 1D' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS Dual Uzi Supply Kit 1D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Dual Uzi Supply Kit 1D' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS RM Supply Kit 3D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS RM Supply Kit 3D' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS RM Supply Kit 7D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS RM Supply Kit 7D' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS RB454 Supply Kit 3D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS RB454 Supply Kit 3D' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS RB454 Supply Kit 7D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS RB454 Supply Kit 7D' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS Famas G2 Supply Kit 1D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Famas G2 Supply Kit 1D' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS Dual Uzi Supply Kit 3D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Dual Uzi Supply Kit 3D' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS Dual Uzi Supply Kit 7D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Dual Uzi Supply Kit 7D' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS Dual Uzi Supply Kit 30D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Dual Uzi Supply Kit 30D' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS Famas G2 Supply Kit 3D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Famas G2 Supply Kit 3D' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS Famas G2 Supply Kit 7D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Famas G2 Supply Kit 7D' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS Famas G2 Supply Kit 30D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Famas G2 Supply Kit 30D' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'1,000 Point')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'1,000 Point' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'10,000 Point')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'10,000 Point' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'30,000 Point')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'30,000 Point' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'500 Point')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'500 Point' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'5,000 Point')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'5,000 Point' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS DSR-1 Random Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS DSR-1 Random Box' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS Dual K-413 Random Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Dual K-413 Random Box' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS New M4 Supply Kit 3D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS New M4 Supply Kit 3D' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS New M4 Supply Kit 7D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS New M4 Supply Kit 7D' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS New M4 Supply Kit 30D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS New M4 Supply Kit 30D' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS AUG A3 Random Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS AUG A3 Random Box' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS Garena Premium Gacha C')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Garena Premium Gacha C' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS AK Supply Kit Elite')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS AK Supply Kit Elite' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS M4 Supply Kit Elite')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS M4 Supply Kit Elite' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS Super Lucky Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Super Lucky Box' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'Black Gacha AUGA3')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'Black Gacha AUGA3' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'Black Gacha Kriss')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'Black Gacha Kriss' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'Black Gacha L115A1')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'Black Gacha L115A1' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS Brazuca Supply Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Brazuca Supply Kit' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS 1Day Gacha')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS 1Day Gacha' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS PSG1 Elite RandomBox')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS PSG1 Elite RandomBox' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS Super Box2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Super Box2' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS SVU Elite RandomBox')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS SVU Elite RandomBox' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'DragunovElite SupplyKit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'DragunovElite SupplyKit' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS SPAS Elite RandomBox')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS SPAS Elite RandomBox' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'G36C Elite Supply Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'G36C Elite Supply Kit' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS PBIC 2014 Gacha TR')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS PBIC 2014 Gacha TR' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS PBIC 2014 Gacha MENA')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS PBIC 2014 Gacha MENA' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS PBIC 2014 Gacha ID')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS PBIC 2014 Gacha ID' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS NewWeapon Supply Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS NewWeapon Supply Kit' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'CS SPAS Elite Gacha TH')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS SPAS Elite Gacha TH' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'Toy series SupplyKit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'Toy series SupplyKit' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'2015_B.O.B_SupplyKIT')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'2015_B.O.B_SupplyKIT' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'BestofLatin2_Supplykit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'BestofLatin2_Supplykit' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 0 AND GoodsId = 0 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (0,0,N'License Sl. 30D 1D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 0) != 0) UPDATE PB_BT_SShopItem SET Name =N'License Sl. 30D 1D' WHERE ItemId = 0
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103001 AND GoodsId = 10300100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103001,10300100,N'SG 550 Ext')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103001) != 0) UPDATE PB_BT_SShopItem SET Name =N'SG 550 Ext' WHERE ItemId = 103001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103002 AND GoodsId = 10300200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103002,10300200,N'AK47 Ext')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103002) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK47 Ext' WHERE ItemId = 103002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103003 AND GoodsId = 10300300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103003,10300300,N'M4A1 Ext')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103003) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4A1 Ext' WHERE ItemId = 103003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103004 AND GoodsId = 10300400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103004,10300400,N'K-2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103004) != 0) UPDATE PB_BT_SShopItem SET Name =N'K-2' WHERE ItemId = 103004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103005 AND GoodsId = 10300500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103005,10300500,N'F2000 Ext')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103005) != 0) UPDATE PB_BT_SShopItem SET Name =N'F2000 Ext' WHERE ItemId = 103005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103009 AND GoodsId = 10300900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103009,10300900,N'AK-47 G.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103009) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK-47 G.' WHERE ItemId = 103009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103010 AND GoodsId = 10301000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103010,10301000,N'M4A1 S.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103010) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4A1 S.' WHERE ItemId = 103010
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103011 AND GoodsId = 10301100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103011,10301100,N'K-201 Ext')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103011) != 0) UPDATE PB_BT_SShopItem SET Name =N'K-201 Ext' WHERE ItemId = 103011
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103013 AND GoodsId = 10301300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103013,10301300,N'G36C Ext')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103013) != 0) UPDATE PB_BT_SShopItem SET Name =N'G36C Ext' WHERE ItemId = 103013
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103014 AND GoodsId = 10301400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103014,10301400,N'SG 550 S.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103014) != 0) UPDATE PB_BT_SShopItem SET Name =N'SG 550 S.' WHERE ItemId = 103014
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103015 AND GoodsId = 10301500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103015,10301500,N'AK SOPMOD')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103015) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK SOPMOD' WHERE ItemId = 103015
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103017 AND GoodsId = 10301700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103017,10301700,N'AK47 Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103017) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK47 Sl' WHERE ItemId = 103017
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103018 AND GoodsId = 10301800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103018,10301800,N'SG 550 MB')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103018) != 0) UPDATE PB_BT_SShopItem SET Name =N'SG 550 MB' WHERE ItemId = 103018
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103019 AND GoodsId = 10301900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103019,10301900,N'SG 550 Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103019) != 0) UPDATE PB_BT_SShopItem SET Name =N'SG 550 Sl' WHERE ItemId = 103019
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103020 AND GoodsId = 10302000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103020,10302000,N'SG 550 Wh.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103020) != 0) UPDATE PB_BT_SShopItem SET Name =N'SG 550 Wh.' WHERE ItemId = 103020
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103021 AND GoodsId = 10302100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103021,10302100,N'M4A1 Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103021) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4A1 Sl' WHERE ItemId = 103021
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103022 AND GoodsId = 10302200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103022,10302200,N'M4A1 Wh.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103022) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4A1 Wh.' WHERE ItemId = 103022
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103023 AND GoodsId = 10302300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103023,10302300,N'M4A1 G.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103023) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4A1 G.' WHERE ItemId = 103023
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103024 AND GoodsId = 10302400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103024,10302400,N'M4A1 G.+')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103024) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4A1 G.+' WHERE ItemId = 103024
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103025 AND GoodsId = 10302500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103025,10302500,N'AK47 Sl+')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103025) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK47 Sl+' WHERE ItemId = 103025
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103026 AND GoodsId = 10302600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103026,10302600,N'SG 550 Metal Blue')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103026) != 0) UPDATE PB_BT_SShopItem SET Name =N'SG 550 Metal Blue' WHERE ItemId = 103026
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103027 AND GoodsId = 10302700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103027,10302700,N'G36C Ext. Event')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103027) != 0) UPDATE PB_BT_SShopItem SET Name =N'G36C Ext. Event' WHERE ItemId = 103027
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103028 AND GoodsId = 10302800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103028,10302800,N'AUG A3 Event')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103028) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 Event' WHERE ItemId = 103028
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103029 AND GoodsId = 10302900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103029,10302900,N'G36C Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103029) != 0) UPDATE PB_BT_SShopItem SET Name =N'G36C Sl' WHERE ItemId = 103029
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103030 AND GoodsId = 10303000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103030,10303000,N'SG 550 Mb. +20%')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103030) != 0) UPDATE PB_BT_SShopItem SET Name =N'SG 550 Mb. +20%' WHERE ItemId = 103030
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103031 AND GoodsId = 10303100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103031,10303100,N'G36C S. + 20')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103031) != 0) UPDATE PB_BT_SShopItem SET Name =N'G36C S. + 20' WHERE ItemId = 103031
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103032 AND GoodsId = 10303200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103032,10303200,N'F2000 Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103032) != 0) UPDATE PB_BT_SShopItem SET Name =N'F2000 Sl' WHERE ItemId = 103032
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103033 AND GoodsId = 10303300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103033,10303300,N'AK SOPMOD +20')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103033) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK SOPMOD +20' WHERE ItemId = 103033
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103034 AND GoodsId = 10303400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103034,10303400,N'G36C Sl. + 20')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103034) != 0) UPDATE PB_BT_SShopItem SET Name =N'G36C Sl. + 20' WHERE ItemId = 103034
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103035 AND GoodsId = 10303500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103035,10303500,N'SG 550 Sp D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103035) != 0) UPDATE PB_BT_SShopItem SET Name =N'SG 550 Sp D' WHERE ItemId = 103035
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103036 AND GoodsId = 10303600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103036,10303600,N'AUG A3')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103036) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3' WHERE ItemId = 103036
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103037 AND GoodsId = 10303700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103037,10303700,N'AUG A3 G.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103037) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 G.' WHERE ItemId = 103037
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103038 AND GoodsId = 10303800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103038,10303800,N'G36C Ext D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103038) != 0) UPDATE PB_BT_SShopItem SET Name =N'G36C Ext D' WHERE ItemId = 103038
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103039 AND GoodsId = 10303900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103039,10303900,N'AK SOPMOD D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103039) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK SOPMOD D' WHERE ItemId = 103039
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103040 AND GoodsId = 10304000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103040,10304000,N'AUG A3 D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103040) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 D' WHERE ItemId = 103040
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103041 AND GoodsId = 10304100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103041,10304100,N'AK SOPMOD CG')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103041) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK SOPMOD CG' WHERE ItemId = 103041
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103042 AND GoodsId = 10304200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103042,10304200,N'M4 SR-16 LV1')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103042) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4 SR-16 LV1' WHERE ItemId = 103042
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103043 AND GoodsId = 10304300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103043,10304300,N'M4 SR-16 LV2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103043) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4 SR-16 LV2' WHERE ItemId = 103043
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103044 AND GoodsId = 10304400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103044,10304400,N'M4 SR-16 LV3')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103044) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4 SR-16 LV3' WHERE ItemId = 103044
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103045 AND GoodsId = 10304500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103045,10304500,N'M4 SR-16 F.C')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103045) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4 SR-16 F.C' WHERE ItemId = 103045
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103046 AND GoodsId = 10304600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103046,10304600,N'F2000 Sl D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103046) != 0) UPDATE PB_BT_SShopItem SET Name =N'F2000 Sl D' WHERE ItemId = 103046
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103048 AND GoodsId = 10304800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103048,10304800,N'AUG A3 Black')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103048) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 Black' WHERE ItemId = 103048
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103049 AND GoodsId = 10304900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103049,10304900,N'Famas G2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103049) != 0) UPDATE PB_BT_SShopItem SET Name =N'Famas G2' WHERE ItemId = 103049
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103050 AND GoodsId = 10305000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103050,10305000,N'Famas G2 Commando')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103050) != 0) UPDATE PB_BT_SShopItem SET Name =N'Famas G2 Commando' WHERE ItemId = 103050
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103051 AND GoodsId = 10305100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103051,10305100,N'Famas G2 Sniper')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103051) != 0) UPDATE PB_BT_SShopItem SET Name =N'Famas G2 Sniper' WHERE ItemId = 103051
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103052 AND GoodsId = 10305200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103052,10305200,N'Famas G2 M203')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103052) != 0) UPDATE PB_BT_SShopItem SET Name =N'Famas G2 M203' WHERE ItemId = 103052
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103053 AND GoodsId = 10305300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103053,10305300,N'SS2-V4 PARA Sniper')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103053) != 0) UPDATE PB_BT_SShopItem SET Name =N'SS2-V4 PARA Sniper' WHERE ItemId = 103053
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103054 AND GoodsId = 10305400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103054,10305400,N'AK47 Gold D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103054) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK47 Gold D' WHERE ItemId = 103054
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103055 AND GoodsId = 10305500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103055,10305500,N'M4A1 Wh. D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103055) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4A1 Wh. D' WHERE ItemId = 103055
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103056 AND GoodsId = 10305600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103056,10305600,N'SG 550 Sl D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103056) != 0) UPDATE PB_BT_SShopItem SET Name =N'SG 550 Sl D' WHERE ItemId = 103056
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103057 AND GoodsId = 10305700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103057,10305700,N'VZ. 52')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103057) != 0) UPDATE PB_BT_SShopItem SET Name =N'VZ. 52' WHERE ItemId = 103057
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103058 AND GoodsId = 10305800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103058,10305800,N'AK47 F.C')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103058) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK47 F.C' WHERE ItemId = 103058
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103059 AND GoodsId = 10305900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103059,10305900,N'AK-47')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103059) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK-47' WHERE ItemId = 103059
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103060 AND GoodsId = 10306000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103060,10306000,N'M4A1')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103060) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4A1' WHERE ItemId = 103060
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103061 AND GoodsId = 10306100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103061,10306100,N'SS2-V4 Para Sniper G')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103061) != 0) UPDATE PB_BT_SShopItem SET Name =N'SS2-V4 Para Sniper G' WHERE ItemId = 103061
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103062 AND GoodsId = 10306200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103062,10306200,N'Famas G2 Commando E-Sport')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103062) != 0) UPDATE PB_BT_SShopItem SET Name =N'Famas G2 Commando E-Sport' WHERE ItemId = 103062
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103063 AND GoodsId = 10306300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103063,10306300,N'AUG A3 E-Sport')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103063) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 E-Sport' WHERE ItemId = 103063
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103064 AND GoodsId = 10306400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103064,10306400,N'FAMAS G2 Commando G')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103064) != 0) UPDATE PB_BT_SShopItem SET Name =N'FAMAS G2 Commando G' WHERE ItemId = 103064
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103065 AND GoodsId = 10306500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103065,10306500,N'AUG A3 Blue')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103065) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 Blue' WHERE ItemId = 103065
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103066 AND GoodsId = 10306600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103066,10306600,N'SG 550')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103066) != 0) UPDATE PB_BT_SShopItem SET Name =N'SG 550' WHERE ItemId = 103066
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103067 AND GoodsId = 10306700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103067,10306700,N'G36C')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103067) != 0) UPDATE PB_BT_SShopItem SET Name =N'G36C' WHERE ItemId = 103067
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103068 AND GoodsId = 10306800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103068,10306800,N'AK-47 FC Red')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103068) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK-47 FC Red' WHERE ItemId = 103068
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103069 AND GoodsId = 10306900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103069,10306900,N'Scar-H CQC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103069) != 0) UPDATE PB_BT_SShopItem SET Name =N'Scar-H CQC' WHERE ItemId = 103069
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103070 AND GoodsId = 10307000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103070,10307000,N'Scar-H STD Silencer')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103070) != 0) UPDATE PB_BT_SShopItem SET Name =N'Scar-H STD Silencer' WHERE ItemId = 103070
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103071 AND GoodsId = 10307100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103071,10307100,N'AUG A3 PBIC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103071) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 PBIC' WHERE ItemId = 103071
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103072 AND GoodsId = 10307200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103072,10307200,N'AK47 Old Spec')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103072) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK47 Old Spec' WHERE ItemId = 103072
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103073 AND GoodsId = 10307300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103073,10307300,N'SG 550 Sp EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103073) != 0) UPDATE PB_BT_SShopItem SET Name =N'SG 550 Sp EV' WHERE ItemId = 103073
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103074 AND GoodsId = 10307400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103074,10307400,N'AK SOPMOD EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103074) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK SOPMOD EV' WHERE ItemId = 103074
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103075 AND GoodsId = 10307500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103075,10307500,N'AUG A3 EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103075) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 EV' WHERE ItemId = 103075
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103076 AND GoodsId = 10307600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103076,10307600,N'AK47 Sl EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103076) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK47 Sl EV' WHERE ItemId = 103076
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103077 AND GoodsId = 10307700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103077,10307700,N'F2000 Sl EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103077) != 0) UPDATE PB_BT_SShopItem SET Name =N'F2000 Sl EV' WHERE ItemId = 103077
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103078 AND GoodsId = 10307800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103078,10307800,N'AK47 Ext EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103078) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK47 Ext EV' WHERE ItemId = 103078
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103079 AND GoodsId = 10307900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103079,10307900,N'M4A1 Ext EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103079) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4A1 Ext EV' WHERE ItemId = 103079
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103080 AND GoodsId = 10308000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103080,10308000,N'M4A1 S. EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103080) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4A1 S. EV' WHERE ItemId = 103080
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103081 AND GoodsId = 10308100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103081,10308100,N'M4A1 Sl EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103081) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4A1 Sl EV' WHERE ItemId = 103081
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103082 AND GoodsId = 10308200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103082,10308200,N'M4 SR-16 LV1 EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103082) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4 SR-16 LV1 EV' WHERE ItemId = 103082
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103083 AND GoodsId = 10308300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103083,10308300,N'M4 SR-16 LV2 EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103083) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4 SR-16 LV2 EV' WHERE ItemId = 103083
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103084 AND GoodsId = 10308400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103084,10308400,N'M4 SR-16 LV3 EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103084) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4 SR-16 LV3 EV' WHERE ItemId = 103084
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103085 AND GoodsId = 10308500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103085,10308500,N'Scar-H CQC D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103085) != 0) UPDATE PB_BT_SShopItem SET Name =N'Scar-H CQC D' WHERE ItemId = 103085
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103086 AND GoodsId = 10308600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103086,10308600,N'AK-47 Goddess')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103086) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK-47 Goddess' WHERE ItemId = 103086
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103087 AND GoodsId = 10308700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103087,10308700,N'Famas G2 Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103087) != 0) UPDATE PB_BT_SShopItem SET Name =N'Famas G2 Sl' WHERE ItemId = 103087
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103088 AND GoodsId = 10308800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103088,10308800,N'Scar-H LB Sniper')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103088) != 0) UPDATE PB_BT_SShopItem SET Name =N'Scar-H LB Sniper' WHERE ItemId = 103088
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103089 AND GoodsId = 10308900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103089,10308900,N'AUG A3 Gold Non')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103089) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 Gold Non' WHERE ItemId = 103089
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103090 AND GoodsId = 10309000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103090,10309000,N'FAMAS G2 Digital')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103090) != 0) UPDATE PB_BT_SShopItem SET Name =N'FAMAS G2 Digital' WHERE ItemId = 103090
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103091 AND GoodsId = 10309100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103091,10309100,N'Famas G2 Commando PBTN')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103091) != 0) UPDATE PB_BT_SShopItem SET Name =N'Famas G2 Commando PBTN' WHERE ItemId = 103091
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103092 AND GoodsId = 10309200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103092,10309200,N'AK SOPMOD R')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103092) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK SOPMOD R' WHERE ItemId = 103092
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103093 AND GoodsId = 10309300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103093,10309300,N'AUG A3 Blaze')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103093) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 Blaze' WHERE ItemId = 103093
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103094 AND GoodsId = 10309400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103094,10309400,N'Scar-L Carbine')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103094) != 0) UPDATE PB_BT_SShopItem SET Name =N'Scar-L Carbine' WHERE ItemId = 103094
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103095 AND GoodsId = 10309500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103095,10309500,N'Scar-L Recon')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103095) != 0) UPDATE PB_BT_SShopItem SET Name =N'Scar-L Recon' WHERE ItemId = 103095
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103096 AND GoodsId = 10309600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103096,10309600,N'Scar-L F.C')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103096) != 0) UPDATE PB_BT_SShopItem SET Name =N'Scar-L F.C' WHERE ItemId = 103096
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103097 AND GoodsId = 10309700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103097,10309700,N'AUG A3 GRS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103097) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 GRS' WHERE ItemId = 103097
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103098 AND GoodsId = 10309800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103098,10309800,N'M4A1 GRS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103098) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4A1 GRS' WHERE ItemId = 103098
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103099 AND GoodsId = 10309900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103099,10309900,N'AK-SOPMOD GRS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103099) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK-SOPMOD GRS' WHERE ItemId = 103099
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103100 AND GoodsId = 10310000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103100,10310000,N'Famas G2 GRS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103100) != 0) UPDATE PB_BT_SShopItem SET Name =N'Famas G2 GRS' WHERE ItemId = 103100
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103101 AND GoodsId = 10310100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103101,10310100,N'AK SOPMOD PBNC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103101) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK SOPMOD PBNC' WHERE ItemId = 103101
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103102 AND GoodsId = 10310200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103102,10310200,N'HK417')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103102) != 0) UPDATE PB_BT_SShopItem SET Name =N'HK417' WHERE ItemId = 103102
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103103 AND GoodsId = 10310300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103103,10310300,N'M4A1 FC H-Dunter')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103103) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4A1 FC H-Dunter' WHERE ItemId = 103103
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103104 AND GoodsId = 10310400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103104,10310400,N'AUG A3 GSL')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103104) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 GSL' WHERE ItemId = 103104
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103105 AND GoodsId = 10310500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103105,10310500,N'FAMAS G2 Commando GSL')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103105) != 0) UPDATE PB_BT_SShopItem SET Name =N'FAMAS G2 Commando GSL' WHERE ItemId = 103105
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103106 AND GoodsId = 10310600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103106,10310600,N'AUG A3 ITALIA')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103106) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 ITALIA' WHERE ItemId = 103106
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103107 AND GoodsId = 10310700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103107,10310700,N'AUG A3 Black(P)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103107) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 Black(P)' WHERE ItemId = 103107
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103108 AND GoodsId = 10310800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103108,10310800,N'AUG A3 GRS EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103108) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 GRS EV' WHERE ItemId = 103108
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103109 AND GoodsId = 10310900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103109,10310900,N'AUG A3 TW1st')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103109) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 TW1st' WHERE ItemId = 103109
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103110 AND GoodsId = 10311000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103110,10311000,N'AUG A3 TW1st(P)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103110) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 TW1st(P)' WHERE ItemId = 103110
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103111 AND GoodsId = 10311100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103111,10311100,N'AUG A3 BR Camo')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103111) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 BR Camo' WHERE ItemId = 103111
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103112 AND GoodsId = 10311200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103112,10311200,N'AUG A3 Turkey')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103112) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 Turkey' WHERE ItemId = 103112
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103113 AND GoodsId = 10311300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103113,10311300,N'Famas G2 GRS EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103113) != 0) UPDATE PB_BT_SShopItem SET Name =N'Famas G2 GRS EV' WHERE ItemId = 103113
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103114 AND GoodsId = 10311400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103114,10311400,N'M4A1 Elite')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103114) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4A1 Elite' WHERE ItemId = 103114
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103115 AND GoodsId = 10311500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103115,10311500,N'AN-94')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103115) != 0) UPDATE PB_BT_SShopItem SET Name =N'AN-94' WHERE ItemId = 103115
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103116 AND GoodsId = 10311600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103116,10311600,N'F-2000 Reload')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103116) != 0) UPDATE PB_BT_SShopItem SET Name =N'F-2000 Reload' WHERE ItemId = 103116
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103117 AND GoodsId = 10311700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103117,10311700,N'SG-550 Reload')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103117) != 0) UPDATE PB_BT_SShopItem SET Name =N'SG-550 Reload' WHERE ItemId = 103117
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103118 AND GoodsId = 10311800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103118,10311800,N'SS2-V4 Para Sniper Reload')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103118) != 0) UPDATE PB_BT_SShopItem SET Name =N'SS2-V4 Para Sniper Reload' WHERE ItemId = 103118
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103119 AND GoodsId = 10311900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103119,10311900,N'AK-47 Elite')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103119) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK-47 Elite' WHERE ItemId = 103119
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103120 AND GoodsId = 10312000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103120,10312000,N'AUG A3 PBIC2013')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103120) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 PBIC2013' WHERE ItemId = 103120
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103121 AND GoodsId = 10312100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103121,10312100,N'M4A1 PBIC2013')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103121) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4A1 PBIC2013' WHERE ItemId = 103121
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103122 AND GoodsId = 10312200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103122,10312200,N'AK-47 PBIC2013')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103122) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK-47 PBIC2013' WHERE ItemId = 103122
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103123 AND GoodsId = 10312300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103123,10312300,N'TAR-21')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103123) != 0) UPDATE PB_BT_SShopItem SET Name =N'TAR-21' WHERE ItemId = 103123
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103124 AND GoodsId = 10312400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103124,10312400,N'AK47 Gold(P)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103124) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK47 Gold(P)' WHERE ItemId = 103124
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103125 AND GoodsId = 10312500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103125,10312500,N'AK47 FC(P)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103125) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK47 FC(P)' WHERE ItemId = 103125
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103126 AND GoodsId = 10312600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103126,10312600,N'AK SOPMOD KingCobra(P)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103126) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK SOPMOD KingCobra(P)' WHERE ItemId = 103126
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103127 AND GoodsId = 10312700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103127,10312700,N'M4A1 SE(P)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103127) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4A1 SE(P)' WHERE ItemId = 103127
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103128 AND GoodsId = 10312800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103128,10312800,N'AUG A3 Azerbaijan')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103128) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 Azerbaijan' WHERE ItemId = 103128
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103129 AND GoodsId = 10312900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103129,10312900,N'AUG A3 BLOODY')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103129) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 BLOODY' WHERE ItemId = 103129
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103130 AND GoodsId = 10313000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103130,10313000,N'AUG A3 BLOODY GOLD')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103130) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 BLOODY GOLD' WHERE ItemId = 103130
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103131 AND GoodsId = 10313100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103131,10313100,N'AUG A3 Latin3')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103131) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 Latin3' WHERE ItemId = 103131
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103142 AND GoodsId = 10314200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103142,10314200,N'AUG A3 TH 1Year')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103142) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 TH 1Year' WHERE ItemId = 103142
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103143 AND GoodsId = 10314300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103143,10314300,N'FG42')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103143) != 0) UPDATE PB_BT_SShopItem SET Name =N'FG42' WHERE ItemId = 103143
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103144 AND GoodsId = 10314400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103144,10314400,N'AUG A3 R')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103144) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 R' WHERE ItemId = 103144
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103145 AND GoodsId = 10314500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103145,10314500,N'FG42 Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103145) != 0) UPDATE PB_BT_SShopItem SET Name =N'FG42 Gold' WHERE ItemId = 103145
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103146 AND GoodsId = 10314600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103146,10314600,N'G36 Elite')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103146) != 0) UPDATE PB_BT_SShopItem SET Name =N'G36 Elite' WHERE ItemId = 103146
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103147 AND GoodsId = 10314700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103147,10314700,N'AUG A3 Inferno')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103147) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 Inferno' WHERE ItemId = 103147
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103148 AND GoodsId = 10314800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103148,10314800,N'AUG A3 PBNC5')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103148) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 PBNC5' WHERE ItemId = 103148
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103149 AND GoodsId = 10314900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103149,10314900,N'AUG A3 GSL2014')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103149) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 GSL2014' WHERE ItemId = 103149
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103150 AND GoodsId = 10315000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103150,10315000,N'TAR-21 Mid-Night')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103150) != 0) UPDATE PB_BT_SShopItem SET Name =N'TAR-21 Mid-Night' WHERE ItemId = 103150
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103151 AND GoodsId = 10315100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103151,10315100,N'TAR-21 BR Camo(+15% Exp)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103151) != 0) UPDATE PB_BT_SShopItem SET Name =N'TAR-21 BR Camo(+15% Exp)' WHERE ItemId = 103151
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103152 AND GoodsId = 10315200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103152,10315200,N'AK SOPMOD BR Camo')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103152) != 0) UPDATE PB_BT_SShopItem SET Name =N'AK SOPMOD BR Camo' WHERE ItemId = 103152
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103153 AND GoodsId = 10315300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103153,10315300,N'SC 2010')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103153) != 0) UPDATE PB_BT_SShopItem SET Name =N'SC 2010' WHERE ItemId = 103153
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103154 AND GoodsId = 10315400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103154,10315400,N'SC 2010 Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103154) != 0) UPDATE PB_BT_SShopItem SET Name =N'SC 2010 Gold' WHERE ItemId = 103154
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103155 AND GoodsId = 10315500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103155,10315500,N'AUG A3 Brazuca')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103155) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 Brazuca' WHERE ItemId = 103155
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103156 AND GoodsId = 10315600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103156,10315600,N'Pindad SS2-V4 Gold GW')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103156) != 0) UPDATE PB_BT_SShopItem SET Name =N'Pindad SS2-V4 Gold GW' WHERE ItemId = 103156
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103157 AND GoodsId = 10315700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103157,10315700,N'AUG A3 Champion')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103157) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 Champion' WHERE ItemId = 103157
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103158 AND GoodsId = 10315800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103158,10315800,N'AUG A3 W.O.E')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103158) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 W.O.E' WHERE ItemId = 103158
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103159 AND GoodsId = 10315900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103159,10315900,N'AUG A3 PBIC2014')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103159) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 PBIC2014' WHERE ItemId = 103159
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103160 AND GoodsId = 10316000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103160,10316000,N'AUG A3 4th Anniversary')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103160) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 4th Anniversary' WHERE ItemId = 103160
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103161 AND GoodsId = 10316100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103161,10316100,N'AUG A3 NoLogo Latin3')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103161) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 NoLogo Latin3' WHERE ItemId = 103161
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103162 AND GoodsId = 10316200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103162,10316200,N'TAR-21 NoLogo Mid-night')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103162) != 0) UPDATE PB_BT_SShopItem SET Name =N'TAR-21 NoLogo Mid-night' WHERE ItemId = 103162
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103163 AND GoodsId = 10316300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103163,10316300,N'AUG A3 PC-Cafe')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103163) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 PC-Cafe' WHERE ItemId = 103163
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103164 AND GoodsId = 10316400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103164,10316400,N'AUG A3 G E-Sport')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103164) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 G E-Sport' WHERE ItemId = 103164
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103165 AND GoodsId = 10316500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103165,10316500,N'AUG A3 Toy')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103165) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 Toy' WHERE ItemId = 103165
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103166 AND GoodsId = 10316600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103166,10316600,N'SC 2010 Q')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103166) != 0) UPDATE PB_BT_SShopItem SET Name =N'SC 2010 Q' WHERE ItemId = 103166
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103167 AND GoodsId = 10316700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103167,10316700,N'AN-94 Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103167) != 0) UPDATE PB_BT_SShopItem SET Name =N'AN-94 Gold' WHERE ItemId = 103167
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103168 AND GoodsId = 10316800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103168,10316800,N'HK-417 Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103168) != 0) UPDATE PB_BT_SShopItem SET Name =N'HK-417 Gold' WHERE ItemId = 103168
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103169 AND GoodsId = 10316900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103169,10316900,N'TAR-21 Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103169) != 0) UPDATE PB_BT_SShopItem SET Name =N'TAR-21 Gold' WHERE ItemId = 103169
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103170 AND GoodsId = 10317000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103170,10317000,N'SCAR-L Carbine Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103170) != 0) UPDATE PB_BT_SShopItem SET Name =N'SCAR-L Carbine Gold' WHERE ItemId = 103170
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103171 AND GoodsId = 10317100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103171,10317100,N'AUG A3 Latin4')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103171) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 Latin4' WHERE ItemId = 103171
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103172 AND GoodsId = 10317200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103172,10317200,N'SnowBall Blaster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103172) != 0) UPDATE PB_BT_SShopItem SET Name =N'SnowBall Blaster' WHERE ItemId = 103172
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103173 AND GoodsId = 10317300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103173,10317300,N'AUG A3 Cangaceiro')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103173) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 Cangaceiro' WHERE ItemId = 103173
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103174 AND GoodsId = 10317400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103174,10317400,N'XM8')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103174) != 0) UPDATE PB_BT_SShopItem SET Name =N'XM8' WHERE ItemId = 103174
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103175 AND GoodsId = 10317500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103175,10317500,N'SCAR-L Carbine D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103175) != 0) UPDATE PB_BT_SShopItem SET Name =N'SCAR-L Carbine D' WHERE ItemId = 103175
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103176 AND GoodsId = 10317600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103176,10317600,N'SCAR-L Recon D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103176) != 0) UPDATE PB_BT_SShopItem SET Name =N'SCAR-L Recon D' WHERE ItemId = 103176
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103177 AND GoodsId = 10317700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103177,10317700,N'XM8 GOLD')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103177) != 0) UPDATE PB_BT_SShopItem SET Name =N'XM8 GOLD' WHERE ItemId = 103177
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103178 AND GoodsId = 10317800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103178,10317800,N'AUG A3 CoupleBreak')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103178) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 CoupleBreak' WHERE ItemId = 103178
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103180 AND GoodsId = 10318000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103180,10318000,N'AUG A3 CNY 2015')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103180) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 CNY 2015' WHERE ItemId = 103180
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103181 AND GoodsId = 10318100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103181,10318100,N'AUG A3 Sheep')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103181) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 Sheep' WHERE ItemId = 103181
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103182 AND GoodsId = 10318200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103182,10318200,N'Tar-21 Sheep')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103182) != 0) UPDATE PB_BT_SShopItem SET Name =N'Tar-21 Sheep' WHERE ItemId = 103182
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 103183 AND GoodsId = 10318300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103183,10318300,N'AUG A3 GRS2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 103183) != 0) UPDATE PB_BT_SShopItem SET Name =N'AUG A3 GRS2' WHERE ItemId = 103183
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104001 AND GoodsId = 10400100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104001,10400100,N'MP5K Ext')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104001) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP5K Ext' WHERE ItemId = 104001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104002 AND GoodsId = 10400200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104002,10400200,N'Spectre Ext')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104002) != 0) UPDATE PB_BT_SShopItem SET Name =N'Spectre Ext' WHERE ItemId = 104002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104003 AND GoodsId = 10400300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104003,10400300,N'K-1 Ext')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104003) != 0) UPDATE PB_BT_SShopItem SET Name =N'K-1 Ext' WHERE ItemId = 104003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104004 AND GoodsId = 10400400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104004,10400400,N'MP7 Ext')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104004) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP7 Ext' WHERE ItemId = 104004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104006 AND GoodsId = 10400600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104006,10400600,N'K-1')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104006) != 0) UPDATE PB_BT_SShopItem SET Name =N'K-1' WHERE ItemId = 104006
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104007 AND GoodsId = 10400700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104007,10400700,N'MP5K G.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104007) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP5K G.' WHERE ItemId = 104007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104008 AND GoodsId = 10400800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104008,10400800,N'UMP45 Ext')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104008) != 0) UPDATE PB_BT_SShopItem SET Name =N'UMP45 Ext' WHERE ItemId = 104008
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104009 AND GoodsId = 10400900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104009,10400900,N'Spectre W.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104009) != 0) UPDATE PB_BT_SShopItem SET Name =N'Spectre W.' WHERE ItemId = 104009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104010 AND GoodsId = 10401000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104010,10401000,N'P90 M.C')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104010) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 M.C' WHERE ItemId = 104010
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104011 AND GoodsId = 10401100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104011,10401100,N'P90 Ext')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104011) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 Ext' WHERE ItemId = 104011
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104013 AND GoodsId = 10401300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104013,10401300,N'Kriss S.V')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104013) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V' WHERE ItemId = 104013
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104014 AND GoodsId = 10401400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104014,10401400,N'MP5K Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104014) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP5K Sl' WHERE ItemId = 104014
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104015 AND GoodsId = 10401500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104015,10401500,N'MP5K Wh.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104015) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP5K Wh.' WHERE ItemId = 104015
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104016 AND GoodsId = 10401600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104016,10401600,N'Spectre Wh.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104016) != 0) UPDATE PB_BT_SShopItem SET Name =N'Spectre Wh.' WHERE ItemId = 104016
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104020 AND GoodsId = 10402000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104020,10402000,N'Spectre Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104020) != 0) UPDATE PB_BT_SShopItem SET Name =N'Spectre Sl' WHERE ItemId = 104020
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104021 AND GoodsId = 10402100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104021,10402100,N'K-1 Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104021) != 0) UPDATE PB_BT_SShopItem SET Name =N'K-1 Sl' WHERE ItemId = 104021
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104022 AND GoodsId = 10402200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104022,10402200,N'MP7 Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104022) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP7 Sl' WHERE ItemId = 104022
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104023 AND GoodsId = 10402300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104023,10402300,N'UMP45 Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104023) != 0) UPDATE PB_BT_SShopItem SET Name =N'UMP45 Sl' WHERE ItemId = 104023
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104024 AND GoodsId = 10402400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104024,10402400,N'MP5K G D.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104024) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP5K G D.' WHERE ItemId = 104024
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104025 AND GoodsId = 10402500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104025,10402500,N'Spectre W D.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104025) != 0) UPDATE PB_BT_SShopItem SET Name =N'Spectre W D.' WHERE ItemId = 104025
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104026 AND GoodsId = 10402600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104026,10402600,N'Kriss S.V G.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104026) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V G.' WHERE ItemId = 104026
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104027 AND GoodsId = 10402700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104027,10402700,N'P90 M.C. S.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104027) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 M.C. S.' WHERE ItemId = 104027
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104029 AND GoodsId = 10402900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104029,10402900,N'P90 Ext D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104029) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 Ext D' WHERE ItemId = 104029
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104031 AND GoodsId = 10403100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104031,10403100,N'Kriss S.V D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104031) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V D' WHERE ItemId = 104031
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104032 AND GoodsId = 10403200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104032,10403200,N'P90 M.C D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104032) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 M.C D' WHERE ItemId = 104032
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104033 AND GoodsId = 10403300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104033,10403300,N'AKS74U Ext')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104033) != 0) UPDATE PB_BT_SShopItem SET Name =N'AKS74U Ext' WHERE ItemId = 104033
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104034 AND GoodsId = 10403400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104034,10403400,N'M4 CQB-R LV1')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104034) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4 CQB-R LV1' WHERE ItemId = 104034
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104035 AND GoodsId = 10403500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104035,10403500,N'M4 CQB-R LV2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104035) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4 CQB-R LV2' WHERE ItemId = 104035
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104036 AND GoodsId = 10403600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104036,10403600,N'M4 CQB-R LV3')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104036) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4 CQB-R LV3' WHERE ItemId = 104036
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104038 AND GoodsId = 10403800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104038,10403800,N'MP7 S.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104038) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP7 S.' WHERE ItemId = 104038
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104039 AND GoodsId = 10403900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104039,10403900,N'Kriss S.V Black')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104039) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V Black' WHERE ItemId = 104039
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104041 AND GoodsId = 10404100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104041,10404100,N'MP5K Sl D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104041) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP5K Sl D' WHERE ItemId = 104041
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104042 AND GoodsId = 10404200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104042,10404200,N'Spectre Sl D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104042) != 0) UPDATE PB_BT_SShopItem SET Name =N'Spectre Sl D' WHERE ItemId = 104042
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104043 AND GoodsId = 10404300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104043,10404300,N'SS1-R5 Carbine')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104043) != 0) UPDATE PB_BT_SShopItem SET Name =N'SS1-R5 Carbine' WHERE ItemId = 104043
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104045 AND GoodsId = 10404500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104045,10404500,N'MP7 Ext D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104045) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP7 Ext D' WHERE ItemId = 104045
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104046 AND GoodsId = 10404600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104046,10404600,N'UMP45 Ext D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104046) != 0) UPDATE PB_BT_SShopItem SET Name =N'UMP45 Ext D' WHERE ItemId = 104046
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104047 AND GoodsId = 10404700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104047,10404700,N'Kriss S.V Batik D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104047) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V Batik D' WHERE ItemId = 104047
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104049 AND GoodsId = 10404900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104049,10404900,N'SS1-R5 Carbine G')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104049) != 0) UPDATE PB_BT_SShopItem SET Name =N'SS1-R5 Carbine G' WHERE ItemId = 104049
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104050 AND GoodsId = 10405000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104050,10405000,N'Kriss S.V E-Sport')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104050) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V E-Sport' WHERE ItemId = 104050
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104052 AND GoodsId = 10405200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104052,10405200,N'Kriss S.V Batik')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104052) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V Batik' WHERE ItemId = 104052
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104054 AND GoodsId = 10405400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104054,10405400,N'Kriss S.V Vector')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104054) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V Vector' WHERE ItemId = 104054
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104056 AND GoodsId = 10405600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104056,10405600,N'MP7')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104056) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP7' WHERE ItemId = 104056
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104057 AND GoodsId = 10405700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104057,10405700,N'Spectre')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104057) != 0) UPDATE PB_BT_SShopItem SET Name =N'Spectre' WHERE ItemId = 104057
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104058 AND GoodsId = 10405800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104058,10405800,N'UMP45 Black')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104058) != 0) UPDATE PB_BT_SShopItem SET Name =N'UMP45 Black' WHERE ItemId = 104058
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104059 AND GoodsId = 10405900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104059,10405900,N'MP7 Special2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104059) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP7 Special2' WHERE ItemId = 104059
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104060 AND GoodsId = 10406000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104060,10406000,N'Kriss S.V PBIC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104060) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V PBIC' WHERE ItemId = 104060
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104062 AND GoodsId = 10406200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104062,10406200,N'Spectre Wh. EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104062) != 0) UPDATE PB_BT_SShopItem SET Name =N'Spectre Wh. EV' WHERE ItemId = 104062
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104063 AND GoodsId = 10406300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104063,10406300,N'MP5K G. EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104063) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP5K G. EV' WHERE ItemId = 104063
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104064 AND GoodsId = 10406400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104064,10406400,N'P90 M.C. S. EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104064) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 M.C. S. EV' WHERE ItemId = 104064
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104065 AND GoodsId = 10406500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104065,10406500,N'Kriss S.V EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104065) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V EV' WHERE ItemId = 104065
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104066 AND GoodsId = 10406600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104066,10406600,N'MP7 Ext EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104066) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP7 Ext EV' WHERE ItemId = 104066
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104067 AND GoodsId = 10406700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104067,10406700,N'MP5K Sl EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104067) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP5K Sl EV' WHERE ItemId = 104067
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104068 AND GoodsId = 10406800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104068,10406800,N'UMP45 Sl EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104068) != 0) UPDATE PB_BT_SShopItem SET Name =N'UMP45 Sl EV' WHERE ItemId = 104068
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104069 AND GoodsId = 10406900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104069,10406900,N'P90 Ext EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104069) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 Ext EV' WHERE ItemId = 104069
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104070 AND GoodsId = 10407000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104070,10407000,N'M4 CQB-R LV1 EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104070) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4 CQB-R LV1 EV' WHERE ItemId = 104070
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104071 AND GoodsId = 10407100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104071,10407100,N'M4 CQB-R LV2 EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104071) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4 CQB-R LV2 EV' WHERE ItemId = 104071
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104072 AND GoodsId = 10407200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104072,10407200,N'M4 CQB-R LV3 EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104072) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4 CQB-R LV3 EV' WHERE ItemId = 104072
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104074 AND GoodsId = 10407400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104074,10407400,N'Spectre Sl EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104074) != 0) UPDATE PB_BT_SShopItem SET Name =N'Spectre Sl EV' WHERE ItemId = 104074
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104075 AND GoodsId = 10407500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104075,10407500,N'P90 G')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104075) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 G' WHERE ItemId = 104075
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104076 AND GoodsId = 10407600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104076,10407600,N'AKS74U 100M')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104076) != 0) UPDATE PB_BT_SShopItem SET Name =N'AKS74U 100M' WHERE ItemId = 104076
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104077 AND GoodsId = 10407700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104077,10407700,N'P90 Wh(PBSC)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104077) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 Wh(PBSC)' WHERE ItemId = 104077
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104078 AND GoodsId = 10407800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104078,10407800,N'P90 Ext PBTN')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104078) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 Ext PBTN' WHERE ItemId = 104078
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104079 AND GoodsId = 10407900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104079,10407900,N'Kriss S.V PBTN')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104079) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V PBTN' WHERE ItemId = 104079
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104081 AND GoodsId = 10408100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104081,10408100,N'MP7 GRS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104081) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP7 GRS' WHERE ItemId = 104081
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104082 AND GoodsId = 10408200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104082,10408200,N'P90 MC GRS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104082) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 MC GRS' WHERE ItemId = 104082
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104083 AND GoodsId = 10408300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104083,10408300,N'Kriss S.V GRS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104083) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V GRS' WHERE ItemId = 104083
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104085 AND GoodsId = 10408500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104085,10408500,N'P90 Newborn')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104085) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 Newborn' WHERE ItemId = 104085
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104086 AND GoodsId = 10408600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104086,10408600,N'P90 M.C PBNC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104086) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 M.C PBNC' WHERE ItemId = 104086
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104087 AND GoodsId = 10408700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104087,10408700,N'Kriss S.V GSL')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104087) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V GSL' WHERE ItemId = 104087
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104089 AND GoodsId = 10408900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104089,10408900,N'P90 GSL')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104089) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 GSL' WHERE ItemId = 104089
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104090 AND GoodsId = 10409000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104090,10409000,N'Kriss SV Black(P)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104090) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss SV Black(P)' WHERE ItemId = 104090
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104092 AND GoodsId = 10409200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104092,10409200,N'UMP45 Black(P)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104092) != 0) UPDATE PB_BT_SShopItem SET Name =N'UMP45 Black(P)' WHERE ItemId = 104092
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104093 AND GoodsId = 10409300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104093,10409300,N'Kriss S.V GRS EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104093) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V GRS EV' WHERE ItemId = 104093
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104095 AND GoodsId = 10409500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104095,10409500,N'P90 MC GRS EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104095) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 MC GRS EV' WHERE ItemId = 104095
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104096 AND GoodsId = 10409600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104096,10409600,N'AKS74u Reload')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104096) != 0) UPDATE PB_BT_SShopItem SET Name =N'AKS74u Reload' WHERE ItemId = 104096
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104097 AND GoodsId = 10409700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104097,10409700,N'UMP-45 ext Reload')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104097) != 0) UPDATE PB_BT_SShopItem SET Name =N'UMP-45 ext Reload' WHERE ItemId = 104097
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104098 AND GoodsId = 10409800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104098,10409800,N'Spectre Reload')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104098) != 0) UPDATE PB_BT_SShopItem SET Name =N'Spectre Reload' WHERE ItemId = 104098
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104099 AND GoodsId = 10409900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104099,10409900,N'Pidad SS1-R5 Reload')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104099) != 0) UPDATE PB_BT_SShopItem SET Name =N'Pidad SS1-R5 Reload' WHERE ItemId = 104099
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104100 AND GoodsId = 10410000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104100,10410000,N'MP5K Reload')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104100) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP5K Reload' WHERE ItemId = 104100
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104101 AND GoodsId = 10410100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104101,10410100,N'KrissSuperV 4th Anniversary')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104101) != 0) UPDATE PB_BT_SShopItem SET Name =N'KrissSuperV 4th Anniversary' WHERE ItemId = 104101
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104103 AND GoodsId = 10410300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104103,10410300,N'KrissSV PBIC2013')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104103) != 0) UPDATE PB_BT_SShopItem SET Name =N'KrissSV PBIC2013' WHERE ItemId = 104103
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104105 AND GoodsId = 10410500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104105,10410500,N'MP7 GRS EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104105) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP7 GRS EV' WHERE ItemId = 104105
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104106 AND GoodsId = 10410600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104106,10410600,N'PP19-Bizon')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104106) != 0) UPDATE PB_BT_SShopItem SET Name =N'PP19-Bizon' WHERE ItemId = 104106
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104107 AND GoodsId = 10410700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104107,10410700,N'MP9 EXT')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104107) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP9 EXT' WHERE ItemId = 104107
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104108 AND GoodsId = 10410800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104108,10410800,N'Kriss S.V Silence')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104108) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V Silence' WHERE ItemId = 104108
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104110 AND GoodsId = 10411000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104110,10411000,N'Kriss S.V Turkey')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104110) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V Turkey' WHERE ItemId = 104110
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104112 AND GoodsId = 10411200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104112,10411200,N'P90 BLOODY')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104112) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 BLOODY' WHERE ItemId = 104112
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104113 AND GoodsId = 10411300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104113,10411300,N'P90 MC Latin3')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104113) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 MC Latin3' WHERE ItemId = 104113
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104114 AND GoodsId = 10411400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104114,10411400,N'P90 BR Camo')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104114) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 BR Camo' WHERE ItemId = 104114
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104121 AND GoodsId = 10412100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104121,10412100,N'Kriss S.V TH 1Year')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104121) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V TH 1Year' WHERE ItemId = 104121
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104123 AND GoodsId = 10412300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104123,10412300,N'Kriss S.V R')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104123) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V R' WHERE ItemId = 104123
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104125 AND GoodsId = 10412500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104125,10412500,N'P90 Newborn 2014')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104125) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 Newborn 2014' WHERE ItemId = 104125
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104126 AND GoodsId = 10412600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104126,10412600,N'Kriss S.V Inferno')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104126) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V Inferno' WHERE ItemId = 104126
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104128 AND GoodsId = 10412800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104128,10412800,N'P90 Inferno')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104128) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 Inferno' WHERE ItemId = 104128
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104129 AND GoodsId = 10412900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104129,10412900,N'P90 Ext PBNC5')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104129) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 Ext PBNC5' WHERE ItemId = 104129
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104130 AND GoodsId = 10413000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104130,10413000,N'Kriss S.V GSL2014')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104130) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V GSL2014' WHERE ItemId = 104130
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104132 AND GoodsId = 10413200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104132,10413200,N'Kriss S.V Mid-Night')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104132) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V Mid-Night' WHERE ItemId = 104132
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104134 AND GoodsId = 10413400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104134,10413400,N'OA-93')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104134) != 0) UPDATE PB_BT_SShopItem SET Name =N'OA-93' WHERE ItemId = 104134
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104136 AND GoodsId = 10413600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104136,10413600,N'OA-93 Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104136) != 0) UPDATE PB_BT_SShopItem SET Name =N'OA-93 Gold' WHERE ItemId = 104136
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104138 AND GoodsId = 10413800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104138,10413800,N'P90 Brazuca')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104138) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 Brazuca' WHERE ItemId = 104138
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104139 AND GoodsId = 10413900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104139,10413900,N'Kriss S.V Brazuca')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104139) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V Brazuca' WHERE ItemId = 104139
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104141 AND GoodsId = 10414100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104141,10414100,N'P90 Ext 5th Anniversary')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104141) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 Ext 5th Anniversary' WHERE ItemId = 104141
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104142 AND GoodsId = 10414200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104142,10414200,N'Kriss S.V Champion')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104142) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V Champion' WHERE ItemId = 104142
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104144 AND GoodsId = 10414400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104144,10414400,N'Kriss S.V W.O.E')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104144) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V W.O.E' WHERE ItemId = 104144
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104146 AND GoodsId = 10414600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104146,10414600,N'P90 Ext PBIC2014')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104146) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 Ext PBIC2014' WHERE ItemId = 104146
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104147 AND GoodsId = 10414700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104147,10414700,N'P90 NoLogo PBSC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104147) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 NoLogo PBSC' WHERE ItemId = 104147
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104148 AND GoodsId = 10414800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104148,10414800,N'P90 MC NoLogo Latin3')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104148) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 MC NoLogo Latin3' WHERE ItemId = 104148
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104149 AND GoodsId = 10414900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104149,10414900,N'KrissS.V NoLogo M-night')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104149) != 0) UPDATE PB_BT_SShopItem SET Name =N'KrissS.V NoLogo M-night' WHERE ItemId = 104149
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104151 AND GoodsId = 10415100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104151,10415100,N'Kriss S.V PC-Cafe')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104151) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V PC-Cafe' WHERE ItemId = 104151
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104153 AND GoodsId = 10415300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104153,10415300,N'Kriss S.V PBSC2014')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104153) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V PBSC2014' WHERE ItemId = 104153
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104155 AND GoodsId = 10415500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104155,10415500,N'Kriss S.V G E-Sport')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104155) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V G E-Sport' WHERE ItemId = 104155
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104157 AND GoodsId = 10415700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104157,10415700,N'Kriss S.V Toy')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104157) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V Toy' WHERE ItemId = 104157
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104159 AND GoodsId = 10415900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104159,10415900,N'OA-93 Q')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104159) != 0) UPDATE PB_BT_SShopItem SET Name =N'OA-93 Q' WHERE ItemId = 104159
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104161 AND GoodsId = 10416100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104161,10416100,N'MP9 Ext. Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104161) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP9 Ext. Gold' WHERE ItemId = 104161
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104162 AND GoodsId = 10416200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104162,10416200,N'PP-19 Bizon Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104162) != 0) UPDATE PB_BT_SShopItem SET Name =N'PP-19 Bizon Gold' WHERE ItemId = 104162
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104163 AND GoodsId = 10416300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104163,10416300,N'P90 Ext Latin4')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104163) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 Ext Latin4' WHERE ItemId = 104163
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104164 AND GoodsId = 10416400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104164,10416400,N'MP9 Ext XMAS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104164) != 0) UPDATE PB_BT_SShopItem SET Name =N'MP9 Ext XMAS' WHERE ItemId = 104164
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104165 AND GoodsId = 10416500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104165,10416500,N'OA-93 XMAS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104165) != 0) UPDATE PB_BT_SShopItem SET Name =N'OA-93 XMAS' WHERE ItemId = 104165
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104167 AND GoodsId = 10416700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104167,10416700,N'PP-19 Bizon XMAS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104167) != 0) UPDATE PB_BT_SShopItem SET Name =N'PP-19 Bizon XMAS' WHERE ItemId = 104167
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104168 AND GoodsId = 10416800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104168,10416800,N'Kriss SuperV XMAS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104168) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss SuperV XMAS' WHERE ItemId = 104168
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104170 AND GoodsId = 10417000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104170,10417000,N'Kriss S.V CoupleBreaker')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104170) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V CoupleBreaker' WHERE ItemId = 104170
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104172 AND GoodsId = 10417200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104172,10417200,N'Kriss S.V CNY 2015')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104172) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V CNY 2015' WHERE ItemId = 104172
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104174 AND GoodsId = 10417400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104174,10417400,N'P90 M.C Sheep')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104174) != 0) UPDATE PB_BT_SShopItem SET Name =N'P90 M.C Sheep' WHERE ItemId = 104174
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 104175 AND GoodsId = 10417500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104175,10417500,N'Kriss S.V GRS2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 104175) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V GRS2' WHERE ItemId = 104175
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105001 AND GoodsId = 10500100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105001,10500100,N'Dragunov')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105001) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dragunov' WHERE ItemId = 105001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105002 AND GoodsId = 10500200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105002,10500200,N'PSG1')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105002) != 0) UPDATE PB_BT_SShopItem SET Name =N'PSG1' WHERE ItemId = 105002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105003 AND GoodsId = 10500300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105003,10500300,N'SSG 69')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105003) != 0) UPDATE PB_BT_SShopItem SET Name =N'SSG 69' WHERE ItemId = 105003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105004 AND GoodsId = 10500400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105004,10500400,N'SSG-69 S.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105004) != 0) UPDATE PB_BT_SShopItem SET Name =N'SSG-69 S.' WHERE ItemId = 105004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105005 AND GoodsId = 10500500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105005,10500500,N'L115A1')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105005) != 0) UPDATE PB_BT_SShopItem SET Name =N'L115A1' WHERE ItemId = 105005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105006 AND GoodsId = 10500600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105006,10500600,N'Dragunov G.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105006) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dragunov G.' WHERE ItemId = 105006
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105007 AND GoodsId = 10500700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105007,10500700,N'PSG1 S.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105007) != 0) UPDATE PB_BT_SShopItem SET Name =N'PSG1 S.' WHERE ItemId = 105007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105008 AND GoodsId = 10500800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105008,10500800,N'SSG 69 Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105008) != 0) UPDATE PB_BT_SShopItem SET Name =N'SSG 69 Sl' WHERE ItemId = 105008
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105009 AND GoodsId = 10500900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105009,10500900,N'PSG1 Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105009) != 0) UPDATE PB_BT_SShopItem SET Name =N'PSG1 Sl' WHERE ItemId = 105009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105010 AND GoodsId = 10501000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105010,10501000,N'Dragunov Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105010) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dragunov Sl' WHERE ItemId = 105010
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105011 AND GoodsId = 10501100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105011,10501100,N'Dragunov CS.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105011) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dragunov CS.' WHERE ItemId = 105011
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105012 AND GoodsId = 10501200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105012,10501200,N'Dragunov CG.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105012) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dragunov CG.' WHERE ItemId = 105012
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105014 AND GoodsId = 10501400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105014,10501400,N'Dragunov G D.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105014) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dragunov G D.' WHERE ItemId = 105014
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105015 AND GoodsId = 10501500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105015,10501500,N'L115A1 Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105015) != 0) UPDATE PB_BT_SShopItem SET Name =N'L115A1 Gold' WHERE ItemId = 105015
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105016 AND GoodsId = 10501600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105016,10501600,N'PSG1 S. D.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105016) != 0) UPDATE PB_BT_SShopItem SET Name =N'PSG1 S. D.' WHERE ItemId = 105016
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105017 AND GoodsId = 10501700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105017,10501700,N'L115A1 D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105017) != 0) UPDATE PB_BT_SShopItem SET Name =N'L115A1 D' WHERE ItemId = 105017
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105018 AND GoodsId = 10501800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105018,10501800,N'M4 SPR LV1')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105018) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4 SPR LV1' WHERE ItemId = 105018
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105019 AND GoodsId = 10501900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105019,10501900,N'M4 SPR LV2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105019) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4 SPR LV2' WHERE ItemId = 105019
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105020 AND GoodsId = 10502000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105020,10502000,N'M4 SPR LV3')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105020) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4 SPR LV3' WHERE ItemId = 105020
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105021 AND GoodsId = 10502100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105021,10502100,N'Rangemaster .338')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105021) != 0) UPDATE PB_BT_SShopItem SET Name =N'Rangemaster .338' WHERE ItemId = 105021
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105022 AND GoodsId = 10502200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105022,10502200,N'Rangemaster 7.62')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105022) != 0) UPDATE PB_BT_SShopItem SET Name =N'Rangemaster 7.62' WHERE ItemId = 105022
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105023 AND GoodsId = 10502300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105023,10502300,N'Rangemaster 7.62 STBY')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105023) != 0) UPDATE PB_BT_SShopItem SET Name =N'Rangemaster 7.62 STBY' WHERE ItemId = 105023
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105024 AND GoodsId = 10502400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105024,10502400,N'PSG1 G')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105024) != 0) UPDATE PB_BT_SShopItem SET Name =N'PSG1 G' WHERE ItemId = 105024
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105026 AND GoodsId = 10502600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105026,10502600,N'L115A1 Black')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105026) != 0) UPDATE PB_BT_SShopItem SET Name =N'L115A1 Black' WHERE ItemId = 105026
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105027 AND GoodsId = 10502700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105027,10502700,N'SSG 69 Sl D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105027) != 0) UPDATE PB_BT_SShopItem SET Name =N'SSG 69 Sl D' WHERE ItemId = 105027
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105028 AND GoodsId = 10502800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105028,10502800,N'Dragunov D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105028) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dragunov D' WHERE ItemId = 105028
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105029 AND GoodsId = 10502900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105029,10502900,N'VSK94')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105029) != 0) UPDATE PB_BT_SShopItem SET Name =N'VSK94' WHERE ItemId = 105029
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105030 AND GoodsId = 10503000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105030,10503000,N'Cheytac M200')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105030) != 0) UPDATE PB_BT_SShopItem SET Name =N'Cheytac M200' WHERE ItemId = 105030
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105031 AND GoodsId = 10503100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105031,10503100,N'Winchester M70')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105031) != 0) UPDATE PB_BT_SShopItem SET Name =N'Winchester M70' WHERE ItemId = 105031
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105032 AND GoodsId = 10503200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105032,10503200,N'Barrett M82A1')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105032) != 0) UPDATE PB_BT_SShopItem SET Name =N'Barrett M82A1' WHERE ItemId = 105032
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105033 AND GoodsId = 10503300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105033,10503300,N'L115A1 E-Sport')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105033) != 0) UPDATE PB_BT_SShopItem SET Name =N'L115A1 E-Sport' WHERE ItemId = 105033
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105034 AND GoodsId = 10503400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105034,10503400,N'DSR-1')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105034) != 0) UPDATE PB_BT_SShopItem SET Name =N'DSR-1' WHERE ItemId = 105034
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105035 AND GoodsId = 10503500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105035,10503500,N'SVU')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105035) != 0) UPDATE PB_BT_SShopItem SET Name =N'SVU' WHERE ItemId = 105035
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105036 AND GoodsId = 10503600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105036,10503600,N'L115A1 S')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105036) != 0) UPDATE PB_BT_SShopItem SET Name =N'L115A1 S' WHERE ItemId = 105036
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105037 AND GoodsId = 10503700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105037,10503700,N'Scar-H LB Sniper')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105037) != 0) UPDATE PB_BT_SShopItem SET Name =N'Scar-H LB Sniper' WHERE ItemId = 105037
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105038 AND GoodsId = 10503800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105038,10503800,N'PSG1 S. EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105038) != 0) UPDATE PB_BT_SShopItem SET Name =N'PSG1 S. EV' WHERE ItemId = 105038
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105039 AND GoodsId = 10503900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105039,10503900,N'Dragunov G. EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105039) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dragunov G. EV' WHERE ItemId = 105039
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105040 AND GoodsId = 10504000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105040,10504000,N'Dragunov Sl EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105040) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dragunov Sl EV' WHERE ItemId = 105040
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105041 AND GoodsId = 10504100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105041,10504100,N'L115A1 EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105041) != 0) UPDATE PB_BT_SShopItem SET Name =N'L115A1 EV' WHERE ItemId = 105041
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105042 AND GoodsId = 10504200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105042,10504200,N'PSG1 Sl EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105042) != 0) UPDATE PB_BT_SShopItem SET Name =N'PSG1 Sl EV' WHERE ItemId = 105042
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105043 AND GoodsId = 10504300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105043,10504300,N'SSG 69 Sl EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105043) != 0) UPDATE PB_BT_SShopItem SET Name =N'SSG 69 Sl EV' WHERE ItemId = 105043
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105044 AND GoodsId = 10504400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105044,10504400,N'M4 SPR Lv1 EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105044) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4 SPR Lv1 EV' WHERE ItemId = 105044
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105045 AND GoodsId = 10504500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105045,10504500,N'M4 SPR Lv2 EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105045) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4 SPR Lv2 EV' WHERE ItemId = 105045
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105046 AND GoodsId = 10504600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105046,10504600,N'M4 SPR Lv3 EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105046) != 0) UPDATE PB_BT_SShopItem SET Name =N'M4 SPR Lv3 EV' WHERE ItemId = 105046
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105047 AND GoodsId = 10504700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105047,10504700,N'Dragunov Red')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105047) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dragunov Red' WHERE ItemId = 105047
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105048 AND GoodsId = 10504800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105048,10504800,N'VSK94 100M')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105048) != 0) UPDATE PB_BT_SShopItem SET Name =N'VSK94 100M' WHERE ItemId = 105048
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105049 AND GoodsId = 10504900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105049,10504900,N'Dragunov CG(P)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105049) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dragunov CG(P)' WHERE ItemId = 105049
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105050 AND GoodsId = 10505000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105050,10505000,N'L115A1 PBTN')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105050) != 0) UPDATE PB_BT_SShopItem SET Name =N'L115A1 PBTN' WHERE ItemId = 105050
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105052 AND GoodsId = 10505200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105052,10505200,N'Cheytec M200 GRS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105052) != 0) UPDATE PB_BT_SShopItem SET Name =N'Cheytec M200 GRS' WHERE ItemId = 105052
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105053 AND GoodsId = 10505300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105053,10505300,N'L115A1 PBNC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105053) != 0) UPDATE PB_BT_SShopItem SET Name =N'L115A1 PBNC' WHERE ItemId = 105053
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105054 AND GoodsId = 10505400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105054,10505400,N'SVU S')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105054) != 0) UPDATE PB_BT_SShopItem SET Name =N'SVU S' WHERE ItemId = 105054
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105055 AND GoodsId = 10505500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105055,10505500,N'CheyTac M200 GSL')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105055) != 0) UPDATE PB_BT_SShopItem SET Name =N'CheyTac M200 GSL' WHERE ItemId = 105055
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105056 AND GoodsId = 10505600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105056,10505600,N'L115A1 Black(P)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105056) != 0) UPDATE PB_BT_SShopItem SET Name =N'L115A1 Black(P)' WHERE ItemId = 105056
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105057 AND GoodsId = 10505700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105057,10505700,N'CheyTac M200 Bloody')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105057) != 0) UPDATE PB_BT_SShopItem SET Name =N'CheyTac M200 Bloody' WHERE ItemId = 105057
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105058 AND GoodsId = 10505800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105058,10505800,N'Cheytac M200 GRS EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105058) != 0) UPDATE PB_BT_SShopItem SET Name =N'Cheytac M200 GRS EV' WHERE ItemId = 105058
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105059 AND GoodsId = 10505900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105059,10505900,N'PSG1 Reload')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105059) != 0) UPDATE PB_BT_SShopItem SET Name =N'PSG1 Reload' WHERE ItemId = 105059
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105063 AND GoodsId = 10506300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105063,10506300,N'CheyTac M200 Turkey')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105063) != 0) UPDATE PB_BT_SShopItem SET Name =N'CheyTac M200 Turkey' WHERE ItemId = 105063
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105064 AND GoodsId = 10506400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105064,10506400,N'L115A1 Latin3')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105064) != 0) UPDATE PB_BT_SShopItem SET Name =N'L115A1 Latin3' WHERE ItemId = 105064
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105065 AND GoodsId = 10506500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105065,10506500,N'L115A1 BR CAMO')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105065) != 0) UPDATE PB_BT_SShopItem SET Name =N'L115A1 BR CAMO' WHERE ItemId = 105065
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105068 AND GoodsId = 10506800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105068,10506800,N'XM2010')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105068) != 0) UPDATE PB_BT_SShopItem SET Name =N'XM2010' WHERE ItemId = 105068
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105071 AND GoodsId = 10507100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105071,10507100,N'Cheytac M200 TH 1Year')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105071) != 0) UPDATE PB_BT_SShopItem SET Name =N'Cheytac M200 TH 1Year' WHERE ItemId = 105071
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105072 AND GoodsId = 10507200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105072,10507200,N'Walther')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105072) != 0) UPDATE PB_BT_SShopItem SET Name =N'Walther' WHERE ItemId = 105072
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105073 AND GoodsId = 10507300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105073,10507300,N'Cheytec M200 R')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105073) != 0) UPDATE PB_BT_SShopItem SET Name =N'Cheytec M200 R' WHERE ItemId = 105073
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105074 AND GoodsId = 10507400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105074,10507400,N'Walther Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105074) != 0) UPDATE PB_BT_SShopItem SET Name =N'Walther Gold' WHERE ItemId = 105074
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105075 AND GoodsId = 10507500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105075,10507500,N'PSG1 Elite')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105075) != 0) UPDATE PB_BT_SShopItem SET Name =N'PSG1 Elite' WHERE ItemId = 105075
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105076 AND GoodsId = 10507600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105076,10507600,N'Dragunov Elite')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105076) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dragunov Elite' WHERE ItemId = 105076
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105077 AND GoodsId = 10507700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105077,10507700,N'SVU Elite')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105077) != 0) UPDATE PB_BT_SShopItem SET Name =N'SVU Elite' WHERE ItemId = 105077
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105078 AND GoodsId = 10507800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105078,10507800,N'VSK Elite')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105078) != 0) UPDATE PB_BT_SShopItem SET Name =N'VSK Elite' WHERE ItemId = 105078
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105079 AND GoodsId = 10507900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105079,10507900,N'CheyTac M200 Inferno')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105079) != 0) UPDATE PB_BT_SShopItem SET Name =N'CheyTac M200 Inferno' WHERE ItemId = 105079
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105080 AND GoodsId = 10508000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105080,10508000,N'Cheytac M200 PBNC5')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105080) != 0) UPDATE PB_BT_SShopItem SET Name =N'Cheytac M200 PBNC5' WHERE ItemId = 105080
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105081 AND GoodsId = 10508100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105081,10508100,N'L115A1 GSL2014')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105081) != 0) UPDATE PB_BT_SShopItem SET Name =N'L115A1 GSL2014' WHERE ItemId = 105081
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105082 AND GoodsId = 10508200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105082,10508200,N'Barrett M82A1(P)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105082) != 0) UPDATE PB_BT_SShopItem SET Name =N'Barrett M82A1(P)' WHERE ItemId = 105082
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105083 AND GoodsId = 10508300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105083,10508300,N'Cheytac M200 Brazuca')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105083) != 0) UPDATE PB_BT_SShopItem SET Name =N'Cheytac M200 Brazuca' WHERE ItemId = 105083
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105084 AND GoodsId = 10508400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105084,10508400,N'Cheytac M200 Champion')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105084) != 0) UPDATE PB_BT_SShopItem SET Name =N'Cheytac M200 Champion' WHERE ItemId = 105084
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105085 AND GoodsId = 10508500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105085,10508500,N'Dragunov W.O.E')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105085) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dragunov W.O.E' WHERE ItemId = 105085
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105086 AND GoodsId = 10508600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105086,10508600,N'Cheytac M200 W.O.E')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105086) != 0) UPDATE PB_BT_SShopItem SET Name =N'Cheytac M200 W.O.E' WHERE ItemId = 105086
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105087 AND GoodsId = 10508700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105087,10508700,N'Cheytac M200 PBIC2014')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105087) != 0) UPDATE PB_BT_SShopItem SET Name =N'Cheytac M200 PBIC2014' WHERE ItemId = 105087
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105088 AND GoodsId = 10508800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105088,10508800,N'L115A1 NoLogo Latin3')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105088) != 0) UPDATE PB_BT_SShopItem SET Name =N'L115A1 NoLogo Latin3' WHERE ItemId = 105088
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105089 AND GoodsId = 10508900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105089,10508900,N'L115A1 PC-Cafe')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105089) != 0) UPDATE PB_BT_SShopItem SET Name =N'L115A1 PC-Cafe' WHERE ItemId = 105089
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105090 AND GoodsId = 10509000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105090,10509000,N'Cheytac M200 Merdeka')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105090) != 0) UPDATE PB_BT_SShopItem SET Name =N'Cheytac M200 Merdeka' WHERE ItemId = 105090
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105091 AND GoodsId = 10509100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105091,10509100,N'Cheytac M200 G E-Sport')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105091) != 0) UPDATE PB_BT_SShopItem SET Name =N'Cheytac M200 G E-Sport' WHERE ItemId = 105091
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105092 AND GoodsId = 10509200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105092,10509200,N'L115A1 Toy')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105092) != 0) UPDATE PB_BT_SShopItem SET Name =N'L115A1 Toy' WHERE ItemId = 105092
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105093 AND GoodsId = 10509300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105093,10509300,N'VSK94 Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105093) != 0) UPDATE PB_BT_SShopItem SET Name =N'VSK94 Gold' WHERE ItemId = 105093
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105094 AND GoodsId = 10509400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105094,10509400,N'SVU Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105094) != 0) UPDATE PB_BT_SShopItem SET Name =N'SVU Gold' WHERE ItemId = 105094
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105095 AND GoodsId = 10509500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105095,10509500,N'DSR-1 Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105095) != 0) UPDATE PB_BT_SShopItem SET Name =N'DSR-1 Gold' WHERE ItemId = 105095
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105096 AND GoodsId = 10509600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105096,10509600,N'Cheytac M200 Latin4')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105096) != 0) UPDATE PB_BT_SShopItem SET Name =N'Cheytac M200 Latin4' WHERE ItemId = 105096
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105097 AND GoodsId = 10509700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105097,10509700,N'Cheytac M200 Cangaceiro')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105097) != 0) UPDATE PB_BT_SShopItem SET Name =N'Cheytac M200 Cangaceiro' WHERE ItemId = 105097
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105098 AND GoodsId = 10509800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105098,10509800,N'DSR-1 D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105098) != 0) UPDATE PB_BT_SShopItem SET Name =N'DSR-1 D' WHERE ItemId = 105098
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105099 AND GoodsId = 10509900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105099,10509900,N'Cheytac CoupleBreaker')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105099) != 0) UPDATE PB_BT_SShopItem SET Name =N'Cheytac CoupleBreaker' WHERE ItemId = 105099
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 105100 AND GoodsId = 10510000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105100,10510000,N'CheyTac M200 GRS2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 105100) != 0) UPDATE PB_BT_SShopItem SET Name =N'CheyTac M200 GRS2' WHERE ItemId = 105100
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106001 AND GoodsId = 10600100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106001,10600100,N'870MCS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106001) != 0) UPDATE PB_BT_SShopItem SET Name =N'870MCS' WHERE ItemId = 106001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106002 AND GoodsId = 10600200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106002,10600200,N'870MCS Event')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106002) != 0) UPDATE PB_BT_SShopItem SET Name =N'870MCS Event' WHERE ItemId = 106002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106003 AND GoodsId = 10600300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106003,10600300,N'SPAS-15')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106003) != 0) UPDATE PB_BT_SShopItem SET Name =N'SPAS-15' WHERE ItemId = 106003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106004 AND GoodsId = 10600400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106004,10600400,N'870MCS W.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106004) != 0) UPDATE PB_BT_SShopItem SET Name =N'870MCS W.' WHERE ItemId = 106004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106005 AND GoodsId = 10600500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106005,10600500,N'M1887')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106005) != 0) UPDATE PB_BT_SShopItem SET Name =N'M1887' WHERE ItemId = 106005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106006 AND GoodsId = 10600600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106006,10600600,N'SPAS 15 Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106006) != 0) UPDATE PB_BT_SShopItem SET Name =N'SPAS 15 Sl' WHERE ItemId = 106006
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106008 AND GoodsId = 10600800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106008,10600800,N'870MCS Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106008) != 0) UPDATE PB_BT_SShopItem SET Name =N'870MCS Sl' WHERE ItemId = 106008
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106009 AND GoodsId = 10600900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106009,10600900,N'SPAS 15 Sl+')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106009) != 0) UPDATE PB_BT_SShopItem SET Name =N'SPAS 15 Sl+' WHERE ItemId = 106009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106010 AND GoodsId = 10601000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106010,10601000,N'M1887 W.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106010) != 0) UPDATE PB_BT_SShopItem SET Name =N'M1887 W.' WHERE ItemId = 106010
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106011 AND GoodsId = 10601100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106011,10601100,N'870MCS W. D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106011) != 0) UPDATE PB_BT_SShopItem SET Name =N'870MCS W. D' WHERE ItemId = 106011
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106012 AND GoodsId = 10601200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106012,10601200,N'SPAS-15 D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106012) != 0) UPDATE PB_BT_SShopItem SET Name =N'SPAS-15 D' WHERE ItemId = 106012
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106013 AND GoodsId = 10601300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106013,10601300,N'M26 LSS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106013) != 0) UPDATE PB_BT_SShopItem SET Name =N'M26 LSS' WHERE ItemId = 106013
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106014 AND GoodsId = 10601400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106014,10601400,N'870MCS Sl D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106014) != 0) UPDATE PB_BT_SShopItem SET Name =N'870MCS Sl D' WHERE ItemId = 106014
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106015 AND GoodsId = 10601500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106015,10601500,N'870MCS D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106015) != 0) UPDATE PB_BT_SShopItem SET Name =N'870MCS D' WHERE ItemId = 106015
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106016 AND GoodsId = 10601600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106016,10601600,N'SPAS 15 Sl D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106016) != 0) UPDATE PB_BT_SShopItem SET Name =N'SPAS 15 Sl D' WHERE ItemId = 106016
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106017 AND GoodsId = 10601700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106017,10601700,N'M1887 D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106017) != 0) UPDATE PB_BT_SShopItem SET Name =N'M1887 D' WHERE ItemId = 106017
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106018 AND GoodsId = 10601800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106018,10601800,N'SPAS-15 MSC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106018) != 0) UPDATE PB_BT_SShopItem SET Name =N'SPAS-15 MSC' WHERE ItemId = 106018
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106019 AND GoodsId = 10601900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106019,10601900,N'JackHammer')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106019) != 0) UPDATE PB_BT_SShopItem SET Name =N'JackHammer' WHERE ItemId = 106019
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106020 AND GoodsId = 10602000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106020,10602000,N'Kel-Tec KSG-15')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106020) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kel-Tec KSG-15' WHERE ItemId = 106020
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106021 AND GoodsId = 10602100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106021,10602100,N'M1887 Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106021) != 0) UPDATE PB_BT_SShopItem SET Name =N'M1887 Sl' WHERE ItemId = 106021
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106022 AND GoodsId = 10602200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106022,10602200,N'SPAS-15 EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106022) != 0) UPDATE PB_BT_SShopItem SET Name =N'SPAS-15 EV' WHERE ItemId = 106022
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106023 AND GoodsId = 10602300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106023,10602300,N'870MCS W. EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106023) != 0) UPDATE PB_BT_SShopItem SET Name =N'870MCS W. EV' WHERE ItemId = 106023
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106024 AND GoodsId = 10602400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106024,10602400,N'SPAS-15 Sl. EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106024) != 0) UPDATE PB_BT_SShopItem SET Name =N'SPAS-15 Sl. EV' WHERE ItemId = 106024
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106026 AND GoodsId = 10602600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106026,10602600,N'M1887 W GRS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106026) != 0) UPDATE PB_BT_SShopItem SET Name =N'M1887 W GRS' WHERE ItemId = 106026
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106027 AND GoodsId = 10602700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106027,10602700,N'M1887 PBNC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106027) != 0) UPDATE PB_BT_SShopItem SET Name =N'M1887 PBNC' WHERE ItemId = 106027
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106028 AND GoodsId = 10602800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106028,10602800,N'M1887 W GRS EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106028) != 0) UPDATE PB_BT_SShopItem SET Name =N'M1887 W GRS EV' WHERE ItemId = 106028
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106029 AND GoodsId = 10602900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106029,10602900,N'SPAS-15 PBSC2013')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106029) != 0) UPDATE PB_BT_SShopItem SET Name =N'SPAS-15 PBSC2013' WHERE ItemId = 106029
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106030 AND GoodsId = 10603000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106030,10603000,N'M1887 BLOODY')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106030) != 0) UPDATE PB_BT_SShopItem SET Name =N'M1887 BLOODY' WHERE ItemId = 106030
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106032 AND GoodsId = 10603200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106032,10603200,N'SPAS-15 Elite')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106032) != 0) UPDATE PB_BT_SShopItem SET Name =N'SPAS-15 Elite' WHERE ItemId = 106032
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106033 AND GoodsId = 10603300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106033,10603300,N'M1887 TH 1Year')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106033) != 0) UPDATE PB_BT_SShopItem SET Name =N'M1887 TH 1Year' WHERE ItemId = 106033
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106034 AND GoodsId = 10603400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106034,10603400,N'M1887 R')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106034) != 0) UPDATE PB_BT_SShopItem SET Name =N'M1887 R' WHERE ItemId = 106034
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106035 AND GoodsId = 10603500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106035,10603500,N'M1887 PBNC5')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106035) != 0) UPDATE PB_BT_SShopItem SET Name =N'M1887 PBNC5' WHERE ItemId = 106035
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106036 AND GoodsId = 10603600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106036,10603600,N'Kel-Tec KSG GSL2014')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106036) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kel-Tec KSG GSL2014' WHERE ItemId = 106036
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106037 AND GoodsId = 10603700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106037,10603700,N'M1887 Brazuca')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106037) != 0) UPDATE PB_BT_SShopItem SET Name =N'M1887 Brazuca' WHERE ItemId = 106037
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106038 AND GoodsId = 10603800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106038,10603800,N'M1887 PBIC2014')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106038) != 0) UPDATE PB_BT_SShopItem SET Name =N'M1887 PBIC2014' WHERE ItemId = 106038
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106039 AND GoodsId = 10603900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106039,10603900,N'UTS-15')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106039) != 0) UPDATE PB_BT_SShopItem SET Name =N'UTS-15' WHERE ItemId = 106039
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106040 AND GoodsId = 10604000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106040,10604000,N'SPAS-15 NoLogo PBSC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106040) != 0) UPDATE PB_BT_SShopItem SET Name =N'SPAS-15 NoLogo PBSC' WHERE ItemId = 106040
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106041 AND GoodsId = 10604100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106041,10604100,N'M1887 LionHeart')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106041) != 0) UPDATE PB_BT_SShopItem SET Name =N'M1887 LionHeart' WHERE ItemId = 106041
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106042 AND GoodsId = 10604200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106042,10604200,N'Zombie Slayer')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106042) != 0) UPDATE PB_BT_SShopItem SET Name =N'Zombie Slayer' WHERE ItemId = 106042
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106043 AND GoodsId = 10604300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106043,10604300,N'Remington ETA')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106043) != 0) UPDATE PB_BT_SShopItem SET Name =N'Remington ETA' WHERE ItemId = 106043
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106044 AND GoodsId = 10604400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106044,10604400,N'Remington ETA G.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106044) != 0) UPDATE PB_BT_SShopItem SET Name =N'Remington ETA G.' WHERE ItemId = 106044
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106045 AND GoodsId = 10604500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106045,10604500,N'UTS-15 Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106045) != 0) UPDATE PB_BT_SShopItem SET Name =N'UTS-15 Gold' WHERE ItemId = 106045
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106046 AND GoodsId = 10604600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106046,10604600,N'UTS-15 D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106046) != 0) UPDATE PB_BT_SShopItem SET Name =N'UTS-15 D' WHERE ItemId = 106046
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106047 AND GoodsId = 10604700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106047,10604700,N'Cerberus Shotgun')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106047) != 0) UPDATE PB_BT_SShopItem SET Name =N'Cerberus Shotgun' WHERE ItemId = 106047
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 106048 AND GoodsId = 10604800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (106048,10604800,N'UTS-15 G.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 106048) != 0) UPDATE PB_BT_SShopItem SET Name =N'UTS-15 G.' WHERE ItemId = 106048
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 110001 AND GoodsId = 11000100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (110001,11000100,N'MK46 Ext')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110001) != 0) UPDATE PB_BT_SShopItem SET Name =N'MK46 Ext' WHERE ItemId = 110001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 110002 AND GoodsId = 11000200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (110002,11000200,N'MK.46 Sl.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110002) != 0) UPDATE PB_BT_SShopItem SET Name =N'MK.46 Sl.' WHERE ItemId = 110002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 110003 AND GoodsId = 11000300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (110003,11000300,N'MG RPD')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110003) != 0) UPDATE PB_BT_SShopItem SET Name =N'MG RPD' WHERE ItemId = 110003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 110004 AND GoodsId = 11000400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (110004,11000400,N'L86 LSW')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110004) != 0) UPDATE PB_BT_SShopItem SET Name =N'L86 LSW' WHERE ItemId = 110004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 110005 AND GoodsId = 11000500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (110005,11000500,N'MK46 Ext EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110005) != 0) UPDATE PB_BT_SShopItem SET Name =N'MK46 Ext EV' WHERE ItemId = 110005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 110006 AND GoodsId = 11000600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (110006,11000600,N'MG RPD 100M')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110006) != 0) UPDATE PB_BT_SShopItem SET Name =N'MG RPD 100M' WHERE ItemId = 110006
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 110007 AND GoodsId = 11000700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (110007,11000700,N'RPD Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110007) != 0) UPDATE PB_BT_SShopItem SET Name =N'RPD Sl' WHERE ItemId = 110007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 110008 AND GoodsId = 11000800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (110008,11000800,N'RPD EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110008) != 0) UPDATE PB_BT_SShopItem SET Name =N'RPD EV' WHERE ItemId = 110008
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 110009 AND GoodsId = 11000900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (110009,11000900,N'MK.46')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110009) != 0) UPDATE PB_BT_SShopItem SET Name =N'MK.46' WHERE ItemId = 110009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 110010 AND GoodsId = 11001000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (110010,11001000,N'L86LSW X-MAS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110010) != 0) UPDATE PB_BT_SShopItem SET Name =N'L86LSW X-MAS' WHERE ItemId = 110010
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 110011 AND GoodsId = 11001100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (110011,11001100,N'Ultimax 100')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110011) != 0) UPDATE PB_BT_SShopItem SET Name =N'Ultimax 100' WHERE ItemId = 110011
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 110012 AND GoodsId = 11001200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (110012,11001200,N'Ultimax 100 Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 110012) != 0) UPDATE PB_BT_SShopItem SET Name =N'Ultimax 100 Gold' WHERE ItemId = 110012
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 118004 AND GoodsId = 11800400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (118004,11800400,N'Dual UZI')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 118004) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual UZI' WHERE ItemId = 118004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 118005 AND GoodsId = 11800500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (118005,11800500,N'Dual UZI Mini')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 118005) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual UZI Mini' WHERE ItemId = 118005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 118006 AND GoodsId = 11800600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (118006,11800600,N'Dual UZI Micro')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 118006) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual UZI Micro' WHERE ItemId = 118006
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 118007 AND GoodsId = 11800700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (118007,11800700,N'Dual UZI Silencer')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 118007) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual UZI Silencer' WHERE ItemId = 118007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 118008 AND GoodsId = 11800800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (118008,11800800,N'Dual UZI Mini Silenver')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 118008) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual UZI Mini Silenver' WHERE ItemId = 118008
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 118009 AND GoodsId = 11800900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (118009,11800900,N'Dual UZI Micro Silencer')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 118009) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual UZI Micro Silencer' WHERE ItemId = 118009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 118011 AND GoodsId = 11801100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (118011,11801100,N'Dual UZI Micro Silencer Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 118011) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual UZI Micro Silencer Sl' WHERE ItemId = 118011
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 118013 AND GoodsId = 11801300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (118013,11801300,N'Dual UZI Mini Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 118013) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual UZI Mini Gold' WHERE ItemId = 118013
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202001 AND GoodsId = 20200100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202001,20200100,N'Desert Eagle')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202001) != 0) UPDATE PB_BT_SShopItem SET Name =N'Desert Eagle' WHERE ItemId = 202001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202002 AND GoodsId = 20200200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202002,20200200,N'MK23 Ext')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202002) != 0) UPDATE PB_BT_SShopItem SET Name =N'MK23 Ext' WHERE ItemId = 202002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202003 AND GoodsId = 20200300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202003,20200300,N'K-5')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202003) != 0) UPDATE PB_BT_SShopItem SET Name =N'K-5' WHERE ItemId = 202003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202005 AND GoodsId = 20200500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202005,20200500,N'D-Eagle S.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202005) != 0) UPDATE PB_BT_SShopItem SET Name =N'D-Eagle S.' WHERE ItemId = 202005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202007 AND GoodsId = 20200700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202007,20200700,N'C. Python')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202007) != 0) UPDATE PB_BT_SShopItem SET Name =N'C. Python' WHERE ItemId = 202007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202010 AND GoodsId = 20201000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202010,20201000,N'C. Python G.+')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202010) != 0) UPDATE PB_BT_SShopItem SET Name =N'C. Python G.+' WHERE ItemId = 202010
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202011 AND GoodsId = 20201100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202011,20201100,N'Glock 18')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202011) != 0) UPDATE PB_BT_SShopItem SET Name =N'Glock 18' WHERE ItemId = 202011
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202012 AND GoodsId = 20201200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202012,20201200,N'C. Python D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202012) != 0) UPDATE PB_BT_SShopItem SET Name =N'C. Python D' WHERE ItemId = 202012
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202013 AND GoodsId = 20201300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202013,20201300,N'R.B 454 SS2M')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202013) != 0) UPDATE PB_BT_SShopItem SET Name =N'R.B 454 SS2M' WHERE ItemId = 202013
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202014 AND GoodsId = 20201400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202014,20201400,N'R.B 454 SS5M')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202014) != 0) UPDATE PB_BT_SShopItem SET Name =N'R.B 454 SS5M' WHERE ItemId = 202014
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202015 AND GoodsId = 20201500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202015,20201500,N'R.B 454 SS8M')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202015) != 0) UPDATE PB_BT_SShopItem SET Name =N'R.B 454 SS8M' WHERE ItemId = 202015
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202016 AND GoodsId = 20201600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202016,20201600,N'R.B 454 SS8M+S')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202016) != 0) UPDATE PB_BT_SShopItem SET Name =N'R.B 454 SS8M+S' WHERE ItemId = 202016
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202017 AND GoodsId = 20201700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202017,20201700,N'C. Python G')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202017) != 0) UPDATE PB_BT_SShopItem SET Name =N'C. Python G' WHERE ItemId = 202017
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202018 AND GoodsId = 20201800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202018,20201800,N'C. Python G D.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202018) != 0) UPDATE PB_BT_SShopItem SET Name =N'C. Python G D.' WHERE ItemId = 202018
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202019 AND GoodsId = 20201900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202019,20201900,N'MK23 Ext D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202019) != 0) UPDATE PB_BT_SShopItem SET Name =N'MK23 Ext D' WHERE ItemId = 202019
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202020 AND GoodsId = 20202000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202020,20202000,N'C. Python P')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202020) != 0) UPDATE PB_BT_SShopItem SET Name =N'C. Python P' WHERE ItemId = 202020
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202021 AND GoodsId = 20202100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202021,20202100,N'Glock 18 D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202021) != 0) UPDATE PB_BT_SShopItem SET Name =N'Glock 18 D' WHERE ItemId = 202021
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202022 AND GoodsId = 20202200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202022,20202200,N'Colt45')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202022) != 0) UPDATE PB_BT_SShopItem SET Name =N'Colt45' WHERE ItemId = 202022
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202023 AND GoodsId = 20202300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202023,20202300,N'IMI UZI 9MM')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202023) != 0) UPDATE PB_BT_SShopItem SET Name =N'IMI UZI 9MM' WHERE ItemId = 202023
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202024 AND GoodsId = 20202400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202024,20202400,N'Kriss S.V Vector SDP')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202024) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kriss S.V Vector SDP' WHERE ItemId = 202024
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202026 AND GoodsId = 20202600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202026,20202600,N'HK 69')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202026) != 0) UPDATE PB_BT_SShopItem SET Name =N'HK 69' WHERE ItemId = 202026
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202027 AND GoodsId = 20202700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202027,20202700,N'MK.23 SI')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202027) != 0) UPDATE PB_BT_SShopItem SET Name =N'MK.23 SI' WHERE ItemId = 202027
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202028 AND GoodsId = 20202800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202028,20202800,N'M79')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202028) != 0) UPDATE PB_BT_SShopItem SET Name =N'M79' WHERE ItemId = 202028
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202029 AND GoodsId = 20202900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202029,20202900,N'GL06')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202029) != 0) UPDATE PB_BT_SShopItem SET Name =N'GL06' WHERE ItemId = 202029
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202030 AND GoodsId = 20203000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202030,20203000,N'Desert Eagle EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202030) != 0) UPDATE PB_BT_SShopItem SET Name =N'Desert Eagle EV' WHERE ItemId = 202030
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202031 AND GoodsId = 20203100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202031,20203100,N'Colt45 100M')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202031) != 0) UPDATE PB_BT_SShopItem SET Name =N'Colt45 100M' WHERE ItemId = 202031
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202033 AND GoodsId = 20203300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202033,20203300,N'Glock 18 PBNC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202033) != 0) UPDATE PB_BT_SShopItem SET Name =N'Glock 18 PBNC' WHERE ItemId = 202033
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202034 AND GoodsId = 20203400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202034,20203400,N'R.B 454 SS8M+S PBSC2013')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202034) != 0) UPDATE PB_BT_SShopItem SET Name =N'R.B 454 SS8M+S PBSC2013' WHERE ItemId = 202034
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202035 AND GoodsId = 20203500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202035,20203500,N'MK23 Reload')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202035) != 0) UPDATE PB_BT_SShopItem SET Name =N'MK23 Reload' WHERE ItemId = 202035
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202036 AND GoodsId = 20203600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202036,20203600,N'Desert Eegle Reload')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202036) != 0) UPDATE PB_BT_SShopItem SET Name =N'Desert Eegle Reload' WHERE ItemId = 202036
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202037 AND GoodsId = 20203700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202037,20203700,N'Glock 18 Turkey')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202037) != 0) UPDATE PB_BT_SShopItem SET Name =N'Glock 18 Turkey' WHERE ItemId = 202037
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202039 AND GoodsId = 20203900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202039,20203900,N'Glock 18 Azerbaijan')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202039) != 0) UPDATE PB_BT_SShopItem SET Name =N'Glock 18 Azerbaijan' WHERE ItemId = 202039
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202040 AND GoodsId = 20204000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202040,20204000,N'C. Python PBNC5')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202040) != 0) UPDATE PB_BT_SShopItem SET Name =N'C. Python PBNC5' WHERE ItemId = 202040
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202047 AND GoodsId = 20204700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202047,20204700,N'Glock 18 BR Camo')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202047) != 0) UPDATE PB_BT_SShopItem SET Name =N'Glock 18 BR Camo' WHERE ItemId = 202047
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202049 AND GoodsId = 20204900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202049,20204900,N'C. Pyhon Brazuca')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202049) != 0) UPDATE PB_BT_SShopItem SET Name =N'C. Pyhon Brazuca' WHERE ItemId = 202049
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202050 AND GoodsId = 20205000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202050,20205000,N'R.B454 SS8M+PBSC NoLogo')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202050) != 0) UPDATE PB_BT_SShopItem SET Name =N'R.B454 SS8M+PBSC NoLogo' WHERE ItemId = 202050
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202052 AND GoodsId = 20205200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202052,20205200,N'C. Python Toy')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202052) != 0) UPDATE PB_BT_SShopItem SET Name =N'C. Python Toy' WHERE ItemId = 202052
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202053 AND GoodsId = 20205300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202053,20205300,N'MGL-32')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202053) != 0) UPDATE PB_BT_SShopItem SET Name =N'MGL-32' WHERE ItemId = 202053
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 202054 AND GoodsId = 20205400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202054,20205400,N'GL-06 CNY 2015')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 202054) != 0) UPDATE PB_BT_SShopItem SET Name =N'GL-06 CNY 2015' WHERE ItemId = 202054
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 213001 AND GoodsId = 21300100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (213001,21300100,N'P99&HAK')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 213001) != 0) UPDATE PB_BT_SShopItem SET Name =N'P99&HAK' WHERE ItemId = 213001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 213002 AND GoodsId = 21300200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (213002,21300200,N'P99&HAK Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 213002) != 0) UPDATE PB_BT_SShopItem SET Name =N'P99&HAK Sl' WHERE ItemId = 213002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 213003 AND GoodsId = 21300300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (213003,21300300,N'P99&HAK D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 213003) != 0) UPDATE PB_BT_SShopItem SET Name =N'P99&HAK D' WHERE ItemId = 213003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 213004 AND GoodsId = 21300400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (213004,21300400,N'P99&HAK Sl D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 213004) != 0) UPDATE PB_BT_SShopItem SET Name =N'P99&HAK Sl D' WHERE ItemId = 213004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 213005 AND GoodsId = 21300500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (213005,21300500,N'P99&HAK EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 213005) != 0) UPDATE PB_BT_SShopItem SET Name =N'P99&HAK EV' WHERE ItemId = 213005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 213006 AND GoodsId = 21300600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (213006,21300600,N'P99&HAK D (GW)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 213006) != 0) UPDATE PB_BT_SShopItem SET Name =N'P99&HAK D (GW)' WHERE ItemId = 213006
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 213007 AND GoodsId = 21300700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (213007,21300700,N'P99&HAK Reload')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 213007) != 0) UPDATE PB_BT_SShopItem SET Name =N'P99&HAK Reload' WHERE ItemId = 213007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 214001 AND GoodsId = 21400100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214001,21400100,N'Dual Handgun')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214001) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual Handgun' WHERE ItemId = 214001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 214002 AND GoodsId = 21400200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214002,21400200,N'Dual Desert Eagle')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214002) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual Desert Eagle' WHERE ItemId = 214002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 214004 AND GoodsId = 21400400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214004,21400400,N'Dual Desert Eagle Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214004) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual Desert Eagle Gold' WHERE ItemId = 214004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 214005 AND GoodsId = 21400500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214005,21400500,N'Dual Handgun D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214005) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual Handgun D' WHERE ItemId = 214005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 214006 AND GoodsId = 21400600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214006,21400600,N'Dual Desert Eagle D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214006) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual Desert Eagle D' WHERE ItemId = 214006
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 214007 AND GoodsId = 21400700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214007,21400700,N'HK45 Dual')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214007) != 0) UPDATE PB_BT_SShopItem SET Name =N'HK45 Dual' WHERE ItemId = 214007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 214008 AND GoodsId = 21400800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214008,21400800,N'Dual HandGun EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214008) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual HandGun EV' WHERE ItemId = 214008
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 214009 AND GoodsId = 21400900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214009,21400900,N'Dual D-Eagle EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214009) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual D-Eagle EV' WHERE ItemId = 214009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 214010 AND GoodsId = 21401000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214010,21401000,N'Dual D-Eagle G (P)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214010) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual D-Eagle G (P)' WHERE ItemId = 214010
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 214011 AND GoodsId = 21401100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214011,21401100,N'Dual Desert Eagle GRS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214011) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual Desert Eagle GRS' WHERE ItemId = 214011
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 214012 AND GoodsId = 21401200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214012,21401200,N'HK45 Pumpkin')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214012) != 0) UPDATE PB_BT_SShopItem SET Name =N'HK45 Pumpkin' WHERE ItemId = 214012
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 214015 AND GoodsId = 21401500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214015,21401500,N'Dual D-Eagle BR Camo')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214015) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual D-Eagle BR Camo' WHERE ItemId = 214015
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 214016 AND GoodsId = 21401600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214016,21401600,N'Dual D-Eagle G E-Sport')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 214016) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual D-Eagle G E-Sport' WHERE ItemId = 214016
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 230001 AND GoodsId = 23000100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (230001,23000100,N'SuperShorty 870')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 230001) != 0) UPDATE PB_BT_SShopItem SET Name =N'SuperShorty 870' WHERE ItemId = 230001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 234001 AND GoodsId = 23400100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (234001,23400100,N'Compound Bow')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 234001) != 0) UPDATE PB_BT_SShopItem SET Name =N'Compound Bow' WHERE ItemId = 234001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 234002 AND GoodsId = 23400200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (234002,23400200,N'Compound Smoke')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 234002) != 0) UPDATE PB_BT_SShopItem SET Name =N'Compound Smoke' WHERE ItemId = 234002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 234003 AND GoodsId = 23400300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (234003,23400300,N'Compound Flashbang')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 234003) != 0) UPDATE PB_BT_SShopItem SET Name =N'Compound Flashbang' WHERE ItemId = 234003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 234004 AND GoodsId = 23400400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (234004,23400400,N'Compound WP smoke')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 234004) != 0) UPDATE PB_BT_SShopItem SET Name =N'Compound WP smoke' WHERE ItemId = 234004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 234005 AND GoodsId = 23400500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (234005,23400500,N'Compound Grenade')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 234005) != 0) UPDATE PB_BT_SShopItem SET Name =N'Compound Grenade' WHERE ItemId = 234005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301001 AND GoodsId = 30100100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301001,30100100,N'M-7')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301001) != 0) UPDATE PB_BT_SShopItem SET Name =N'M-7' WHERE ItemId = 301001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301004 AND GoodsId = 30100400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301004,30100400,N'Amok Kukri')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301004) != 0) UPDATE PB_BT_SShopItem SET Name =N'Amok Kukri' WHERE ItemId = 301004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301007 AND GoodsId = 30100700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301007,30100700,N'Mini Axe')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301007) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mini Axe' WHERE ItemId = 301007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301009 AND GoodsId = 30100900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301009,30100900,N'M-7 G.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301009) != 0) UPDATE PB_BT_SShopItem SET Name =N'M-7 G.' WHERE ItemId = 301009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301011 AND GoodsId = 30101100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301011,30101100,N'Amok Kukri D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301011) != 0) UPDATE PB_BT_SShopItem SET Name =N'Amok Kukri D' WHERE ItemId = 301011
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301012 AND GoodsId = 30101200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301012,30101200,N'Mini Axe D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301012) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mini Axe D' WHERE ItemId = 301012
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301014 AND GoodsId = 30101400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301014,30101400,N'Combat Machete')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301014) != 0) UPDATE PB_BT_SShopItem SET Name =N'Combat Machete' WHERE ItemId = 301014
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301017 AND GoodsId = 30101700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301017,30101700,N'Fang Blade')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301017) != 0) UPDATE PB_BT_SShopItem SET Name =N'Fang Blade' WHERE ItemId = 301017
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301018 AND GoodsId = 30101800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301018,30101800,N'Ballistic Knife')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301018) != 0) UPDATE PB_BT_SShopItem SET Name =N'Ballistic Knife' WHERE ItemId = 301018
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301021 AND GoodsId = 30102100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301021,30102100,N'Keris')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301021) != 0) UPDATE PB_BT_SShopItem SET Name =N'Keris' WHERE ItemId = 301021
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301023 AND GoodsId = 30102300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301023,30102300,N'Keris S.')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301023) != 0) UPDATE PB_BT_SShopItem SET Name =N'Keris S.' WHERE ItemId = 301023
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301024 AND GoodsId = 30102400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301024,30102400,N'Candy Cane')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301024) != 0) UPDATE PB_BT_SShopItem SET Name =N'Candy Cane' WHERE ItemId = 301024
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301025 AND GoodsId = 30102500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301025,30102500,N'Field Shovel')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301025) != 0) UPDATE PB_BT_SShopItem SET Name =N'Field Shovel' WHERE ItemId = 301025
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301026 AND GoodsId = 30102600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301026,30102600,N'Amok Kukri EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301026) != 0) UPDATE PB_BT_SShopItem SET Name =N'Amok Kukri EV' WHERE ItemId = 301026
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301027 AND GoodsId = 30102700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301027,30102700,N'Mini Axe EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301027) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mini Axe EV' WHERE ItemId = 301027
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301037 AND GoodsId = 30103700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301037,30103700,N'Ballistic Knife GRS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301037) != 0) UPDATE PB_BT_SShopItem SET Name =N'Ballistic Knife GRS' WHERE ItemId = 301037
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301041 AND GoodsId = 30104100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301041,30104100,N'Aribian Sword')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301041) != 0) UPDATE PB_BT_SShopItem SET Name =N'Aribian Sword' WHERE ItemId = 301041
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301042 AND GoodsId = 30104200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301042,30104200,N'M-9 PBNC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301042) != 0) UPDATE PB_BT_SShopItem SET Name =N'M-9 PBNC' WHERE ItemId = 301042
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301043 AND GoodsId = 30104300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301043,30104300,N'Combat Machete Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301043) != 0) UPDATE PB_BT_SShopItem SET Name =N'Combat Machete Gold' WHERE ItemId = 301043
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301044 AND GoodsId = 30104400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301044,30104400,N'Combat Machete PBSC2013')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301044) != 0) UPDATE PB_BT_SShopItem SET Name =N'Combat Machete PBSC2013' WHERE ItemId = 301044
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301046 AND GoodsId = 30104600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301046,30104600,N'Amok Kukri Turkey')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301046) != 0) UPDATE PB_BT_SShopItem SET Name =N'Amok Kukri Turkey' WHERE ItemId = 301046
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301047 AND GoodsId = 30104700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301047,30104700,N'Keris S. X-MAS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301047) != 0) UPDATE PB_BT_SShopItem SET Name =N'Keris S. X-MAS' WHERE ItemId = 301047
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301049 AND GoodsId = 30104900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301049,30104900,N'Arabian Sword2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301049) != 0) UPDATE PB_BT_SShopItem SET Name =N'Arabian Sword2' WHERE ItemId = 301049
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301050 AND GoodsId = 30105000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301050,30105000,N'Fang Blade PBNC5')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301050) != 0) UPDATE PB_BT_SShopItem SET Name =N'Fang Blade PBNC5' WHERE ItemId = 301050
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301051 AND GoodsId = 30105100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301051,30105100,N'Fang Blade GSL2014')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301051) != 0) UPDATE PB_BT_SShopItem SET Name =N'Fang Blade GSL2014' WHERE ItemId = 301051
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301052 AND GoodsId = 30105200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301052,30105200,N'Fang Blade Brazuca')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301052) != 0) UPDATE PB_BT_SShopItem SET Name =N'Fang Blade Brazuca' WHERE ItemId = 301052
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301057 AND GoodsId = 30105700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301057,30105700,N'Fang Blade Inferno')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301057) != 0) UPDATE PB_BT_SShopItem SET Name =N'Fang Blade Inferno' WHERE ItemId = 301057
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301058 AND GoodsId = 30105800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301058,30105800,N'ChineseCleaver')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301058) != 0) UPDATE PB_BT_SShopItem SET Name =N'ChineseCleaver' WHERE ItemId = 301058
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301059 AND GoodsId = 30105900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301059,30105900,N'GH5007 NoLogo PBSC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301059) != 0) UPDATE PB_BT_SShopItem SET Name =N'GH5007 NoLogo PBSC' WHERE ItemId = 301059
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301064 AND GoodsId = 30106400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301064,30106400,N'BadmintonRacket')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301064) != 0) UPDATE PB_BT_SShopItem SET Name =N'BadmintonRacket' WHERE ItemId = 301064
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301065 AND GoodsId = 30106500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301065,30106500,N'Keris G E-Sport')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301065) != 0) UPDATE PB_BT_SShopItem SET Name =N'Keris G E-Sport' WHERE ItemId = 301065
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301066 AND GoodsId = 30106600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301066,30106600,N'Death Scythe')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301066) != 0) UPDATE PB_BT_SShopItem SET Name =N'Death Scythe' WHERE ItemId = 301066
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301067 AND GoodsId = 30106700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301067,30106700,N'FangBlade Latin4')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301067) != 0) UPDATE PB_BT_SShopItem SET Name =N'FangBlade Latin4' WHERE ItemId = 301067
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301068 AND GoodsId = 30106800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301068,30106800,N'Icicle')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301068) != 0) UPDATE PB_BT_SShopItem SET Name =N'Icicle' WHERE ItemId = 301068
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301069 AND GoodsId = 30106900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301069,30106900,N'Ice Fork')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301069) != 0) UPDATE PB_BT_SShopItem SET Name =N'Ice Fork' WHERE ItemId = 301069
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301070 AND GoodsId = 30107000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301070,30107000,N'GH5007 Cangaceiro')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301070) != 0) UPDATE PB_BT_SShopItem SET Name =N'GH5007 Cangaceiro' WHERE ItemId = 301070
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301071 AND GoodsId = 30107100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301071,30107100,N'Goat hammer')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301071) != 0) UPDATE PB_BT_SShopItem SET Name =N'Goat hammer' WHERE ItemId = 301071
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 301073 AND GoodsId = 30107300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301073,30107300,N'Chinese Cleaver CNY')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 301073) != 0) UPDATE PB_BT_SShopItem SET Name =N'Chinese Cleaver CNY' WHERE ItemId = 301073
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 315001 AND GoodsId = 31500100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (315001,31500100,N'Dual Knife')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 315001) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual Knife' WHERE ItemId = 315001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 315002 AND GoodsId = 31500200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (315002,31500200,N'Dual Knife D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 315002) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual Knife D' WHERE ItemId = 315002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 315003 AND GoodsId = 31500300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (315003,31500300,N'Bone Knife')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 315003) != 0) UPDATE PB_BT_SShopItem SET Name =N'Bone Knife' WHERE ItemId = 315003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 315004 AND GoodsId = 31500400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (315004,31500400,N'Dual Knife EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 315004) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual Knife EV' WHERE ItemId = 315004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 315005 AND GoodsId = 31500500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (315005,31500500,N'Bone Knife GRS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 315005) != 0) UPDATE PB_BT_SShopItem SET Name =N'Bone Knife GRS' WHERE ItemId = 315005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 315006 AND GoodsId = 31500600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (315006,31500600,N'Dual Knife PBNC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 315006) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual Knife PBNC' WHERE ItemId = 315006
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 315007 AND GoodsId = 31500700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (315007,31500700,N'Boneknife GRS2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 315007) != 0) UPDATE PB_BT_SShopItem SET Name =N'Boneknife GRS2' WHERE ItemId = 315007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 323001 AND GoodsId = 32300100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (323001,32300100,N'Bare Fist')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 323001) != 0) UPDATE PB_BT_SShopItem SET Name =N'Bare Fist' WHERE ItemId = 323001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 323002 AND GoodsId = 32300200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (323002,32300200,N'Black Knuckle')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 323002) != 0) UPDATE PB_BT_SShopItem SET Name =N'Black Knuckle' WHERE ItemId = 323002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 323003 AND GoodsId = 32300300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (323003,32300300,N'Brass Knuckle')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 323003) != 0) UPDATE PB_BT_SShopItem SET Name =N'Brass Knuckle' WHERE ItemId = 323003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 323004 AND GoodsId = 32300400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (323004,32300400,N'Silver Knuckle')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 323004) != 0) UPDATE PB_BT_SShopItem SET Name =N'Silver Knuckle' WHERE ItemId = 323004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 323005 AND GoodsId = 32300500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (323005,32300500,N'Pumpkin Knuckle')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 323005) != 0) UPDATE PB_BT_SShopItem SET Name =N'Pumpkin Knuckle' WHERE ItemId = 323005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 323006 AND GoodsId = 32300600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (323006,32300600,N'Spiked Knuckle')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 323006) != 0) UPDATE PB_BT_SShopItem SET Name =N'Spiked Knuckle' WHERE ItemId = 323006
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 323007 AND GoodsId = 32300700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (323007,32300700,N'Ballock Knuckle')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 323007) != 0) UPDATE PB_BT_SShopItem SET Name =N'Ballock Knuckle' WHERE ItemId = 323007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 323009 AND GoodsId = 32300900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (323009,32300900,N'Garena Knuckle')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 323009) != 0) UPDATE PB_BT_SShopItem SET Name =N'Garena Knuckle' WHERE ItemId = 323009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407001 AND GoodsId = 40700100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407001,40700100,N'K-400')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407001) != 0) UPDATE PB_BT_SShopItem SET Name =N'K-400' WHERE ItemId = 407001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407002 AND GoodsId = 40700200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407002,40700200,N'C-5')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407002) != 0) UPDATE PB_BT_SShopItem SET Name =N'C-5' WHERE ItemId = 407002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407003 AND GoodsId = 40700300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407003,40700300,N'C-5 D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407003) != 0) UPDATE PB_BT_SShopItem SET Name =N'C-5 D' WHERE ItemId = 407003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407004 AND GoodsId = 40700400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407004,40700400,N'K-413 Dual Granade')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407004) != 0) UPDATE PB_BT_SShopItem SET Name =N'K-413 Dual Granade' WHERE ItemId = 407004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407005 AND GoodsId = 40700500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407005,40700500,N'Chocolate Grenade')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407005) != 0) UPDATE PB_BT_SShopItem SET Name =N'Chocolate Grenade' WHERE ItemId = 407005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407006 AND GoodsId = 40700600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407006,40700600,N'C-5 EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407006) != 0) UPDATE PB_BT_SShopItem SET Name =N'C-5 EV' WHERE ItemId = 407006
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407007 AND GoodsId = 40700700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407007,40700700,N'K-413 G')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407007) != 0) UPDATE PB_BT_SShopItem SET Name =N'K-413 G' WHERE ItemId = 407007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407008 AND GoodsId = 40700800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407008,40700800,N'Ketupat Grenade')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407008) != 0) UPDATE PB_BT_SShopItem SET Name =N'Ketupat Grenade' WHERE ItemId = 407008
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407009 AND GoodsId = 40700900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407009,40700900,N'Candy Grenade')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407009) != 0) UPDATE PB_BT_SShopItem SET Name =N'Candy Grenade' WHERE ItemId = 407009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407010 AND GoodsId = 40701000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407010,40701000,N'Lantern Grenade')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407010) != 0) UPDATE PB_BT_SShopItem SET Name =N'Lantern Grenade' WHERE ItemId = 407010
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407011 AND GoodsId = 40701100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407011,40701100,N'Ma`amoul Grenade')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407011) != 0) UPDATE PB_BT_SShopItem SET Name =N'Ma`amoul Grenade' WHERE ItemId = 407011
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407012 AND GoodsId = 40701200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407012,40701200,N'Decoy Bomb')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407012) != 0) UPDATE PB_BT_SShopItem SET Name =N'Decoy Bomb' WHERE ItemId = 407012
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407013 AND GoodsId = 40701300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407013,40701300,N'M14 Mine')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407013) != 0) UPDATE PB_BT_SShopItem SET Name =N'M14 Mine' WHERE ItemId = 407013
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407014 AND GoodsId = 40701400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407014,40701400,N'K479')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407014) != 0) UPDATE PB_BT_SShopItem SET Name =N'K479' WHERE ItemId = 407014
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407015 AND GoodsId = 40701500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407015,40701500,N'Easter Day Egg')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407015) != 0) UPDATE PB_BT_SShopItem SET Name =N'Easter Day Egg' WHERE ItemId = 407015
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407016 AND GoodsId = 40701600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407016,40701600,N'Football Bomb')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407016) != 0) UPDATE PB_BT_SShopItem SET Name =N'Football Bomb' WHERE ItemId = 407016
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407017 AND GoodsId = 40701700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407017,40701700,N'K-413 T')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407017) != 0) UPDATE PB_BT_SShopItem SET Name =N'K-413 T' WHERE ItemId = 407017
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407018 AND GoodsId = 40701800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407018,40701800,N'M14 Mine D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407018) != 0) UPDATE PB_BT_SShopItem SET Name =N'M14 Mine D' WHERE ItemId = 407018
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407019 AND GoodsId = 40701900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407019,40701900,N'K-413 Traditional')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407019) != 0) UPDATE PB_BT_SShopItem SET Name =N'K-413 Traditional' WHERE ItemId = 407019
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407020 AND GoodsId = 40702000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407020,40702000,N'Dual K-413 PC-Cafe')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407020) != 0) UPDATE PB_BT_SShopItem SET Name =N'Dual K-413 PC-Cafe' WHERE ItemId = 407020
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407021 AND GoodsId = 40702100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407021,40702100,N'Shuttlecock')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407021) != 0) UPDATE PB_BT_SShopItem SET Name =N'Shuttlecock' WHERE ItemId = 407021
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407022 AND GoodsId = 40702200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407022,40702200,N'SepaktakrawBall')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407022) != 0) UPDATE PB_BT_SShopItem SET Name =N'SepaktakrawBall' WHERE ItemId = 407022
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407023 AND GoodsId = 40702300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407023,40702300,N'M18A1 Claymore')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407023) != 0) UPDATE PB_BT_SShopItem SET Name =N'M18A1 Claymore' WHERE ItemId = 407023
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407024 AND GoodsId = 40702400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407024,40702400,N'K-413 GW')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407024) != 0) UPDATE PB_BT_SShopItem SET Name =N'K-413 GW' WHERE ItemId = 407024
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407025 AND GoodsId = 40702500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407025,40702500,N'K-400 Goat')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407025) != 0) UPDATE PB_BT_SShopItem SET Name =N'K-400 Goat' WHERE ItemId = 407025
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407044 AND GoodsId = 40704400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407044,40704400,N'C-5 CNY 2015')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407044) != 0) UPDATE PB_BT_SShopItem SET Name =N'C-5 CNY 2015' WHERE ItemId = 407044
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 407045 AND GoodsId = 40704500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407045,40704500,N'K-413 Chocolate')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 407045) != 0) UPDATE PB_BT_SShopItem SET Name =N'K-413 Chocolate' WHERE ItemId = 407045
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 508001 AND GoodsId = 50800100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (508001,50800100,N'Smoke')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 508001) != 0) UPDATE PB_BT_SShopItem SET Name =N'Smoke' WHERE ItemId = 508001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 508002 AND GoodsId = 50800200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (508002,50800200,N'Flash Bang')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 508002) != 0) UPDATE PB_BT_SShopItem SET Name =N'Flash Bang' WHERE ItemId = 508002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 508003 AND GoodsId = 50800300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (508003,50800300,N'Smoke Plus')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 508003) != 0) UPDATE PB_BT_SShopItem SET Name =N'Smoke Plus' WHERE ItemId = 508003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 508004 AND GoodsId = 50800400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (508004,50800400,N'Flash Bang Plus')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 508004) != 0) UPDATE PB_BT_SShopItem SET Name =N'Flash Bang Plus' WHERE ItemId = 508004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 527001 AND GoodsId = 52700100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (527001,52700100,N'WP Smoke')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 527001) != 0) UPDATE PB_BT_SShopItem SET Name =N'WP Smoke' WHERE ItemId = 527001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 527002 AND GoodsId = 52700200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (527002,52700200,N'WP Smoke D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 527002) != 0) UPDATE PB_BT_SShopItem SET Name =N'WP Smoke D' WHERE ItemId = 527002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 527003 AND GoodsId = 52700300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (527003,52700300,N'WP Smoke Plus')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 527003) != 0) UPDATE PB_BT_SShopItem SET Name =N'WP Smoke Plus' WHERE ItemId = 527003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 527004 AND GoodsId = 52700400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (527004,52700400,N'WP Smoke EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 527004) != 0) UPDATE PB_BT_SShopItem SET Name =N'WP Smoke EV' WHERE ItemId = 527004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 527005 AND GoodsId = 52700500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (527005,52700500,N'WP Smoke Plus P')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 527005) != 0) UPDATE PB_BT_SShopItem SET Name =N'WP Smoke Plus P' WHERE ItemId = 527005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 528001 AND GoodsId = 52800100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (528001,52800100,N'Medical Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 528001) != 0) UPDATE PB_BT_SShopItem SET Name =N'Medical Kit' WHERE ItemId = 528001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 528002 AND GoodsId = 52800200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (528002,52800200,N'Halloween Medical Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 528002) != 0) UPDATE PB_BT_SShopItem SET Name =N'Halloween Medical Kit' WHERE ItemId = 528002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 528003 AND GoodsId = 52800300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (528003,52800300,N'Chocolate Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 528003) != 0) UPDATE PB_BT_SShopItem SET Name =N'Chocolate Kit' WHERE ItemId = 528003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 528004 AND GoodsId = 52800400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (528004,52800400,N'Medical Kit Lotus')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 528004) != 0) UPDATE PB_BT_SShopItem SET Name =N'Medical Kit Lotus' WHERE ItemId = 528004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 528005 AND GoodsId = 52800500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (528005,52800500,N'Medical Kit Opor Ayam')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 528005) != 0) UPDATE PB_BT_SShopItem SET Name =N'Medical Kit Opor Ayam' WHERE ItemId = 528005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 528006 AND GoodsId = 52800600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (528006,52800600,N'Medicla Kit PBNC5')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 528006) != 0) UPDATE PB_BT_SShopItem SET Name =N'Medicla Kit PBNC5' WHERE ItemId = 528006
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 528007 AND GoodsId = 52800700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (528007,52800700,N'Easter Day Medical Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 528007) != 0) UPDATE PB_BT_SShopItem SET Name =N'Easter Day Medical Kit' WHERE ItemId = 528007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 528008 AND GoodsId = 52800800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (528008,52800800,N'Medical Kit Lotus2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 528008) != 0) UPDATE PB_BT_SShopItem SET Name =N'Medical Kit Lotus2' WHERE ItemId = 528008
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 528009 AND GoodsId = 52800900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (528009,52800900,N'Medical Kit Kurma')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 528009) != 0) UPDATE PB_BT_SShopItem SET Name =N'Medical Kit Kurma' WHERE ItemId = 528009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 601001 AND GoodsId = 60100100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (601001,60100100,N'Red Bulls')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601001) != 0) UPDATE PB_BT_SShopItem SET Name =N'Red Bulls' WHERE ItemId = 601001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 601003 AND GoodsId = 60100300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (601003,60100300,N'Tarantula')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601003) != 0) UPDATE PB_BT_SShopItem SET Name =N'Tarantula' WHERE ItemId = 601003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 601005 AND GoodsId = 60100500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (601005,60100500,N'Normal Viper Red')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601005) != 0) UPDATE PB_BT_SShopItem SET Name =N'Normal Viper Red' WHERE ItemId = 601005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 601007 AND GoodsId = 60100700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (601007,60100700,N'Normal D-fox')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601007) != 0) UPDATE PB_BT_SShopItem SET Name =N'Normal D-fox' WHERE ItemId = 601007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 601010 AND GoodsId = 60101000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (601010,60101000,N'Rica')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601010) != 0) UPDATE PB_BT_SShopItem SET Name =N'Rica' WHERE ItemId = 601010
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 601012 AND GoodsId = 60101200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (601012,60101200,N'D-fox exp +20%')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601012) != 0) UPDATE PB_BT_SShopItem SET Name =N'D-fox exp +20%' WHERE ItemId = 601012
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 601015 AND GoodsId = 60101500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (601015,60101500,N'Viper Red point +30%')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601015) != 0) UPDATE PB_BT_SShopItem SET Name =N'Viper Red point +30%' WHERE ItemId = 601015
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 601016 AND GoodsId = 60101600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (601016,60101600,N'Reinforced Red Bulls')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601016) != 0) UPDATE PB_BT_SShopItem SET Name =N'Reinforced Red Bulls' WHERE ItemId = 601016
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 601018 AND GoodsId = 60101800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (601018,60101800,N'Reinforced Tarantula')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601018) != 0) UPDATE PB_BT_SShopItem SET Name =N'Reinforced Tarantula' WHERE ItemId = 601018
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 601020 AND GoodsId = 60102000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (601020,60102000,N'Reinforced D-fox')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601020) != 0) UPDATE PB_BT_SShopItem SET Name =N'Reinforced D-fox' WHERE ItemId = 601020
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 601022 AND GoodsId = 60102200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (601022,60102200,N'Reinforced Viper Red')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601022) != 0) UPDATE PB_BT_SShopItem SET Name =N'Reinforced Viper Red' WHERE ItemId = 601022
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 601024 AND GoodsId = 60102400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (601024,60102400,N'Reinforced Rica')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601024) != 0) UPDATE PB_BT_SShopItem SET Name =N'Reinforced Rica' WHERE ItemId = 601024
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 601026 AND GoodsId = 60102600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (601026,60102600,N'Reinforced Combo D-Fox')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601026) != 0) UPDATE PB_BT_SShopItem SET Name =N'Reinforced Combo D-Fox' WHERE ItemId = 601026
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 601028 AND GoodsId = 60102800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (601028,60102800,N'Reinforced Combo Viper Red')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601028) != 0) UPDATE PB_BT_SShopItem SET Name =N'Reinforced Combo Viper Red' WHERE ItemId = 601028
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 601030 AND GoodsId = 60103000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (601030,60103000,N'REBEL Male GRS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601030) != 0) UPDATE PB_BT_SShopItem SET Name =N'REBEL Male GRS' WHERE ItemId = 601030
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 601032 AND GoodsId = 60103200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (601032,60103200,N'REBEL Female GRS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601032) != 0) UPDATE PB_BT_SShopItem SET Name =N'REBEL Female GRS' WHERE ItemId = 601032
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 601037 AND GoodsId = 60103700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (601037,60103700,N'2014 WC Tarantula')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601037) != 0) UPDATE PB_BT_SShopItem SET Name =N'2014 WC Tarantula' WHERE ItemId = 601037
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 601038 AND GoodsId = 60103800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (601038,60103800,N'Viper Red Shadow')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 601038) != 0) UPDATE PB_BT_SShopItem SET Name =N'Viper Red Shadow' WHERE ItemId = 601038
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 602002 AND GoodsId = 60200200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (602002,60200200,N'Acid Paul')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602002) != 0) UPDATE PB_BT_SShopItem SET Name =N'Acid Paul' WHERE ItemId = 602002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 602004 AND GoodsId = 60200400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (602004,60200400,N'Keen Eyes')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602004) != 0) UPDATE PB_BT_SShopItem SET Name =N'Keen Eyes' WHERE ItemId = 602004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 602008 AND GoodsId = 60200800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (602008,60200800,N'Normal Leopard')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602008) != 0) UPDATE PB_BT_SShopItem SET Name =N'Normal Leopard' WHERE ItemId = 602008
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 602009 AND GoodsId = 60200900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (602009,60200900,N'Normal Hide')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602009) != 0) UPDATE PB_BT_SShopItem SET Name =N'Normal Hide' WHERE ItemId = 602009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 602011 AND GoodsId = 60201100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (602011,60201100,N'Chou')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602011) != 0) UPDATE PB_BT_SShopItem SET Name =N'Chou' WHERE ItemId = 602011
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 602013 AND GoodsId = 60201300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (602013,60201300,N'Leopard Exp+20%')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602013) != 0) UPDATE PB_BT_SShopItem SET Name =N'Leopard Exp+20%' WHERE ItemId = 602013
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 602014 AND GoodsId = 60201400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (602014,60201400,N'Hide point +30%')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602014) != 0) UPDATE PB_BT_SShopItem SET Name =N'Hide point +30%' WHERE ItemId = 602014
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 602017 AND GoodsId = 60201700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (602017,60201700,N'Reinforced Acid Paul')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602017) != 0) UPDATE PB_BT_SShopItem SET Name =N'Reinforced Acid Paul' WHERE ItemId = 602017
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 602019 AND GoodsId = 60201900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (602019,60201900,N'Reinforced Keen Eyes')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602019) != 0) UPDATE PB_BT_SShopItem SET Name =N'Reinforced Keen Eyes' WHERE ItemId = 602019
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 602021 AND GoodsId = 60202100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (602021,60202100,N'Reinforced Leopard')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602021) != 0) UPDATE PB_BT_SShopItem SET Name =N'Reinforced Leopard' WHERE ItemId = 602021
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 602023 AND GoodsId = 60202300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (602023,60202300,N'Reinforced Hide')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602023) != 0) UPDATE PB_BT_SShopItem SET Name =N'Reinforced Hide' WHERE ItemId = 602023
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 602025 AND GoodsId = 60202500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (602025,60202500,N'Reinforced Chou')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602025) != 0) UPDATE PB_BT_SShopItem SET Name =N'Reinforced Chou' WHERE ItemId = 602025
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 602027 AND GoodsId = 60202700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (602027,60202700,N'Reinforced Combo Leopard')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602027) != 0) UPDATE PB_BT_SShopItem SET Name =N'Reinforced Combo Leopard' WHERE ItemId = 602027
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 602029 AND GoodsId = 60202900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (602029,60202900,N'Reinforced Combo Hide')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602029) != 0) UPDATE PB_BT_SShopItem SET Name =N'Reinforced Combo Hide' WHERE ItemId = 602029
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 602031 AND GoodsId = 60203100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (602031,60203100,N'SWAT Male GRS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602031) != 0) UPDATE PB_BT_SShopItem SET Name =N'SWAT Male GRS' WHERE ItemId = 602031
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 602033 AND GoodsId = 60203300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (602033,60203300,N'SWAT Female GRS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602033) != 0) UPDATE PB_BT_SShopItem SET Name =N'SWAT Female GRS' WHERE ItemId = 602033
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 602034 AND GoodsId = 60203400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (602034,60203400,N'Hide Kopassus')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602034) != 0) UPDATE PB_BT_SShopItem SET Name =N'Hide Kopassus' WHERE ItemId = 602034
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 602035 AND GoodsId = 60203500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (602035,60203500,N'Leopard Bope')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602035) != 0) UPDATE PB_BT_SShopItem SET Name =N'Leopard Bope' WHERE ItemId = 602035
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 602036 AND GoodsId = 60203600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (602036,60203600,N'2014 WC Hide')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602036) != 0) UPDATE PB_BT_SShopItem SET Name =N'2014 WC Hide' WHERE ItemId = 602036
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 602039 AND GoodsId = 60203900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (602039,60203900,N'Hide Recon')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 602039) != 0) UPDATE PB_BT_SShopItem SET Name =N'Hide Recon' WHERE ItemId = 602039
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700002 AND GoodsId = 70000200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700002,70000200,N'Acid Pol Headgear')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700002) != 0) UPDATE PB_BT_SShopItem SET Name =N'Acid Pol Headgear' WHERE ItemId = 700002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700005 AND GoodsId = 70000500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700005,70000500,N'Red Bulls Headgear')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700005) != 0) UPDATE PB_BT_SShopItem SET Name =N'Red Bulls Headgear' WHERE ItemId = 700005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700006 AND GoodsId = 70000600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700006,70000600,N'Tarantula Headgear')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700006) != 0) UPDATE PB_BT_SShopItem SET Name =N'Tarantula Headgear' WHERE ItemId = 700006
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700017 AND GoodsId = 70001700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700017,70001700,N'Keen Eyes Headgear')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700017) != 0) UPDATE PB_BT_SShopItem SET Name =N'Keen Eyes Headgear' WHERE ItemId = 700017
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700018 AND GoodsId = 70001800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700018,70001800,N'D-Fox Headgear')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700018) != 0) UPDATE PB_BT_SShopItem SET Name =N'D-Fox Headgear' WHERE ItemId = 700018
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700019 AND GoodsId = 70001900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700019,70001900,N'Leopard Headgear')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700019) != 0) UPDATE PB_BT_SShopItem SET Name =N'Leopard Headgear' WHERE ItemId = 700019
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700020 AND GoodsId = 70002000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700020,70002000,N'Viper Red Headgear')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700020) != 0) UPDATE PB_BT_SShopItem SET Name =N'Viper Red Headgear' WHERE ItemId = 700020
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700021 AND GoodsId = 70002100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700021,70002100,N'Hide Headgear')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700021) != 0) UPDATE PB_BT_SShopItem SET Name =N'Hide Headgear' WHERE ItemId = 700021
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700030 AND GoodsId = 70003000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700030,70003000,N'UDT Boonie hat')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700030) != 0) UPDATE PB_BT_SShopItem SET Name =N'UDT Boonie hat' WHERE ItemId = 700030
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700031 AND GoodsId = 70003100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700031,70003100,N'Beret Bope')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700031) != 0) UPDATE PB_BT_SShopItem SET Name =N'Beret Bope' WHERE ItemId = 700031
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700032 AND GoodsId = 70003200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700032,70003200,N'Marine Cap')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700032) != 0) UPDATE PB_BT_SShopItem SET Name =N'Marine Cap' WHERE ItemId = 700032
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700033 AND GoodsId = 70003300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700033,70003300,N'Pentagram Army Cap')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700033) != 0) UPDATE PB_BT_SShopItem SET Name =N'Pentagram Army Cap' WHERE ItemId = 700033
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700035 AND GoodsId = 70003500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700035,70003500,N'707 Unit Headgear')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700035) != 0) UPDATE PB_BT_SShopItem SET Name =N'707 Unit Headgear' WHERE ItemId = 700035
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700036 AND GoodsId = 70003600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700036,70003600,N'China Marine Headgear')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700036) != 0) UPDATE PB_BT_SShopItem SET Name =N'China Marine Headgear' WHERE ItemId = 700036
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700037 AND GoodsId = 70003700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700037,70003700,N'SanTa Hat')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700037) != 0) UPDATE PB_BT_SShopItem SET Name =N'SanTa Hat' WHERE ItemId = 700037
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700038 AND GoodsId = 70003800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700038,70003800,N'Pumpkin Hat')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700038) != 0) UPDATE PB_BT_SShopItem SET Name =N'Pumpkin Hat' WHERE ItemId = 700038
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700039 AND GoodsId = 70003900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700039,70003900,N'Mask Tiger')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700039) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Tiger' WHERE ItemId = 700039
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700040 AND GoodsId = 70004000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700040,70004000,N'Super Beret')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700040) != 0) UPDATE PB_BT_SShopItem SET Name =N'Super Beret' WHERE ItemId = 700040
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700047 AND GoodsId = 70004700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700047,70004700,N'Mask Chinese Hat2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700047) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Chinese Hat2' WHERE ItemId = 700047
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700048 AND GoodsId = 70004800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700048,70004800,N'Patrol Helmet')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700048) != 0) UPDATE PB_BT_SShopItem SET Name =N'Patrol Helmet' WHERE ItemId = 700048
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700101 AND GoodsId = 70010100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700101,70010100,N'Headgear Default')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700101) != 0) UPDATE PB_BT_SShopItem SET Name =N'Headgear Default' WHERE ItemId = 700101
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700102 AND GoodsId = 70010200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700102,70010200,N'Headgear Normal')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700102) != 0) UPDATE PB_BT_SShopItem SET Name =N'Headgear Normal' WHERE ItemId = 700102
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700103 AND GoodsId = 70010300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700103,70010300,N'Headgear Reinforce')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700103) != 0) UPDATE PB_BT_SShopItem SET Name =N'Headgear Reinforce' WHERE ItemId = 700103
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700104 AND GoodsId = 70010400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700104,70010400,N'Headgear Target Tracking')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700104) != 0) UPDATE PB_BT_SShopItem SET Name =N'Headgear Target Tracking' WHERE ItemId = 700104
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700105 AND GoodsId = 70010500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700105,70010500,N'Headgear Reinforced Plus')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700105) != 0) UPDATE PB_BT_SShopItem SET Name =N'Headgear Reinforced Plus' WHERE ItemId = 700105
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700106 AND GoodsId = 70010600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700106,70010600,N'Headgear Super')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700106) != 0) UPDATE PB_BT_SShopItem SET Name =N'Headgear Super' WHERE ItemId = 700106
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700107 AND GoodsId = 70010700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700107,70010700,N'Angel Ring')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700107) != 0) UPDATE PB_BT_SShopItem SET Name =N'Angel Ring' WHERE ItemId = 700107
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700108 AND GoodsId = 70010800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700108,70010800,N'Headgear GRS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700108) != 0) UPDATE PB_BT_SShopItem SET Name =N'Headgear GRS' WHERE ItemId = 700108
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700152 AND GoodsId = 70015200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700152,70015200,N'Mask GRS White')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700152) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask GRS White' WHERE ItemId = 700152
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700153 AND GoodsId = 70015300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700153,70015300,N'Mask GRS Red')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700153) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask GRS Red' WHERE ItemId = 700153
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700154 AND GoodsId = 70015400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700154,70015400,N'Mask GRS Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700154) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask GRS Gold' WHERE ItemId = 700154
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700155 AND GoodsId = 70015500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700155,70015500,N'Mask GRS')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700155) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask GRS' WHERE ItemId = 700155
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700156 AND GoodsId = 70015600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700156,70015600,N'SASR Cap')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700156) != 0) UPDATE PB_BT_SShopItem SET Name =N'SASR Cap' WHERE ItemId = 700156
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700157 AND GoodsId = 70015700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700157,70015700,N'Pilot Hat')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700157) != 0) UPDATE PB_BT_SShopItem SET Name =N'Pilot Hat' WHERE ItemId = 700157
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700158 AND GoodsId = 70015800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700158,70015800,N'Toy Hat')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700158) != 0) UPDATE PB_BT_SShopItem SET Name =N'Toy Hat' WHERE ItemId = 700158
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700159 AND GoodsId = 70015900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700159,70015900,N'Special General Beret')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700159) != 0) UPDATE PB_BT_SShopItem SET Name =N'Special General Beret' WHERE ItemId = 700159
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700160 AND GoodsId = 70016000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700160,70016000,N'GM Hat')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700160) != 0) UPDATE PB_BT_SShopItem SET Name =N'GM Hat' WHERE ItemId = 700160
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 700161 AND GoodsId = 70016100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (700161,70016100,N'Cangaceiro Hat')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 700161) != 0) UPDATE PB_BT_SShopItem SET Name =N'Cangaceiro Hat' WHERE ItemId = 700161
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 701109 AND GoodsId = 70110900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (701109,70110900,N'Red beret(Assault)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701109) != 0) UPDATE PB_BT_SShopItem SET Name =N'Red beret(Assault)' WHERE ItemId = 701109
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 701110 AND GoodsId = 70111000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (701110,70111000,N'Red beret(Submachine)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701110) != 0) UPDATE PB_BT_SShopItem SET Name =N'Red beret(Submachine)' WHERE ItemId = 701110
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 701111 AND GoodsId = 70111100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (701111,70111100,N'Red beret(Sniper)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701111) != 0) UPDATE PB_BT_SShopItem SET Name =N'Red beret(Sniper)' WHERE ItemId = 701111
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 701112 AND GoodsId = 70111200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (701112,70111200,N'Red beret(Shotgun)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701112) != 0) UPDATE PB_BT_SShopItem SET Name =N'Red beret(Shotgun)' WHERE ItemId = 701112
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 701113 AND GoodsId = 70111300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (701113,70111300,N'Red beret(handgun)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701113) != 0) UPDATE PB_BT_SShopItem SET Name =N'Red beret(handgun)' WHERE ItemId = 701113
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 701114 AND GoodsId = 70111400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (701114,70111400,N'Beret Black')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701114) != 0) UPDATE PB_BT_SShopItem SET Name =N'Beret Black' WHERE ItemId = 701114
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 701115 AND GoodsId = 70111500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (701115,70111500,N'Beret RedStar')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701115) != 0) UPDATE PB_BT_SShopItem SET Name =N'Beret RedStar' WHERE ItemId = 701115
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 701116 AND GoodsId = 70111600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (701116,70111600,N'Beret YellowStar')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701116) != 0) UPDATE PB_BT_SShopItem SET Name =N'Beret YellowStar' WHERE ItemId = 701116
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 701117 AND GoodsId = 70111700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (701117,70111700,N'Beret CrossSword')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701117) != 0) UPDATE PB_BT_SShopItem SET Name =N'Beret CrossSword' WHERE ItemId = 701117
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 701118 AND GoodsId = 70111800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (701118,70111800,N'Beret PBTN Champion')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701118) != 0) UPDATE PB_BT_SShopItem SET Name =N'Beret PBTN Champion' WHERE ItemId = 701118
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 701119 AND GoodsId = 70111900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (701119,70111900,N'Beret BlackPB')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701119) != 0) UPDATE PB_BT_SShopItem SET Name =N'Beret BlackPB' WHERE ItemId = 701119
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 701120 AND GoodsId = 70112000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (701120,70112000,N'Beret Turkey')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701120) != 0) UPDATE PB_BT_SShopItem SET Name =N'Beret Turkey' WHERE ItemId = 701120
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 701121 AND GoodsId = 70112100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (701121,70112100,N'Beret Kopassus')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701121) != 0) UPDATE PB_BT_SShopItem SET Name =N'Beret Kopassus' WHERE ItemId = 701121
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 701146 AND GoodsId = 70114600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (701146,70114600,N'Beret PBNC5')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 701146) != 0) UPDATE PB_BT_SShopItem SET Name =N'Beret PBNC5' WHERE ItemId = 701146
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 702122 AND GoodsId = 70212200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (702122,70212200,N'Mask Skull')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702122) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Skull' WHERE ItemId = 702122
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 702123 AND GoodsId = 70212300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (702123,70212300,N'Mask Alien Blue eyes')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702123) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Alien Blue eyes' WHERE ItemId = 702123
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 702124 AND GoodsId = 70212400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (702124,70212400,N'Mask Alien Red eyes')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702124) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Alien Red eyes' WHERE ItemId = 702124
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 702125 AND GoodsId = 70212500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (702125,70212500,N'Mask Panda')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702125) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Panda' WHERE ItemId = 702125
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 702126 AND GoodsId = 70212600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (702126,70212600,N'Mask Egg')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702126) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Egg' WHERE ItemId = 702126
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 702128 AND GoodsId = 70212800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (702128,70212800,N'Mask Red Devil')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702128) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Red Devil' WHERE ItemId = 702128
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 702129 AND GoodsId = 70212900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (702129,70212900,N'Mask Dino Skull')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702129) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Dino Skull' WHERE ItemId = 702129
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 702130 AND GoodsId = 70213000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (702130,70213000,N'Headgear Trex')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702130) != 0) UPDATE PB_BT_SShopItem SET Name =N'Headgear Trex' WHERE ItemId = 702130
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 702131 AND GoodsId = 70213100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (702131,70213100,N'Mask Gatotkaca')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702131) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Gatotkaca' WHERE ItemId = 702131
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 702132 AND GoodsId = 70213200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (702132,70213200,N'Mask Gatotkaca Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702132) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Gatotkaca Gold' WHERE ItemId = 702132
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 702135 AND GoodsId = 70213500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (702135,70213500,N'Mask Sheep')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702135) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Sheep' WHERE ItemId = 702135
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 702136 AND GoodsId = 70213600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (702136,70213600,N'Mask Zombie')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702136) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Zombie' WHERE ItemId = 702136
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 702143 AND GoodsId = 70214300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (702143,70214300,N'Mask Azerbaijan')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702143) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Azerbaijan' WHERE ItemId = 702143
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 702144 AND GoodsId = 70214400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (702144,70214400,N'Mask Latin3')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702144) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Latin3' WHERE ItemId = 702144
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 702145 AND GoodsId = 70214500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (702145,70214500,N'Mask Latin3 Premium')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702145) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Latin3 Premium' WHERE ItemId = 702145
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 702147 AND GoodsId = 70214700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (702147,70214700,N'Mask Egg KeenEyes')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702147) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Egg KeenEyes' WHERE ItemId = 702147
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 702148 AND GoodsId = 70214800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (702148,70214800,N'Mask Egg AcidPol')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702148) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Egg AcidPol' WHERE ItemId = 702148
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 702149 AND GoodsId = 70214900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (702149,70214900,N'Mask TH 1Year')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 702149) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask TH 1Year' WHERE ItemId = 702149
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 703137 AND GoodsId = 70313700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (703137,70313700,N'Mask - Santa')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 703137) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask - Santa' WHERE ItemId = 703137
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 703138 AND GoodsId = 70313800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (703138,70313800,N'Mask China')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 703138) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask China' WHERE ItemId = 703138
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 703139 AND GoodsId = 70313900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (703139,70313900,N'Mask Bandana')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 703139) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Bandana' WHERE ItemId = 703139
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 703140 AND GoodsId = 70314000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (703140,70314000,N'Mask Cowboy Hat')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 703140) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Cowboy Hat' WHERE ItemId = 703140
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 703141 AND GoodsId = 70314100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (703141,70314100,N'Mask Bandana (GW)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 703141) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Bandana (GW)' WHERE ItemId = 703141
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 703142 AND GoodsId = 70314200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (703142,70314200,N'Mask Fes Hat')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 703142) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Fes Hat' WHERE ItemId = 703142
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 703150 AND GoodsId = 70315000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (703150,70315000,N'Mask Chinese Hat2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 703150) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Chinese Hat2' WHERE ItemId = 703150
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 703151 AND GoodsId = 70315100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (703151,70315100,N'Kopassus Boonie Hat')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 703151) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kopassus Boonie Hat' WHERE ItemId = 703151
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800001 AND GoodsId = 80000100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800001,80000100,N'Boeing Sunglass')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800001) != 0) UPDATE PB_BT_SShopItem SET Name =N'Boeing Sunglass' WHERE ItemId = 800001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800002 AND GoodsId = 80000200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800002,80000200,N'Gas Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800002) != 0) UPDATE PB_BT_SShopItem SET Name =N'Gas Mask' WHERE ItemId = 800002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800003 AND GoodsId = 80000300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800003,80000300,N'Red Bulls Facegear')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800003) != 0) UPDATE PB_BT_SShopItem SET Name =N'Red Bulls Facegear' WHERE ItemId = 800003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800004 AND GoodsId = 80000400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800004,80000400,N'Viper Red Facegear')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800004) != 0) UPDATE PB_BT_SShopItem SET Name =N'Viper Red Facegear' WHERE ItemId = 800004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800005 AND GoodsId = 80000500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800005,80000500,N'M15 Gas mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800005) != 0) UPDATE PB_BT_SShopItem SET Name =N'M15 Gas mask' WHERE ItemId = 800005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800006 AND GoodsId = 80000600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800006,80000600,N'Sport Sunglass')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800006) != 0) UPDATE PB_BT_SShopItem SET Name =N'Sport Sunglass' WHERE ItemId = 800006
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800007 AND GoodsId = 80000700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800007,80000700,N'Skull Warmer')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800007) != 0) UPDATE PB_BT_SShopItem SET Name =N'Skull Warmer' WHERE ItemId = 800007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800008 AND GoodsId = 80000800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800008,80000800,N'G PB V2 Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800008) != 0) UPDATE PB_BT_SShopItem SET Name =N'G PB V2 Mask' WHERE ItemId = 800008
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800009 AND GoodsId = 80000900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800009,80000900,N'Wiley P Sunglass')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800009) != 0) UPDATE PB_BT_SShopItem SET Name =N'Wiley P Sunglass' WHERE ItemId = 800009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800010 AND GoodsId = 80001000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800010,80001000,N'PB 6200 Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800010) != 0) UPDATE PB_BT_SShopItem SET Name =N'PB 6200 Mask' WHERE ItemId = 800010
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800011 AND GoodsId = 80001100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800011,80001100,N'Skull Faceguard')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800011) != 0) UPDATE PB_BT_SShopItem SET Name =N'Skull Faceguard' WHERE ItemId = 800011
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800012 AND GoodsId = 80001200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800012,80001200,N'Standard Faceguard')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800012) != 0) UPDATE PB_BT_SShopItem SET Name =N'Standard Faceguard' WHERE ItemId = 800012
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800013 AND GoodsId = 80001300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800013,80001300,N'G PB V2 Steel Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800013) != 0) UPDATE PB_BT_SShopItem SET Name =N'G PB V2 Steel Mask' WHERE ItemId = 800013
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800014 AND GoodsId = 80001400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800014,80001400,N'Joker Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800014) != 0) UPDATE PB_BT_SShopItem SET Name =N'Joker Mask' WHERE ItemId = 800014
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800015 AND GoodsId = 80001500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800015,80001500,N'Mask Determinado')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800015) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Determinado' WHERE ItemId = 800015
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800016 AND GoodsId = 80001600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800016,80001600,N'Mask Fuuu')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800016) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Fuuu' WHERE ItemId = 800016
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800017 AND GoodsId = 80001700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800017,80001700,N'Mask Troll')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800017) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Troll' WHERE ItemId = 800017
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800018 AND GoodsId = 80001800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800018,80001800,N'Mask China Red')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800018) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask China Red' WHERE ItemId = 800018
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800019 AND GoodsId = 80001900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800019,80001900,N'Mask Death')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800019) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Death' WHERE ItemId = 800019
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800020 AND GoodsId = 80002000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800020,80002000,N'Frail Skull Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800020) != 0) UPDATE PB_BT_SShopItem SET Name =N'Frail Skull Mask' WHERE ItemId = 800020
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800021 AND GoodsId = 80002100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800021,80002100,N'Mask Frail Skull Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800021) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Frail Skull Gold' WHERE ItemId = 800021
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800022 AND GoodsId = 80002200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800022,80002200,N'Mask Black')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800022) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Black' WHERE ItemId = 800022
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800023 AND GoodsId = 80002300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800023,80002300,N'Mask Russian Blue Tiger')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800023) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Russian Blue Tiger' WHERE ItemId = 800023
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800024 AND GoodsId = 80002400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800024,80002400,N'Mask Korea Marine')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800024) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Korea Marine' WHERE ItemId = 800024
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800025 AND GoodsId = 80002500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800025,80002500,N'Mask France Desert')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800025) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask France Desert' WHERE ItemId = 800025
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800026 AND GoodsId = 80002600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800026,80002600,N'Mask Flame')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800026) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Flame' WHERE ItemId = 800026
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800027 AND GoodsId = 80002700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800027,80002700,N'Mask Jester BW')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800027) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Jester BW' WHERE ItemId = 800027
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800028 AND GoodsId = 80002800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800028,80002800,N'Mask Jewel Pink')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800028) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Jewel Pink' WHERE ItemId = 800028
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800029 AND GoodsId = 80002900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800029,80002900,N'Mask Two tone')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800029) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Two tone' WHERE ItemId = 800029
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800030 AND GoodsId = 80003000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800030,80003000,N'Mask Iron')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800030) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Iron' WHERE ItemId = 800030
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800031 AND GoodsId = 80003100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800031,80003100,N'Mask Jason')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800031) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Jason' WHERE ItemId = 800031
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800032 AND GoodsId = 80003200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800032,80003200,N'Mask Pumpkin')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800032) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Pumpkin' WHERE ItemId = 800032
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800033 AND GoodsId = 80003300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800033,80003300,N'Mask Red Cross')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800033) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Red Cross' WHERE ItemId = 800033
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800034 AND GoodsId = 80003400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800034,80003400,N'Mask Rorschach')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800034) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Rorschach' WHERE ItemId = 800034
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800035 AND GoodsId = 80003500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800035,80003500,N'Mask Golden Smile')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800035) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Golden Smile' WHERE ItemId = 800035
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800036 AND GoodsId = 80003600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800036,80003600,N'Mask Target')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800036) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Target' WHERE ItemId = 800036
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800037 AND GoodsId = 80003700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800037,80003700,N'Mask White')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800037) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask White' WHERE ItemId = 800037
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800038 AND GoodsId = 80003800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800038,80003800,N'Acid Pol Facegear')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800038) != 0) UPDATE PB_BT_SShopItem SET Name =N'Acid Pol Facegear' WHERE ItemId = 800038
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800039 AND GoodsId = 80003900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800039,80003900,N'Keen Eyes Facegear')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800039) != 0) UPDATE PB_BT_SShopItem SET Name =N'Keen Eyes Facegear' WHERE ItemId = 800039
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800040 AND GoodsId = 80004000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800040,80004000,N'Hide Facegear')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800040) != 0) UPDATE PB_BT_SShopItem SET Name =N'Hide Facegear' WHERE ItemId = 800040
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800041 AND GoodsId = 80004100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800041,80004100,N'Leopard Facegear')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800041) != 0) UPDATE PB_BT_SShopItem SET Name =N'Leopard Facegear' WHERE ItemId = 800041
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800042 AND GoodsId = 80004200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800042,80004200,N'D-Fox Facegear')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800042) != 0) UPDATE PB_BT_SShopItem SET Name =N'D-Fox Facegear' WHERE ItemId = 800042
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800043 AND GoodsId = 80004300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800043,80004300,N'Tarantula Facegear')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800043) != 0) UPDATE PB_BT_SShopItem SET Name =N'Tarantula Facegear' WHERE ItemId = 800043
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800084 AND GoodsId = 80008400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800084,80008400,N'Mask BlackSnake')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800084) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask BlackSnake' WHERE ItemId = 800084
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800113 AND GoodsId = 80011300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800113,80011300,N'Mask Pierro')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800113) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Pierro' WHERE ItemId = 800113
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800117 AND GoodsId = 80011700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800117,80011700,N'Mask Argentina (WC)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800117) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Argentina (WC)' WHERE ItemId = 800117
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800118 AND GoodsId = 80011800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800118,80011800,N'Mask Brazil (WC)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800118) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Brazil (WC)' WHERE ItemId = 800118
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800119 AND GoodsId = 80011900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800119,80011900,N'Mask Egland (WC)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800119) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Egland (WC)' WHERE ItemId = 800119
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800120 AND GoodsId = 80012000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800120,80012000,N'Mask France (WC)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800120) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask France (WC)' WHERE ItemId = 800120
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800121 AND GoodsId = 80012100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800121,80012100,N'Mask Germany (WC)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800121) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Germany (WC)' WHERE ItemId = 800121
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800122 AND GoodsId = 80012200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800122,80012200,N'Mask Italy (WC)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800122) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Italy (WC)' WHERE ItemId = 800122
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800123 AND GoodsId = 80012300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800123,80012300,N'Mask Japan (WC)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800123) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Japan (WC)' WHERE ItemId = 800123
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800124 AND GoodsId = 80012400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800124,80012400,N'Mask South Africa (WC)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800124) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask South Africa (WC)' WHERE ItemId = 800124
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800125 AND GoodsId = 80012500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800125,80012500,N'Mask South Korea (WC)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800125) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask South Korea (WC)' WHERE ItemId = 800125
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800126 AND GoodsId = 80012600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800126,80012600,N'Mask Spain')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800126) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Spain' WHERE ItemId = 800126
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800129 AND GoodsId = 80012900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800129,80012900,N'Mask Russia')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800129) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Russia' WHERE ItemId = 800129
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800130 AND GoodsId = 80013000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800130,80013000,N'Mask Ukraine')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800130) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Ukraine' WHERE ItemId = 800130
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800131 AND GoodsId = 80013100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800131,80013100,N'Mask Belorus')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800131) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Belorus' WHERE ItemId = 800131
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800132 AND GoodsId = 80013200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800132,80013200,N'Mask Kazahstan')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800132) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Kazahstan' WHERE ItemId = 800132
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800133 AND GoodsId = 80013300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800133,80013300,N'Mask - Besiktas FC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800133) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask - Besiktas FC' WHERE ItemId = 800133
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800134 AND GoodsId = 80013400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800134,80013400,N'Mask Bursapor FC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800134) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Bursapor FC' WHERE ItemId = 800134
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800135 AND GoodsId = 80013500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800135,80013500,N'Mask Fenerbahce FC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800135) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Fenerbahce FC' WHERE ItemId = 800135
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800136 AND GoodsId = 80013600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800136,80013600,N'Mask Galatasaray FC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800136) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Galatasaray FC' WHERE ItemId = 800136
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800137 AND GoodsId = 80013700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800137,80013700,N'Mask Trabzonspor FC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800137) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Trabzonspor FC' WHERE ItemId = 800137
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800138 AND GoodsId = 80013800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800138,80013800,N'Mask - Frail Skull')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800138) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask - Frail Skull' WHERE ItemId = 800138
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800139 AND GoodsId = 80013900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800139,80013900,N'Mask Bahamas')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800139) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Bahamas' WHERE ItemId = 800139
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800140 AND GoodsId = 80014000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800140,80014000,N'Mask Bolivia')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800140) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Bolivia' WHERE ItemId = 800140
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800141 AND GoodsId = 80014100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800141,80014100,N'Mask Cayman Islands')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800141) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Cayman Islands' WHERE ItemId = 800141
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800142 AND GoodsId = 80014200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800142,80014200,N'Mask Chile')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800142) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Chile' WHERE ItemId = 800142
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800143 AND GoodsId = 80014300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800143,80014300,N'Mask Colombia')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800143) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Colombia' WHERE ItemId = 800143
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800144 AND GoodsId = 80014400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800144,80014400,N'Mask Costa Rica')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800144) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Costa Rica' WHERE ItemId = 800144
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800145 AND GoodsId = 80014500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800145,80014500,N'Mask Honduras')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800145) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Honduras' WHERE ItemId = 800145
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800146 AND GoodsId = 80014600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800146,80014600,N'Mask Jamaica')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800146) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Jamaica' WHERE ItemId = 800146
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800147 AND GoodsId = 80014700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800147,80014700,N'Mask Mexico')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800147) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Mexico' WHERE ItemId = 800147
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800148 AND GoodsId = 80014800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800148,80014800,N'Mask Nicaragua')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800148) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Nicaragua' WHERE ItemId = 800148
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800149 AND GoodsId = 80014900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800149,80014900,N'Mask Panama')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800149) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Panama' WHERE ItemId = 800149
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800150 AND GoodsId = 80015000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800150,80015000,N'Mask Paraguay')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800150) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Paraguay' WHERE ItemId = 800150
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800151 AND GoodsId = 80015100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800151,80015100,N'Mask Dominican')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800151) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Dominican' WHERE ItemId = 800151
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800152 AND GoodsId = 80015200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800152,80015200,N'Mask Ecuador')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800152) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Ecuador' WHERE ItemId = 800152
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800153 AND GoodsId = 80015300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800153,80015300,N'Mask French Guiana')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800153) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask French Guiana' WHERE ItemId = 800153
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800154 AND GoodsId = 80015400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800154,80015400,N'Mask Guatemala')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800154) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Guatemala' WHERE ItemId = 800154
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800155 AND GoodsId = 80015500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800155,80015500,N'Mask Guyana')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800155) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Guyana' WHERE ItemId = 800155
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800156 AND GoodsId = 80015600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800156,80015600,N'Mask Haiti')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800156) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Haiti' WHERE ItemId = 800156
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800157 AND GoodsId = 80015700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800157,80015700,N'Mask Peru')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800157) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Peru' WHERE ItemId = 800157
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800158 AND GoodsId = 80015800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800158,80015800,N'Mask Puerto Rico')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800158) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Puerto Rico' WHERE ItemId = 800158
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800159 AND GoodsId = 80015900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800159,80015900,N'Mask Suriname')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800159) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Suriname' WHERE ItemId = 800159
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800160 AND GoodsId = 80016000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800160,80016000,N'Mask Trinidad and Tobago')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800160) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Trinidad and Tobago' WHERE ItemId = 800160
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800161 AND GoodsId = 80016100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800161,80016100,N'Mask Uruguay')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800161) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Uruguay' WHERE ItemId = 800161
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800162 AND GoodsId = 80016200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800162,80016200,N'Mask Venezuela')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800162) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Venezuela' WHERE ItemId = 800162
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800163 AND GoodsId = 80016300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800163,80016300,N'Mask Argentina')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800163) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Argentina' WHERE ItemId = 800163
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800164 AND GoodsId = 80016400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800164,80016400,N'Mask Raptr')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800164) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Raptr' WHERE ItemId = 800164
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800165 AND GoodsId = 80016500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800165,80016500,N'Mask Canada')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800165) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Canada' WHERE ItemId = 800165
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800166 AND GoodsId = 80016600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800166,80016600,N'Mask United Kingdom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800166) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask United Kingdom' WHERE ItemId = 800166
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800167 AND GoodsId = 80016700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800167,80016700,N'Mask USA')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800167) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask USA' WHERE ItemId = 800167
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800168 AND GoodsId = 80016800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800168,80016800,N'Mask Indonesia')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800168) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Indonesia' WHERE ItemId = 800168
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800169 AND GoodsId = 80016900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800169,80016900,N'Mask PBTN')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800169) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask PBTN' WHERE ItemId = 800169
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800171 AND GoodsId = 80017100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800171,80017100,N'Mask Reinforce Black')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800171) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Reinforce Black' WHERE ItemId = 800171
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800172 AND GoodsId = 80017200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800172,80017200,N'Mask Azzurro_E_Bianco')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800172) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Azzurro_E_Bianco' WHERE ItemId = 800172
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800173 AND GoodsId = 80017300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800173,80017300,N'Mask Azzurro_E_Rossa')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800173) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Azzurro_E_Rossa' WHERE ItemId = 800173
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800174 AND GoodsId = 80017400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800174,80017400,N'Mask Bianca_E_Nera')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800174) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Bianca_E_Nera' WHERE ItemId = 800174
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800175 AND GoodsId = 80017500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800175,80017500,N'Mask Bianco_E_Azzurro')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800175) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Bianco_E_Azzurro' WHERE ItemId = 800175
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800176 AND GoodsId = 80017600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800176,80017600,N'Mask Blu_E_Nera')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800176) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Blu_E_Nera' WHERE ItemId = 800176
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800177 AND GoodsId = 80017700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800177,80017700,N'Mask Blu_E_Rossa')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800177) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Blu_E_Rossa' WHERE ItemId = 800177
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800178 AND GoodsId = 80017800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800178,80017800,N'Mask Croce_Nera')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800178) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Croce_Nera' WHERE ItemId = 800178
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800179 AND GoodsId = 80017900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800179,80017900,N'Mask Giallo_E_Amaranto')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800179) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Giallo_E_Amaranto' WHERE ItemId = 800179
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800180 AND GoodsId = 80018000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800180,80018000,N'Mask-Nera_Bianca_E_Rossa')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800180) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask-Nera_Bianca_E_Rossa' WHERE ItemId = 800180
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800181 AND GoodsId = 80018100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800181,80018100,N'Mask Nera_E_Bianca')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800181) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Nera_E_Bianca' WHERE ItemId = 800181
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800182 AND GoodsId = 80018200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800182,80018200,N'Mask Nera_E_Blu')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800182) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Nera_E_Blu' WHERE ItemId = 800182
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800183 AND GoodsId = 80018300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800183,80018300,N'Mask Nera_E_Rosa')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800183) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Nera_E_Rosa' WHERE ItemId = 800183
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800184 AND GoodsId = 80018400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800184,80018400,N'Mask Nera_Gialla_E_Bianca')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800184) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Nera_Gialla_E_Bianca' WHERE ItemId = 800184
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800185 AND GoodsId = 80018500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800185,80018500,N'Mask Rosso_E_Gialla')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800185) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Rosso_E_Gialla' WHERE ItemId = 800185
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800186 AND GoodsId = 80018600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800186,80018600,N'Mask Stella')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800186) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Stella' WHERE ItemId = 800186
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800187 AND GoodsId = 80018700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800187,80018700,N'Mask Viola')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800187) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Viola' WHERE ItemId = 800187
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800191 AND GoodsId = 80019100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800191,80019100,N'Mask Trojan')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800191) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Trojan' WHERE ItemId = 800191
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800193 AND GoodsId = 80019300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800193,80019300,N'Mask Tk FC Besiktas V2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800193) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Tk FC Besiktas V2' WHERE ItemId = 800193
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800194 AND GoodsId = 80019400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800194,80019400,N'Mask Tk FC Bursapor V2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800194) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Tk FC Bursapor V2' WHERE ItemId = 800194
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800195 AND GoodsId = 80019500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800195,80019500,N'Mask Tk FC Fenerbahce V2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800195) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Tk FC Fenerbahce V2' WHERE ItemId = 800195
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800196 AND GoodsId = 80019600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800196,80019600,N'Mask Tk FC Galatasaray V2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800196) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Tk FC Galatasaray V2' WHERE ItemId = 800196
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800197 AND GoodsId = 80019700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800197,80019700,N'Mask Tk FC Trabzonspor V2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800197) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Tk FC Trabzonspor V2' WHERE ItemId = 800197
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800198 AND GoodsId = 80019800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800198,80019800,N'Mask PBIC 2012')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800198) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask PBIC 2012' WHERE ItemId = 800198
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800199 AND GoodsId = 80019900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800199,80019900,N'Mask Russian Blue EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800199) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Russian Blue EV' WHERE ItemId = 800199
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800200 AND GoodsId = 80020000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800200,80020000,N'Mask ROK Marine EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800200) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask ROK Marine EV' WHERE ItemId = 800200
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800201 AND GoodsId = 80020100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800201,80020100,N'Mask French Desert EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800201) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask French Desert EV' WHERE ItemId = 800201
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800202 AND GoodsId = 80020200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800202,80020200,N'Mask Black EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800202) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Black EV' WHERE ItemId = 800202
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800203 AND GoodsId = 80020300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800203,80020300,N'Mask White EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800203) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask White EV' WHERE ItemId = 800203
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800204 AND GoodsId = 80020400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800204,80020400,N'Mask Russian Blue')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800204) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Russian Blue' WHERE ItemId = 800204
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800205 AND GoodsId = 80020500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800205,80020500,N'Mask ROK Marine')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800205) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask ROK Marine' WHERE ItemId = 800205
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800206 AND GoodsId = 80020600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800206,80020600,N'Mask French Desert')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800206) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask French Desert' WHERE ItemId = 800206
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800207 AND GoodsId = 80020700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800207,80020700,N'Mask PBSC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800207) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask PBSC' WHERE ItemId = 800207
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800208 AND GoodsId = 80020800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800208,80020800,N'Mask PBTN 2012')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800208) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask PBTN 2012' WHERE ItemId = 800208
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800209 AND GoodsId = 80020900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800209,80020900,N'Mask Death GW')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800209) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Death GW' WHERE ItemId = 800209
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800210 AND GoodsId = 80021000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800210,80021000,N'Mask Garena Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800210) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Garena Gold' WHERE ItemId = 800210
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800211 AND GoodsId = 80021100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800211,80021100,N'Mask Garena Red')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800211) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Garena Red' WHERE ItemId = 800211
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800212 AND GoodsId = 80021200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800212,80021200,N'Mask Garena White')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800212) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Garena White' WHERE ItemId = 800212
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800213 AND GoodsId = 80021300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800213,80021300,N'Mask BlackSnake')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800213) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask BlackSnake' WHERE ItemId = 800213
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800214 AND GoodsId = 80021400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800214,80021400,N'Mask GSL')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800214) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask GSL' WHERE ItemId = 800214
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800215 AND GoodsId = 80021500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800215,80021500,N'Mask PBNC4')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800215) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask PBNC4' WHERE ItemId = 800215
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800216 AND GoodsId = 80021600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800216,80021600,N'Mask PBIC2013')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800216) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask PBIC2013' WHERE ItemId = 800216
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800217 AND GoodsId = 80021700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800217,80021700,N'Mask PBSC2013')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800217) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask PBSC2013' WHERE ItemId = 800217
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800224 AND GoodsId = 80022400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800224,80022400,N'Mask WC 2014 Aregentina')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800224) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask WC 2014 Aregentina' WHERE ItemId = 800224
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800225 AND GoodsId = 80022500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800225,80022500,N'Mask WC 2014 Brazil')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800225) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask WC 2014 Brazil' WHERE ItemId = 800225
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800226 AND GoodsId = 80022600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800226,80022600,N'Mask WC 2014 Chile')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800226) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask WC 2014 Chile' WHERE ItemId = 800226
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800227 AND GoodsId = 80022700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800227,80022700,N'Mask WC 2014 Colombia')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800227) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask WC 2014 Colombia' WHERE ItemId = 800227
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800228 AND GoodsId = 80022800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800228,80022800,N'Mask WC 2014 Ecuador')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800228) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask WC 2014 Ecuador' WHERE ItemId = 800228
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800229 AND GoodsId = 80022900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800229,80022900,N'Mask WC 2014 England')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800229) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask WC 2014 England' WHERE ItemId = 800229
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800230 AND GoodsId = 80023000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800230,80023000,N'Mask WC 2014 France')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800230) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask WC 2014 France' WHERE ItemId = 800230
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800231 AND GoodsId = 80023100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800231,80023100,N'Mask WC 2014 Germany')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800231) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask WC 2014 Germany' WHERE ItemId = 800231
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800232 AND GoodsId = 80023200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800232,80023200,N'Mask WC 2014 Italy')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800232) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask WC 2014 Italy' WHERE ItemId = 800232
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800233 AND GoodsId = 80023300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800233,80023300,N'Mask WC 2014 Japan')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800233) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask WC 2014 Japan' WHERE ItemId = 800233
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800234 AND GoodsId = 80023400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800234,80023400,N'Mask WC 2014 Korea')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800234) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask WC 2014 Korea' WHERE ItemId = 800234
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800235 AND GoodsId = 80023500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800235,80023500,N'Mask WC 2014 Spain')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800235) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask WC 2014 Spain' WHERE ItemId = 800235
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800236 AND GoodsId = 80023600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800236,80023600,N'Mask WC 2014 U.S.A')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800236) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask WC 2014 U.S.A' WHERE ItemId = 800236
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800237 AND GoodsId = 80023700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800237,80023700,N'Mask WC 2014 Uruguay')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800237) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask WC 2014 Uruguay' WHERE ItemId = 800237
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800238 AND GoodsId = 80023800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800238,80023800,N'Mask WC 2014 Honduras')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800238) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask WC 2014 Honduras' WHERE ItemId = 800238
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800239 AND GoodsId = 80023900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800239,80023900,N'Mask WC 2014 Mexico')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800239) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask WC 2014 Mexico' WHERE ItemId = 800239
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800240 AND GoodsId = 80024000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800240,80024000,N'Mask WC 2014 CostaRica')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800240) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask WC 2014 CostaRica' WHERE ItemId = 800240
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800241 AND GoodsId = 80024100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800241,80024100,N'Mask GSL2014')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800241) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask GSL2014' WHERE ItemId = 800241
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800242 AND GoodsId = 80024200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800242,80024200,N'WC 2014 Greece Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800242) != 0) UPDATE PB_BT_SShopItem SET Name =N'WC 2014 Greece Mask' WHERE ItemId = 800242
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800243 AND GoodsId = 80024300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800243,80024300,N'WC 2014 Netherlands Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800243) != 0) UPDATE PB_BT_SShopItem SET Name =N'WC 2014 Netherlands Mask' WHERE ItemId = 800243
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800244 AND GoodsId = 80024400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800244,80024400,N'WC 2014 Belguim Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800244) != 0) UPDATE PB_BT_SShopItem SET Name =N'WC 2014 Belguim Mask' WHERE ItemId = 800244
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800245 AND GoodsId = 80024500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800245,80024500,N'WC 2014 BaH Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800245) != 0) UPDATE PB_BT_SShopItem SET Name =N'WC 2014 BaH Mask' WHERE ItemId = 800245
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800246 AND GoodsId = 80024600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800246,80024600,N'WC 2014 Switzerland Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800246) != 0) UPDATE PB_BT_SShopItem SET Name =N'WC 2014 Switzerland Mask' WHERE ItemId = 800246
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800247 AND GoodsId = 80024700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800247,80024700,N'WC 2014 Croatia Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800247) != 0) UPDATE PB_BT_SShopItem SET Name =N'WC 2014 Croatia Mask' WHERE ItemId = 800247
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800248 AND GoodsId = 80024800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800248,80024800,N'WC 2014 Portugal Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800248) != 0) UPDATE PB_BT_SShopItem SET Name =N'WC 2014 Portugal Mask' WHERE ItemId = 800248
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800249 AND GoodsId = 80024900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800249,80024900,N'WC 2014 Ghana Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800249) != 0) UPDATE PB_BT_SShopItem SET Name =N'WC 2014 Ghana Mask' WHERE ItemId = 800249
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800250 AND GoodsId = 80025000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800250,80025000,N'WC 2014 Nigeria Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800250) != 0) UPDATE PB_BT_SShopItem SET Name =N'WC 2014 Nigeria Mask' WHERE ItemId = 800250
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800251 AND GoodsId = 80025100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800251,80025100,N'WC 2014 Algeria Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800251) != 0) UPDATE PB_BT_SShopItem SET Name =N'WC 2014 Algeria Mask' WHERE ItemId = 800251
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800252 AND GoodsId = 80025200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800252,80025200,N'WC 2014 Cameroon Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800252) != 0) UPDATE PB_BT_SShopItem SET Name =N'WC 2014 Cameroon Mask' WHERE ItemId = 800252
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800253 AND GoodsId = 80025300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800253,80025300,N'WC 2014 Cote dIvoire Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800253) != 0) UPDATE PB_BT_SShopItem SET Name =N'WC 2014 Cote dIvoire Mask' WHERE ItemId = 800253
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800254 AND GoodsId = 80025400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800254,80025400,N'WC 2014 Iran Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800254) != 0) UPDATE PB_BT_SShopItem SET Name =N'WC 2014 Iran Mask' WHERE ItemId = 800254
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800255 AND GoodsId = 80025500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800255,80025500,N'WC 2014 Australia Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800255) != 0) UPDATE PB_BT_SShopItem SET Name =N'WC 2014 Australia Mask' WHERE ItemId = 800255
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800256 AND GoodsId = 80025600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800256,80025600,N'WC 2014 Russia Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800256) != 0) UPDATE PB_BT_SShopItem SET Name =N'WC 2014 Russia Mask' WHERE ItemId = 800256
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800257 AND GoodsId = 80025700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800257,80025700,N'Mask Mid-Night')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800257) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Mid-Night' WHERE ItemId = 800257
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800258 AND GoodsId = 80025800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800258,80025800,N'Brazuca Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800258) != 0) UPDATE PB_BT_SShopItem SET Name =N'Brazuca Mask' WHERE ItemId = 800258
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800259 AND GoodsId = 80025900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800259,80025900,N'Mask Sol')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800259) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Sol' WHERE ItemId = 800259
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800260 AND GoodsId = 80026000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800260,80026000,N'Mask Sol Premium')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800260) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Sol Premium' WHERE ItemId = 800260
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800261 AND GoodsId = 80026100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800261,80026100,N'Mask PBIC2014')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800261) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask PBIC2014' WHERE ItemId = 800261
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800262 AND GoodsId = 80026200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800262,80026200,N'Mask Unicorn')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800262) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Unicorn' WHERE ItemId = 800262
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800263 AND GoodsId = 80026300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800263,80026300,N'Mask Soccer Eagle')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800263) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Soccer Eagle' WHERE ItemId = 800263
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800264 AND GoodsId = 80026400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800264,80026400,N'Mask Soccer Storm')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800264) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Soccer Storm' WHERE ItemId = 800264
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800265 AND GoodsId = 80026500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800265,80026500,N'Mask Soccer Lion')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800265) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Soccer Lion' WHERE ItemId = 800265
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800266 AND GoodsId = 80026600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800266,80026600,N'Mask Soccer Canary')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800266) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Soccer Canary' WHERE ItemId = 800266
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800267 AND GoodsId = 80026700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800267,80026700,N'Mask Indonesia GW')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800267) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Indonesia GW' WHERE ItemId = 800267
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800268 AND GoodsId = 80026800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800268,80026800,N'Mask NoLogo PBSC')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800268) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask NoLogo PBSC' WHERE ItemId = 800268
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800269 AND GoodsId = 80026900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800269,80026900,N'Mask NoLogo Latin3')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800269) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask NoLogo Latin3' WHERE ItemId = 800269
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800270 AND GoodsId = 80027000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800270,80027000,N'Mask NoLogo Latin3 Pre')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800270) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask NoLogo Latin3 Pre' WHERE ItemId = 800270
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800271 AND GoodsId = 80027100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800271,80027100,N'Mask NoLogo Mid-night')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800271) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask NoLogo Mid-night' WHERE ItemId = 800271
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800272 AND GoodsId = 80027200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800272,80027200,N'Mask GSL 2013')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800272) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask GSL 2013' WHERE ItemId = 800272
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800273 AND GoodsId = 80027300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800273,80027300,N'Mask Latin4')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800273) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask Latin4' WHERE ItemId = 800273
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800274 AND GoodsId = 80027400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800274,80027400,N'Latin4 Premium Mask')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800274) != 0) UPDATE PB_BT_SShopItem SET Name =N'Latin4 Premium Mask' WHERE ItemId = 800274
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 800275 AND GoodsId = 80027500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (800275,80027500,N'Mask GRS2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 800275) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mask GRS2' WHERE ItemId = 800275
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900002 AND GoodsId = 90000200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900002,90000200,N'Man Half-sleeve Shirt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900002) != 0) UPDATE PB_BT_SShopItem SET Name =N'Man Half-sleeve Shirt' WHERE ItemId = 900002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900004 AND GoodsId = 90000400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900004,90000400,N'Red Bulls Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900004) != 0) UPDATE PB_BT_SShopItem SET Name =N'Red Bulls Top' WHERE ItemId = 900004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900005 AND GoodsId = 90000500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900005,90000500,N'Acid Pol Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900005) != 0) UPDATE PB_BT_SShopItem SET Name =N'Acid Pol Top' WHERE ItemId = 900005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900007 AND GoodsId = 90000700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900007,90000700,N'Keen Eyes Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900007) != 0) UPDATE PB_BT_SShopItem SET Name =N'Keen Eyes Top' WHERE ItemId = 900007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900008 AND GoodsId = 90000800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900008,90000800,N'Tarantula Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900008) != 0) UPDATE PB_BT_SShopItem SET Name =N'Tarantula Top' WHERE ItemId = 900008
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900012 AND GoodsId = 90001200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900012,90001200,N'UDT Unit Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900012) != 0) UPDATE PB_BT_SShopItem SET Name =N'UDT Unit Top' WHERE ItemId = 900012
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900014 AND GoodsId = 90001400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900014,90001400,N'Bope Unit Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900014) != 0) UPDATE PB_BT_SShopItem SET Name =N'Bope Unit Top' WHERE ItemId = 900014
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900015 AND GoodsId = 90001500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900015,90001500,N'D-Fox Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900015) != 0) UPDATE PB_BT_SShopItem SET Name =N'D-Fox Top' WHERE ItemId = 900015
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900016 AND GoodsId = 90001600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900016,90001600,N'Leopard Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900016) != 0) UPDATE PB_BT_SShopItem SET Name =N'Leopard Top' WHERE ItemId = 900016
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900017 AND GoodsId = 90001700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900017,90001700,N'Viper Red Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900017) != 0) UPDATE PB_BT_SShopItem SET Name =N'Viper Red Top' WHERE ItemId = 900017
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900018 AND GoodsId = 90001800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900018,90001800,N'Hide Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900018) != 0) UPDATE PB_BT_SShopItem SET Name =N'Hide Top' WHERE ItemId = 900018
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900028 AND GoodsId = 90002800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900028,90002800,N'Kopassus Unit Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900028) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kopassus Unit Top' WHERE ItemId = 900028
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900034 AND GoodsId = 90003400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900034,90003400,N'Digital Pattern Jacket F')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900034) != 0) UPDATE PB_BT_SShopItem SET Name =N'Digital Pattern Jacket F' WHERE ItemId = 900034
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900035 AND GoodsId = 90003500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900035,90003500,N'Gray Spy Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900035) != 0) UPDATE PB_BT_SShopItem SET Name =N'Gray Spy Top' WHERE ItemId = 900035
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900036 AND GoodsId = 90003600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900036,90003600,N'707 Unit Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900036) != 0) UPDATE PB_BT_SShopItem SET Name =N'707 Unit Top' WHERE ItemId = 900036
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900037 AND GoodsId = 90003700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900037,90003700,N'Digital Pattern Jacket M')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900037) != 0) UPDATE PB_BT_SShopItem SET Name =N'Digital Pattern Jacket M' WHERE ItemId = 900037
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900038 AND GoodsId = 90003800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900038,90003800,N'Air-force Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900038) != 0) UPDATE PB_BT_SShopItem SET Name =N'Air-force Top' WHERE ItemId = 900038
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900039 AND GoodsId = 90003900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900039,90003900,N'Spets Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900039) != 0) UPDATE PB_BT_SShopItem SET Name =N'Spets Top' WHERE ItemId = 900039
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900040 AND GoodsId = 90004000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900040,90004000,N'SAS Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900040) != 0) UPDATE PB_BT_SShopItem SET Name =N'SAS Top' WHERE ItemId = 900040
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900041 AND GoodsId = 90004100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900041,90004100,N'China Marine Top(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900041) != 0) UPDATE PB_BT_SShopItem SET Name =N'China Marine Top(Female)' WHERE ItemId = 900041
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900042 AND GoodsId = 90004200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900042,90004200,N'SASR Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900042) != 0) UPDATE PB_BT_SShopItem SET Name =N'SASR Top' WHERE ItemId = 900042
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900043 AND GoodsId = 90004300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900043,90004300,N'China Marine Top(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900043) != 0) UPDATE PB_BT_SShopItem SET Name =N'China Marine Top(Male)' WHERE ItemId = 900043
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900044 AND GoodsId = 90004400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900044,90004400,N'STF Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900044) != 0) UPDATE PB_BT_SShopItem SET Name =N'STF Top' WHERE ItemId = 900044
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900045 AND GoodsId = 90004500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900045,90004500,N'Maneuver Force Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900045) != 0) UPDATE PB_BT_SShopItem SET Name =N'Maneuver Force Top' WHERE ItemId = 900045
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900046 AND GoodsId = 90004600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900046,90004600,N'FC.PB Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900046) != 0) UPDATE PB_BT_SShopItem SET Name =N'FC.PB Top' WHERE ItemId = 900046
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900047 AND GoodsId = 90004700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900047,90004700,N'CT-Force SF Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900047) != 0) UPDATE PB_BT_SShopItem SET Name =N'CT-Force SF Top' WHERE ItemId = 900047
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900048 AND GoodsId = 90004800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900048,90004800,N'SWAT SF Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900048) != 0) UPDATE PB_BT_SShopItem SET Name =N'SWAT SF Top' WHERE ItemId = 900048
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900049 AND GoodsId = 90004900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900049,90004900,N'Feebie Unit Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900049) != 0) UPDATE PB_BT_SShopItem SET Name =N'Feebie Unit Top' WHERE ItemId = 900049
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900050 AND GoodsId = 90005000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900050,90005000,N'Devgru Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900050) != 0) UPDATE PB_BT_SShopItem SET Name =N'Devgru Top' WHERE ItemId = 900050
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900051 AND GoodsId = 90005100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900051,90005100,N'Black Condor Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900051) != 0) UPDATE PB_BT_SShopItem SET Name =N'Black Condor Top' WHERE ItemId = 900051
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900052 AND GoodsId = 90005200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900052,90005200,N'OMON Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900052) != 0) UPDATE PB_BT_SShopItem SET Name =N'OMON Top' WHERE ItemId = 900052
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900053 AND GoodsId = 90005300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900053,90005300,N'Spearhead Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900053) != 0) UPDATE PB_BT_SShopItem SET Name =N'Spearhead Top' WHERE ItemId = 900053
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900054 AND GoodsId = 90005400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900054,90005400,N'CSI Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900054) != 0) UPDATE PB_BT_SShopItem SET Name =N'CSI Top' WHERE ItemId = 900054
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 900055 AND GoodsId = 90005500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900055,90005500,N'Omega Top')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 900055) != 0) UPDATE PB_BT_SShopItem SET Name =N'Omega Top' WHERE ItemId = 900055
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000001 AND GoodsId = 100000100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000001,100000100,N'Jeans Pants')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000001) != 0) UPDATE PB_BT_SShopItem SET Name =N'Jeans Pants' WHERE ItemId = 1000001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000002 AND GoodsId = 100000200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000002,100000200,N'Acid Pol Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000002) != 0) UPDATE PB_BT_SShopItem SET Name =N'Acid Pol Bottom' WHERE ItemId = 1000002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000004 AND GoodsId = 100000400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000004,100000400,N'Digital pattern Bottom F')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000004) != 0) UPDATE PB_BT_SShopItem SET Name =N'Digital pattern Bottom F' WHERE ItemId = 1000004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000005 AND GoodsId = 100000500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000005,100000500,N'Red Bulls Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000005) != 0) UPDATE PB_BT_SShopItem SET Name =N'Red Bulls Bottom' WHERE ItemId = 1000005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000006 AND GoodsId = 100000600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000006,100000600,N'Keen Eyes Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000006) != 0) UPDATE PB_BT_SShopItem SET Name =N'Keen Eyes Bottom' WHERE ItemId = 1000006
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000007 AND GoodsId = 100000700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000007,100000700,N'Tarantula Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000007) != 0) UPDATE PB_BT_SShopItem SET Name =N'Tarantula Bottom' WHERE ItemId = 1000007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000009 AND GoodsId = 100000900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000009,100000900,N'Digital pattern Bottom M')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000009) != 0) UPDATE PB_BT_SShopItem SET Name =N'Digital pattern Bottom M' WHERE ItemId = 1000009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000010 AND GoodsId = 100001000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000010,100001000,N'Bope Unit Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000010) != 0) UPDATE PB_BT_SShopItem SET Name =N'Bope Unit Bottom' WHERE ItemId = 1000010
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000012 AND GoodsId = 100001200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000012,100001200,N'D-Fox Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000012) != 0) UPDATE PB_BT_SShopItem SET Name =N'D-Fox Bottom' WHERE ItemId = 1000012
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000013 AND GoodsId = 100001300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000013,100001300,N'Leopard Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000013) != 0) UPDATE PB_BT_SShopItem SET Name =N'Leopard Bottom' WHERE ItemId = 1000013
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000014 AND GoodsId = 100001400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000014,100001400,N'Viper Red Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000014) != 0) UPDATE PB_BT_SShopItem SET Name =N'Viper Red Bottom' WHERE ItemId = 1000014
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000015 AND GoodsId = 100001500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000015,100001500,N'Hide Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000015) != 0) UPDATE PB_BT_SShopItem SET Name =N'Hide Bottom' WHERE ItemId = 1000015
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000016 AND GoodsId = 100001600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000016,100001600,N'UDT Unit Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000016) != 0) UPDATE PB_BT_SShopItem SET Name =N'UDT Unit Bottom' WHERE ItemId = 1000016
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000023 AND GoodsId = 100002300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000023,100002300,N'Kopassus Unit Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000023) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kopassus Unit Bottom' WHERE ItemId = 1000023
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000027 AND GoodsId = 100002700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000027,100002700,N'Gray Spy Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000027) != 0) UPDATE PB_BT_SShopItem SET Name =N'Gray Spy Bottom' WHERE ItemId = 1000027
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000028 AND GoodsId = 100002800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000028,100002800,N'707 Unit Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000028) != 0) UPDATE PB_BT_SShopItem SET Name =N'707 Unit Bottom' WHERE ItemId = 1000028
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000029 AND GoodsId = 100002900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000029,100002900,N'Air-force Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000029) != 0) UPDATE PB_BT_SShopItem SET Name =N'Air-force Bottom' WHERE ItemId = 1000029
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000030 AND GoodsId = 100003000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000030,100003000,N'Spets Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000030) != 0) UPDATE PB_BT_SShopItem SET Name =N'Spets Bottom' WHERE ItemId = 1000030
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000031 AND GoodsId = 100003100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000031,100003100,N'SAS Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000031) != 0) UPDATE PB_BT_SShopItem SET Name =N'SAS Bottom' WHERE ItemId = 1000031
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000032 AND GoodsId = 100003200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000032,100003200,N'China Marine Bottom(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000032) != 0) UPDATE PB_BT_SShopItem SET Name =N'China Marine Bottom(Female)' WHERE ItemId = 1000032
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000033 AND GoodsId = 100003300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000033,100003300,N'SASR Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000033) != 0) UPDATE PB_BT_SShopItem SET Name =N'SASR Bottom' WHERE ItemId = 1000033
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000034 AND GoodsId = 100003400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000034,100003400,N'China Marine Bottom(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000034) != 0) UPDATE PB_BT_SShopItem SET Name =N'China Marine Bottom(Male)' WHERE ItemId = 1000034
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000035 AND GoodsId = 100003500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000035,100003500,N'STF Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000035) != 0) UPDATE PB_BT_SShopItem SET Name =N'STF Bottom' WHERE ItemId = 1000035
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000036 AND GoodsId = 100003600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000036,100003600,N'Maneuver Force Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000036) != 0) UPDATE PB_BT_SShopItem SET Name =N'Maneuver Force Bottom' WHERE ItemId = 1000036
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000037 AND GoodsId = 100003700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000037,100003700,N'FC.PB Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000037) != 0) UPDATE PB_BT_SShopItem SET Name =N'FC.PB Bottom' WHERE ItemId = 1000037
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000038 AND GoodsId = 100003800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000038,100003800,N'CT-Force SF Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000038) != 0) UPDATE PB_BT_SShopItem SET Name =N'CT-Force SF Bottom' WHERE ItemId = 1000038
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000039 AND GoodsId = 100003900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000039,100003900,N'SWAT SF Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000039) != 0) UPDATE PB_BT_SShopItem SET Name =N'SWAT SF Bottom' WHERE ItemId = 1000039
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000040 AND GoodsId = 100004000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000040,100004000,N'Feebie Unit Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000040) != 0) UPDATE PB_BT_SShopItem SET Name =N'Feebie Unit Bottom' WHERE ItemId = 1000040
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000041 AND GoodsId = 100004100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000041,100004100,N'Devgru Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000041) != 0) UPDATE PB_BT_SShopItem SET Name =N'Devgru Bottom' WHERE ItemId = 1000041
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000042 AND GoodsId = 100004200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000042,100004200,N'Black Condor Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000042) != 0) UPDATE PB_BT_SShopItem SET Name =N'Black Condor Bottom' WHERE ItemId = 1000042
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000043 AND GoodsId = 100004300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000043,100004300,N'OMON Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000043) != 0) UPDATE PB_BT_SShopItem SET Name =N'OMON Bottom' WHERE ItemId = 1000043
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000044 AND GoodsId = 100004400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000044,100004400,N'Spearhead Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000044) != 0) UPDATE PB_BT_SShopItem SET Name =N'Spearhead Bottom' WHERE ItemId = 1000044
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000045 AND GoodsId = 100004500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000045,100004500,N'CSI Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000045) != 0) UPDATE PB_BT_SShopItem SET Name =N'CSI Bottom' WHERE ItemId = 1000045
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1000046 AND GoodsId = 100004600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1000046,100004600,N'Omega Bottom')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1000046) != 0) UPDATE PB_BT_SShopItem SET Name =N'Omega Bottom' WHERE ItemId = 1000046
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100001 AND GoodsId = 110000100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100001,110000100,N'Man Basic Glove')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100001) != 0) UPDATE PB_BT_SShopItem SET Name =N'Man Basic Glove' WHERE ItemId = 1100001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100002 AND GoodsId = 110000200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100002,110000200,N'Acid Pol Glove')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100002) != 0) UPDATE PB_BT_SShopItem SET Name =N'Acid Pol Glove' WHERE ItemId = 1100002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100003 AND GoodsId = 110000300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100003,110000300,N'Tarantula Glove')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100003) != 0) UPDATE PB_BT_SShopItem SET Name =N'Tarantula Glove' WHERE ItemId = 1100003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100004 AND GoodsId = 110000400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100004,110000400,N'Woman Basic Glove')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100004) != 0) UPDATE PB_BT_SShopItem SET Name =N'Woman Basic Glove' WHERE ItemId = 1100004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100006 AND GoodsId = 110000600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100006,110000600,N'Aquaharbinger')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100006) != 0) UPDATE PB_BT_SShopItem SET Name =N'Aquaharbinger' WHERE ItemId = 1100006
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100008 AND GoodsId = 110000800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100008,110000800,N'Keen Eyes Glove')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100008) != 0) UPDATE PB_BT_SShopItem SET Name =N'Keen Eyes Glove' WHERE ItemId = 1100008
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100010 AND GoodsId = 110001000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100010,110001000,N'D-Fox Glove')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100010) != 0) UPDATE PB_BT_SShopItem SET Name =N'D-Fox Glove' WHERE ItemId = 1100010
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100011 AND GoodsId = 110001100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100011,110001100,N'Leopard Glove')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100011) != 0) UPDATE PB_BT_SShopItem SET Name =N'Leopard Glove' WHERE ItemId = 1100011
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100012 AND GoodsId = 110001200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100012,110001200,N'Viper Red Glove')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100012) != 0) UPDATE PB_BT_SShopItem SET Name =N'Viper Red Glove' WHERE ItemId = 1100012
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100013 AND GoodsId = 110001300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100013,110001300,N'Hide Glove')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100013) != 0) UPDATE PB_BT_SShopItem SET Name =N'Hide Glove' WHERE ItemId = 1100013
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100014 AND GoodsId = 110001400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100014,110001400,N'Red Bulls Glove')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100014) != 0) UPDATE PB_BT_SShopItem SET Name =N'Red Bulls Glove' WHERE ItemId = 1100014
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100015 AND GoodsId = 110001500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100015,110001500,N'Factory Glove')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100015) != 0) UPDATE PB_BT_SShopItem SET Name =N'Factory Glove' WHERE ItemId = 1100015
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100016 AND GoodsId = 110001600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100016,110001600,N'Red Digital Glove F')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100016) != 0) UPDATE PB_BT_SShopItem SET Name =N'Red Digital Glove F' WHERE ItemId = 1100016
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100017 AND GoodsId = 110001700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100017,110001700,N'Pinkglow Glove F')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100017) != 0) UPDATE PB_BT_SShopItem SET Name =N'Pinkglow Glove F' WHERE ItemId = 1100017
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100018 AND GoodsId = 110001800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100018,110001800,N'Red Digital Glove M')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100018) != 0) UPDATE PB_BT_SShopItem SET Name =N'Red Digital Glove M' WHERE ItemId = 1100018
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100019 AND GoodsId = 110001900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100019,110001900,N'Digital Combat Glove')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100019) != 0) UPDATE PB_BT_SShopItem SET Name =N'Digital Combat Glove' WHERE ItemId = 1100019
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100020 AND GoodsId = 110002000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100020,110002000,N'Pinkglow Glove M')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100020) != 0) UPDATE PB_BT_SShopItem SET Name =N'Pinkglow Glove M' WHERE ItemId = 1100020
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100025 AND GoodsId = 110002500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100025,110002500,N'Kopassus Unit Glove')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100025) != 0) UPDATE PB_BT_SShopItem SET Name =N'Kopassus Unit Glove' WHERE ItemId = 1100025
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100026 AND GoodsId = 110002600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100026,110002600,N'Aqua Harbinger')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100026) != 0) UPDATE PB_BT_SShopItem SET Name =N'Aqua Harbinger' WHERE ItemId = 1100026
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100027 AND GoodsId = 110002700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100027,110002700,N'Factory Glove(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100027) != 0) UPDATE PB_BT_SShopItem SET Name =N'Factory Glove(Female)' WHERE ItemId = 1100027
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100028 AND GoodsId = 110002800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100028,110002800,N'Skeleton Glove(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100028) != 0) UPDATE PB_BT_SShopItem SET Name =N'Skeleton Glove(Male)' WHERE ItemId = 1100028
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100029 AND GoodsId = 110002900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100029,110002900,N'Skeleton Glove(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100029) != 0) UPDATE PB_BT_SShopItem SET Name =N'Skeleton Glove(Female)' WHERE ItemId = 1100029
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100030 AND GoodsId = 110003000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100030,110003000,N'Digital Glove')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100030) != 0) UPDATE PB_BT_SShopItem SET Name =N'Digital Glove' WHERE ItemId = 1100030
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100031 AND GoodsId = 110003100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100031,110003100,N'F Blue Skeleton Glove')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100031) != 0) UPDATE PB_BT_SShopItem SET Name =N'F Blue Skeleton Glove' WHERE ItemId = 1100031
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1100032 AND GoodsId = 110003200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1100032,110003200,N'M Blue Skeleton Glove')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1100032) != 0) UPDATE PB_BT_SShopItem SET Name =N'M Blue Skeleton Glove' WHERE ItemId = 1100032
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1200001 AND GoodsId = 120000100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1200001,120000100,N'General Belt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200001) != 0) UPDATE PB_BT_SShopItem SET Name =N'General Belt' WHERE ItemId = 1200001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1200002 AND GoodsId = 120000200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1200002,120000200,N'Water Bottle Belt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200002) != 0) UPDATE PB_BT_SShopItem SET Name =N'Water Bottle Belt' WHERE ItemId = 1200002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1200003 AND GoodsId = 120000300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1200003,120000300,N'Acid Pol Belt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200003) != 0) UPDATE PB_BT_SShopItem SET Name =N'Acid Pol Belt' WHERE ItemId = 1200003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1200004 AND GoodsId = 120000400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1200004,120000400,N'Keen Eyes Belt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200004) != 0) UPDATE PB_BT_SShopItem SET Name =N'Keen Eyes Belt' WHERE ItemId = 1200004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1200005 AND GoodsId = 120000500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1200005,120000500,N'Red Bulls Belt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200005) != 0) UPDATE PB_BT_SShopItem SET Name =N'Red Bulls Belt' WHERE ItemId = 1200005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1200006 AND GoodsId = 120000600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1200006,120000600,N'Tarantula Belt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200006) != 0) UPDATE PB_BT_SShopItem SET Name =N'Tarantula Belt' WHERE ItemId = 1200006
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1200007 AND GoodsId = 120000700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1200007,120000700,N'D-Fox Belt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200007) != 0) UPDATE PB_BT_SShopItem SET Name =N'D-Fox Belt' WHERE ItemId = 1200007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1200008 AND GoodsId = 120000800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1200008,120000800,N'Hide Belt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200008) != 0) UPDATE PB_BT_SShopItem SET Name =N'Hide Belt' WHERE ItemId = 1200008
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1200009 AND GoodsId = 120000900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1200009,120000900,N'Leopard Belt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200009) != 0) UPDATE PB_BT_SShopItem SET Name =N'Leopard Belt' WHERE ItemId = 1200009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1200010 AND GoodsId = 120001000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1200010,120001000,N'ViperRed Belt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200010) != 0) UPDATE PB_BT_SShopItem SET Name =N'ViperRed Belt' WHERE ItemId = 1200010
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1200011 AND GoodsId = 120001100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1200011,120001100,N'Ammo Pocket Belt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200011) != 0) UPDATE PB_BT_SShopItem SET Name =N'Ammo Pocket Belt' WHERE ItemId = 1200011
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1200012 AND GoodsId = 120001200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1200012,120001200,N'Glow Stick Belt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200012) != 0) UPDATE PB_BT_SShopItem SET Name =N'Glow Stick Belt' WHERE ItemId = 1200012
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1200013 AND GoodsId = 120001300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1200013,120001300,N'Square Canteen Belt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200013) != 0) UPDATE PB_BT_SShopItem SET Name =N'Square Canteen Belt' WHERE ItemId = 1200013
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1200014 AND GoodsId = 120001400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1200014,120001400,N'USGI Canteen Belt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200014) != 0) UPDATE PB_BT_SShopItem SET Name =N'USGI Canteen Belt' WHERE ItemId = 1200014
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1200015 AND GoodsId = 120001500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1200015,120001500,N'WW2 Canteen Belt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200015) != 0) UPDATE PB_BT_SShopItem SET Name =N'WW2 Canteen Belt' WHERE ItemId = 1200015
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1200016 AND GoodsId = 120001600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1200016,120001600,N'Big Canteen Belt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200016) != 0) UPDATE PB_BT_SShopItem SET Name =N'Big Canteen Belt' WHERE ItemId = 1200016
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1200017 AND GoodsId = 120001700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1200017,120001700,N'Warm Bottle Belt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200017) != 0) UPDATE PB_BT_SShopItem SET Name =N'Warm Bottle Belt' WHERE ItemId = 1200017
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1200018 AND GoodsId = 120001800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1200018,120001800,N'Messkit Belt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200018) != 0) UPDATE PB_BT_SShopItem SET Name =N'Messkit Belt' WHERE ItemId = 1200018
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1200019 AND GoodsId = 120001900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1200019,120001900,N'Double Eagle Belt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200019) != 0) UPDATE PB_BT_SShopItem SET Name =N'Double Eagle Belt' WHERE ItemId = 1200019
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1200020 AND GoodsId = 120002000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1200020,120002000,N'Skull Belt')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1200020) != 0) UPDATE PB_BT_SShopItem SET Name =N'Skull Belt' WHERE ItemId = 1200020
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300001 AND GoodsId = 130000100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300001,130000100,N'General Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300001) != 0) UPDATE PB_BT_SShopItem SET Name =N'General Holster' WHERE ItemId = 1300001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300002 AND GoodsId = 130000200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300002,130000200,N'Brown H Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300002) != 0) UPDATE PB_BT_SShopItem SET Name =N'Brown H Holster' WHERE ItemId = 1300002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300003 AND GoodsId = 130000300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300003,130000300,N'Acid Pol Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300003) != 0) UPDATE PB_BT_SShopItem SET Name =N'Acid Pol Holster' WHERE ItemId = 1300003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300004 AND GoodsId = 130000400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300004,130000400,N'Keen Eyes Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300004) != 0) UPDATE PB_BT_SShopItem SET Name =N'Keen Eyes Holster' WHERE ItemId = 1300004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300005 AND GoodsId = 130000500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300005,130000500,N'Red Bulls Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300005) != 0) UPDATE PB_BT_SShopItem SET Name =N'Red Bulls Holster' WHERE ItemId = 1300005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300006 AND GoodsId = 130000600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300006,130000600,N'Tarantula Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300006) != 0) UPDATE PB_BT_SShopItem SET Name =N'Tarantula Holster' WHERE ItemId = 1300006
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300007 AND GoodsId = 130000700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300007,130000700,N'D-Fox Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300007) != 0) UPDATE PB_BT_SShopItem SET Name =N'D-Fox Holster' WHERE ItemId = 1300007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300008 AND GoodsId = 130000800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300008,130000800,N'Leopard Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300008) != 0) UPDATE PB_BT_SShopItem SET Name =N'Leopard Holster' WHERE ItemId = 1300008
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300009 AND GoodsId = 130000900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300009,130000900,N'Viper Red Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300009) != 0) UPDATE PB_BT_SShopItem SET Name =N'Viper Red Holster' WHERE ItemId = 1300009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300010 AND GoodsId = 130001000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300010,130001000,N'Hide Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300010) != 0) UPDATE PB_BT_SShopItem SET Name =N'Hide Holster' WHERE ItemId = 1300010
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300011 AND GoodsId = 130001100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300011,130001100,N'Classic leader Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300011) != 0) UPDATE PB_BT_SShopItem SET Name =N'Classic leader Holster' WHERE ItemId = 1300011
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300012 AND GoodsId = 130001200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300012,130001200,N'Long Gun Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300012) != 0) UPDATE PB_BT_SShopItem SET Name =N'Long Gun Holster' WHERE ItemId = 1300012
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300013 AND GoodsId = 130001300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300013,130001300,N'Solid thigh Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300013) != 0) UPDATE PB_BT_SShopItem SET Name =N'Solid thigh Holster' WHERE ItemId = 1300013
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300014 AND GoodsId = 130001400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300014,130001400,N'Solid furrow Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300014) != 0) UPDATE PB_BT_SShopItem SET Name =N'Solid furrow Holster' WHERE ItemId = 1300014
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300015 AND GoodsId = 130001500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300015,130001500,N'Hard Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300015) != 0) UPDATE PB_BT_SShopItem SET Name =N'Hard Holster' WHERE ItemId = 1300015
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300016 AND GoodsId = 130001600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300016,130001600,N'Longgun-leather Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300016) != 0) UPDATE PB_BT_SShopItem SET Name =N'Longgun-leather Holster' WHERE ItemId = 1300016
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300017 AND GoodsId = 130001700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300017,130001700,N'Velcro Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300017) != 0) UPDATE PB_BT_SShopItem SET Name =N'Velcro Holster' WHERE ItemId = 1300017
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300018 AND GoodsId = 130001800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300018,130001800,N'Swat Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300018) != 0) UPDATE PB_BT_SShopItem SET Name =N'Swat Holster' WHERE ItemId = 1300018
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300019 AND GoodsId = 130001900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300019,130001900,N'Croc leather Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300019) != 0) UPDATE PB_BT_SShopItem SET Name =N'Croc leather Holster' WHERE ItemId = 1300019
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300020 AND GoodsId = 130002000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300020,130002000,N'2 Buckle Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300020) != 0) UPDATE PB_BT_SShopItem SET Name =N'2 Buckle Holster' WHERE ItemId = 1300020
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300021 AND GoodsId = 130002100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300021,130002100,N'1911 Holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300021) != 0) UPDATE PB_BT_SShopItem SET Name =N'1911 Holster' WHERE ItemId = 1300021
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1300022 AND GoodsId = 130002200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1300022,130002200,N'Snake leather holster')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1300022) != 0) UPDATE PB_BT_SShopItem SET Name =N'Snake leather holster' WHERE ItemId = 1300022
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1402009 AND GoodsId = 140200900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1402009,140200900,N'Vacance SwimSuit Male')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1402009) != 0) UPDATE PB_BT_SShopItem SET Name =N'Vacance SwimSuit Male' WHERE ItemId = 1402009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1402010 AND GoodsId = 140201000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1402010,140201000,N'Santa male')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1402010) != 0) UPDATE PB_BT_SShopItem SET Name =N'Santa male' WHERE ItemId = 1402010
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1402011 AND GoodsId = 140201100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1402011,140201100,N'Valentine Male')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1402011) != 0) UPDATE PB_BT_SShopItem SET Name =N'Valentine Male' WHERE ItemId = 1402011
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1403009 AND GoodsId = 140300900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1403009,140300900,N'Vacance SwimSuit Female')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1403009) != 0) UPDATE PB_BT_SShopItem SET Name =N'Vacance SwimSuit Female' WHERE ItemId = 1403009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1403010 AND GoodsId = 140301000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1403010,140301000,N'Grim Reaper Female')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1403010) != 0) UPDATE PB_BT_SShopItem SET Name =N'Grim Reaper Female' WHERE ItemId = 1403010
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1403011 AND GoodsId = 140301100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1403011,140301100,N'Santa Female')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1403011) != 0) UPDATE PB_BT_SShopItem SET Name =N'Santa Female' WHERE ItemId = 1403011
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1403012 AND GoodsId = 140301200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1403012,140301200,N'Sheep Female')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1403012) != 0) UPDATE PB_BT_SShopItem SET Name =N'Sheep Female' WHERE ItemId = 1403012
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1403013 AND GoodsId = 140301300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1403013,140301300,N'Valentine Female')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1403013) != 0) UPDATE PB_BT_SShopItem SET Name =N'Valentine Female' WHERE ItemId = 1403013
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1500501 AND GoodsId = 150050100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1500501,150050100,N'Acid (Escape Dino)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500501) != 0) UPDATE PB_BT_SShopItem SET Name =N'Acid (Escape Dino)' WHERE ItemId = 1500501
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1500502 AND GoodsId = 150050200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1500502,150050200,N'Elite (Escape Dino)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500502) != 0) UPDATE PB_BT_SShopItem SET Name =N'Elite (Escape Dino)' WHERE ItemId = 1500502
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1500503 AND GoodsId = 150050300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1500503,150050300,N'Raptor (Escape Dino)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500503) != 0) UPDATE PB_BT_SShopItem SET Name =N'Raptor (Escape Dino)' WHERE ItemId = 1500503
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1500504 AND GoodsId = 150050400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1500504,150050400,N'Sting (Escape Dino)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500504) != 0) UPDATE PB_BT_SShopItem SET Name =N'Sting (Escape Dino)' WHERE ItemId = 1500504
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1500506 AND GoodsId = 150050600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1500506,150050600,N'Tank (Escape Dino)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500506) != 0) UPDATE PB_BT_SShopItem SET Name =N'Tank (Escape Dino)' WHERE ItemId = 1500506
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1500511 AND GoodsId = 150051100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1500511,150051100,N'Raptor (CC Dino)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500511) != 0) UPDATE PB_BT_SShopItem SET Name =N'Raptor (CC Dino)' WHERE ItemId = 1500511
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1500512 AND GoodsId = 150051200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1500512,150051200,N'Sting (CC Dino)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500512) != 0) UPDATE PB_BT_SShopItem SET Name =N'Sting (CC Dino)' WHERE ItemId = 1500512
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1500513 AND GoodsId = 150051300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1500513,150051300,N'Acid (CC Dino)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500513) != 0) UPDATE PB_BT_SShopItem SET Name =N'Acid (CC Dino)' WHERE ItemId = 1500513
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1500514 AND GoodsId = 150051400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1500514,150051400,N'Reinforced Raptor (CC Dino)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500514) != 0) UPDATE PB_BT_SShopItem SET Name =N'Reinforced Raptor (CC Dino)' WHERE ItemId = 1500514
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1500515 AND GoodsId = 150051500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1500515,150051500,N'Reinforced Sting (CC Dino)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500515) != 0) UPDATE PB_BT_SShopItem SET Name =N'Reinforced Sting (CC Dino)' WHERE ItemId = 1500515
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1500516 AND GoodsId = 150051600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1500516,150051600,N'Reinforced Acid (CC Dino)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1500516) != 0) UPDATE PB_BT_SShopItem SET Name =N'Reinforced Acid (CC Dino)' WHERE ItemId = 1500516
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600002 AND GoodsId = 160000200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600002,160000200,N'EXP 130%')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600002) != 0) UPDATE PB_BT_SShopItem SET Name =N'EXP 130%' WHERE ItemId = 1600002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600003 AND GoodsId = 160000300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600003,160000300,N'EXP 150%')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600003) != 0) UPDATE PB_BT_SShopItem SET Name =N'EXP 150%' WHERE ItemId = 1600003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600004 AND GoodsId = 160000400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600004,160000400,N'Point 130%')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600004) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 130%' WHERE ItemId = 1600004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600005 AND GoodsId = 160000500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600005,160000500,N'Color Clan Name')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600005) != 0) UPDATE PB_BT_SShopItem SET Name =N'Color Clan Name' WHERE ItemId = 1600005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600006 AND GoodsId = 160000600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600006,160000600,N'Color Nickname')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600006) != 0) UPDATE PB_BT_SShopItem SET Name =N'Color Nickname' WHERE ItemId = 1600006
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600007 AND GoodsId = 160000700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600007,160000700,N'Short Respawn 30%')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600007) != 0) UPDATE PB_BT_SShopItem SET Name =N'Short Respawn 30%' WHERE ItemId = 1600007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600008 AND GoodsId = 160000800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600008,160000800,N'Ammo Up')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600008) != 0) UPDATE PB_BT_SShopItem SET Name =N'Ammo Up' WHERE ItemId = 1600008
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600009 AND GoodsId = 160000900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600009,160000900,N'Disguise Rank')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600009) != 0) UPDATE PB_BT_SShopItem SET Name =N'Disguise Rank' WHERE ItemId = 1600009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600010 AND GoodsId = 160001000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600010,160001000,N'Disguise Nick')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600010) != 0) UPDATE PB_BT_SShopItem SET Name =N'Disguise Nick' WHERE ItemId = 1600010
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600011 AND GoodsId = 160001100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600011,160001100,N'Free Move(Unit)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600011) != 0) UPDATE PB_BT_SShopItem SET Name =N'Free Move(Unit)' WHERE ItemId = 1600011
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600012 AND GoodsId = 160001200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600012,160001200,N'Clan Exp 150%')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600012) != 0) UPDATE PB_BT_SShopItem SET Name =N'Clan Exp 150%' WHERE ItemId = 1600012
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600014 AND GoodsId = 160001400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600014,160001400,N'Color CrossHair')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600014) != 0) UPDATE PB_BT_SShopItem SET Name =N'Color CrossHair' WHERE ItemId = 1600014
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600017 AND GoodsId = 160001700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600017,160001700,N'Get Dropped Weapon')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600017) != 0) UPDATE PB_BT_SShopItem SET Name =N'Get Dropped Weapon' WHERE ItemId = 1600017
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600018 AND GoodsId = 160001800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600018,160001800,N'Exp X2(12~15)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600018) != 0) UPDATE PB_BT_SShopItem SET Name =N'Exp X2(12~15)' WHERE ItemId = 1600018
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600019 AND GoodsId = 160001900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600019,160001900,N'Exp X2(15~18)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600019) != 0) UPDATE PB_BT_SShopItem SET Name =N'Exp X2(15~18)' WHERE ItemId = 1600019
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600020 AND GoodsId = 160002000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600020,160002000,N'Exp X2(18~21)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600020) != 0) UPDATE PB_BT_SShopItem SET Name =N'Exp X2(18~21)' WHERE ItemId = 1600020
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600021 AND GoodsId = 160002100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600021,160002100,N'Exp X2(21~00)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600021) != 0) UPDATE PB_BT_SShopItem SET Name =N'Exp X2(21~00)' WHERE ItemId = 1600021
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600022 AND GoodsId = 160002200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600022,160002200,N'Point X2 (12~15)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600022) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point X2 (12~15)' WHERE ItemId = 1600022
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600023 AND GoodsId = 160002300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600023,160002300,N'Point X2 (15~18)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600023) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point X2 (15~18)' WHERE ItemId = 1600023
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600024 AND GoodsId = 160002400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600024,160002400,N'Point X2 (18~21)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600024) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point X2 (18~21)' WHERE ItemId = 1600024
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600025 AND GoodsId = 160002500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600025,160002500,N'Point X2 (21~00)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600025) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point X2 (21~00)' WHERE ItemId = 1600025
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600026 AND GoodsId = 160002600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600026,160002600,N'Quick Change Weapon')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600026) != 0) UPDATE PB_BT_SShopItem SET Name =N'Quick Change Weapon' WHERE ItemId = 1600026
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600027 AND GoodsId = 160002700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600027,160002700,N'Quick Change Magazine')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600027) != 0) UPDATE PB_BT_SShopItem SET Name =N'Quick Change Magazine' WHERE ItemId = 1600027
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600028 AND GoodsId = 160002800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600028,160002800,N'Mega HP 10%')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600028) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mega HP 10%' WHERE ItemId = 1600028
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600029 AND GoodsId = 160002900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600029,160002900,N'Increase Invincible Time')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600029) != 0) UPDATE PB_BT_SShopItem SET Name =N'Increase Invincible Time' WHERE ItemId = 1600029
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600030 AND GoodsId = 160003000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600030,160003000,N'Bullet Proof Vest')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600030) != 0) UPDATE PB_BT_SShopItem SET Name =N'Bullet Proof Vest' WHERE ItemId = 1600030
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600031 AND GoodsId = 160003100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600031,160003100,N'AP Rounds')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600031) != 0) UPDATE PB_BT_SShopItem SET Name =N'AP Rounds' WHERE ItemId = 1600031
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600032 AND GoodsId = 160003200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600032,160003200,N'Hollow Point Ammo')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600032) != 0) UPDATE PB_BT_SShopItem SET Name =N'Hollow Point Ammo' WHERE ItemId = 1600032
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600034 AND GoodsId = 160003400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600034,160003400,N'C4 Speed Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600034) != 0) UPDATE PB_BT_SShopItem SET Name =N'C4 Speed Kit' WHERE ItemId = 1600034
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600035 AND GoodsId = 160003500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600035,160003500,N'Increase Grenade Slot')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600035) != 0) UPDATE PB_BT_SShopItem SET Name =N'Increase Grenade Slot' WHERE ItemId = 1600035
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600036 AND GoodsId = 160003600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600036,160003600,N'Jecketed Hollow Point Ammo')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600036) != 0) UPDATE PB_BT_SShopItem SET Name =N'Jecketed Hollow Point Ammo' WHERE ItemId = 1600036
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600037 AND GoodsId = 160003700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600037,160003700,N'Exp 200%')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600037) != 0) UPDATE PB_BT_SShopItem SET Name =N'Exp 200%' WHERE ItemId = 1600037
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600038 AND GoodsId = 160003800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600038,160003800,N'Point 200%')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600038) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 200%' WHERE ItemId = 1600038
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600040 AND GoodsId = 160004000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600040,160004000,N'Mega HP 5%')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600040) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mega HP 5%' WHERE ItemId = 1600040
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600044 AND GoodsId = 160004400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600044,160004400,N'Bullet Proof Vest Plus')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600044) != 0) UPDATE PB_BT_SShopItem SET Name =N'Bullet Proof Vest Plus' WHERE ItemId = 1600044
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600062 AND GoodsId = 160006200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600062,160006200,N'Exp 200% (0 ~ 6)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600062) != 0) UPDATE PB_BT_SShopItem SET Name =N'Exp 200% (0 ~ 6)' WHERE ItemId = 1600062
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600063 AND GoodsId = 160006300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600063,160006300,N'Exp 200% (6 ~ 12)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600063) != 0) UPDATE PB_BT_SShopItem SET Name =N'Exp 200% (6 ~ 12)' WHERE ItemId = 1600063
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600064 AND GoodsId = 160006400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600064,160006400,N'Short Respawn 50%')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600064) != 0) UPDATE PB_BT_SShopItem SET Name =N'Short Respawn 50%' WHERE ItemId = 1600064
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600065 AND GoodsId = 160006500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600065,160006500,N'Bullet Proof Vest Plus(GM)')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600065) != 0) UPDATE PB_BT_SShopItem SET Name =N'Bullet Proof Vest Plus(GM)' WHERE ItemId = 1600065
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600077 AND GoodsId = 160007700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600077,160007700,N'Short Respawn 20%')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600077) != 0) UPDATE PB_BT_SShopItem SET Name =N'Short Respawn 20%' WHERE ItemId = 1600077
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600078 AND GoodsId = 160007800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600078,160007800,N'Hollow Point Ammo Plus')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600078) != 0) UPDATE PB_BT_SShopItem SET Name =N'Hollow Point Ammo Plus' WHERE ItemId = 1600078
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600079 AND GoodsId = 160007900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600079,160007900,N'Metal Bullet Proof Vest')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600079) != 0) UPDATE PB_BT_SShopItem SET Name =N'Metal Bullet Proof Vest' WHERE ItemId = 1600079
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600080 AND GoodsId = 160008000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600080,160008000,N'Short Respawn 100%')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600080) != 0) UPDATE PB_BT_SShopItem SET Name =N'Short Respawn 100%' WHERE ItemId = 1600080
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600119 AND GoodsId = 160011900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600119,160011900,N'Point 150%')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600119) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 150%' WHERE ItemId = 1600119
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600159 AND GoodsId = 160015900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600159,160015900,N'Point 130% EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600159) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 130% EV' WHERE ItemId = 1600159
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600160 AND GoodsId = 160016000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600160,160016000,N'EXP 130% EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600160) != 0) UPDATE PB_BT_SShopItem SET Name =N'EXP 130% EV' WHERE ItemId = 1600160
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600161 AND GoodsId = 160016100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600161,160016100,N'Clan Exp 150% EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600161) != 0) UPDATE PB_BT_SShopItem SET Name =N'Clan Exp 150% EV' WHERE ItemId = 1600161
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600162 AND GoodsId = 160016200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600162,160016200,N'Free Move(Period) EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600162) != 0) UPDATE PB_BT_SShopItem SET Name =N'Free Move(Period) EV' WHERE ItemId = 1600162
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600163 AND GoodsId = 160016300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600163,160016300,N'Short Respawn 30% EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600163) != 0) UPDATE PB_BT_SShopItem SET Name =N'Short Respawn 30% EV' WHERE ItemId = 1600163
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600164 AND GoodsId = 160016400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600164,160016400,N'Short Respawn 20% EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600164) != 0) UPDATE PB_BT_SShopItem SET Name =N'Short Respawn 20% EV' WHERE ItemId = 1600164
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600165 AND GoodsId = 160016500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600165,160016500,N'Quick Change Weapon EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600165) != 0) UPDATE PB_BT_SShopItem SET Name =N'Quick Change Weapon EV' WHERE ItemId = 1600165
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600166 AND GoodsId = 160016600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600166,160016600,N'Quick Change Magazine EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600166) != 0) UPDATE PB_BT_SShopItem SET Name =N'Quick Change Magazine EV' WHERE ItemId = 1600166
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600167 AND GoodsId = 160016700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600167,160016700,N'Tactical Insertion EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600167) != 0) UPDATE PB_BT_SShopItem SET Name =N'Tactical Insertion EV' WHERE ItemId = 1600167
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600168 AND GoodsId = 160016800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600168,160016800,N'Get Dropped Weapon EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600168) != 0) UPDATE PB_BT_SShopItem SET Name =N'Get Dropped Weapon EV' WHERE ItemId = 1600168
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600169 AND GoodsId = 160016900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600169,160016900,N'Hollow Point Ammo EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600169) != 0) UPDATE PB_BT_SShopItem SET Name =N'Hollow Point Ammo EV' WHERE ItemId = 1600169
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600170 AND GoodsId = 160017000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600170,160017000,N'Full Metal Jacket Ammo EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600170) != 0) UPDATE PB_BT_SShopItem SET Name =N'Full Metal Jacket Ammo EV' WHERE ItemId = 1600170
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600172 AND GoodsId = 160017200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600172,160017200,N'Mig_Reinforce item')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600172) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mig_Reinforce item' WHERE ItemId = 1600172
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600173 AND GoodsId = 160017300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600173,160017300,N'Mig_Control Increase')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600173) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mig_Control Increase' WHERE ItemId = 1600173
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600174 AND GoodsId = 160017400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600174,160017400,N'Mig_Penet. Increase')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600174) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mig_Penet. Increase' WHERE ItemId = 1600174
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600175 AND GoodsId = 160017500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600175,160017500,N'Mig_Accuracy Increase')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600175) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mig_Accuracy Increase' WHERE ItemId = 1600175
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600176 AND GoodsId = 160017600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600176,160017600,N'Mig_Reaction Decrease')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600176) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mig_Reaction Decrease' WHERE ItemId = 1600176
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600177 AND GoodsId = 160017700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600177,160017700,N'Mig_Damage Increase')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600177) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mig_Damage Increase' WHERE ItemId = 1600177
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600178 AND GoodsId = 160017800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600178,160017800,N'Mig_Defense Increase')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600178) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mig_Defense Increase' WHERE ItemId = 1600178
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600180 AND GoodsId = 160018000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600180,160018000,N'Skill Time Boost X2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600180) != 0) UPDATE PB_BT_SShopItem SET Name =N'Skill Time Boost X2' WHERE ItemId = 1600180
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600181 AND GoodsId = 160018100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600181,160018100,N'Captain Armband')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600181) != 0) UPDATE PB_BT_SShopItem SET Name =N'Captain Armband' WHERE ItemId = 1600181
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600182 AND GoodsId = 160018200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600182,160018200,N'Short Respawn 100% EV')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600182) != 0) UPDATE PB_BT_SShopItem SET Name =N'Short Respawn 100% EV' WHERE ItemId = 1600182
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600183 AND GoodsId = 160018300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600183,160018300,N'Short Respawn 50% GW')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600183) != 0) UPDATE PB_BT_SShopItem SET Name =N'Short Respawn 50% GW' WHERE ItemId = 1600183
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1600185 AND GoodsId = 160018500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600185,160018500,N'Ammo Up 10% GW')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1600185) != 0) UPDATE PB_BT_SShopItem SET Name =N'Ammo Up 10% GW' WHERE ItemId = 1600185
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800039 AND GoodsId = 180003900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800039,180003900,N'CS Giga Bomb')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800039) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Giga Bomb' WHERE ItemId = 1800039
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800041 AND GoodsId = 180004100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800041,180004100,N'CS Defcon1 Pack')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800041) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Defcon1 Pack' WHERE ItemId = 1800041
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800042 AND GoodsId = 180004200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800042,180004200,N'CS Defcon2 Pack')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800042) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Defcon2 Pack' WHERE ItemId = 1800042
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800043 AND GoodsId = 180004300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800043,180004300,N'CS Defcon3 Pack')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800043) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Defcon3 Pack' WHERE ItemId = 1800043
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800045 AND GoodsId = 180004500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800045,180004500,N'CS Weapon Crate 2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800045) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Weapon Crate 2' WHERE ItemId = 1800045
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800046 AND GoodsId = 180004600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800046,180004600,N'CS Mini Bomb')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800046) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Mini Bomb' WHERE ItemId = 1800046
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800047 AND GoodsId = 180004700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800047,180004700,N'Change Nickname')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800047) != 0) UPDATE PB_BT_SShopItem SET Name =N'Change Nickname' WHERE ItemId = 1800047
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800048 AND GoodsId = 180004800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800048,180004800,N'Clear History')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800048) != 0) UPDATE PB_BT_SShopItem SET Name =N'Clear History' WHERE ItemId = 1800048
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800049 AND GoodsId = 180004900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800049,180004900,N'Clear Score')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800049) != 0) UPDATE PB_BT_SShopItem SET Name =N'Clear Score' WHERE ItemId = 1800049
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800050 AND GoodsId = 180005000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800050,180005000,N'Reset Escape')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800050) != 0) UPDATE PB_BT_SShopItem SET Name =N'Reset Escape' WHERE ItemId = 1800050
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800051 AND GoodsId = 180005100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800051,180005100,N'Change Clan Name')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800051) != 0) UPDATE PB_BT_SShopItem SET Name =N'Change Clan Name' WHERE ItemId = 1800051
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800052 AND GoodsId = 180005200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800052,180005200,N'Change Clan Mark')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800052) != 0) UPDATE PB_BT_SShopItem SET Name =N'Change Clan Mark' WHERE ItemId = 1800052
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800053 AND GoodsId = 180005300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800053,180005300,N'Clear Clan History')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800053) != 0) UPDATE PB_BT_SShopItem SET Name =N'Clear Clan History' WHERE ItemId = 1800053
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800055 AND GoodsId = 180005500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800055,180005500,N'Clan Member +50')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800055) != 0) UPDATE PB_BT_SShopItem SET Name =N'Clan Member +50' WHERE ItemId = 1800055
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800056 AND GoodsId = 180005600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800056,180005600,N'Clear Clan Point')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800056) != 0) UPDATE PB_BT_SShopItem SET Name =N'Clear Clan Point' WHERE ItemId = 1800056
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800058 AND GoodsId = 180005800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800058,180005800,N'CS Mega Bomb')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800058) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Mega Bomb' WHERE ItemId = 1800058
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800059 AND GoodsId = 180005900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800059,180005900,N'CS Weapon Crate')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800059) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Weapon Crate' WHERE ItemId = 1800059
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800060 AND GoodsId = 180006000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800060,180006000,N'CS Survival Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800060) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Survival Kit' WHERE ItemId = 1800060
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800061 AND GoodsId = 180006100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800061,180006100,N'Mega Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800061) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mega Box' WHERE ItemId = 1800061
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800066 AND GoodsId = 180006600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800066,180006600,N'CS Point Cachpon1')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800066) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Point Cachpon1' WHERE ItemId = 1800066
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800067 AND GoodsId = 180006700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800067,180006700,N'CS Point Gachpon2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800067) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Point Gachpon2' WHERE ItemId = 1800067
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800068 AND GoodsId = 180006800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800068,180006800,N'CS Giga Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800068) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Giga Box' WHERE ItemId = 1800068
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800069 AND GoodsId = 180006900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800069,180006900,N'CS Swat Kit(Assault) 7D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800069) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Swat Kit(Assault) 7D' WHERE ItemId = 1800069
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800070 AND GoodsId = 180007000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800070,180007000,N'CS Swat Kit(Assault) 30D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800070) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Swat Kit(Assault) 30D' WHERE ItemId = 1800070
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800071 AND GoodsId = 180007100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800071,180007100,N'CS Swat Kit(Support) 30D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800071) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Swat Kit(Support) 30D' WHERE ItemId = 1800071
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800072 AND GoodsId = 180007200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800072,180007200,N'CS Swat Kit(Support) 7D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800072) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Swat Kit(Support) 7D' WHERE ItemId = 1800072
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800073 AND GoodsId = 180007300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800073,180007300,N'CS Swat Kit Infiltrat 7D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800073) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Swat Kit Infiltrat 7D' WHERE ItemId = 1800073
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800074 AND GoodsId = 180007400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800074,180007400,N'CS Swat kit lnfiltrat 30D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800074) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Swat kit lnfiltrat 30D' WHERE ItemId = 1800074
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800075 AND GoodsId = 180007500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800075,180007500,N'CS Swat Kit(Combo) 30D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800075) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Swat Kit(Combo) 30D' WHERE ItemId = 1800075
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800076 AND GoodsId = 180007600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800076,180007600,N'CS Swat Kit(Combo) 7D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800076) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Swat Kit(Combo) 7D' WHERE ItemId = 1800076
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800081 AND GoodsId = 180008100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800081,180008100,N'CS Mega Bomb Russia')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800081) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Mega Bomb Russia' WHERE ItemId = 1800081
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800082 AND GoodsId = 180008200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800082,180008200,N'CS Item Box1')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800082) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Item Box1' WHERE ItemId = 1800082
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800083 AND GoodsId = 180008300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800083,180008300,N'CS Item Box2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800083) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Item Box2' WHERE ItemId = 1800083
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800084 AND GoodsId = 180008400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800084,180008400,N'CS Angpao')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800084) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Angpao' WHERE ItemId = 1800084
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800085 AND GoodsId = 180008500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800085,180008500,N'View Other User Info')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800085) != 0) UPDATE PB_BT_SShopItem SET Name =N'View Other User Info' WHERE ItemId = 1800085
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800087 AND GoodsId = 180008700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800087,180008700,N'CS Gift Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800087) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Gift Box' WHERE ItemId = 1800087
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800091 AND GoodsId = 180009100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800091,180009100,N'Ketupat')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800091) != 0) UPDATE PB_BT_SShopItem SET Name =N'Ketupat' WHERE ItemId = 1800091
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800092 AND GoodsId = 180009200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800092,180009200,N'CS Random Mask Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800092) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Random Mask Box' WHERE ItemId = 1800092
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800101 AND GoodsId = 180010100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800101,180010100,N'Valentine Chocolate')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800101) != 0) UPDATE PB_BT_SShopItem SET Name =N'Valentine Chocolate' WHERE ItemId = 1800101
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800108 AND GoodsId = 180010800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800108,180010800,N'CS Che Guevara')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800108) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Che Guevara' WHERE ItemId = 1800108
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800111 AND GoodsId = 180011100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800111,180011100,N'CS Ready Box Silver Latin')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800111) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Ready Box Silver Latin' WHERE ItemId = 1800111
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800112 AND GoodsId = 180011200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800112,180011200,N'CS Ready Box Gold Latin')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800112) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Ready Box Gold Latin' WHERE ItemId = 1800112
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800113 AND GoodsId = 180011300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800113,180011300,N'CS Ready Box Normal Latin')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800113) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Ready Box Normal Latin' WHERE ItemId = 1800113
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800120 AND GoodsId = 180012000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800120,180012000,N'CS Point Bomb')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800120) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Point Bomb' WHERE ItemId = 1800120
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800121 AND GoodsId = 180012100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800121,180012100,N'CS Point Bomb Premium')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800121) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Point Bomb Premium' WHERE ItemId = 1800121
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800122 AND GoodsId = 180012200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800122,180012200,N'CS Ready Box Normal Prize')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800122) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Ready Box Normal Prize' WHERE ItemId = 1800122
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800123 AND GoodsId = 180012300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800123,180012300,N'CS Ready Box Silver Prize')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800123) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Ready Box Silver Prize' WHERE ItemId = 1800123
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800124 AND GoodsId = 180012400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800124,180012400,N'CS Ready Box Gold Prize')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800124) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Ready Box Gold Prize' WHERE ItemId = 1800124
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800125 AND GoodsId = 180012500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800125,180012500,N'CS Ready Box Silver')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800125) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Ready Box Silver' WHERE ItemId = 1800125
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800126 AND GoodsId = 180012600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800126,180012600,N'CS Ready Box Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800126) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Ready Box Gold' WHERE ItemId = 1800126
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800127 AND GoodsId = 180012700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800127,180012700,N'CS Ready Box Normal')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800127) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Ready Box Normal' WHERE ItemId = 1800127
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800128 AND GoodsId = 180012800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800128,180012800,N'Prize Money 100')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800128) != 0) UPDATE PB_BT_SShopItem SET Name =N'Prize Money 100' WHERE ItemId = 1800128
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800129 AND GoodsId = 180012900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800129,180012900,N'CS Christmas Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800129) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Christmas Box' WHERE ItemId = 1800129
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800130 AND GoodsId = 180013000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800130,180013000,N'CS Gift Box North')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800130) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Gift Box North' WHERE ItemId = 1800130
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800131 AND GoodsId = 180013100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800131,180013100,N'CS Facebook Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800131) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Facebook Box' WHERE ItemId = 1800131
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800132 AND GoodsId = 180013200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800132,180013200,N'CS Raptor Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800132) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Raptor Box' WHERE ItemId = 1800132
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800133 AND GoodsId = 180013300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800133,180013300,N'CS X-File Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800133) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS X-File Box' WHERE ItemId = 1800133
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800171 AND GoodsId = 180017100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800171,180017100,N'Clan Person Plus 10')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800171) != 0) UPDATE PB_BT_SShopItem SET Name =N'Clan Person Plus 10' WHERE ItemId = 1800171
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800179 AND GoodsId = 180017900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800179,180017900,N'Skill Master Lv Ticket')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800179) != 0) UPDATE PB_BT_SShopItem SET Name =N'Skill Master Lv Ticket' WHERE ItemId = 1800179
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800183 AND GoodsId = 180018300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800183,180018300,N'MegaPhone')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800183) != 0) UPDATE PB_BT_SShopItem SET Name =N'MegaPhone' WHERE ItemId = 1800183
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800500 AND GoodsId = 180050000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800500,180050000,N'CS Random Box Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800500) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Random Box Gold' WHERE ItemId = 1800500
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800501 AND GoodsId = 180050100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800501,180050100,N'CS Random Box Black')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800501) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Random Box Black' WHERE ItemId = 1800501
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800502 AND GoodsId = 180050200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800502,180050200,N'CS Gatotkaca Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800502) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Gatotkaca Box' WHERE ItemId = 1800502
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800503 AND GoodsId = 180050300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800503,180050300,N'CS Present Box Latin')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800503) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Present Box Latin' WHERE ItemId = 1800503
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800504 AND GoodsId = 180050400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800504,180050400,N'CS Random Box Black RU')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800504) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Random Box Black RU' WHERE ItemId = 1800504
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800505 AND GoodsId = 180050500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800505,180050500,N'CS Random Box Gold RU')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800505) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Random Box Gold RU' WHERE ItemId = 1800505
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800507 AND GoodsId = 180050700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800507,180050700,N'CS Unique Random Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800507) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Unique Random Box' WHERE ItemId = 1800507
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800513 AND GoodsId = 180051300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800513,180051300,N'CS Tournament Ticket Sl')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800513) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Tournament Ticket Sl' WHERE ItemId = 1800513
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800514 AND GoodsId = 180051400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800514,180051400,N'CS Tournament Ticket G')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800514) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Tournament Ticket G' WHERE ItemId = 1800514
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800515 AND GoodsId = 180051500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800515,180051500,N'CS Tournament Ticket On')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800515) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Tournament Ticket On' WHERE ItemId = 1800515
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800516 AND GoodsId = 180051600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800516,180051600,N'CS Super Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800516) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Super Box' WHERE ItemId = 1800516
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800517 AND GoodsId = 180051700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800517,180051700,N'CS PBNC4 Supplykit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800517) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS PBNC4 Supplykit' WHERE ItemId = 1800517
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800518 AND GoodsId = 180051800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800518,180051800,N'CS Point Random Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800518) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Point Random Box' WHERE ItemId = 1800518
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800519 AND GoodsId = 180051900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800519,180051900,N'CS Garena Premium Gacha A')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800519) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Garena Premium Gacha A' WHERE ItemId = 1800519
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800520 AND GoodsId = 180052000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800520,180052000,N'CS Garena Premium Gacha B')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800520) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Garena Premium Gacha B' WHERE ItemId = 1800520
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800521 AND GoodsId = 180052100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800521,180052100,N'CS Ramadan Random Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800521) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Ramadan Random Box' WHERE ItemId = 1800521
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800522 AND GoodsId = 180052200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800522,180052200,N'CS Super Box ID')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800522) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Super Box ID' WHERE ItemId = 1800522
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800526 AND GoodsId = 180052600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800526,180052600,N'CS PBIC2012 RandomBox')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800526) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS PBIC2012 RandomBox' WHERE ItemId = 1800526
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800527 AND GoodsId = 180052700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800527,180052700,N'CS Golden Box TR')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800527) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Golden Box TR' WHERE ItemId = 1800527
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800528 AND GoodsId = 180052800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800528,180052800,N'CS GRS Supply Kit A')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800528) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS GRS Supply Kit A' WHERE ItemId = 1800528
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800529 AND GoodsId = 180052900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800529,180052900,N'CS GRS Supply Kit B')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800529) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS GRS Supply Kit B' WHERE ItemId = 1800529
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800530 AND GoodsId = 180053000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800530,180053000,N'CS Elite Gacha TH Assault')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800530) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Elite Gacha TH Assault' WHERE ItemId = 1800530
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800531 AND GoodsId = 180053100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800531,180053100,N'CS Garena Premium Gacha D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800531) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Garena Premium Gacha D' WHERE ItemId = 1800531
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800532 AND GoodsId = 180053200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800532,180053200,N'CS Crossover Supply Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800532) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Crossover Supply Kit' WHERE ItemId = 1800532
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800533 AND GoodsId = 180053300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800533,180053300,N'CS Friendship Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800533) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Friendship Box' WHERE ItemId = 1800533
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800534 AND GoodsId = 180053400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800534,180053400,N'CS BOB Gacha TH')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800534) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS BOB Gacha TH' WHERE ItemId = 1800534
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800535 AND GoodsId = 180053500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800535,180053500,N'CS Neon RandomBox')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800535) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Neon RandomBox' WHERE ItemId = 1800535
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800536 AND GoodsId = 180053600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800536,180053600,N'CS Neon RandomBox MENA')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800536) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Neon RandomBox MENA' WHERE ItemId = 1800536
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800537 AND GoodsId = 180053700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800537,180053700,N'CS Vacation Supply Kit 2013')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800537) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Vacation Supply Kit 2013' WHERE ItemId = 1800537
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800538 AND GoodsId = 180053800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800538,180053800,N'CS BOB Gacha ID')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800538) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS BOB Gacha ID' WHERE ItemId = 1800538
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800539 AND GoodsId = 180053900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800539,180053900,N'CS PC Cafe Gacha')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800539) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS PC Cafe Gacha' WHERE ItemId = 1800539
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800540 AND GoodsId = 180054000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800540,180054000,N'CS Zepetto Main Supply Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800540) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Zepetto Main Supply Kit' WHERE ItemId = 1800540
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800541 AND GoodsId = 180054100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800541,180054100,N'CS Zepetto Sec. Supply Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800541) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Zepetto Sec. Supply Kit' WHERE ItemId = 1800541
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800542 AND GoodsId = 180054200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800542,180054200,N'CS Zepetto Melee Supply Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800542) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Zepetto Melee Supply Kit' WHERE ItemId = 1800542
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800543 AND GoodsId = 180054300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800543,180054300,N'CS Zepetto Mask Supply Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800543) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Zepetto Mask Supply Kit' WHERE ItemId = 1800543
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800544 AND GoodsId = 180054400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800544,180054400,N'CS Sochi2014 Supply Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800544) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Sochi2014 Supply Kit' WHERE ItemId = 1800544
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800545 AND GoodsId = 180054500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800545,180054500,N'CS WC 2014 RandomBox A')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800545) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS WC 2014 RandomBox A' WHERE ItemId = 1800545
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800546 AND GoodsId = 180054600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800546,180054600,N'CS WC 2014 RandomBox B')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800546) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS WC 2014 RandomBox B' WHERE ItemId = 1800546
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800547 AND GoodsId = 180054700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800547,180054700,N'CS WC 2014 RandomBox C')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800547) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS WC 2014 RandomBox C' WHERE ItemId = 1800547
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800548 AND GoodsId = 180054800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800548,180054800,N'CS WC 2014 RandomBox D')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800548) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS WC 2014 RandomBox D' WHERE ItemId = 1800548
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800549 AND GoodsId = 180054900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800549,180054900,N'CS PBNC5 Supply Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800549) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS PBNC5 Supply Kit' WHERE ItemId = 1800549
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800555 AND GoodsId = 180055500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800555,180055500,N'CS RandomBox Black 1.5')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800555) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS RandomBox Black 1.5' WHERE ItemId = 1800555
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800558 AND GoodsId = 180055800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800558,180055800,N'CS WC Ticket')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800558) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS WC Ticket' WHERE ItemId = 1800558
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800559 AND GoodsId = 180055900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800559,180055900,N'CS WC RedCard')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800559) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS WC RedCard' WHERE ItemId = 1800559
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800560 AND GoodsId = 180056000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800560,180056000,N'CS WC YellowCard')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800560) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS WC YellowCard' WHERE ItemId = 1800560
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800561 AND GoodsId = 180056100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800561,180056100,N'CS Brazil Brazuca Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800561) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS Brazil Brazuca Kit' WHERE ItemId = 1800561
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800562 AND GoodsId = 180056200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800562,180056200,N'CS WC 2014 Event Gacha')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800562) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS WC 2014 Event Gacha' WHERE ItemId = 1800562
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800563 AND GoodsId = 180056300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800563,180056300,N'Sol Festival RandomBox')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800563) != 0) UPDATE PB_BT_SShopItem SET Name =N'Sol Festival RandomBox' WHERE ItemId = 1800563
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800564 AND GoodsId = 180056400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800564,180056400,N'LAM Ramadan RandomBox')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800564) != 0) UPDATE PB_BT_SShopItem SET Name =N'LAM Ramadan RandomBox' WHERE ItemId = 1800564
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800566 AND GoodsId = 180056600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800566,180056600,N'CS PBIC2013 Gachapon')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800566) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS PBIC2013 Gachapon' WHERE ItemId = 1800566
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800568 AND GoodsId = 180056800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800568,180056800,N'CS PBIC2012 Gachapon PH')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800568) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS PBIC2012 Gachapon PH' WHERE ItemId = 1800568
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800569 AND GoodsId = 180056900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800569,180056900,N'CS PBIC2013 Gachapon PH')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800569) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS PBIC2013 Gachapon PH' WHERE ItemId = 1800569
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800578 AND GoodsId = 180057800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800578,180057800,N'Assault Rifle Gachapon')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800578) != 0) UPDATE PB_BT_SShopItem SET Name =N'Assault Rifle Gachapon' WHERE ItemId = 1800578
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800579 AND GoodsId = 180057900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800579,180057900,N'Submachine Gun Gachapon')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800579) != 0) UPDATE PB_BT_SShopItem SET Name =N'Submachine Gun Gachapon' WHERE ItemId = 1800579
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800580 AND GoodsId = 180058000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800580,180058000,N'PSG1 Elite Supply Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800580) != 0) UPDATE PB_BT_SShopItem SET Name =N'PSG1 Elite Supply Kit' WHERE ItemId = 1800580
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800581 AND GoodsId = 180058100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800581,180058100,N'VSK94 Elite Supply Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800581) != 0) UPDATE PB_BT_SShopItem SET Name =N'VSK94 Elite Supply Kit' WHERE ItemId = 1800581
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800582 AND GoodsId = 180058200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800582,180058200,N'SVU Elite Supply Kit')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800582) != 0) UPDATE PB_BT_SShopItem SET Name =N'SVU Elite Supply Kit' WHERE ItemId = 1800582
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800583 AND GoodsId = 180058300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800583,180058300,N'Garena`s P90 G Gacha')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800583) != 0) UPDATE PB_BT_SShopItem SET Name =N'Garena`s P90 G Gacha' WHERE ItemId = 1800583
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800584 AND GoodsId = 180058400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800584,180058400,N'M1887 Lion Heart Gacha')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800584) != 0) UPDATE PB_BT_SShopItem SET Name =N'M1887 Lion Heart Gacha' WHERE ItemId = 1800584
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800585 AND GoodsId = 180058500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800585,180058500,N'Best of Best 2014 Gacha')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800585) != 0) UPDATE PB_BT_SShopItem SET Name =N'Best of Best 2014 Gacha' WHERE ItemId = 1800585
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800586 AND GoodsId = 180058600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800586,180058600,N'Mini Gachapon')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800586) != 0) UPDATE PB_BT_SShopItem SET Name =N'Mini Gachapon' WHERE ItemId = 1800586
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800587 AND GoodsId = 180058700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800587,180058700,N'CS X-mas RandomBox')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800587) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS X-mas RandomBox' WHERE ItemId = 1800587
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800588 AND GoodsId = 180058800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800588,180058800,N'CS X-mas GiftBox')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800588) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS X-mas GiftBox' WHERE ItemId = 1800588
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800589 AND GoodsId = 180058900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800589,180058900,N'CS 2015 NY SupplyKit_ID')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800589) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS 2015 NY SupplyKit_ID' WHERE ItemId = 1800589
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800590 AND GoodsId = 180059000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800590,180059000,N'Best of 2014 Gacha')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800590) != 0) UPDATE PB_BT_SShopItem SET Name =N'Best of 2014 Gacha' WHERE ItemId = 1800590
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800591 AND GoodsId = 180059100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800591,180059100,N'Collection X-mas Gacha')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800591) != 0) UPDATE PB_BT_SShopItem SET Name =N'Collection X-mas Gacha' WHERE ItemId = 1800591
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800592 AND GoodsId = 180059200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800592,180059200,N'E-sport Random Box A')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800592) != 0) UPDATE PB_BT_SShopItem SET Name =N'E-sport Random Box A' WHERE ItemId = 1800592
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800593 AND GoodsId = 180059300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800593,180059300,N'E-sport Random Box B')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800593) != 0) UPDATE PB_BT_SShopItem SET Name =N'E-sport Random Box B' WHERE ItemId = 1800593
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800594 AND GoodsId = 180059400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800594,180059400,N'PBIC2013 Random Box A')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800594) != 0) UPDATE PB_BT_SShopItem SET Name =N'PBIC2013 Random Box A' WHERE ItemId = 1800594
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800595 AND GoodsId = 180059500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800595,180059500,N'PBIC2013 Random Box B')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800595) != 0) UPDATE PB_BT_SShopItem SET Name =N'PBIC2013 Random Box B' WHERE ItemId = 1800595
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800596 AND GoodsId = 180059600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800596,180059600,N'CS X-mas RandomBox RU')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800596) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS X-mas RandomBox RU' WHERE ItemId = 1800596
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800597 AND GoodsId = 180059700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800597,180059700,N'CS NY Random Box TR')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800597) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS NY Random Box TR' WHERE ItemId = 1800597
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800598 AND GoodsId = 180059800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800598,180059800,N'CS 1st Anniversary Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800598) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS 1st Anniversary Box' WHERE ItemId = 1800598
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800602 AND GoodsId = 180060200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800602,180060200,N'CS sorry random box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800602) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS sorry random box' WHERE ItemId = 1800602
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800603 AND GoodsId = 180060300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800603,180060300,N'CS PaysafeRandombox Mini')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800603) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS PaysafeRandombox Mini' WHERE ItemId = 1800603
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800604 AND GoodsId = 180060400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800604,180060400,N'CS PaysafeRandombox Mega')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800604) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS PaysafeRandombox Mega' WHERE ItemId = 1800604
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800605 AND GoodsId = 180060500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800605,180060500,N'Sorry RandomBox2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800605) != 0) UPDATE PB_BT_SShopItem SET Name =N'Sorry RandomBox2' WHERE ItemId = 1800605
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800606 AND GoodsId = 180060600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800606,180060600,N'Thanks RandomBox 1')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800606) != 0) UPDATE PB_BT_SShopItem SET Name =N'Thanks RandomBox 1' WHERE ItemId = 1800606
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800607 AND GoodsId = 180060700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800607,180060700,N'Thanks RandomBox 2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800607) != 0) UPDATE PB_BT_SShopItem SET Name =N'Thanks RandomBox 2' WHERE ItemId = 1800607
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800608 AND GoodsId = 180060800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800608,180060800,N'New Year AUG A3 Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800608) != 0) UPDATE PB_BT_SShopItem SET Name =N'New Year AUG A3 Box' WHERE ItemId = 1800608
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800609 AND GoodsId = 180060900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800609,180060900,N'CS AUG A3 kasasi Random Box')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800609) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS AUG A3 kasasi Random Box' WHERE ItemId = 1800609
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800611 AND GoodsId = 180061100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800611,180061100,N'CS CNY 2015 AUG Gacha')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800611) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS CNY 2015 AUG Gacha' WHERE ItemId = 1800611
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 1800612 AND GoodsId = 180061200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800612,180061200,N'CS CNY 2015 Kriss Gacha')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 1800612) != 0) UPDATE PB_BT_SShopItem SET Name =N'CS CNY 2015 Kriss Gacha' WHERE ItemId = 1800612
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000001 AND GoodsId = 200000100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000001,200000100,N'Point 100')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000001) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 100' WHERE ItemId = 2000001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000002 AND GoodsId = 200000200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000002,200000200,N'Point 200')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000002) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 200' WHERE ItemId = 2000002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000003 AND GoodsId = 200000300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000003,200000300,N'Point 300')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000003) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 300' WHERE ItemId = 2000003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000004 AND GoodsId = 200000400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000004,200000400,N'Point 400')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000004) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 400' WHERE ItemId = 2000004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000005 AND GoodsId = 200000500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000005,200000500,N'Point 500')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000005) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 500' WHERE ItemId = 2000005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000006 AND GoodsId = 200000600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000006,200000600,N'Point 600')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000006) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 600' WHERE ItemId = 2000006
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000007 AND GoodsId = 200000700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000007,200000700,N'Point 700')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000007) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 700' WHERE ItemId = 2000007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000008 AND GoodsId = 200000800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000008,200000800,N'Point 800')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000008) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 800' WHERE ItemId = 2000008
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000009 AND GoodsId = 200000900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000009,200000900,N'Point 900')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000009) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 900' WHERE ItemId = 2000009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000010 AND GoodsId = 200001000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000010,200001000,N'Point 1,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000010) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 1,000' WHERE ItemId = 2000010
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000015 AND GoodsId = 200001500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000015,200001500,N'Point 1,500')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000015) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 1,500' WHERE ItemId = 2000015
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000020 AND GoodsId = 200002000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000020,200002000,N'Point 2,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000020) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 2,000' WHERE ItemId = 2000020
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000025 AND GoodsId = 200002500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000025,200002500,N'Point 2,500')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000025) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 2,500' WHERE ItemId = 2000025
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000030 AND GoodsId = 200003000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000030,200003000,N'Point 3,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000030) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 3,000' WHERE ItemId = 2000030
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000032 AND GoodsId = 200003200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000032,200003200,N'Point 3,200')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000032) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 3,200' WHERE ItemId = 2000032
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000035 AND GoodsId = 200003500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000035,200003500,N'Point 3,500')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000035) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 3,500' WHERE ItemId = 2000035
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000040 AND GoodsId = 200004000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000040,200004000,N'Point 4,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000040) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 4,000' WHERE ItemId = 2000040
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000045 AND GoodsId = 200004500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000045,200004500,N'Point 4,500')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000045) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 4,500' WHERE ItemId = 2000045
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000050 AND GoodsId = 200005000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000050,200005000,N'Point 5,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000050) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 5,000' WHERE ItemId = 2000050
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000055 AND GoodsId = 200005500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000055,200005500,N'Point 5,500')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000055) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 5,500' WHERE ItemId = 2000055
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000060 AND GoodsId = 200006000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000060,200006000,N'Point 6,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000060) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 6,000' WHERE ItemId = 2000060
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000065 AND GoodsId = 200006500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000065,200006500,N'Point 6,500')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000065) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 6,500' WHERE ItemId = 2000065
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000070 AND GoodsId = 200007000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000070,200007000,N'Point 7,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000070) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 7,000' WHERE ItemId = 2000070
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000075 AND GoodsId = 200007500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000075,200007500,N'Point 7,500')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000075) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 7,500' WHERE ItemId = 2000075
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000080 AND GoodsId = 200008000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000080,200008000,N'Point 8,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000080) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 8,000' WHERE ItemId = 2000080
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000085 AND GoodsId = 200008500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000085,200008500,N'Point 8,500')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000085) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 8,500' WHERE ItemId = 2000085
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000090 AND GoodsId = 200009000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000090,200009000,N'Point 9,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000090) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 9,000' WHERE ItemId = 2000090
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000095 AND GoodsId = 200009500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000095,200009500,N'Point 9,500')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000095) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 9,500' WHERE ItemId = 2000095
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000100 AND GoodsId = 200010000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000100,200010000,N'Point 10,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000100) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 10,000' WHERE ItemId = 2000100
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000110 AND GoodsId = 200011000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000110,200011000,N'Point 11,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000110) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 11,000' WHERE ItemId = 2000110
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000120 AND GoodsId = 200012000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000120,200012000,N'Point 12,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000120) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 12,000' WHERE ItemId = 2000120
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000125 AND GoodsId = 200012500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000125,200012500,N'Point 12,500')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000125) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 12,500' WHERE ItemId = 2000125
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000130 AND GoodsId = 200013000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000130,200013000,N'Point 13,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000130) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 13,000' WHERE ItemId = 2000130
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000140 AND GoodsId = 200014000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000140,200014000,N'Point 14,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000140) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 14,000' WHERE ItemId = 2000140
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000150 AND GoodsId = 200015000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000150,200015000,N'Point 15,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000150) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 15,000' WHERE ItemId = 2000150
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000160 AND GoodsId = 200016000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000160,200016000,N'Point 16,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000160) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 16,000' WHERE ItemId = 2000160
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000170 AND GoodsId = 200017000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000170,200017000,N'Point 17,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000170) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 17,000' WHERE ItemId = 2000170
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000180 AND GoodsId = 200018000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000180,200018000,N'Point 18,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000180) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 18,000' WHERE ItemId = 2000180
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000190 AND GoodsId = 200019000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000190,200019000,N'Point 19,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000190) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 19,000' WHERE ItemId = 2000190
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000200 AND GoodsId = 200020000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000200,200020000,N'Point 20,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000200) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 20,000' WHERE ItemId = 2000200
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000220 AND GoodsId = 200022000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000220,200022000,N'Point 22,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000220) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 22,000' WHERE ItemId = 2000220
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000240 AND GoodsId = 200024000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000240,200024000,N'Point 24,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000240) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 24,000' WHERE ItemId = 2000240
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000250 AND GoodsId = 200025000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000250,200025000,N'Point 25,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000250) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 25,000' WHERE ItemId = 2000250
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000280 AND GoodsId = 200028000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000280,200028000,N'Point 28,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000280) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 28,000' WHERE ItemId = 2000280
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000300 AND GoodsId = 200030000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000300,200030000,N'Point 30,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000300) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 30,000' WHERE ItemId = 2000300
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000320 AND GoodsId = 200032000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000320,200032000,N'Point 32,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000320) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 32,000' WHERE ItemId = 2000320
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000350 AND GoodsId = 200035000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000350,200035000,N'Point 35,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000350) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 35,000' WHERE ItemId = 2000350
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000360 AND GoodsId = 200036000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000360,200036000,N'Point 36,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000360) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 36,000' WHERE ItemId = 2000360
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000400 AND GoodsId = 200040000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000400,200040000,N'Point 40,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000400) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 40,000' WHERE ItemId = 2000400
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000450 AND GoodsId = 200045000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000450,200045000,N'Point 45,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000450) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 45,000' WHERE ItemId = 2000450
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000500 AND GoodsId = 200050000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000500,200050000,N'Point 50,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000500) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 50,000' WHERE ItemId = 2000500
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000550 AND GoodsId = 200055000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000550,200055000,N'Point 55,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000550) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 55,000' WHERE ItemId = 2000550
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000600 AND GoodsId = 200060000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000600,200060000,N'Point 60,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000600) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 60,000' WHERE ItemId = 2000600
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000700 AND GoodsId = 200070000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000700,200070000,N'Point 70,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000700) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 70,000' WHERE ItemId = 2000700
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2000850 AND GoodsId = 200085000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000850,200085000,N'Point 85,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2000850) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 85,000' WHERE ItemId = 2000850
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2001000 AND GoodsId = 200100000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2001000,200100000,N'Point 100,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2001000) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 100,000' WHERE ItemId = 2001000
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2002000 AND GoodsId = 200200000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2002000,200200000,N'Point 200,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2002000) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 200,000' WHERE ItemId = 2002000
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2002500 AND GoodsId = 200250000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2002500,200250000,N'Point 250,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2002500) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 250,000' WHERE ItemId = 2002500
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2003000 AND GoodsId = 200300000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2003000,200300000,N'Point 300,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2003000) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 300,000' WHERE ItemId = 2003000
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2005000 AND GoodsId = 200500000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2005000,200500000,N'Point 500,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2005000) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 500,000' WHERE ItemId = 2005000
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2010000 AND GoodsId = 201000000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2010000,201000000,N'Point 1,000,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2010000) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 1,000,000' WHERE ItemId = 2010000
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2030000 AND GoodsId = 203000000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2030000,203000000,N'Point 3,000,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2030000) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 3,000,000' WHERE ItemId = 2030000
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2080000 AND GoodsId = 208000000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2080000,208000000,N'Point 8,000,000')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2080000) != 0) UPDATE PB_BT_SShopItem SET Name =N'Point 8,000,000' WHERE ItemId = 2080000
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2200001 AND GoodsId = 220000100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2200001,220000100,N'RSTab1')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2200001) != 0) UPDATE PB_BT_SShopItem SET Name =N'RSTab1' WHERE ItemId = 2200001
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2200002 AND GoodsId = 220000200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2200002,220000200,N'RSTab2')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2200002) != 0) UPDATE PB_BT_SShopItem SET Name =N'RSTab2' WHERE ItemId = 2200002
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2200003 AND GoodsId = 220000300 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2200003,220000300,N'RSTab3')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2200003) != 0) UPDATE PB_BT_SShopItem SET Name =N'RSTab3' WHERE ItemId = 2200003
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2600004 AND GoodsId = 260000400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2600004,260000400,N'QC Gold')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600004) != 0) UPDATE PB_BT_SShopItem SET Name =N'QC Gold' WHERE ItemId = 2600004
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2600005 AND GoodsId = 260000500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2600005,260000500,N'QC Assault')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600005) != 0) UPDATE PB_BT_SShopItem SET Name =N'QC Assault' WHERE ItemId = 2600005
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2600006 AND GoodsId = 260000600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2600006,260000600,N'QC BackUp')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600006) != 0) UPDATE PB_BT_SShopItem SET Name =N'QC BackUp' WHERE ItemId = 2600006
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2600007 AND GoodsId = 260000700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2600007,260000700,N'QC Infiltration')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600007) != 0) UPDATE PB_BT_SShopItem SET Name =N'QC Infiltration' WHERE ItemId = 2600007
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2600008 AND GoodsId = 260000800 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2600008,260000800,N'QC Special')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600008) != 0) UPDATE PB_BT_SShopItem SET Name =N'QC Special' WHERE ItemId = 2600008
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2600009 AND GoodsId = 260000900 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2600009,260000900,N'QC Defcon')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600009) != 0) UPDATE PB_BT_SShopItem SET Name =N'QC Defcon' WHERE ItemId = 2600009
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2600010 AND GoodsId = 260001000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2600010,260001000,N'QC Commissioned O')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600010) != 0) UPDATE PB_BT_SShopItem SET Name =N'QC Commissioned O' WHERE ItemId = 2600010
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2600011 AND GoodsId = 260001100 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2600011,260001100,N'QC Company O')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600011) != 0) UPDATE PB_BT_SShopItem SET Name =N'QC Company O' WHERE ItemId = 2600011
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2600012 AND GoodsId = 260001200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2600012,260001200,N'QC Field O')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600012) != 0) UPDATE PB_BT_SShopItem SET Name =N'QC Field O' WHERE ItemId = 2600012
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2600014 AND GoodsId = 260001400 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2600014,260001400,N'QC Dino 2ND')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600014) != 0) UPDATE PB_BT_SShopItem SET Name =N'QC Dino 2ND' WHERE ItemId = 2600014
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2600015 AND GoodsId = 260001500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2600015,260001500,N'QC Human 2ND')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600015) != 0) UPDATE PB_BT_SShopItem SET Name =N'QC Human 2ND' WHERE ItemId = 2600015
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2600016 AND GoodsId = 260001600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2600016,260001600,N'QC Dino 3RD')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600016) != 0) UPDATE PB_BT_SShopItem SET Name =N'QC Dino 3RD' WHERE ItemId = 2600016
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE ItemID = 2600017 AND GoodsId = 260001700 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2600017,260001700,N'QC Human 3RD')
IF((SELECT COUNT(*) FROM PB_BT_SShopItem WHERE ItemID = 2600017) != 0) UPDATE PB_BT_SShopItem SET Name =N'QC Human 3RD' WHERE ItemId = 2600017
GO

/***************************************
* UPDATE SET MATCHING INFO 
***************************************/
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000100 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990000100
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000100 AND ItemId = 1600011 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600011,990000100,N'Free Move +20000P')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000100 AND ItemId = 2000200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000200,990000100,N'Free Move +20000P')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000100) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Free Move +20000P' WHERE GoodsId = 990000100
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000200 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990000200
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000200 AND ItemId = 1600002 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600002,990000200,N'Double Up 130%')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000200 AND ItemId = 1600004 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600004,990000200,N'Double Up 130%')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000200) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Double Up 130%' WHERE GoodsId = 990000200
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000300 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990000300
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000300 AND ItemId = 1800047 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800047,990000300,N'Reset Package A')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000300 AND ItemId = 1800048 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800048,990000300,N'Reset Package A')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000300 AND ItemId = 2000200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000200,990000300,N'Reset Package A')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000300) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Reset Package A' WHERE GoodsId = 990000300
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000400 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990000400
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000400 AND ItemId = 1800047 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800047,990000400,N'Reset Package B')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000400 AND ItemId = 1800049 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800049,990000400,N'Reset Package B')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000400 AND ItemId = 2000200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000200,990000400,N'Reset Package B')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000400) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Reset Package B' WHERE GoodsId = 990000400
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000500 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990000500
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000500 AND ItemId = 1800047 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800047,990000500,N'Reset Package C')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000500 AND ItemId = 1800050 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800050,990000500,N'Reset Package C')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000500 AND ItemId = 2000200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000200,990000500,N'Reset Package C')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000500) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Reset Package C' WHERE GoodsId = 990000500
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000600 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990000600
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000600 AND ItemId = 1800047 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1800047,990000600,N'ChangeNick +20000P')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000600 AND ItemId = 2000200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000200,990000600,N'ChangeNick +20000P')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990000600) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'ChangeNick +20000P' WHERE GoodsId = 990000600
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990008400 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990008400
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990008400 AND ItemId = 1600004 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600004,990008400,N'Point 130% 7D +20000P')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990008400 AND ItemId = 2000200 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000200,990008400,N'Point 130% 7D +20000P')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990008400) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Point 130% 7D +20000P' WHERE GoodsId = 990008400
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990019800 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990019800
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990019800 AND ItemId = 1600017 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600017,990019800,N'Weapon Pickup + 65000Point')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990019800 AND ItemId = 2000050 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000050,990019800,N'Weapon Pickup + 65000Point')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990019800 AND ItemId = 2000600 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000600,990019800,N'Weapon Pickup + 65000Point')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990019800) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Weapon Pickup + 65000Point' WHERE GoodsId = 990019800
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990019900 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990019900
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990019900 AND ItemId = 1600017 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600017,990019900,N'Weapon Pickup + 150000Point')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990019900 AND ItemId = 2000500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000500,990019900,N'Weapon Pickup + 150000Point')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990019900 AND ItemId = 2001000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2001000,990019900,N'Weapon Pickup + 150000Point')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990019900) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Weapon Pickup + 150000Point' WHERE GoodsId = 990019900
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990020600 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990020600
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990020600 AND ItemId = 1600002 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600002,990020600,N'Exp 130% + 65000Point')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990020600 AND ItemId = 2000150 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000150,990020600,N'Exp 130% + 65000Point')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990020600 AND ItemId = 2000500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000500,990020600,N'Exp 130% + 65000Point')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990020600) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Exp 130% + 65000Point' WHERE GoodsId = 990020600
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990020700 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990020700
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990020700 AND ItemId = 1600002 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600002,990020700,N'Exp 130% + 150000Point')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990020700 AND ItemId = 2000500 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2000500,990020700,N'Exp 130% + 150000Point')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990020700 AND ItemId = 2001000 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (2001000,990020700,N'Exp 130% + 150000Point')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990020700) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Exp 130% + 150000Point' WHERE GoodsId = 990020700
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021500 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990021500
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021500 AND ItemId = 103064 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103064,990021500,N'Gold FAMAS G2 Commando Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021500 AND ItemId = 214004 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214004,990021500,N'Gold FAMAS G2 Commando Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021500 AND ItemId = 301043 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301043,990021500,N'Gold FAMAS G2 Commando Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021500 AND ItemId = 407007 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407007,990021500,N'Gold FAMAS G2 Commando Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021500 AND ItemId = 1600007 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600007,990021500,N'Gold FAMAS G2 Commando Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021500) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Gold FAMAS G2 Commando Set' WHERE GoodsId = 990021500
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021600 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990021600
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021600 AND ItemId = 103023 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103023,990021600,N'Gold M4A1 Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021600 AND ItemId = 214004 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214004,990021600,N'Gold M4A1 Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021600 AND ItemId = 301043 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301043,990021600,N'Gold M4A1 Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021600 AND ItemId = 407007 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407007,990021600,N'Gold M4A1 Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021600 AND ItemId = 1600007 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600007,990021600,N'Gold M4A1 Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021600) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Gold M4A1 Set' WHERE GoodsId = 990021600
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021700 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990021700
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021700 AND ItemId = 104049 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104049,990021700,N'Gold SS1-R5 Carbine Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021700 AND ItemId = 214004 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214004,990021700,N'Gold SS1-R5 Carbine Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021700 AND ItemId = 301043 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301043,990021700,N'Gold SS1-R5 Carbine Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021700 AND ItemId = 407007 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407007,990021700,N'Gold SS1-R5 Carbine Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021700 AND ItemId = 1600007 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600007,990021700,N'Gold SS1-R5 Carbine Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021700) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Gold SS1-R5 Carbine Set' WHERE GoodsId = 990021700
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021800 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990021800
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021800 AND ItemId = 104075 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104075,990021800,N'Gold P90 Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021800 AND ItemId = 214004 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214004,990021800,N'Gold P90 Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021800 AND ItemId = 301043 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301043,990021800,N'Gold P90 Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021800 AND ItemId = 407007 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407007,990021800,N'Gold P90 Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021800 AND ItemId = 1600007 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600007,990021800,N'Gold P90 Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021800) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Gold P90 Set' WHERE GoodsId = 990021800
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021900 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990021900
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021900 AND ItemId = 105015 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105015,990021900,N'Gold L115A1 Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021900 AND ItemId = 214004 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214004,990021900,N'Gold L115A1 Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021900 AND ItemId = 301043 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301043,990021900,N'Gold L115A1 Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021900 AND ItemId = 407007 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407007,990021900,N'Gold L115A1 Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021900 AND ItemId = 1600007 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600007,990021900,N'Gold L115A1 Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990021900) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Gold L115A1 Set' WHERE GoodsId = 990021900
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022400 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990022400
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022400 AND ItemId = 104031 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104031,990022400,N'Air-force Top')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022400 AND ItemId = 301011 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301011,990022400,N'Air-force Top')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022400 AND ItemId = 407003 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407003,990022400,N'Air-force Top')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022400 AND ItemId = 900038 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900038,990022400,N'Air-force Top')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022400 AND ItemId = 1600077 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600077,990022400,N'Air-force Top')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022400) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Air-force Top' WHERE GoodsId = 990022400
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022400 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990022400
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022400 AND ItemId = 104031 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104031,990022400,N'Kriss S V Package(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022400 AND ItemId = 301011 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301011,990022400,N'Kriss S V Package(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022400 AND ItemId = 407003 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407003,990022400,N'Kriss S V Package(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022400 AND ItemId = 900038 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900038,990022400,N'Kriss S V Package(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022400 AND ItemId = 1600077 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600077,990022400,N'Kriss S V Package(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022400) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Kriss S V Package(Female)' WHERE GoodsId = 990022400
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022500 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990022500
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022500 AND ItemId = 104031 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104031,990022500,N'Kriss S V Package(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022500 AND ItemId = 301011 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301011,990022500,N'Kriss S V Package(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022500 AND ItemId = 407003 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407003,990022500,N'Kriss S V Package(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022500 AND ItemId = 900042 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900042,990022500,N'Kriss S V Package(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022500 AND ItemId = 1600011 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600011,990022500,N'Kriss S V Package(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022500) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Kriss S V Package(Male)' WHERE GoodsId = 990022500
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022500 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990022500
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022500 AND ItemId = 104031 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104031,990022500,N'SASR Top')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022500 AND ItemId = 301011 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301011,990022500,N'SASR Top')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022500 AND ItemId = 407003 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407003,990022500,N'SASR Top')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022500 AND ItemId = 900042 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900042,990022500,N'SASR Top')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022500 AND ItemId = 1600011 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600011,990022500,N'SASR Top')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022500) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'SASR Top' WHERE GoodsId = 990022500
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022600 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990022600
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022600 AND ItemId = 103040 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103040,990022600,N'AUG A3 Package(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022600 AND ItemId = 202021 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202021,990022600,N'AUG A3 Package(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022600 AND ItemId = 508003 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (508003,990022600,N'AUG A3 Package(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022600 AND ItemId = 900041 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900041,990022600,N'AUG A3 Package(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022600 AND ItemId = 1600029 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600029,990022600,N'AUG A3 Package(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022600) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'AUG A3 Package(Female)' WHERE GoodsId = 990022600
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022600 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990022600
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022600 AND ItemId = 103040 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103040,990022600,N'China Marine Top(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022600 AND ItemId = 202021 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202021,990022600,N'China Marine Top(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022600 AND ItemId = 508003 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (508003,990022600,N'China Marine Top(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022600 AND ItemId = 900041 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900041,990022600,N'China Marine Top(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022600 AND ItemId = 1600029 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600029,990022600,N'China Marine Top(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022600) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'China Marine Top(Female)' WHERE GoodsId = 990022600
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022700 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990022700
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022700 AND ItemId = 103040 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103040,990022700,N'AUG A3 Package(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022700 AND ItemId = 202021 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202021,990022700,N'AUG A3 Package(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022700 AND ItemId = 508003 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (508003,990022700,N'AUG A3 Package(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022700 AND ItemId = 900043 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900043,990022700,N'AUG A3 Package(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022700 AND ItemId = 1600034 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600034,990022700,N'AUG A3 Package(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022700) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'AUG A3 Package(Male)' WHERE GoodsId = 990022700
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022700 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990022700
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022700 AND ItemId = 103040 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103040,990022700,N'China Marine Top(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022700 AND ItemId = 202021 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202021,990022700,N'China Marine Top(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022700 AND ItemId = 508003 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (508003,990022700,N'China Marine Top(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022700 AND ItemId = 900043 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900043,990022700,N'China Marine Top(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022700 AND ItemId = 1600034 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600034,990022700,N'China Marine Top(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022700) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'China Marine Top(Male)' WHERE GoodsId = 990022700
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022800 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990022800
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022800 AND ItemId = 105030 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105030,990022800,N'Cheytac M200 Package(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022800 AND ItemId = 214006 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214006,990022800,N'Cheytac M200 Package(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022800 AND ItemId = 407007 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407007,990022800,N'Cheytac M200 Package(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022800 AND ItemId = 900046 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900046,990022800,N'Cheytac M200 Package(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022800 AND ItemId = 1600004 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600004,990022800,N'Cheytac M200 Package(Female)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022800) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Cheytac M200 Package(Female)' WHERE GoodsId = 990022800
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022800 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990022800
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022800 AND ItemId = 105030 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105030,990022800,N'FC.PB Top')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022800 AND ItemId = 214006 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214006,990022800,N'FC.PB Top')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022800 AND ItemId = 407007 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407007,990022800,N'FC.PB Top')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022800 AND ItemId = 900046 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900046,990022800,N'FC.PB Top')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022800 AND ItemId = 1600004 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600004,990022800,N'FC.PB Top')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022800) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'FC.PB Top' WHERE GoodsId = 990022800
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022900 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990022900
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022900 AND ItemId = 105030 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105030,990022900,N'Cheytac M200 Package(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022900 AND ItemId = 214006 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214006,990022900,N'Cheytac M200 Package(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022900 AND ItemId = 407007 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407007,990022900,N'Cheytac M200 Package(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022900 AND ItemId = 900040 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900040,990022900,N'Cheytac M200 Package(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022900 AND ItemId = 1600002 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600002,990022900,N'Cheytac M200 Package(Male)')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022900) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Cheytac M200 Package(Male)' WHERE GoodsId = 990022900
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022900 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990022900
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022900 AND ItemId = 105030 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105030,990022900,N'SAS Top')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022900 AND ItemId = 214006 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (214006,990022900,N'SAS Top')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022900 AND ItemId = 407007 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407007,990022900,N'SAS Top')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022900 AND ItemId = 900040 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (900040,990022900,N'SAS Top')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022900 AND ItemId = 1600002 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600002,990022900,N'SAS Top')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990022900) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'SAS Top' WHERE GoodsId = 990022900
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990023000 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990023000
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990023000 AND ItemId = 103147 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103147,990023000,N'FangBlade Inferno Set A')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990023000 AND ItemId = 301057 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301057,990023000,N'FangBlade Inferno Set A')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990023000) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'FangBlade Inferno Set A' WHERE GoodsId = 990023000
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990023100 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990023100
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990023100 AND ItemId = 104126 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104126,990023100,N'FangBlade Inferno Set C')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990023100 AND ItemId = 301057 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301057,990023100,N'FangBlade Inferno Set C')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990023100) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'FangBlade Inferno Set C' WHERE GoodsId = 990023100
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990023200 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990023200
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990023200 AND ItemId = 105079 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105079,990023200,N'FangBlade Inferno Set B')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990023200 AND ItemId = 301057 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301057,990023200,N'FangBlade Inferno Set B')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990023200) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'FangBlade Inferno Set B' WHERE GoodsId = 990023200
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990023300 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990023300
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990023300 AND ItemId = 104128 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104128,990023300,N'FangBlade Inferno Set D')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990023300 AND ItemId = 301057 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301057,990023300,N'FangBlade Inferno Set D')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990023300) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'FangBlade Inferno Set D' WHERE GoodsId = 990023300
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024500 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990024500
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024500 AND ItemId = 103063 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103063,990024500,N'E-sport VIP Package')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024500 AND ItemId = 104050 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104050,990024500,N'E-sport VIP Package')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024500 AND ItemId = 105033 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (105033,990024500,N'E-sport VIP Package')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024500) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'E-sport VIP Package' WHERE GoodsId = 990024500
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024600 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990024600
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024600 AND ItemId = 103120 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103120,990024600,N'PBIC2013 VIP Package')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024600 AND ItemId = 103121 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103121,990024600,N'PBIC2013 VIP Package')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024600 AND ItemId = 104103 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104103,990024600,N'PBIC2013 VIP Package')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024600) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'PBIC2013 VIP Package' WHERE GoodsId = 990024600
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024700 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990024700
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024700 AND ItemId = 103166 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103166,990024700,N'Newbie Package 1')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024700 AND ItemId = 104031 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104031,990024700,N'Newbie Package 1')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024700 AND ItemId = 1600002 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600002,990024700,N'Newbie Package 1')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024700 AND ItemId = 1600011 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600011,990024700,N'Newbie Package 1')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024700 AND ItemId = 1600077 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600077,990024700,N'Newbie Package 1')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024700) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Newbie Package 1' WHERE GoodsId = 990024700
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024800 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990024800
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024800 AND ItemId = 103040 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103040,990024800,N'Newbie Package 2')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024800 AND ItemId = 104159 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104159,990024800,N'Newbie Package 2')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024800 AND ItemId = 1600002 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600002,990024800,N'Newbie Package 2')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024800 AND ItemId = 1600011 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600011,990024800,N'Newbie Package 2')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024800 AND ItemId = 1600077 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (1600077,990024800,N'Newbie Package 2')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024800) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'Newbie Package 2' WHERE GoodsId = 990024800
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024900 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990024900
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024900 AND ItemId = 103180 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (103180,990024900,N'CNY 2015 AUG A3 Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024900 AND ItemId = 202054 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202054,990024900,N'CNY 2015 AUG A3 Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024900 AND ItemId = 301073 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301073,990024900,N'CNY 2015 AUG A3 Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024900 AND ItemId = 407044 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407044,990024900,N'CNY 2015 AUG A3 Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990024900) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'CNY 2015 AUG A3 Set' WHERE GoodsId = 990024900
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990025000 ) != 0) DELETE PB_BT_SItemGoodsMatching WHERE GoodsId = 990025000
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990025000 AND ItemId = 104172 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (104172,990025000,N'CNY 2015 Kriss S.V Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990025000 AND ItemId = 202054 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (202054,990025000,N'CNY 2015 Kriss S.V Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990025000 AND ItemId = 301073 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (301073,990025000,N'CNY 2015 Kriss S.V Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990025000 AND ItemId = 407044 ) = 0) INSERT INTO PB_BT_SItemGoodsMatching ( ItemID, GoodsID, GoodsName ) VALUES (407044,990025000,N'CNY 2015 Kriss S.V Set')
IF((SELECT COUNT(*) FROM PB_BT_SItemGoodsMatching WHERE GoodsId = 990025000) != 0) UPDATE PB_BT_SItemGoodsMatching SET GoodsName = N'CNY 2015 Kriss S.V Set' WHERE GoodsId = 990025000
GO


/***************************************
* DELETE USER INVENTORY 
***************************************/
GO
