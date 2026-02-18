#pragma once
/*
class CGameStringTable
{
public:
	enum
	{
		INVALID_NATION_ID	= -1,
		MAX_NATION_CNT		= 13,
	};

public:
	const std::string&		GetString( const std::string& szKey ) const;
	const std::string&		GetString( int nNationID, const std::string& szKey ) const;

public:
	bool	Init( const char* szWorkDir );

private:
	void	AddNation( int nNationID );
	void	ResetNationString( int nNationID );

	const char*		GetNationString( int id ) const;

private:
	struct s_nation
	{
		int nID;
		int	nNationID;		
		const char* szNation;
	};

	typedef std::map< int, s_nation >					map_nation;
	typedef std::map< std::wstring, std::wstring >		map_string;

private:
	map_nation		m_Nations;
	map_string		m_NationStrings[ MAX_NATION_CNT ];

	std::string		m_szWorkDir;

public:
	CGameStringTable();
	~CGameStringTable();
};
*/
//extern CGameStringTable		theGameString;