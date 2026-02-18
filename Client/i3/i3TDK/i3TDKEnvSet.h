#if !defined( __I3_TDK_ENVSET_H)
#define __I3_TDK_ENVSET_H

struct I3TDK_PROJECT_INFO
{
	TCHAR		m_szProject[256] = { 0 };
	TCHAR		m_szWorkDir[MAX_PATH] = { 0 };

	TCHAR		m_szDesc[2048] = { 0 };

	i3::vector<i3::rc_string>	m_vecChangeTexPefPath;	// 이와 같이 변경..(여러개 등록할수 있도록)..유니코드문제시, rc_wstring으로 후일 교체..
//	TCHAR		m_szChangeTexPefPath[ MAX_PATH];

	void		init(void)
	{
		memset(m_szProject,0,sizeof(TCHAR) * 256);
		memset(m_szWorkDir,0,sizeof(TCHAR) * MAX_PATH);
		memset(m_szDesc,0,sizeof(TCHAR) * 2048);
	}
};


class I3_EXPORT_TDK i3TDKEnvSet : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3TDKEnvSet, i3ElementBase);
protected:
	I3TDK_PROJECT_INFO	m_Active;

	HKEY			m_hRegKey = nullptr;

protected:
	bool				_Open(void);

public:
	i3TDKEnvSet(void);
	virtual ~i3TDKEnvSet(void);

	void				Load(void);
	void				Save(void);

	const TCHAR *		getCurWorkDir(void)			{ return m_Active.m_szWorkDir; }

	INT32				 getCurNumChangeTexPefPath() const { return INT32(m_Active.m_vecChangeTexPefPath.size()); }
	const i3::rc_string& getCurChangeTexPefPath(INT32 idx) const { return m_Active.m_vecChangeTexPefPath[idx]; }

	const TCHAR *		getCurProject(void)			{ return m_Active.m_szProject; }

	INT32				getProjectCount(void);
	bool				getProject( INT32 idx, I3TDK_PROJECT_INFO * pInfo);
	bool				getProject( const TCHAR * pszProjectName, I3TDK_PROJECT_INFO * pInfo);
	bool				setProject( I3TDK_PROJECT_INFO * pInfo);

	void				SelectProject( const TCHAR * pszProjName);
	void				DeleteProject(I3TDK_PROJECT_INFO * pInfo);
};

#endif
