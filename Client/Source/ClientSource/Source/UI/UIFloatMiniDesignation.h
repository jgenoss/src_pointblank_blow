/**
 * \file	UI\UIFloatMiniDesignation.h
 *
 * Declares the user interface float mini designation class.
 */


#if !defined( __UI_FLOAT_MINIDESIGNATION_H__)
#define __UI_FLOAT_MINIDESIGNATION_H__

#include "UIFloatingBase.h"

class UIFloatMiniDesignation : public UIFloatingBase
{
	I3_CLASS_DEFINE( UIFloatMiniDesignation, UIFloatingBase);

public:
	UIFloatMiniDesignation();
	virtual ~UIFloatMiniDesignation();

	virtual void	OnCreate( i3GameNode * pParent);

	/** \brief 최소 크기 */
	virtual void	OnMinimize( void);

	/** \brief 최대 크기 */
	virtual void	OnMaximize( void);

	/** \brief 이전 크기 */
	virtual void	OnPreSize( void);
};

#endif
