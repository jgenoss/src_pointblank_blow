#if !defined( __I3_TDK_WINDOW_INFO_H)
#define __I3_TDK_WINDOW_INFO_H

#include "i3Base.h"

class I3_EXPORT_TDK i3TDKWindowInfo : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3TDKWindowInfo, i3PersistantElement);

protected:
	i3TDKWindowInfo * m_pParent = nullptr;

	i3TDKWindowInfo * m_pPane[2] = { nullptr, nullptr };

	INT32		m_X = 0;
	INT32		m_Y = 0;
	INT32		m_Width = 0;
	INT32		m_Height = 0;

	INT32		m_Size = 0;

public:
	i3TDKWindowInfo(void) {}
	virtual ~i3TDKWindowInfo(void);

	i3TDKWindowInfo *		GetParent(void)						{ return m_pParent; }
	void				SetParent( i3TDKWindowInfo * pParent)	{ m_pParent = pParent; }

	i3TDKWindowInfo *		GetPane( INT32 idx)					{ return m_pPane[idx]; }
	void				SetPane( INT32 idx, i3TDKWindowInfo * pPane)
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

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif