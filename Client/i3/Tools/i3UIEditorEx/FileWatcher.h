#pragma once

// Library에 설정된 Script와 Screen에 설정된 Script 두 파일을 감시하기 위한 스레드
// 하나의 working folder와 두개의 file path에 대해서만 감시
// 나중에 감시해야할 파일의 수가 늘어나면 StringList를 통해 
// 구조를 변경하여 여러개의 파일을 감시할 수 있다. 
class FileWatcher : public i3Thread
{
	I3_CLASS_DEFINE( FileWatcher, i3Thread);

protected:
	char m_szWorkingFolder[MAX_PATH];
	char m_szScript_Scr[MAX_PATH];
	char m_szScript_Lib[MAX_PATH];

	bool	m_bModified_Scr;
	bool	m_bModified_Lib;
public:
	FileWatcher(void);
	virtual ~FileWatcher(void);

	BOOL		Create( const char* pszWorkingFolder, 
						const char* pszScript_Screen, 
						const char* pszScript_Library);

	virtual UINT32 OnRunning( void* pUserData) override;

	bool	isModified_Screen( void)	{	return m_bModified_Scr;	}
	bool	isModified_Library( void)	{	return m_bModified_Lib;	}

	void	setModifed_Screen( bool bModified)	{	m_bModified_Scr = bModified;	}
	void	setModifed_Library( bool bModified)	{	m_bModified_Lib = bModified;	}

	//void	SetScript_Screen( const char* pszScript)	{	i3::string_ncopy_nullpad( m_szScript_Scr, pszScript, MAX_PATH);	}
	//void	SetScript_Script( const char* pszScript)	{	i3::string_ncopy_nullpad( m_szScript_Lib, pszScript, MAX_PATH);	}

};
