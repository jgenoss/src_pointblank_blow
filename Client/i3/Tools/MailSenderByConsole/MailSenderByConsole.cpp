// MailSenderByConsole.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include "stdafx.h"

enum MAIL_TYPE
{	
	MAIL_TYPE_NORMAL = 0,
	MAIL_TYPE_BUILD_SUCESS,
	MAIL_TYPE_BUILD_FAIL,
};

BOOL LoadMailDB(void);
void SendMail(void);

char szDBFile[256];
char szFilePath[512];
char szFile[256][256];
char szServer[512];
char szMailList[256][256];
INT32 nMailListCount = 0;
INT32 nFileCount = 0;
INT32 nMessageLineCount = 0;
MAIL_TYPE mailType;

// Normal Mail에 사용되는 변수입니다.
char szSubject[512];
char szMessage[2048][2048];

int _tmain(int argc, _TCHAR* argv[])
{
	if( argv[1] != NULL)
	{
		sprintf( szDBFile,"%s",argv[1]);
	}
	else
	{
		printf("Mail DB file is not founded...\n");
		return 0;
	}

	if( argv[2] != NULL)
	{
		if( i3String::Compare( argv[2], "BUILD_SUCESS") == 0)
		{
			mailType = MAIL_TYPE_BUILD_SUCESS;
		}
		else if( i3String::Compare( argv[2], "BUILD_FAIL") == 0)
		{
			mailType = MAIL_TYPE_BUILD_FAIL;
		}		
	}
	else
	{
		mailType = MAIL_TYPE_NORMAL;
	}

	i3MemoryInit();

	i3BaseRegisterMetas();	

    // TODO: Please replace the sample code below with your own.
	printf("Start Mail Sending~!\n");

	//1. Load E-Mail Address 
	if( LoadMailDB())
	{
		//2. Send Mail
		SendMail();
	}

	printf("Mail Sender Terminate...\n");

	return 0;
}

BOOL LoadMailDB(void)
{
	char szTemp[2048] = {0,};
	i3FileStream file; 
	BOOL bMessage = FALSE;

	if( FALSE == file.Open( szDBFile, STREAM_READ) ) 
	{
		printf("There is no MailDB!!\n");
		return FALSE;
	}

	BOOL bContinue = TRUE;
	while( bContinue )
	{
		UINT32 nReadLength = file.ReadLine( szTemp, 2048 );
		I3ASSERT( 2048 > nReadLength );		

		if( i3String::Compare( szTemp, "[MailListEnd]" ) == 0 ) 
		{
			bContinue = FALSE;
			break;
		}
		else if( i3String::Contain( szTemp, "[Mail_Server]" ) >= 0 ) 
		{
			i3String::Copy( szServer, &szTemp[13]);
			INT32 nLength = i3String::Length(szServer);
			szServer[nLength] = 0;
		}
		else if( i3String::Contain( szTemp, "[File_Path]" ) >= 0 )
		{
			i3String::Copy( szFilePath, &szTemp[11]);
			INT32 nLength = i3String::Length(szFilePath);
			szFilePath[nLength] = 0;
		}
		else if( i3String::Contain( szTemp, "[File]" ) >= 0 )
		{
			i3String::Copy( szFile[nFileCount], &szTemp[6]);
			INT32 nLength = i3String::Length(szFile[nFileCount]);
			szFile[nFileCount][nLength] = 0;
			if( nLength > 0)
			{				
				nFileCount++;
			}
		}
		else if( i3String::Contain( szTemp, "[Mail_Subject]" ) >= 0 )
		{
			i3String::Copy( szSubject, &szTemp[14]);
			INT32 nLength = i3String::Length(szSubject);
			szSubject[nLength] = 0;
		}
		else if( i3String::Contain( szTemp, "[Mail_Message]" ) >= 0 )
		{
			bMessage = TRUE;
		}
		else if( i3String::Contain( szTemp, "[Mail_Message_End]" ) >= 0 )
		{
			bMessage = FALSE;
		}
		else if( szTemp[0] != ';' && bMessage)
		{
			i3String::Copy( szMessage[nMessageLineCount], szTemp);
			INT32 nLength = i3String::Length(szMessage[nMessageLineCount]);
			szMessage[nMessageLineCount][nLength] = '\n';
			if( nLength>0)
			{
				nMessageLineCount++;
			}
		}
		else if( szTemp[0] != ';' && i3String::Length(szTemp) > 0)
		{
			if( i3String::ContainChar( szTemp, '@') >= 0)
			{
				i3String::Copy( szMailList[nMailListCount], szTemp);
				INT32 nLength = i3String::Length(szTemp);
				szMailList[nMailListCount][nLength] = 0;
				if( nLength>0)
				{
					nMailListCount++;
				}
			}
		}
	}

	file.Close();

	printf("MailDB load complete.\n");

	return TRUE;
}

void SendMail(void)
{
	INT32 i = 0;
	i3SMTPMail	mail;

	if( mail.ConnectServer( szServer ) ) 
	{
		SYSTEMTIME sysTime;
		::GetLocalTime(&sysTime);		
	
		// 보내는사람
		mail.SetSenderName("BuildSystem");
		mail.SetSenderEmail("BuildSystem@BuildSystem.com");
		
		// 메일제목
		{
			char szTemp[2048] = {0,};
			char szDate[256] = {0,};

			sprintf( szDate, "%d.%d.%d(%d:%d)", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute);

			switch( mailType)
			{
			case MAIL_TYPE_BUILD_SUCESS:
				sprintf( szTemp, szDate);
				strcat( szTemp, " Cruise Control Build에 성공하였습니다.");
				break;
			case MAIL_TYPE_BUILD_FAIL:
				sprintf( szTemp, szDate);
				strcat( szTemp, " Cruise Control Build에 실패하였습니다.");
				break;
			case MAIL_TYPE_NORMAL:
				sprintf( szTemp, szSubject);
				break;
			}

			mail.SetSubject(szTemp);
		}
		
		// 메일내용
		{
			char szTemp[2048] = {0,};

			switch( mailType)
			{
			case MAIL_TYPE_BUILD_SUCESS:
				sprintf( szTemp, "Build에 성공하였습니다.\n--------------------------------------------------------------------\nBuild System\n \nzepetto.co\n \n--------------------------------------------------------------------");
				break;
			case MAIL_TYPE_BUILD_FAIL:
				sprintf( szTemp, "Build에 실패하였습니다.\n--------------------------------------------------------------------\nBuild System\n \nzepetto.co\n \n--------------------------------------------------------------------");
				break;
			case MAIL_TYPE_NORMAL:
				{
					for(i=0; i<nMessageLineCount; i++)
					{
						i3String::Concat( szTemp, szMessage[i]);
					}

					szTemp[ i3String::Length( szTemp)] = 0;
				}
				break;
			}

			mail.SetMessageBody( szTemp);			
		}

		for( i=0; i<nMailListCount; i++)
		{
			// 받는사람
			mail.AddRecipient(szMailList[i]);
		}

		// 추가파일		
		if( i3String::Length(szFilePath) >= 0 )
		{
			for( i=0; i<nFileCount; i++)
			{
				char szDirFullPath[512] = {0,};
				i3String::Concat(szDirFullPath, szFilePath);
				i3String::Concat(szDirFullPath, szFile[i]);

				mail.SetFileAttach(szDirFullPath);	
			}
		}
		// 추가파일

		if ( mail.GetConnectStatus() ) 
		{	
			mail.Send();			
			mail.Disconnect();
		}

		printf("All mail sended! [Total:%d mail(s)]\n", nMailListCount);
	}
	else
	{
		printf("Mail Connect Server Fail.\nCheck your mail server in your MailDB.txt\n");
	}

	return;
}



