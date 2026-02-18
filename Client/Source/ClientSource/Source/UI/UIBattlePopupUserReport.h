#pragma once

#define REPORT_FILE_EXTENTION ".txt"
#define REPORT_IMAGE_EXTENTION ".jpg"

class i3ThreadReport : public i3Thread
{
	I3_CLASS_DEFINE( i3ThreadReport, i3Thread );

private:
	i3NetworkFTP*	m_ftp;

private:
	struct _Upload
	{
		std::string dir, local, remote;
	};

	struct _AutoCs
	{
		CRITICAL_SECTION* m_pcs;

		_AutoCs( CRITICAL_SECTION* pcs ) : m_pcs(pcs)	{ ::EnterCriticalSection( m_pcs ); }
		~_AutoCs()										{ ::LeaveCriticalSection( m_pcs ); }
	};

private:
	bool	m_bSetFTP;

private:
	std::string		m_ip;
	int				m_port;

private:
	_Upload					m_cur;
	std::deque< _Upload >	m_que;

private:
	CRITICAL_SECTION	m_cs;

public:
	void		SetFTPServer( const std::string& ip, int nport );
	void		SetUploadFile( const std::string& _dir, const std::string& _local, const std::string& _remote );


public:
	void		BeforeRunning( void* )
	{
		_AutoCs _auto( &m_cs );
		m_State = THREAD_RUNNING;
	}

	void		AfterRunning( void* )
	{
		_AutoCs _auto( &m_cs );
		m_State = THREAD_STOPPED;
	}

	UINT32		OnRunning( void* );

private:
	void		_DeleteFile( const std::string& file );


public:
	i3ThreadReport();
	~i3ThreadReport();
};


#include "UIBattlePopupBase.h"

class UIBattlePopupUserReport : public UIBattlePopupBase
{
	I3_CLASS_DEFINE( UIBattlePopupUserReport, UIBattlePopupBase );

private:	
	i3TextureDX*			m_pTex;

	i3::string			m_szTemp;
	i3::string			m_szRemote;
	i3::string			m_szDir;

	i3::string			m_szTempTXT;
	i3::string			m_szTempIMG;


	i3::string			m_szTime;
	i3::string			m_szDate;

	DWORD	m_dwPrevReportTime;
	bool	m_bReported;

private:
	struct s_UserInfo
	{
		s_UserInfo(INT32 idx) : slotIdx(idx) {} INT32 slotIdx;
	};
	i3::vector< s_UserInfo* > m_UserList;

private:
	i3ThreadReport*			m_pTrheadReport;

public:	
	UIBattlePopupUserReport();
	virtual ~UIBattlePopupUserReport();


public:
	void	IngameReportReq();

private:
	void	_GatherUserInfo();
	void	_SetUserInfo();

	void	_CalculateRemoteFilePath();
	void	_SaveReportTXT();

	void	_DeleteReportFile( const i3::string& path );

public:
	virtual void		OnCreate( i3GameNode * pParent ) override;

	virtual bool		OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual bool		OnExitStart() override;

	virtual bool		OnKey_Enter() override;
	virtual bool		OnKey_Escape() override;

	virtual void		ProcessGameEvent( INT32 _event, INT32 _arg , const i3::user_data& UserData);

protected:
	virtual void		_InitializeAtLoad( i3UIScene * pScene ) override;
	virtual void		_ClearAtUnload( i3UIScene * pScene ) override;
};