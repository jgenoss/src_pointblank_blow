#if !defined( __SCRIPT_CMD_H)
#define __SCRIPT_CMD_H

enum {MAX_CMD_PARAM = 3, CMD_PARAM_SIZE = 512};

class ScriptCmd
{
public:
	enum CMD_TYPE
	{
		CMD_NA = -1,
		CMD_ADD = 0,
		CMD_REMOVE,
		CMD_ENCRIPT,
		CMD_SAVE,
		CMD_CHECKSUM,

		MAX_CMD
	};

protected:
	CMD_TYPE			m_Code;
	char				m_CmdParam[MAX_CMD_PARAM][CMD_PARAM_SIZE];

protected:
	CMD_TYPE			_matchCmd( char * pszCmd);

public:
	ScriptCmd(void);
	virtual ~ScriptCmd(void);

	CMD_TYPE			getCode(void)						{ return m_Code; }

	const char *		getCmdParam(INT32 index = 0)					{ return m_CmdParam[index]; }

	bool				Parse( const char * pszCmd);

	static	bool		ParseScript( const char * pszPath, i3::vector<ScriptCmd*>& List);
};

#endif
