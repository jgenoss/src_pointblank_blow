#if !defined( __UI_TAB_PROFILEBASE_V10_H__)
#define __UI_TAB_PROFILEBASE_V10_H__

#include "UI/UITabBase.h"

class UITabProfileBase_V10 : public UITabBase
{
	I3_ABSTRACT_CLASS_DEFINE( UITabProfileBase_V10, UITabBase);

public:
	UITabProfileBase_V10();
	virtual ~UITabProfileBase_V10();
	void OnCreate( i3GameNode * pParent);

//	virtual void	OnOpen();
protected:
	i3UIScene *		m_pScene;
};

#endif
