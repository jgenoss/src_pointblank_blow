#if !defined( __UI_TAB_PROFILEBASE_H__)
#define __UI_TAB_PROFILEBASE_H__

#include "UITabBase.h"

typedef void (*TABPROFILE_NEXT_PROCESS)( void* );

class UITabProfileBase : public UITabBase
{
	I3_ABSTRACT_CLASS_DEFINE( UITabProfileBase, UITabBase);

public:
	UITabProfileBase();
	virtual ~UITabProfileBase();

	void	SetNextProcess( TABPROFILE_NEXT_PROCESS pFn, void* ptr );

	virtual bool	RequestChangeTab()
	{	
		//if( GetSlide() )
		//	GetSlide()->SetTargetTime( false, 0.f );		

		return OnExitStart();
	}

	virtual bool	RequestExitTab()
	{
		return true;
	}

	virtual void	GotoBackTab() { };

public:
	virtual void	OnCreate( i3GameNode * pParent ) override;
	virtual void	OnExitEnd() override;

	virtual void	OnLoadAllScenes() override;

private:
	struct _NextPorcessParam
	{
		TABPROFILE_NEXT_PROCESS		fn;
		void*						ptr;

		_NextPorcessParam() : fn( nullptr ), ptr( nullptr )
		{
		}
	};

	_NextPorcessParam	m_NextParam;
	
//	virtual void	OnOpen();
};

#endif
