#if !defined( __UI_MOVIE_SET_CTRL_H__)
#define __UI_MOVIE_SET_CTRL_H__

class UIMovieSetCtrl;

class UIMovieSetGroup
{
public:
	UIMovieSetGroup();
	~UIMovieSetGroup();

	UIMovieSetCtrl*		CreateMovieSetCtrl( i3UIImageBoxEx* bound_rsc) const;
	UIMovieSetCtrl*		CreateMovieSetCtrl( i3UIScene* scn, const char* bound_rsc) const;
private:
	friend class		UIMovieSetCtrl;
};

class UIMovieSetCtrl : public i3GameNode
{
	I3_CLASS_DEFINE( UIMovieSetCtrl, i3GameNode);
	friend class UIMovieSetGroup;

public:
	UIMovieSetCtrl();
	~UIMovieSetCtrl();

	virtual bool	OnLostDevice( bool bLostDevice) override;
	virtual bool	OnRevive( i3RenderContext * pCtx) override;
	
private:
	void	Initialize(const UIMovieSetGroup* tpl_grp, i3UIImageBoxEx* bound_rsc);

public:
	void	InitMovie();
	bool	PlayVideo(UINT32);
	void	StopVideo( void);
	void	CloseVideo( void);

	void			registFile(const char * movieFile)	{	m_pMovieFileList.push_back(movieFile);	};
	const char *	getMoviePath(INT32 idx)				{	return m_pMovieFileList[idx].c_str();	};

private:
	const UIMovieSetGroup*		m_pMovieGroup;
	i3UIImageBoxEx*				m_pBoundImage;
	i3TextureDX *				m_pTex;
	i3::vector<i3::string>		m_pMovieFileList;

	INT32			m_CurrentPlayIdx;

	
};

inline UIMovieSetGroup*	GetMovieMgr()			// 접근 Static 변수한개로.
{
	static UIMovieSetGroup s_mgr;
	return &s_mgr;
}

#endif