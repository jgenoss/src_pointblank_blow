/**
 * \file	UI\UIResizer.h
 *
 * Declares the user interface resizer class.
 */


#if !defined( __UI_RESIZER_H__)
#define __UI_RESIZER_H__

enum UI_SIZESTATE
{
	UI_SIZESTATE_MINIMIZE = 0,
	UI_SIZESTATE_MINIMIZE_DONE,
	UI_SIZESTATE_MAXIMIZE,
	UI_SIZESTATE_MAXIMIZE_DONE
};

enum UI_SIZESTYLE
{
	UI_SIZESTYLE_MOVEBOTTOM = 0,
	UI_SIZESTYLE_MOVETOP,
	UI_SIZESTYLE_MOVELEFT,
	UI_SIZESTYLE_MOVERIGHT,
	UI_SIZESTYLE_FOLDBOTTOM,
	UI_SIZESTYLE_FOLDTOP,
	UI_SIZESTYLE_FOLDLEFT,
	UI_SIZESTYLE_FOLDRIGHT,
};


typedef void (*UIRESIZE_PROC)();

class UIResizer : public i3GameNode
{
	I3_CLASS_DEFINE( UIResizer, i3GameNode);

protected:
	UI_SIZESTYLE	m_SizeStyle;
	UI_SIZESTATE	m_PreSizeState;
	UI_SIZESTATE	m_SizeState;
	REAL32			m_SizeUpdateTime;

	i3UIFrameWnd *	m_pFrameWnd;
	REAL32			m_TargetValue;
	REAL32			m_originalX;
	REAL32			m_originalY;

	UIRESIZE_PROC	m_MinimizeDoneProc;
	UIRESIZE_PROC	m_MaximizeDoneProc;

protected:
	void			_MoveToBottom( void);
	void			_MoveFromBottom( void);

	void			_MinimizeToTop( void);
	void			_MaximizeFromTop( void);

public:
	bool			IsMinimizeDone( void)		{ return m_SizeState == UI_SIZESTATE_MINIMIZE_DONE; }
	bool			IsMaximizeDone( void)		{ return m_SizeState == UI_SIZESTATE_MAXIMIZE_DONE; }
	UI_SIZESTATE	GetSizeState(void) const	{ return m_SizeState; }		
public:
	UIResizer();
	virtual ~UIResizer();

	void			BindFrameWnd( i3UIFrameWnd * pFrameWnd, REAL32 targetValue,
		UIRESIZE_PROC minimizeDoneProc = nullptr, UIRESIZE_PROC maximizeDoneProc = nullptr,
		UI_SIZESTYLE style = UI_SIZESTYLE_MOVEBOTTOM);

	void			ResetBindFrameWnd(VEC3D* pos, VEC2D* size, REAL32 targetValue,
		UIRESIZE_PROC minimizeDoneProc = nullptr, UIRESIZE_PROC maximizeDoneProc = nullptr,
		UI_SIZESTYLE style = UI_SIZESTYLE_MOVEBOTTOM);

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	/** \brief 최소화 설정 */
	virtual void	OnMinimizeStart( void);

	/** \brief 최소화중 */
	virtual void	OnMinimize( void);

	/** \brief 최소화 완료 */
	virtual void	OnMinimizeEnd( void);

	/** \brief 최대화 설정 */
	virtual void	OnMaximizeStart( void);

	/** \brief 최대 크기 */
	virtual void	OnMaximize( void);

	/** \brief 최대화 완료 */
	virtual void	OnMaximizeEnd( void);

	/** \brief 이전 크기 */
	virtual void	OnPreSize( void)			{ m_SizeState = m_PreSizeState;}

	virtual bool	OnResizing( REAL32 tm)		{return true;}
};

#endif
