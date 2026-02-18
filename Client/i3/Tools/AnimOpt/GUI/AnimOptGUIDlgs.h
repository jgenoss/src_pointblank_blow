
#ifndef _ANIMOPTGUIDLGS_H_
#define _ANIMOPTGUIDLGS_H_

class AnimOptDlg;

class FileDropTargetCtl
{
public:
	class CallBack 
	{
	public:
		// ret 0: finish, 1:continue
		virtual void Reset() {}
		virtual INT32 OnIterateDropFiles(INT32 totalCnt, INT32 cur, const char* fileName) = 0;
		virtual void OnIterateAfter(INT32 num) {}
		void*		m_dropInfo;
	};

	FileDropTargetCtl(HWND hCtl, CallBack* callBack);
	~FileDropTargetCtl();

protected:
	static LRESULT CALLBACK	WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
	void OnDropFiles(HDROP hDrop);

	HWND							m_hwnd;
	WNDPROC							m_oldProc;
	CallBack*						m_callBack;
};


struct I3AniPathStore
{
	enum ETOKEN
	{	
		ETK_0 = 0,
		ETK_1,
		ETK_2,
		ETK_3,
		ETK_4,
		ETK_5,
		ETK_MAX,
		ETK_NONE = -1
	};

	enum { ETOKEN_SIZ = 64 };

	void Reset()
	{
		m_cmpFlag = 0;
		m_exactMatchFlag = 0;
		memset(m_token, 0, sizeof(m_token));
	}

	I3AniPathStore(const char* path=NULL)
	{
		Reset();
		
		if(!path)
			return;

		DWORD tokenType = (DWORD)ETK_NONE;

		const char* c = path;

		INT32 len = 0;
		while(*c) { c++, len++; }

		c = path;
		const char* cd = NULL;

		do 
		{			
			cd = strchr(c, '/');

			if(cd)
			{
				tokenType++;
				INT32 pos = 0;

				while(c < cd)
					m_token[tokenType][pos++] = *c++;

				m_token[tokenType][pos] = NULL;

				c = cd;
				c++;
			}
			else
			{
				if(*c)
				{
					tokenType++;
					INT32 pos = 0;

					while(*c)
						m_token[tokenType][pos++] = *c++;

					m_token[tokenType][pos] = NULL;
				}

				break;
			}

		} while(cd);		
	}

	I3AniPathStore(const I3AniPathStore& rhs)
	{
		*this = rhs;
	}

	I3AniPathStore& operator =(const I3AniPathStore& rhs)
	{
		if(this == &rhs)
			return *this;

		memcpy(m_token, rhs.m_token, sizeof(m_token));

		return *this;
	}

	bool operator ==(const I3AniPathStore& rhs) const
	{
		if(this == &rhs)
			return true;

		for(INT32 i=0; i<ETK_MAX; i++)
		{
			if(m_cmpFlag && !(m_cmpFlag & (2 << i)))
				continue;

			if(!m_token[i][0] && !rhs.m_token[i][0])
				continue;

			if(m_exactMatchFlag && (m_exactMatchFlag & (2 << i)))
			{
				INT32 len = strlen(m_token[i]);
				if(!strnicmp(m_token[i], rhs.m_token[i], len))
					continue;				
			}
			else
			{
				if(!stricmp(m_token[i], rhs.m_token[i]))
					continue;
			}

			return false;
		}

		return true;
	}

	bool operator !=(const I3AniPathStore& rhs) const
	{
		return !operator ==(rhs);
	}

	void SetToken(ETOKEN tkn, const char* token)
	{
		if(token)
			strcpy_s(m_token[tkn], ETOKEN_SIZ, token);
		else
			m_token[tkn][0] = NULL;
	}

	void SetCmpFlag(ETOKEN tokenCmpFlag, bool replace=false)
	{
		if(tokenCmpFlag == ETK_NONE)
		{
			m_cmpFlag = 0;
			return;
		}

		if(replace)
			m_cmpFlag = 2 << tokenCmpFlag;
		else
			m_cmpFlag |= 2 << tokenCmpFlag;
	}

	void SetCmpExactMatchFlag(ETOKEN tokenCmpFlag, bool replace=false)
	{
		if(tokenCmpFlag == ETK_NONE)
		{
			m_cmpFlag = 0;
			return;
		}

		if(replace)
			m_exactMatchFlag = 2 << tokenCmpFlag;
		else
			m_exactMatchFlag |= 2 << tokenCmpFlag;
	}

	//bool CmpWithFlag(const I3AniPathStore& rhs, DWORD cmpFlag=0) const
	//{
	//	if(!cmpFlag)
	//		return operator ==(rhs);

	//	for(INT32 i=0; i<ETK_MAX; i++)
	//	{
	//		if(cmpFlag & (i >> 2))
	//			continue;			

	//		if(!m_token[i][0] && !rhs.m_token[i][0])
	//			continue;

	//		if(!stricmp(m_token[i], rhs.m_token[i]))
	//			continue;

	//		return false;
	//	}

	//	return true;
	//}

	char		m_token[ETK_MAX][ETOKEN_SIZ] = { 0 };
	DWORD		m_cmpFlag = 0;
	DWORD		m_exactMatchFlag = 0;
};


class APKFileViewCtl
{
public:
	struct Row
	{
		INT32 m_no;
		DWORD m_flag; // 1:16bit, 2:removeTrans
		DWORD m_type; // 0 : i3Animation, 1:i3Animation2, 2:i3Animation2 & 16Bits 3:i3Animation2 TrackTable's 32bits
		UINT32 m_trackCnt;
		INT64 m_Offset;
		
		char m_animPakFileName[MAX_PATH];
		char m_i3AName[MAX_PATH];

		bool m_filtered;
	};

	enum EFILTER_FLAG
	{
		EFIL_BADSTATE_ANIMPACK			= 0x00000001,
		EFIL_I3ANIPATH					= 0x00000ff0
	};

	typedef std::multimap<std::string, Row* > RowMMap;

	APKFileViewCtl(AnimOptDlg* owner, HWND listViewWnd);
	~APKFileViewCtl();

	void ResetContent();
	bool AddAnimPack(const char* path, bool append=false);
	void RefreshRow();

	HWND GetHWnd() { return m_hwnd; }
	LRESULT OnCustDrawItem(LPARAM param);

protected:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

	bool GenerateAnimPackFileDescRow(const char* path);
	void InitView();
	
public:
	HWND							m_hwnd;

	bool							m_filterView;
	DWORD							m_filterFlag;

protected:
	AnimOptDlg*						m_owner;	
	WNDPROC							m_oldProc;
	HIMAGELIST						m_imgListStatus;

public:
	RowMMap							m_row;
	I3AniPathStore					m_filterI3AniPath;
};


class ColorListBox
{
public:
	ColorListBox(HWND hCtl);
	~ColorListBox();

	void AddLog(const char* fmt, ...);
	void ResetContent();

	LRESULT OnOwnerDrawItem(LPARAM param);

	HWND GetHwnd() { return m_hwnd; }

protected:
	static LRESULT CALLBACK	WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

	HWND							m_hwnd;
	WNDPROC							m_oldProc;
};


class AnimOptDlg
{
	friend class FileDropCallBack;

public:
	enum ECTL
	{
		LV_APKFILE_LEFT,
		EB_ANALYZE_WORKDIR,
		BT_EXEC_ANALYZE,
		LB_ANALYZE_RST,

		VF_CK_BADSTATE_ANIMPACK,
		VF_CK_I3ANIPATH,
			VF_CK_0,	VF_EB_0,	VF_CK_EXACTMATCH_0,
			VF_CK_1,	VF_EB_1,	VF_CK_EXACTMATCH_1,	
			VF_CK_2,	VF_EB_2,	VF_CK_EXACTMATCH_2,
			VF_CK_3,	VF_EB_3,	VF_CK_EXACTMATCH_3,
			VF_CK_4,	VF_EB_4,	VF_CK_EXACTMATCH_4,
			VF_CK_5,	VF_EB_5,	VF_CK_EXACTMATCH_5,
		CTL_MAX
	};

	enum EDIRTY_FLAG
	{
		EDF_APKFILE				= 0x00000001,
		EDF_REFRESH_VIEW		= 0x00000002,
		EDF_VIEWFILTER			= 0x00000010,
	};

	AnimOptDlg();
	~AnimOptDlg();

	INT32 Do();
	HWND GetHwnd() { return m_hwnd; }

	void SetDirty(DWORD flag, bool clearDirty=true)
	{
		m_df |= flag;

		if(clearDirty)
			ClearDirty();
	}

	void ClearDirty();

protected:
	static BOOL CALLBACK DlgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
	BOOL OnCmd(WPARAM wParam, LPARAM lParam);

	bool OnInitDialog();
	void OnDestroyDlg();
//	BOOL OnDrawControlItem(HWND hWnd, DRAWITEMSTRUCT* itemDrawInfo);

	void Analyze();

protected:

	class FileDropCallBack : public FileDropTargetCtl::CallBack
	{
	public:
		typedef std::vector<std::string>	FileList;

		virtual void Reset() 
		{
			m_fileList.clear();
		}

		virtual INT32 OnIterateDropFiles(INT32 totalCnt, INT32 cur, const char* fileName)
		{
			m_fileList.push_back(fileName);

			if(totalCnt == cur+1)
				m_owner->SetDirty(AnimOptDlg::EDF_APKFILE, false);

			return 1;
		}

		virtual void OnIterateAfter(INT32 num)
		{
			if(num > 0)
				m_owner->ClearDirty();
		}

		INT32 GetNum() { return m_fileList.size(); }
		const char* GetFileName(INT32 index) { return m_fileList[index].c_str(); }

		FileList						m_fileList;
		AnimOptDlg*						m_owner;
	};

	HWND								m_hwnd;
	HWND								m_ctl[CTL_MAX];

	APKFileViewCtl*						m_apkFileView;
	ColorListBox*						m_apkFileAnalyzeLog;

	FileDropTargetCtl*					m_apkFileDropTarget;
	FileDropCallBack*					m_apkFileDropCallBack;

	DWORD								m_df;

public:
	HINSTANCE							m_moduleInst;
};


#endif
