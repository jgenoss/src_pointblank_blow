#if !defined( __I3UI_CONTROL_DUMMY_H__)
#define __I3UI_CONTROL_DUMMY_H__

#include "i3UIControl.h"

namespace UIControl_Dummy
{
	enum ALING_STYLE
	{
		ALIGN_NONE	= 0,			//정렬 안함..
		ALIGN_CENTER,				//cell 중앙에 정렬..

		ALIGN_MAX,
	};
};
namespace ud = UIControl_Dummy;

class I3_EXPORT_FRAMEWORK i3UIControl_Dummy : public i3UIControl
{
	I3_EXPORT_CLASS_DEFINE( i3UIControl_Dummy, i3UIControl);

private:
	i3::vector<i3UIControl*>	m_ControlList;
	i3::vector<i3::pack::RECT>			m_ChildRectList;
	i3::vector<ud::ALING_STYLE>	m_AlignStyleList;

	i3UIControl*				m_pPickedControl = nullptr;
public:
	i3UIControl_Dummy(void) {}
	~i3UIControl_Dummy(void);

	void	AddChildControl( i3UITemplate* pTemplate);

	void	SetChildRect( INT32 nChildIdx, REAL32 x, REAL32 y, REAL32 w, REAL32 h);
	void	GetChildRect( INT32 nChildIdx, i3::pack::RECT * p);

	void	SetChildPos( INT32 nChildIdx, REAL32 x, REAL32 y, REAL32 z = 0.f);
	void    SetChildSize( INT32 nChildIdx, REAL32 cx, REAL32 cy);

	void	SetChildAlignStyle( INT32 nChildIdx, INT32 nStyle);
	ud::ALING_STYLE GetChildAlignStyle( INT32 nChildIdx) const;

	void	UpdatePosAndSize();									//위치, 사이즈 반영
	void	CalcControlPosAndSize( REAL32 fx, REAL32 fy);		//위치, 사이즈 업데이트
};


#endif