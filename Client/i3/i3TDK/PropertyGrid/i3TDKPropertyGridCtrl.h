#ifndef		__I3_TDK_PROPERTY_GRID_CTRL_H__
#define		__I3_TDK_PROPERTY_GRID_CTRL_H__

#if defined( I3_WINDOWS)

#include "i3TDKPropertyGridProperty.h"
#include "i3TDKPropertyGridImageViewer.h"
#include "i3TDKPropertyGridProperty_Image.h"

#define I3_TDK_UPDATE_BEFORE_PROP_CHANGE	0x00000001

//모든 프로퍼티를 관리하는 클래스
//윈도우 메시지 처리
//툴팁에 이미지 표시하기 위해서 정의해놓은 변수들, 함수들은 현재 안쓰임
class I3_EXPORT_TDK i3TDKPropertyGridCtrl : public CMFCPropertyGridCtrl
{
	DECLARE_DYNAMIC( i3TDKPropertyGridCtrl )

private:
	i3ElementBase*					m_pBase = nullptr;		//AddProp에서 설정됩니다.
	CMFCPropertyGridProperty*		m_pCurProp = nullptr;
	BOOL							m_bCreated = FALSE;
	
	i3::vector<i3TDKPropertyGridProperty*>	m_DrawProps;			
	
	i3::vector<CRect*>								m_Rects;				
	i3::vector<i3TDKPropertyGridImageViewer*>		m_ImageViewers;		
	i3::vector<i3TDKPropertyGridProperty_Image*>	m_ImageProps;			

private:
	
	virtual void	OnPropertyChanged(  CMFCPropertyGridProperty* pProp ) const;
	
	CMFCPropertyGridProperty*			_GetProperty( PROPERTYINFO* pInfo);
public:
	i3TDKPropertyGridCtrl() {}
	virtual ~i3TDKPropertyGridCtrl();

	virtual BOOL	Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID );
	virtual void OnClickButton(CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	i3ElementBase*			getBaseObj( void)		{	return m_pBase;	}
	
public:
	BOOL			isCreated( void)			{ return m_bCreated; }

	void			AddProp( i3ElementBase* pObj);
	void			ClearProps( void);

	void			AddDrawProp( i3TDKPropertyGridProperty* pProp);
	void			RemoveDrawProp( i3TDKPropertyGridProperty* pProp);

	void			AddRect( CRect* pRect);
	void			RemoveRect( CRect* pRect);

	void			AddImageViewer( i3TDKPropertyGridImageViewer* pImageViewer);
	void			RemoveImageViewer( i3TDKPropertyGridImageViewer* pImageViewer);

	void			AddImageProp( i3TDKPropertyGridProperty_Image*	pImageProp);
	void			RemoveImageProp( i3TDKPropertyGridProperty_Image*	pImageProp);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void			_UpdateToProperties( CMFCPropertyGridProperty* pProp);
	void			UpdateToProperties( void);

	i3TDKPropertyGridProperty*	_GetPropertyByName( CMFCPropertyGridProperty* pProp, const char* pszName);
	i3TDKPropertyGridProperty*	GetPropertyByName( const char* pszName);

	void			ProcessMessages( const i3::vector<PROPERTYMSG*>& msgList);
	bool			_ProcessMessage( i3TDKPropertyGridProperty* pProperty, EI3TDK_PROPERTY_MSG msg);

protected:
	i3::vector<struct OBJ_INFO*>	m_NotifyList;		
public:
	void			addToNotifyList( i3ElementBase* pObj);
	void			removeFromNotifyList( i3ElementBase* pObj);
	void			clearNotifyList( void);
	void			getNotifyObjList( i3::vector<i3ElementBase*>& Out);
	bool			notifyListContainsSameObjTypes( void);

};

#endif

#endif