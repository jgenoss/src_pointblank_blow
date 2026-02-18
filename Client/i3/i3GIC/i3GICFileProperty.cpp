#include "stdafx.h"
#include "i3GICFileProperty.h"
#include "i3GICShapeRenderTexture.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#if defined( I3_DEBUG)

IMPLEMENT_DYNAMIC(i3GICFileProperty, CMFCPropertyGridFileProperty)

void i3GICFileProperty::ChangeValue( void)
{
	I3ASSERT( m_pShape != nullptr);

	COleVariant var = GetValue();

	//CHAR16 szTemp[MAX_PATH];
	//WideCharToMultiByte( CP_ACP, 0, var.bstrVal, -1, szTemp, MAX_PATH, nullptr, nullptr);
	//i3::safe_string_copy( szTemp, var.bstrVal, MAX_PATH);

	if( (var.bstrVal[0] == L'\0') )
		return;

	if( m_UpdateType == GIC_PROPERTY_UPDATE_PATH_TEXTURE)
	{
		if( i3::kind_of<i3GICShapeRenderTexture*>(m_pShape))
		{
			i3ImageFile file;
			i3::string str;
			i3::utf16_to_mb((const wchar_t*)var.bstrVal, str);

			i3Texture * pTex = file.Load( str.c_str());
			((i3GICShapeRenderTexture*) m_pShape)->SetTexture( pTex);
			if (pTex)
				pTex->SetName( str );
			I3_SAFE_RELEASE( pTex);

			m_pShape->addState( I3GIC_STATE_REDRAW);
		}
	}
}

/*virtual*/ BOOL i3GICFileProperty::OnUpdateValue( void)
{
	BOOL bRv = CMFCPropertyGridFileProperty::OnUpdateValue();

	if( m_pShape != nullptr)
		ChangeValue();

	return bRv;
}

/*virtual*/ void i3GICFileProperty::OnClickButton( CPoint point)
{
	CMFCPropertyGridFileProperty::OnClickButton( point);

	if( m_pShape != nullptr)
		ChangeValue();

}

/*virtual*/ BOOL i3GICFileProperty::OnEndEdit( void)
{
	BOOL bRv = CMFCPropertyGridFileProperty::OnEndEdit();

	if( m_pShape != nullptr)
		ChangeValue();

	return bRv;
}


#endif	// I3_DEBUG

