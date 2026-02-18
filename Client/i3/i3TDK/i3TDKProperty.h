#if !defined( __I3_TDK_PROPERTY_H)
#define __I3_TDK_PROPERTY_H

#include "i3Base.h"
#include "i3TDKPropertyContainer.h"

#define I3TDK_PST_READONLY			0x00000001			// Property가 읽기 전용임을 의미한다.
#define I3TDK_PST_EDITABLE			0x00000002			// Edit() 함수를 통해 수정되어야 함을 의미한다. (Ex. Color)
#define I3TDK_PST_OWNERDRAW			0x00000004			// Draw() 함수를 통해 직접 표현되어야 함을 의미한다.
#define I3TDK_PST_INVISIBLE			0x00000008			// Property Control 등에서 나타나지 않음을 의미한다.
#define I3TDK_PST_VOLATILE			0x00000100			// 해당 Property가 파일에 저장되지 않아야 함을 의미한다.

//클릭 되었을때 상태를 편집 방법
#define I3TDK_PST_LIST				0x00000010			// 주어진 List 값의 범위에서 선택되어야 함을 의미한다.(예 , TRUE-FALSE)
#define I3TDK_PST_EDIT				0x00000020			// Edit Box로 값을 변환(예: 이름, 간단한 정수, 실수 값)
#define I3TDK_PST_DIALOG			0x00000040			// Dialog box로 값을 변환 한다. 
#define I3TDK_PST_MODALDIALOG		0x00000080			// Dialog Box가 뜰 수 있지만 반드시 Modal로 동작해야 하는 경우

struct VAL2STRINFO
{
	INT32	m_Value = 0;
	char	m_szText[64] = { 0 };
} ;

class I3_EXPORT_TDK i3TDKProperty : public i3TDKPropertyContainer
{
	I3_EXPORT_CLASS_DEFINE( i3TDKProperty, i3TDKPropertyContainer);
protected:
	UINT32		m_Style = 0;

	// 자신을 포함하고 있는 Element/Resource에 대한 포인터
	i3TDKPropertyContainer *	m_pOwner = nullptr;
	BOOL						m_bNotifyToOwner = TRUE;

public:
	i3TDKProperty(void) {}
	virtual ~i3TDKProperty(void) {}

	i3TDKPropertyContainer *		GetOwner(void)								{ return m_pOwner; }	
	void			SetOwner( i3TDKPropertyContainer * pOwner)				{ m_pOwner = pOwner; }

	void			SetStyle(INT32 Style)								{ m_Style = Style; }
	INT32			GetStyle(void)										{ return m_Style; }
	void			AddStyle(INT32 Style) 								{ m_Style = m_Style | Style; }
	void			RemoveStyle(INT32 Style)					 		{ m_Style = m_Style & ~(Style ); }

	BOOL			GetNotifyEnable(void)								{ return m_bNotifyToOwner; }
	void			SetNotifyEnable( BOOL state)						{ m_bNotifyToOwner = state; }

	// 주어진 영역 안에 자신을 표현
	virtual void Draw(HDC * pDC, RECT * pRect) {}

	// 문자열 형으로 자신의 값을 표현
	virtual void Get( TCHAR* pszValueStr, const INT32 nValueStrMax) = 0;

	// 문자열 형으로 주어진 사용자의 값을 자신의 값에 적용시킨다.
	virtual void Set( const TCHAR* pszValueStr) = 0;
	
	virtual void				GetValueList(i3::vector<i3::rc_string>& List) {}
	virtual void				PopupModalDialog(void);

	virtual void				CopyFrom( i3TDKPropertyContainer * pOther, BOOL bRef = TRUE);

	i3TDKProperty & operator = (const i3TDKProperty & pProperty);
};

#endif