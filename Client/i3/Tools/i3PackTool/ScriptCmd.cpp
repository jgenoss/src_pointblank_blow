#include "stdafx.h"
#include "ScriptCmd.h"
#include "GlobalVar.h"
#include "i3Base/string/ext/remove_blank_from_head.h"
#include "i3Base/string/ext/remove_blank_from_tail.h"

static char s_Separator[] = " \t\r\n";

ScriptCmd::ScriptCmd(void)
{
	m_Code = CMD_NA;
	i3mem::FillZero(m_CmdParam, sizeof(m_CmdParam));
}

ScriptCmd::~ScriptCmd(void)
{
}

ScriptCmd::CMD_TYPE ScriptCmd::_matchCmd( char * pszCmd)
{
	static char s_Cmd[][32] =
	{
		"ADD",
			"REMOVE",
			"ENC",
			"SAVE",
			"CHECKSUM",
			""
	};

	INT32 i;

	for( i = 0; s_Cmd[i][0] != 0; i++)
	{
		if( i3::generic_is_iequal( pszCmd, s_Cmd[i]) )
			return (CMD_TYPE) i;
	}

	return CMD_NA;
}


bool ScriptCmd::Parse( const char * pszCmd)
{
	i3LineLexer lex;
	char szToken[ 256], sep;

	lex.SetMasterString( (char *) pszCmd);

	// Command
	if( 0 >= lex.GetNextToken( szToken, &sep, s_Separator))
		return false;

	m_Code = _matchCmd( szToken);

	switch( m_Code)
	{
	case CMD_ADD :
	case CMD_REMOVE :
	case CMD_ENCRIPT :
	case CMD_SAVE :
	case CMD_CHECKSUM :
		{
			for (INT32 i = 0 ; i < MAX_CMD_PARAM ; ++i)
			{
				if ( 0 <= lex.GetNextToken( m_CmdParam[i], &sep, s_Separator) )
				{
					i3::remove_blank_from_head( m_CmdParam[i]);
					i3::remove_blank_from_tail( m_CmdParam[i]);
				}
				else
				{
					m_CmdParam[i][0] = NULL;
				}

			}
		}
		break;

	default :
		return false;
	}

	return true;
}

bool ScriptCmd::ParseScript( const char * pszPath, i3::vector<ScriptCmd*>& List)
{
	i3FileStream file;
	char szLine[512];
	INT32 line;

	if( file.Open( pszPath, STREAM_READ | STREAM_SHAREREAD) == FALSE)
	{
		I3PRINTLOG(I3LOG_FATAL, "%s 파일을 열 수 없습니다.", pszPath);
		return false;
	}

	line = 1;

	while( file.ReadLine( szLine, sizeof(szLine) - 1) > 0)
	{
		i3::remove_blank_from_head( szLine);

		if( (szLine[0] == 0) || (szLine[0] == '#' ))
		{
			// Comment
			continue;
		}

		ScriptCmd * pCmd = new ScriptCmd;

		if( pCmd->Parse( szLine) == false)
		{
			I3PRINTLOG(I3LOG_FATAL, "ERROR (%d) %s\n", line, szLine);
		}
		else
		{
			List.push_back( pCmd);
		}

		line++;
	}

	file.Close();

	PRINTF( "\n");

	return true;
}
