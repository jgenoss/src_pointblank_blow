#include "i3FrameworkPCH.h"
#include "i3UIDoubleText.h"

#include "i3UIStaticText.h"
#include "i3UITemplate_DoubleText.h"
#include "i3Base/string/ext/utf16_to_utf8.h"

I3_CLASS_INSTANCE( i3UIDoubleText);


i3UIDoubleText::~i3UIDoubleText()
{
	I3_SAFE_RELEASE( m_pText1);
	I3_SAFE_RELEASE( m_pText2);
}

/*virtual*/ void i3UIDoubleText::OnBindTemplate(void)
{
	i3UIControl::OnBindTemplate();

	if( m_pText1 == nullptr)
	{
		m_pText1 = i3UIStaticText::new_object();
		AddControl( m_pText1);
		m_pText1->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
		m_pText1->addStyle( I3UI_STYLE_SLAVE);
	}

	if( m_pText2 == nullptr)
	{
		m_pText2 = i3UIStaticText::new_object();
		AddControl( m_pText2);
		m_pText2->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
		m_pText2->addStyle( I3UI_STYLE_SLAVE);
	}

	m_pText1->SetTemplate( m_pTemplate->getSubTemplate( I3UI_DOUBLETEXT_SUBTEMPLATE_TEXT0));
	m_pText1->SetTemplate( m_pTemplate->getSubTemplate( I3UI_DOUBLETEXT_SUBTEMPLATE_TEXT1));

	_UpdateShapeState();
}

const WCHAR16 * i3UIDoubleText::GetText( INT32 idx)
{
	if( idx == 0)
		return m_pText1->getText();
	else
		return m_pText2->getText();
}

void i3UIDoubleText::SetText( const WCHAR16 * pText1, const char * pText2)
{
	I3ASSERT( m_pText1 != nullptr);
	I3ASSERT( m_pText2 != nullptr);

	m_pText1->SetText( pText1);
	m_pText2->SetText( pText2);
}

void i3UIDoubleText::SetText( const char* pText1, const char * pText2)
{
	I3ASSERT( m_pText1 != nullptr);
	I3ASSERT( m_pText2 != nullptr);

	m_pText1->SetText( pText1);
	m_pText2->SetText( pText2);
}

extern "C" {
	int i3UIDoubleText_GetText( LuaState * L)
	{
		i3UIDoubleText * pCtrl = (i3UIDoubleText*) i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
//			char szStr[ MAX_PATH];
//			i3mem::FillZero( szStr, sizeof( szStr));
			i3::string u8str;
			i3::utf16_to_utf8(pCtrl->GetText(0), u8str);			
			i3Lua::PushString(L, u8str.c_str());							

			i3::utf16_to_utf8(pCtrl->GetText(1), u8str);
			i3Lua::PushString(L, u8str.c_str());

			// wchar_t -> mb -> utf8으로 변환하는건 아주 비효율임!
			// 실제로는 wchar_t->mb->wchar_t->utf8이 과정이 되므로 뻘스러운 원상복귀가 중간에 끼이기도한다...
			// utf16 -> utf8은 mb변환보다 속도도 빠르기 때문에 그냥 그걸 쓰면 된다..
			//
			//	i3String::CopyWCHARtoASCII( szStr, MAX_PATH, pCtrl->GetText( 0), i3::generic_string_size( pCtrl->GetText( 0)));
			//	i3Lua::PushA2UTF8String( L, szStr);
			
			//	i3mem::FillZero( szStr, sizeof( szStr));
			//  i3String::CopyWCHARtoASCII( szStr, MAX_PATH, pCtrl->GetText( 1), i3::generic_string_size( pCtrl->GetText( 1)));
			//  i3Lua::PushA2UTF8String( L, szStr);

			return 2;
		}

		return 0;
	}

	int i3UIDoubleText_SetText( LuaState * L)
	{
		i3UIDoubleText * pCtrl = (i3UIDoubleText*) i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			char szTemp[ MAX_PATH] = {0,};
			char szTemp2[ MAX_PATH] = {0,};

			i3Lua::GetStringArg( L, 2, szTemp, sizeof( szTemp));
			i3Lua::GetStringArg( L, 3, szTemp2, sizeof( szTemp2));

			pCtrl->SetText( szTemp, szTemp2);
		}
		return 0;
	}
}

LuaFuncReg i3UIDoubleText_Glue[] = {
	{ "uiDoubleText_getText",			i3UIDoubleText_GetText},
	{ "uiDoubleText_setText",			i3UIDoubleText_SetText},
	{ nullptr,								nullptr }
};

void i3UIDoubleText::RegisterLuaFunction( LuaState * pState)
{
	INT32 i;

	if( pState == nullptr)
		return;

	for( i = 0; i3UIDoubleText_Glue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UIDoubleText_Glue[i].name, i3UIDoubleText_Glue[i].func);
	}
}
