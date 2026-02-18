#if !defined( __I3_TDK_UPDATE_SYSTEM_H)
#define __I3_TDK_UPDATE_SYSTEM_H

#include "i3Base.h"

#define		I3_TDK_UPDATE_ADD			0x00010000
#define		I3_TDK_UPDATE_REMOVE		0x00020000
#define		I3_TDK_UPDATE_EDIT			0x00040000
#define		I3_TDK_UPDATE_SELECT		0x00080000
#define		I3_TDK_UPDATE_UNSELECT		0x00100000
#define		I3_TDK_UPDATE_FREEZE		0x00200000
#define		I3_TDK_UPDATE_MOVE			0x00400000
#define		I3_TDK_UPDATE_ROTATE		0x00800000
#define		I3_TDK_UPDATE_SCALE			0x01000000
#define		I3_TDK_UPDATE_SCENEGRAPH	0x02000000
#define		I3_TDK_UPDATE_SCENE			0x04000000
#define		I3_TDK_UPDATE_SCENECAMERA	0x08000000
#define		I3_TDK_UPDATE_SET			0x10000000
#define		I3_TDK_UPDATE_REQUEST		0x20000000
#define		I3_TDK_UPDATE_POPUP_SPEC	0x40000000
#define		I3_TDK_UPDATE_UI			0x80000000

#define		I3_TDK_UPDATE_USERMASK		0x0000FFFF


#define		I3_TDK_UPDATE_ALL		0xFFFFFFFF

#define		WM_TDK_UPDATE				(WM_USER + 3434)

struct I3_TDK_UPDATE_REG_INFO
{
	HWND	m_hWindow = nullptr;
	i3ClassMeta *	m_pClassMeta = nullptr;
	UINT32			m_EventMask = 0;
} ;

struct I3_TDK_UPDATE_INFO
{
	HWND			m_hwndFrom = nullptr;
	UINT32			m_Event = 0;

	i3ClassMeta *	m_pMeta = nullptr;
	i3ElementBase * m_pObject = nullptr;
} ;

class I3_EXPORT_TDK i3TDKUpdateSystem : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3TDKUpdateSystem, i3ElementBase);

protected:
	i3::vector<I3_TDK_UPDATE_REG_INFO*>	m_UpdateList;
	bool		m_bPending = false;
	
public:
	i3TDKUpdateSystem(void) {}
	virtual ~i3TDKUpdateSystem(void);

	void		AddNotify( HWND hwnd, i3ClassMeta * pMeta, UINT32 EventMask);
	void		RemoveNotify( INT32 idx);
	void		RemoveNotify( HWND hwnd);
	void		RemoveAllNotify(void);
	INT32		FindNotify( HWND hwnd, i3ClassMeta* pMeta, UINT32 EventMask);

	bool		getPendingState(void)					{ return m_bPending; }
	void		setPendingState( bool bState)			{ m_bPending = bState; }

	void		Update( HWND hwnd, UINT32 event, i3ElementBase * pObj, i3ClassMeta * pMeta = nullptr);
};

namespace i3TDK
{
	I3_EXPORT_TDK	void	RegisterUpdate( HWND hwnd, i3ClassMeta * pMeta, UINT32 EventMask);
	I3_EXPORT_TDK	void	Update( HWND hwnd, UINT32 event, i3ElementBase * pObj, i3ClassMeta * pMeta = nullptr);
	I3_EXPORT_TDK	bool	GetPendingUpdateState(void);
	I3_EXPORT_TDK	void	SetPendingUpdateState( bool bState);
};

#endif