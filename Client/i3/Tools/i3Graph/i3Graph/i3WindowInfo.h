#if !defined( __I3_WINDOW_INFO_H)
#define __I3_WINDOW_INFO_H

#include "i3Base.h"

class i3WindowInfo : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3WindowInfo);

protected:
	i3WindowInfo * m_pParent;

	i3WindowInfo * m_pPane[2];

	INT32		m_X;
	INT32		m_Y;
	INT32		m_Width;
	INT32		m_Height;

	INT32		m_Size;

public:
	i3WindowInfo(void);
	virtual ~i3WindowInfo(void);

	i3WindowInfo *		GetParent(void)						{ return m_pParent; }
	void				SetParent( i3WindowInfo * pParent)	{ m_pParent = pParent; }

	i3WindowInfo *		GetPane( INT32 idx)					{ return m_pPane[idx]; }
	void				SetPane( INT32 idx, i3WindowInfo * pPane)
	{
		m_pPane[idx] = pPane;
	}

	INT32				GetX(void)							{ return m_X; }
	void				SetX( INT32 x)						{ m_X = x; }

	INT32				GetY(void)							{ return m_Y; }
	void				SetY( INT32 y)						{ m_Y = y; }

	INT32				GetWidth(void)						{ return m_Width; }
	void				SetWidth( INT32 w)					{ m_Width = w; }

	INT32				GetHeight(void)						{ return m_Height; }
	void				SetHeight( INT32 h)					{ m_Height = h; }

	INT32				GetSize(void)						{ return m_Size; }
	void				SetSize( INT32 sz)					{ m_Size = sz; }

	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif