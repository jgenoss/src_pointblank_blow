#if !defined( __UI_FLOAT_MINIMISSONINFO_H__)
#define __UI_FLOAT_MINIMISSIONINFO_H__

#include "UIFloatingBase.h"

class UIFloatMiniMissionInfo : public UIFloatingBase
{
	I3_CLASS_DEFINE( UIFloatMiniMissionInfo);

public:
	UIFloatMiniMissionInfo();
	virtual ~UIFloatMiniMissionInfo();

	virtual void	OnCreate( i3GameNode * pParent);

	/** \brief 최소 크기 */
	virtual void	OnMinimize( void);

	/** \brief 최대 크기 */
	virtual void	OnMaximize( void);

	/** \brief 이전 크기 */
	virtual void	OnPreSize( void);

	virtual bool	OnSliding( REAL32 rDeltaSeconds);
};

#endif
