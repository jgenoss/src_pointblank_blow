#if !defined( __UI_TAB_PROFILEBASE_H__)
#define __UI_TAB_PROFILEBASE_H__

#include "UI/UITabBase.h"

class UITabProfileBase_V11 : public UITabBase
{
	I3_ABSTRACT_CLASS_DEFINE( UITabProfileBase_V11, UITabBase);

public:
	UITabProfileBase_V11();
	virtual ~UITabProfileBase_V11();
	void OnCreate( i3GameNode * pParent);

//	virtual void	OnOpen();
protected:
	i3UIScene *		m_pScene;
};

#endif
