/**
 * \file	UI\UIFloatMiniDesignation.cpp
 *
 * Implements the user interface float mini designation class.
 */


#include "pch.h"
#include "UIFloatMiniDesignation.h"

I3_CLASS_INSTANCE( UIFloatMiniDesignation);//, UIFloatingBase);

UIFloatMiniDesignation::UIFloatMiniDesignation()
{
	m_AlignedPos = UIFLOATING_POS_LEFTTOP;

#if defined( I3_DEBUG)
	SetName( "MiniDesignation");
#endif
}

UIFloatMiniDesignation::~UIFloatMiniDesignation()
{
}


//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
}

LuaFuncReg FloatMiniDesignation_Glue[] =
{
	{ NULL,			NULL}
};

//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
/*virtual*/ void UIFloatMiniDesignation::OnCreate( i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);

	// Load Scene
}

/** \brief 최소 크기 */
/*virtual*/ void UIFloatMiniDesignation::OnMinimize( void)
{
}

/** \brief 최대 크기 */
/*virtual*/ void UIFloatMiniDesignation::OnMaximize( void)
{
}

/** \brief 이전 크기 */
/*virtual*/ void UIFloatMiniDesignation::OnPreSize( void)
{
}
