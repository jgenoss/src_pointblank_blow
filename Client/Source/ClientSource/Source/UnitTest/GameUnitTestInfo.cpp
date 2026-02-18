#include "pch.h"
#if ENABLE_UNIT_TEST

#include "GameUnitTestInfo.h"

#include "GameUnitTestInfo_Popup.h"

#include "../UI/UIMainFrame.h"
#include "../UI/UIPopupReceiveMail.h"

namespace GameUnitTest
{
	bool PopupList::AddPopup( UIPOPUPWIN _type)
	{
		for( size_t cnt = 0; cnt < m_list.size(); ++cnt)
		{
			if( m_list[cnt]->GetType() == _type)
				return false;
		}
		Popup::TesterBase* p = Popup::g_factory.get_obj( _type);
		m_list.push_back( p);
		return true;
	}

	void PopupList::Clear()
	{
		for( size_t cnt = 0; cnt < m_list.size(); ++cnt)
		{
			I3_SAFE_DELETE( m_list[cnt]);
		}
		m_list.clear();
	}

	bool PopupList::OpenPopup()
	{
		bool rv = false;
		if( IsOpenPopup() == false && m_list.size() > 0)
		{
			INT32 idx = i3Math::Rand()%m_list.size();
			m_list[idx]->Open();
			rv = true;
		}
		return rv;
	}

	void PopupList::ProcessPopup()
	{
		for( size_t cnt = 0; cnt < m_list.size(); ++cnt)
		{
			if( g_pFramework->GetUIMainframe()->IsOpenedPopup( m_list[cnt]->GetType()))
				m_list[cnt]->Process();
		}
	}

	bool PopupList::IsOpenPopup()
	{
		for( size_t cnt = 0; cnt < m_list.size(); ++cnt)
		{
			if( g_pFramework->GetUIMainframe()->IsOpenedPopup( m_list[cnt]->GetType()))
				return true;
		}
		return false;
	}
};

#endif //end of ENABLE_UNIT_TEST