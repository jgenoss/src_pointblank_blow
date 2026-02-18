/**
 * \file	UI\UISlotBase.h
 *
 * Declares the user interface slot base class.
 */


#if !defined( __UI_SLOT_BASE_H__)
#define __UI_SLOT_BASE_H__

#include "UIBase.h"

class UISlotBase : public UIBase
{
	I3_ABSTRACT_CLASS_DEFINE( UISlotBase, UIBase);

protected:
	VEC3D			m_vPos;
	VEC3D			m_vOffset;
	VEC3D			m_vOriginalPos;
	VEC2D			m_vSize;

	i3::pack::RECT			m_Rect;

public:
	UISlotBase();
	virtual ~UISlotBase();

	/** \brief 슬롯의 위치값을 계산한다.
		\desc Offset Size에 따라 결정됨 */
	void			CalculatePos( VEC3D * pOut, INT32 argX, INT32 argY, VEC3D * pOffset, VEC2D * pSize, REAL32 px = 0.0f, REAL32 py = 0.0f);

	/** \brief 슬롯을 Enable시킵니다.
		\desc Arg 값에 따라 위치를 설정해줍니다. Size로 계산되어 다음 위치를 설정해줌..
		\param[in] argX X축으로 위치 Index
		\param[in] argY Y축으로 위치 Index
		\param[in] pOffset 기준 위치
		\param[in] pOut 설정된 결과 위치 */
	virtual void	EnableSlot( INT32 argX, INT32 argY, VEC3D * pOffset = nullptr, VEC3D * pOut = nullptr, REAL32 px = 0.0f, REAL32 py = 0.0f);
	virtual void	EnableSlot2( REAL32 totalHeight, REAL32 totalXOffset, REAL32 totalYOffset,VEC3D * pOffset, VEC3D * pOut);

	virtual void	SetEmptySlot(void) {}

	/** \brief 선택에 대한 처리 */
	virtual void	OnSelect( void) {}

	/** \brief 선택 해제에 대한 처리 */
	virtual void	OnUnselect( void) {}
	virtual void	UpdateData( void) {}

	void			SetPos( VEC3D * pPos);
	VEC3D *			GetPos();
	VEC2D *			GetSize( void);

	VEC3D *			GetOriginalPos( void)			{ return &m_vOriginalPos; }
	void			SetOriginalPos( VEC3D * pPos)	{ i3Vector::Copy( &m_vOriginalPos, pPos); }
};

#endif
