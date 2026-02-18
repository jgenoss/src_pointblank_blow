USE [PBOption]
/***************************************
* UPDATE CODE INFO
***************************************/
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 1 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 1, N'Asia/Pacific Region', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 1) != 0) UPDATE CodeInfo SET Name = N'Asia/Pacific Region', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 2 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 2, N'Europe', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 2) != 0) UPDATE CodeInfo SET Name = N'Europe', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 3 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 3, N'Andorra', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 3) != 0) UPDATE CodeInfo SET Name = N'Andorra', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 4 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 4, N'United Arab Emirates', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 4) != 0) UPDATE CodeInfo SET Name = N'United Arab Emirates', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 5 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 5, N'Afghanistan', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 5) != 0) UPDATE CodeInfo SET Name = N'Afghanistan', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 6 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 6, N'Antigua and Barbuda', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 6) != 0) UPDATE CodeInfo SET Name = N'Antigua and Barbuda', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 7 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 7, N'Anguilla', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 7) != 0) UPDATE CodeInfo SET Name = N'Anguilla', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 8 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 8, N'Albania', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 8) != 0) UPDATE CodeInfo SET Name = N'Albania', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 9 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 9, N'Armenia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 9) != 0) UPDATE CodeInfo SET Name = N'Armenia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 10 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 10, N'Curacao', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 10) != 0) UPDATE CodeInfo SET Name = N'Curacao', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 11 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 11, N'Angola', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 11) != 0) UPDATE CodeInfo SET Name = N'Angola', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 12 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 12, N'Antarctica', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 12) != 0) UPDATE CodeInfo SET Name = N'Antarctica', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 13 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 13, N'Argentina', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 13) != 0) UPDATE CodeInfo SET Name = N'Argentina', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 14 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 14, N'American Samoa', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 14) != 0) UPDATE CodeInfo SET Name = N'American Samoa', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 15 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 15, N'Austria', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 15) != 0) UPDATE CodeInfo SET Name = N'Austria', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 16 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 16, N'Australia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 16) != 0) UPDATE CodeInfo SET Name = N'Australia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 17 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 17, N'Aruba', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 17) != 0) UPDATE CodeInfo SET Name = N'Aruba', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 18 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 18, N'Azerbaijan', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 18) != 0) UPDATE CodeInfo SET Name = N'Azerbaijan', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 19 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 19, N'Bosnia and Herzegovina', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 19) != 0) UPDATE CodeInfo SET Name = N'Bosnia and Herzegovina', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 20 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 20, N'Barbados', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 20) != 0) UPDATE CodeInfo SET Name = N'Barbados', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 21 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 21, N'Bangladesh', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 21) != 0) UPDATE CodeInfo SET Name = N'Bangladesh', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 22 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 22, N'Belgium', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 22) != 0) UPDATE CodeInfo SET Name = N'Belgium', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 23 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 23, N'Burkina Faso', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 23) != 0) UPDATE CodeInfo SET Name = N'Burkina Faso', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 24 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 24, N'Bulgaria', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 24) != 0) UPDATE CodeInfo SET Name = N'Bulgaria', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 25 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 25, N'Bahrain', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 25) != 0) UPDATE CodeInfo SET Name = N'Bahrain', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 26 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 26, N'Burundi', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 26) != 0) UPDATE CodeInfo SET Name = N'Burundi', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 27 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 27, N'Benin', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 27) != 0) UPDATE CodeInfo SET Name = N'Benin', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 28 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 28, N'Bermuda', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 28) != 0) UPDATE CodeInfo SET Name = N'Bermuda', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 29 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 29, N'Brunei Darussalam', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 29) != 0) UPDATE CodeInfo SET Name = N'Brunei Darussalam', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 30 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 30, N'Bolivia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 30) != 0) UPDATE CodeInfo SET Name = N'Bolivia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 31 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 31, N'Brazil', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 31) != 0) UPDATE CodeInfo SET Name = N'Brazil', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 32 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 32, N'Bahamas', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 32) != 0) UPDATE CodeInfo SET Name = N'Bahamas', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 33 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 33, N'Bhutan', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 33) != 0) UPDATE CodeInfo SET Name = N'Bhutan', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 34 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 34, N'Bouvet Island', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 34) != 0) UPDATE CodeInfo SET Name = N'Bouvet Island', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 35 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 35, N'Botswana', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 35) != 0) UPDATE CodeInfo SET Name = N'Botswana', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 36 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 36, N'Belarus', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 36) != 0) UPDATE CodeInfo SET Name = N'Belarus', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 37 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 37, N'Belize', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 37) != 0) UPDATE CodeInfo SET Name = N'Belize', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 38 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 38, N'Canada', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 38) != 0) UPDATE CodeInfo SET Name = N'Canada', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 39 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 39, N'Cocos (Keeling) Islands', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 39) != 0) UPDATE CodeInfo SET Name = N'Cocos (Keeling) Islands', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 40 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 40, N'Congo, The Democratic Republic of the', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 40) != 0) UPDATE CodeInfo SET Name = N'Congo, The Democratic Republic of the', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 41 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 41, N'Central African Republic', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 41) != 0) UPDATE CodeInfo SET Name = N'Central African Republic', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 42 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 42, N'Congo', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 42) != 0) UPDATE CodeInfo SET Name = N'Congo', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 43 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 43, N'Switzerland', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 43) != 0) UPDATE CodeInfo SET Name = N'Switzerland', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 44 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 44, N'Cote D''Ivoire', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 44) != 0) UPDATE CodeInfo SET Name = N'Cote D''Ivoire', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 45 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 45, N'Cook Islands', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 45) != 0) UPDATE CodeInfo SET Name = N'Cook Islands', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 46 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 46, N'Chile', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 46) != 0) UPDATE CodeInfo SET Name = N'Chile', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 47 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 47, N'Cameroon', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 47) != 0) UPDATE CodeInfo SET Name = N'Cameroon', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 48 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 48, N'China', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 48) != 0) UPDATE CodeInfo SET Name = N'China', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 49 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 49, N'Colombia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 49) != 0) UPDATE CodeInfo SET Name = N'Colombia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 50 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 50, N'Costa Rica', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 50) != 0) UPDATE CodeInfo SET Name = N'Costa Rica', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 51 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 51, N'Cuba', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 51) != 0) UPDATE CodeInfo SET Name = N'Cuba', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 52 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 52, N'Cape Verde', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 52) != 0) UPDATE CodeInfo SET Name = N'Cape Verde', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 53 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 53, N'Christmas Island', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 53) != 0) UPDATE CodeInfo SET Name = N'Christmas Island', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 54 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 54, N'Cyprus', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 54) != 0) UPDATE CodeInfo SET Name = N'Cyprus', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 55 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 55, N'Czech Republic', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 55) != 0) UPDATE CodeInfo SET Name = N'Czech Republic', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 56 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 56, N'Germany', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 56) != 0) UPDATE CodeInfo SET Name = N'Germany', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 57 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 57, N'Djibouti', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 57) != 0) UPDATE CodeInfo SET Name = N'Djibouti', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 58 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 58, N'Denmark', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 58) != 0) UPDATE CodeInfo SET Name = N'Denmark', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 59 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 59, N'Dominica', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 59) != 0) UPDATE CodeInfo SET Name = N'Dominica', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 60 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 60, N'Dominican Republic', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 60) != 0) UPDATE CodeInfo SET Name = N'Dominican Republic', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 61 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 61, N'Algeria', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 61) != 0) UPDATE CodeInfo SET Name = N'Algeria', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 62 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 62, N'Ecuador', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 62) != 0) UPDATE CodeInfo SET Name = N'Ecuador', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 63 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 63, N'Estonia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 63) != 0) UPDATE CodeInfo SET Name = N'Estonia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 64 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 64, N'Egypt', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 64) != 0) UPDATE CodeInfo SET Name = N'Egypt', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 65 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 65, N'Western Sahara', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 65) != 0) UPDATE CodeInfo SET Name = N'Western Sahara', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 66 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 66, N'Eritrea', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 66) != 0) UPDATE CodeInfo SET Name = N'Eritrea', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 67 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 67, N'Spain', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 67) != 0) UPDATE CodeInfo SET Name = N'Spain', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 68 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 68, N'Ethiopia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 68) != 0) UPDATE CodeInfo SET Name = N'Ethiopia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 69 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 69, N'Finland', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 69) != 0) UPDATE CodeInfo SET Name = N'Finland', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 70 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 70, N'Fiji', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 70) != 0) UPDATE CodeInfo SET Name = N'Fiji', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 71 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 71, N'Falkland Islands (Malvinas)', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 71) != 0) UPDATE CodeInfo SET Name = N'Falkland Islands (Malvinas)', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 72 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 72, N'Micronesia, Federated States of', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 72) != 0) UPDATE CodeInfo SET Name = N'Micronesia, Federated States of', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 73 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 73, N'Faroe Islands', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 73) != 0) UPDATE CodeInfo SET Name = N'Faroe Islands', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 74 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 74, N'France', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 74) != 0) UPDATE CodeInfo SET Name = N'France', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 75 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 75, N'Sint Maarten (Dutch part)', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 75) != 0) UPDATE CodeInfo SET Name = N'Sint Maarten (Dutch part)', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 76 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 76, N'Gabon', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 76) != 0) UPDATE CodeInfo SET Name = N'Gabon', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 77 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 77, N'United Kingdom', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 77) != 0) UPDATE CodeInfo SET Name = N'United Kingdom', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 78 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 78, N'Grenada', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 78) != 0) UPDATE CodeInfo SET Name = N'Grenada', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 79 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 79, N'Georgia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 79) != 0) UPDATE CodeInfo SET Name = N'Georgia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 80 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 80, N'French Guiana', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 80) != 0) UPDATE CodeInfo SET Name = N'French Guiana', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 81 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 81, N'Ghana', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 81) != 0) UPDATE CodeInfo SET Name = N'Ghana', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 82 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 82, N'Gibraltar', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 82) != 0) UPDATE CodeInfo SET Name = N'Gibraltar', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 83 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 83, N'Greenland', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 83) != 0) UPDATE CodeInfo SET Name = N'Greenland', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 84 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 84, N'Gambia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 84) != 0) UPDATE CodeInfo SET Name = N'Gambia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 85 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 85, N'Guinea', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 85) != 0) UPDATE CodeInfo SET Name = N'Guinea', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 86 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 86, N'Guadeloupe', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 86) != 0) UPDATE CodeInfo SET Name = N'Guadeloupe', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 87 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 87, N'Equatorial Guinea', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 87) != 0) UPDATE CodeInfo SET Name = N'Equatorial Guinea', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 88 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 88, N'Greece', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 88) != 0) UPDATE CodeInfo SET Name = N'Greece', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 89 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 89, N'South Georgia and the South Sandwich Islands', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 89) != 0) UPDATE CodeInfo SET Name = N'South Georgia and the South Sandwich Islands', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 90 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 90, N'Guatemala', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 90) != 0) UPDATE CodeInfo SET Name = N'Guatemala', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 91 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 91, N'Guam', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 91) != 0) UPDATE CodeInfo SET Name = N'Guam', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 92 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 92, N'Guinea-Bissau', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 92) != 0) UPDATE CodeInfo SET Name = N'Guinea-Bissau', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 93 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 93, N'Guyana', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 93) != 0) UPDATE CodeInfo SET Name = N'Guyana', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 94 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 94, N'Hong Kong', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 94) != 0) UPDATE CodeInfo SET Name = N'Hong Kong', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 95 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 95, N'Heard Island and McDonald Islands', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 95) != 0) UPDATE CodeInfo SET Name = N'Heard Island and McDonald Islands', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 96 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 96, N'Honduras', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 96) != 0) UPDATE CodeInfo SET Name = N'Honduras', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 97 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 97, N'Croatia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 97) != 0) UPDATE CodeInfo SET Name = N'Croatia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 98 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 98, N'Haiti', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 98) != 0) UPDATE CodeInfo SET Name = N'Haiti', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 99 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 99, N'Hungary', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 99) != 0) UPDATE CodeInfo SET Name = N'Hungary', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 100 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 100, N'Indonesia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 100) != 0) UPDATE CodeInfo SET Name = N'Indonesia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 101 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 101, N'Ireland', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 101) != 0) UPDATE CodeInfo SET Name = N'Ireland', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 102 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 102, N'Israel', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 102) != 0) UPDATE CodeInfo SET Name = N'Israel', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 103 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 103, N'India', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 103) != 0) UPDATE CodeInfo SET Name = N'India', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 104 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 104, N'British Indian Ocean Territory', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 104) != 0) UPDATE CodeInfo SET Name = N'British Indian Ocean Territory', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 105 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 105, N'Iraq', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 105) != 0) UPDATE CodeInfo SET Name = N'Iraq', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 106 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 106, N'Iran, Islamic Republic of', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 106) != 0) UPDATE CodeInfo SET Name = N'Iran, Islamic Republic of', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 107 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 107, N'Iceland', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 107) != 0) UPDATE CodeInfo SET Name = N'Iceland', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 108 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 108, N'Italy', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 108) != 0) UPDATE CodeInfo SET Name = N'Italy', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 109 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 109, N'Jamaica', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 109) != 0) UPDATE CodeInfo SET Name = N'Jamaica', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 110 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 110, N'Jordan', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 110) != 0) UPDATE CodeInfo SET Name = N'Jordan', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 111 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 111, N'Japan', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 111) != 0) UPDATE CodeInfo SET Name = N'Japan', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 112 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 112, N'Kenya', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 112) != 0) UPDATE CodeInfo SET Name = N'Kenya', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 113 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 113, N'Kyrgyzstan', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 113) != 0) UPDATE CodeInfo SET Name = N'Kyrgyzstan', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 114 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 114, N'Cambodia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 114) != 0) UPDATE CodeInfo SET Name = N'Cambodia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 115 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 115, N'Kiribati', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 115) != 0) UPDATE CodeInfo SET Name = N'Kiribati', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 116 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 116, N'Comoros', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 116) != 0) UPDATE CodeInfo SET Name = N'Comoros', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 117 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 117, N'Saint Kitts and Nevis', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 117) != 0) UPDATE CodeInfo SET Name = N'Saint Kitts and Nevis', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 118 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 118, N'Korea, Democratic People''s Republic of', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 118) != 0) UPDATE CodeInfo SET Name = N'Korea, Democratic People''s Republic of', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 119 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 119, N'Korea, Republic of', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 119) != 0) UPDATE CodeInfo SET Name = N'Korea, Republic of', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 120 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 120, N'Kuwait', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 120) != 0) UPDATE CodeInfo SET Name = N'Kuwait', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 121 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 121, N'Cayman Islands', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 121) != 0) UPDATE CodeInfo SET Name = N'Cayman Islands', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 122 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 122, N'Kazakhstan', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 122) != 0) UPDATE CodeInfo SET Name = N'Kazakhstan', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 123 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 123, N'Lao People''s Democratic Republic', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 123) != 0) UPDATE CodeInfo SET Name = N'Lao People''s Democratic Republic', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 124 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 124, N'Lebanon', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 124) != 0) UPDATE CodeInfo SET Name = N'Lebanon', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 125 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 125, N'Saint Lucia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 125) != 0) UPDATE CodeInfo SET Name = N'Saint Lucia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 126 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 126, N'Liechtenstein', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 126) != 0) UPDATE CodeInfo SET Name = N'Liechtenstein', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 127 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 127, N'Sri Lanka', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 127) != 0) UPDATE CodeInfo SET Name = N'Sri Lanka', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 128 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 128, N'Liberia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 128) != 0) UPDATE CodeInfo SET Name = N'Liberia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 129 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 129, N'Lesotho', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 129) != 0) UPDATE CodeInfo SET Name = N'Lesotho', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 130 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 130, N'Lithuania', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 130) != 0) UPDATE CodeInfo SET Name = N'Lithuania', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 131 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 131, N'Luxembourg', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 131) != 0) UPDATE CodeInfo SET Name = N'Luxembourg', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 132 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 132, N'Latvia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 132) != 0) UPDATE CodeInfo SET Name = N'Latvia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 133 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 133, N'Libya', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 133) != 0) UPDATE CodeInfo SET Name = N'Libya', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 134 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 134, N'Morocco', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 134) != 0) UPDATE CodeInfo SET Name = N'Morocco', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 135 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 135, N'Monaco', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 135) != 0) UPDATE CodeInfo SET Name = N'Monaco', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 136 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 136, N'Moldova, Republic of', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 136) != 0) UPDATE CodeInfo SET Name = N'Moldova, Republic of', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 137 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 137, N'Madagascar', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 137) != 0) UPDATE CodeInfo SET Name = N'Madagascar', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 138 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 138, N'Marshall Islands', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 138) != 0) UPDATE CodeInfo SET Name = N'Marshall Islands', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 139 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 139, N'Macedonia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 139) != 0) UPDATE CodeInfo SET Name = N'Macedonia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 140 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 140, N'Mali', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 140) != 0) UPDATE CodeInfo SET Name = N'Mali', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 141 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 141, N'Myanmar', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 141) != 0) UPDATE CodeInfo SET Name = N'Myanmar', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 142 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 142, N'Mongolia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 142) != 0) UPDATE CodeInfo SET Name = N'Mongolia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 143 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 143, N'Macau', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 143) != 0) UPDATE CodeInfo SET Name = N'Macau', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 144 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 144, N'Northern Mariana Islands', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 144) != 0) UPDATE CodeInfo SET Name = N'Northern Mariana Islands', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 145 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 145, N'Martinique', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 145) != 0) UPDATE CodeInfo SET Name = N'Martinique', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 146 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 146, N'Mauritania', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 146) != 0) UPDATE CodeInfo SET Name = N'Mauritania', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 147 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 147, N'Montserrat', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 147) != 0) UPDATE CodeInfo SET Name = N'Montserrat', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 148 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 148, N'Malta', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 148) != 0) UPDATE CodeInfo SET Name = N'Malta', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 149 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 149, N'Mauritius', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 149) != 0) UPDATE CodeInfo SET Name = N'Mauritius', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 150 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 150, N'Maldives', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 150) != 0) UPDATE CodeInfo SET Name = N'Maldives', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 151 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 151, N'Malawi', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 151) != 0) UPDATE CodeInfo SET Name = N'Malawi', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 152 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 152, N'Mexico', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 152) != 0) UPDATE CodeInfo SET Name = N'Mexico', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 153 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 153, N'Malaysia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 153) != 0) UPDATE CodeInfo SET Name = N'Malaysia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 154 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 154, N'Mozambique', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 154) != 0) UPDATE CodeInfo SET Name = N'Mozambique', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 155 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 155, N'Namibia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 155) != 0) UPDATE CodeInfo SET Name = N'Namibia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 156 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 156, N'New Caledonia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 156) != 0) UPDATE CodeInfo SET Name = N'New Caledonia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 157 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 157, N'Niger', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 157) != 0) UPDATE CodeInfo SET Name = N'Niger', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 158 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 158, N'Norfolk Island', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 158) != 0) UPDATE CodeInfo SET Name = N'Norfolk Island', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 159 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 159, N'Nigeria', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 159) != 0) UPDATE CodeInfo SET Name = N'Nigeria', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 160 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 160, N'Nicaragua', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 160) != 0) UPDATE CodeInfo SET Name = N'Nicaragua', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 161 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 161, N'Netherlands', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 161) != 0) UPDATE CodeInfo SET Name = N'Netherlands', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 162 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 162, N'Norway', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 162) != 0) UPDATE CodeInfo SET Name = N'Norway', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 163 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 163, N'Nepal', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 163) != 0) UPDATE CodeInfo SET Name = N'Nepal', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 164 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 164, N'Nauru', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 164) != 0) UPDATE CodeInfo SET Name = N'Nauru', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 165 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 165, N'Niue', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 165) != 0) UPDATE CodeInfo SET Name = N'Niue', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 166 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 166, N'New Zealand', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 166) != 0) UPDATE CodeInfo SET Name = N'New Zealand', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 167 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 167, N'Oman', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 167) != 0) UPDATE CodeInfo SET Name = N'Oman', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 168 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 168, N'Panama', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 168) != 0) UPDATE CodeInfo SET Name = N'Panama', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 169 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 169, N'Peru', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 169) != 0) UPDATE CodeInfo SET Name = N'Peru', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 170 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 170, N'French Polynesia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 170) != 0) UPDATE CodeInfo SET Name = N'French Polynesia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 171 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 171, N'Papua New Guinea', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 171) != 0) UPDATE CodeInfo SET Name = N'Papua New Guinea', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 172 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 172, N'Philippines', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 172) != 0) UPDATE CodeInfo SET Name = N'Philippines', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 173 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 173, N'Pakistan', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 173) != 0) UPDATE CodeInfo SET Name = N'Pakistan', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 174 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 174, N'Poland', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 174) != 0) UPDATE CodeInfo SET Name = N'Poland', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 175 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 175, N'Saint Pierre and Miquelon', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 175) != 0) UPDATE CodeInfo SET Name = N'Saint Pierre and Miquelon', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 176 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 176, N'Pitcairn Islands', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 176) != 0) UPDATE CodeInfo SET Name = N'Pitcairn Islands', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 177 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 177, N'Puerto Rico', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 177) != 0) UPDATE CodeInfo SET Name = N'Puerto Rico', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 178 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 178, N'Palestinian Territory', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 178) != 0) UPDATE CodeInfo SET Name = N'Palestinian Territory', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 179 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 179, N'Portugal', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 179) != 0) UPDATE CodeInfo SET Name = N'Portugal', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 180 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 180, N'Palau', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 180) != 0) UPDATE CodeInfo SET Name = N'Palau', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 181 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 181, N'Paraguay', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 181) != 0) UPDATE CodeInfo SET Name = N'Paraguay', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 182 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 182, N'Qatar', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 182) != 0) UPDATE CodeInfo SET Name = N'Qatar', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 183 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 183, N'Reunion', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 183) != 0) UPDATE CodeInfo SET Name = N'Reunion', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 184 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 184, N'Romania', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 184) != 0) UPDATE CodeInfo SET Name = N'Romania', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 185 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 185, N'Russian Federation', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 185) != 0) UPDATE CodeInfo SET Name = N'Russian Federation', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 186 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 186, N'Rwanda', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 186) != 0) UPDATE CodeInfo SET Name = N'Rwanda', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 187 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 187, N'Saudi Arabia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 187) != 0) UPDATE CodeInfo SET Name = N'Saudi Arabia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 188 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 188, N'Solomon Islands', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 188) != 0) UPDATE CodeInfo SET Name = N'Solomon Islands', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 189 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 189, N'Seychelles', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 189) != 0) UPDATE CodeInfo SET Name = N'Seychelles', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 190 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 190, N'Sudan', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 190) != 0) UPDATE CodeInfo SET Name = N'Sudan', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 191 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 191, N'Sweden', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 191) != 0) UPDATE CodeInfo SET Name = N'Sweden', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 192 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 192, N'Singapore', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 192) != 0) UPDATE CodeInfo SET Name = N'Singapore', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 193 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 193, N'Saint Helena', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 193) != 0) UPDATE CodeInfo SET Name = N'Saint Helena', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 194 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 194, N'Slovenia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 194) != 0) UPDATE CodeInfo SET Name = N'Slovenia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 195 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 195, N'Svalbard and Jan Mayen', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 195) != 0) UPDATE CodeInfo SET Name = N'Svalbard and Jan Mayen', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 196 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 196, N'Slovakia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 196) != 0) UPDATE CodeInfo SET Name = N'Slovakia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 197 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 197, N'Sierra Leone', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 197) != 0) UPDATE CodeInfo SET Name = N'Sierra Leone', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 198 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 198, N'San Marino', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 198) != 0) UPDATE CodeInfo SET Name = N'San Marino', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 199 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 199, N'Senegal', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 199) != 0) UPDATE CodeInfo SET Name = N'Senegal', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 200 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 200, N'Somalia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 200) != 0) UPDATE CodeInfo SET Name = N'Somalia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 201 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 201, N'Suriname', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 201) != 0) UPDATE CodeInfo SET Name = N'Suriname', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 202 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 202, N'Sao Tome and Principe', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 202) != 0) UPDATE CodeInfo SET Name = N'Sao Tome and Principe', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 203 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 203, N'El Salvador', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 203) != 0) UPDATE CodeInfo SET Name = N'El Salvador', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 204 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 204, N'Syrian Arab Republic', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 204) != 0) UPDATE CodeInfo SET Name = N'Syrian Arab Republic', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 205 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 205, N'Swaziland', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 205) != 0) UPDATE CodeInfo SET Name = N'Swaziland', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 206 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 206, N'Turks and Caicos Islands', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 206) != 0) UPDATE CodeInfo SET Name = N'Turks and Caicos Islands', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 207 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 207, N'Chad', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 207) != 0) UPDATE CodeInfo SET Name = N'Chad', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 208 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 208, N'French Southern Territories', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 208) != 0) UPDATE CodeInfo SET Name = N'French Southern Territories', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 209 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 209, N'Togo', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 209) != 0) UPDATE CodeInfo SET Name = N'Togo', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 210 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 210, N'Thailand', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 210) != 0) UPDATE CodeInfo SET Name = N'Thailand', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 211 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 211, N'Tajikistan', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 211) != 0) UPDATE CodeInfo SET Name = N'Tajikistan', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 212 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 212, N'Tokelau', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 212) != 0) UPDATE CodeInfo SET Name = N'Tokelau', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 213 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 213, N'Turkmenistan', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 213) != 0) UPDATE CodeInfo SET Name = N'Turkmenistan', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 214 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 214, N'Tunisia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 214) != 0) UPDATE CodeInfo SET Name = N'Tunisia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 215 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 215, N'Tonga', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 215) != 0) UPDATE CodeInfo SET Name = N'Tonga', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 216 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 216, N'Timor-Leste', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 216) != 0) UPDATE CodeInfo SET Name = N'Timor-Leste', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 217 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 217, N'Turkey', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 217) != 0) UPDATE CodeInfo SET Name = N'Turkey', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 218 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 218, N'Trinidad and Tobago', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 218) != 0) UPDATE CodeInfo SET Name = N'Trinidad and Tobago', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 219 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 219, N'Tuvalu', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 219) != 0) UPDATE CodeInfo SET Name = N'Tuvalu', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 220 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 220, N'Taiwan', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 220) != 0) UPDATE CodeInfo SET Name = N'Taiwan', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 221 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 221, N'Tanzania, United Republic of', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 221) != 0) UPDATE CodeInfo SET Name = N'Tanzania, United Republic of', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 222 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 222, N'Ukraine', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 222) != 0) UPDATE CodeInfo SET Name = N'Ukraine', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 223 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 223, N'Uganda', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 223) != 0) UPDATE CodeInfo SET Name = N'Uganda', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 224 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 224, N'United States Minor Outlying Islands', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 224) != 0) UPDATE CodeInfo SET Name = N'United States Minor Outlying Islands', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 225 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 225, N'United States', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 225) != 0) UPDATE CodeInfo SET Name = N'United States', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 226 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 226, N'Uruguay', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 226) != 0) UPDATE CodeInfo SET Name = N'Uruguay', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 227 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 227, N'Uzbekistan', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 227) != 0) UPDATE CodeInfo SET Name = N'Uzbekistan', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 228 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 228, N'Holy See (Vatican City State)', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 228) != 0) UPDATE CodeInfo SET Name = N'Holy See (Vatican City State)', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 229 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 229, N'Saint Vincent and the Grenadines', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 229) != 0) UPDATE CodeInfo SET Name = N'Saint Vincent and the Grenadines', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 230 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 230, N'Venezuela', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 230) != 0) UPDATE CodeInfo SET Name = N'Venezuela', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 231 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 231, N'Virgin Islands, British', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 231) != 0) UPDATE CodeInfo SET Name = N'Virgin Islands, British', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 232 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 232, N'Virgin Islands, U.S.', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 232) != 0) UPDATE CodeInfo SET Name = N'Virgin Islands, U.S.', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 233 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 233, N'Vietnam', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 233) != 0) UPDATE CodeInfo SET Name = N'Vietnam', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 234 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 234, N'Vanuatu', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 234) != 0) UPDATE CodeInfo SET Name = N'Vanuatu', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 235 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 235, N'Wallis and Futuna', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 235) != 0) UPDATE CodeInfo SET Name = N'Wallis and Futuna', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 236 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 236, N'Samoa', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 236) != 0) UPDATE CodeInfo SET Name = N'Samoa', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 237 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 237, N'Yemen', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 237) != 0) UPDATE CodeInfo SET Name = N'Yemen', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 238 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 238, N'Mayotte', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 238) != 0) UPDATE CodeInfo SET Name = N'Mayotte', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 239 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 239, N'Serbia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 239) != 0) UPDATE CodeInfo SET Name = N'Serbia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 240 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 240, N'South Africa', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 240) != 0) UPDATE CodeInfo SET Name = N'South Africa', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 241 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 241, N'Zambia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 241) != 0) UPDATE CodeInfo SET Name = N'Zambia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 242 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 242, N'Montenegro', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 242) != 0) UPDATE CodeInfo SET Name = N'Montenegro', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 243 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 243, N'Zimbabwe', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 243) != 0) UPDATE CodeInfo SET Name = N'Zimbabwe', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 244 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 244, N'Anonymous Proxy', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 244) != 0) UPDATE CodeInfo SET Name = N'Anonymous Proxy', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 245 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 245, N'Satellite Provider', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 245) != 0) UPDATE CodeInfo SET Name = N'Satellite Provider', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 247 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 247, N'Aland Islands', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 247) != 0) UPDATE CodeInfo SET Name = N'Aland Islands', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 248 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 248, N'Guernsey', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 248) != 0) UPDATE CodeInfo SET Name = N'Guernsey', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 249 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 249, N'Isle of Man', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 249) != 0) UPDATE CodeInfo SET Name = N'Isle of Man', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 250 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 250, N'Jersey', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 250) != 0) UPDATE CodeInfo SET Name = N'Jersey', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 251 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 251, N'Saint Barthelemy', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 251) != 0) UPDATE CodeInfo SET Name = N'Saint Barthelemy', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 252 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 252, N'Saint Martin', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 252) != 0) UPDATE CodeInfo SET Name = N'Saint Martin', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 253 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 253, N'Bonaire, Saint Eustatius and Saba', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 253) != 0) UPDATE CodeInfo SET Name = N'Bonaire, Saint Eustatius and Saba', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 254 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (1, 254, N'South Sudan', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 1 AND Code = 254) != 0) UPDATE CodeInfo SET Name = N'South Sudan', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 1 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (2, 1, N'Normal 1', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 1) != 0) UPDATE CodeInfo SET Name = N'Normal 1', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 2 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (2, 2, N'Begin 1', N' Kill/Death', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 2) != 0) UPDATE CodeInfo SET Name = N'Begin 1', Description = N' Kill/Death' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 3 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (2, 3, N'Begin 2', N' Rank', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 3) != 0) UPDATE CodeInfo SET Name = N'Begin 2', Description = N' Rank' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 4 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (2, 4, N'Clan', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 4) != 0) UPDATE CodeInfo SET Name = N'Clan', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 5 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (2, 5, N'Expert 1', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 5) != 0) UPDATE CodeInfo SET Name = N'Expert 1', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 6 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (2, 6, N'Expert 2', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 6) != 0) UPDATE CodeInfo SET Name = N'Expert 2', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 7 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (2, 7, N'Championship', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 7) != 0) UPDATE CodeInfo SET Name = N'Championship', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 8 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (2, 8, N'Normal 2', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 8) != 0) UPDATE CodeInfo SET Name = N'Normal 2', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 9 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (2, 9, N'National', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 9) != 0) UPDATE CodeInfo SET Name = N'National', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 10 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (2, 10, N'Middle', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 10) != 0) UPDATE CodeInfo SET Name = N'Middle', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 11 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (2, 11, N'PCCafe Silver', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 11) != 0) UPDATE CodeInfo SET Name = N'PCCafe Silver', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 12 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (2, 12, N'PCCafe Gold', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 12) != 0) UPDATE CodeInfo SET Name = N'PCCafe Gold', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 13 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (2, 13, N'PCCafe Clan', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 13) != 0) UPDATE CodeInfo SET Name = N'PCCafe Clan', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 14 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (2, 14, N'Azerbaijan', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 2 AND Code = 14) != 0) UPDATE CodeInfo SET Name = N'Azerbaijan', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 1 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (3, 1, N'Death', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 1) != 0) UPDATE CodeInfo SET Name = N'Death', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 2 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (3, 2, N'Bomb', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 2) != 0) UPDATE CodeInfo SET Name = N'Bomb', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 3 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (3, 3, N'Destroy', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 3) != 0) UPDATE CodeInfo SET Name = N'Destroy', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 4 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (3, 4, N'Annihilation', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 4) != 0) UPDATE CodeInfo SET Name = N'Annihilation', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 5 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (3, 5, N'Defence', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 5) != 0) UPDATE CodeInfo SET Name = N'Defence', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 6 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (3, 6, N'SuddenDeath', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 6) != 0) UPDATE CodeInfo SET Name = N'SuddenDeath', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 7 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (3, 7, N'Escape', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 7) != 0) UPDATE CodeInfo SET Name = N'Escape', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 8 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (3, 8, N'Head Hunter', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 8) != 0) UPDATE CodeInfo SET Name = N'Head Hunter', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 9 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (3, 9, N'Head Killer', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 9) != 0) UPDATE CodeInfo SET Name = N'Head Killer', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 10 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (3, 10, N'Tutorial', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 10) != 0) UPDATE CodeInfo SET Name = N'Tutorial', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 11 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (3, 11, N'Domination', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 11) != 0) UPDATE CodeInfo SET Name = N'Domination', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 12 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (3, 12, N'CrossCount', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 12) != 0) UPDATE CodeInfo SET Name = N'CrossCount', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 13 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (3, 13, N'Convoy', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 13) != 0) UPDATE CodeInfo SET Name = N'Convoy', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 14 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (3, 14, N'RunAway', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 3 AND Code = 14) != 0) UPDATE CodeInfo SET Name = N'RunAway', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 4 AND Code = 1 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (4, 1, N'Normal PC', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 4 AND Code = 1) != 0) UPDATE CodeInfo SET Name = N'Normal PC', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 4 AND Code = 2 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (4, 2, N'Premium PC', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 4 AND Code = 2) != 0) UPDATE CodeInfo SET Name = N'Premium PC', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 4 AND Code = 3 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (4, 3, N'Vacation Event PC', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 4 AND Code = 3) != 0) UPDATE CodeInfo SET Name = N'Vacation Event PC', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 4 AND Code = 4 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (4, 4, N'Indo Telcom PC', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 4 AND Code = 4) != 0) UPDATE CodeInfo SET Name = N'Indo Telcom PC', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 4 AND Code = 5 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (4, 5, N'Indo Silver PC', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 4 AND Code = 5) != 0) UPDATE CodeInfo SET Name = N'Indo Silver PC', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 4 AND Code = 6 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (4, 6, N'Indo Gold PC', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 4 AND Code = 6) != 0) UPDATE CodeInfo SET Name = N'Indo Gold PC', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 4 AND Code = 7 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (4, 7, N'Garena GCA PC', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 4 AND Code = 7) != 0) UPDATE CodeInfo SET Name = N'Garena GCA PC', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 4 AND Code = 8 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (4, 8, N'Garena GCA Plus PC', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 4 AND Code = 8) != 0) UPDATE CodeInfo SET Name = N'Garena GCA Plus PC', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 5 AND Code = 1 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (5, 1, N'Medal', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 5 AND Code = 1) != 0) UPDATE CodeInfo SET Name = N'Medal', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 6 AND Code = 1 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (6, 1, N'GameGuard', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 6 AND Code = 1) != 0) UPDATE CodeInfo SET Name = N'GameGuard', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 6 AND Code = 2 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (6, 2, N'Cheat', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 6 AND Code = 2) != 0) UPDATE CodeInfo SET Name = N'Cheat', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 7 AND Code = 50 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (7, 50, N'MOD', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 7 AND Code = 50) != 0) UPDATE CodeInfo SET Name = N'MOD', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 7 AND Code = 100 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (7, 100, N'GM', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 7 AND Code = 100) != 0) UPDATE CodeInfo SET Name = N'GM', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 8 AND Code = 1 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (8, 1, N'KNUCKLE', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 8 AND Code = 1) != 0) UPDATE CodeInfo SET Name = N'KNUCKLE', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 8 AND Code = 2 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (8, 2, N'ESCAPE', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 8 AND Code = 2) != 0) UPDATE CodeInfo SET Name = N'ESCAPE', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 8 AND Code = 3 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (8, 3, N'SHOTGUN', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 8 AND Code = 3) != 0) UPDATE CodeInfo SET Name = N'SHOTGUN', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 8 AND Code = 4 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (8, 4, N'SNIPER', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 8 AND Code = 4) != 0) UPDATE CodeInfo SET Name = N'SNIPER', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 8 AND Code = 5 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (8, 5, N'CLANMATCH', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 8 AND Code = 5) != 0) UPDATE CodeInfo SET Name = N'CLANMATCH', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 8 AND Code = 6 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (8, 6, N'TRAINING', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 8 AND Code = 6) != 0) UPDATE CodeInfo SET Name = N'TRAINING', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 8 AND Code = 7 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (8, 7, N'HEADHUNTER', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 8 AND Code = 7) != 0) UPDATE CodeInfo SET Name = N'HEADHUNTER', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 1 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (9, 1, N'Korea', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 1) != 0) UPDATE CodeInfo SET Name = N'Korea', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 2 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (9, 2, N'Thailand', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 2) != 0) UPDATE CodeInfo SET Name = N'Thailand', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 3 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (9, 3, N'Indonesia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 3) != 0) UPDATE CodeInfo SET Name = N'Indonesia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 4 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (9, 4, N'Russia', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 4) != 0) UPDATE CodeInfo SET Name = N'Russia', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 5 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (9, 5, N'Turkey', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 5) != 0) UPDATE CodeInfo SET Name = N'Turkey', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 6 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (9, 6, N'China', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 6) != 0) UPDATE CodeInfo SET Name = N'China', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 7 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (9, 7, N'Taiwan', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 7) != 0) UPDATE CodeInfo SET Name = N'Taiwan', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 8 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (9, 8, N'Japan', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 8) != 0) UPDATE CodeInfo SET Name = N'Japan', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 9 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (9, 9, N'Brazil', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 9) != 0) UPDATE CodeInfo SET Name = N'Brazil', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 10 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (9, 10, N'LatinAmerica', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 10) != 0) UPDATE CodeInfo SET Name = N'LatinAmerica', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 11 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (9, 11, N'NorthAmerica', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 11) != 0) UPDATE CodeInfo SET Name = N'NorthAmerica', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 12 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (9, 12, N'Italy', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 12) != 0) UPDATE CodeInfo SET Name = N'Italy', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 13 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (9, 13, N'MiddleEast', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 13) != 0) UPDATE CodeInfo SET Name = N'MiddleEast', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 14 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (9, 14, N'Philippines', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 14) != 0) UPDATE CodeInfo SET Name = N'Philippines', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 15 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (9, 15, N'Match', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 15) != 0) UPDATE CodeInfo SET Name = N'Match', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 16 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (9, 16, N'Singmal', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 16) != 0) UPDATE CodeInfo SET Name = N'Singmal', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 17 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (9, 17, N'Vietnam', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 17) != 0) UPDATE CodeInfo SET Name = N'Vietnam', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 18 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (9, 18, N'KoreaDaum', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 18) != 0) UPDATE CodeInfo SET Name = N'KoreaDaum', Description = N'' 
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 19 ) = 0) INSERT INTO CodeInfo ( GroupCode, Code, Name, Description, Enable ) VALUES (9, 19, N'KoreaPicaon', N'', 1 )
IF((SELECT COUNT(*) FROM CodeInfo WHERE GroupCode = 9 AND Code = 19) != 0) UPDATE CodeInfo SET Name = N'KoreaPicaon', Description = N'' 
GO
/***************************************
* UPDATE Revision
***************************************/
UPDATE dbo.PB_BT_ConfigRevision SET CodeInfo = 1
GO