#ifndef __I3G_SYSTEM_FONT_H__
#define __I3G_SYSTEM_FONT_H__

#include "i3Texture.h"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace i3
{
	namespace pack
	{
		struct PACKED FONT_CHARINFO
		{
			UINT16		m_U = 0;				// Fixed Point (0.0 == 0, 1.0 == 65535)
			UINT16		m_V = 0;				// Fixed Point (0.0 == 0, 1.0 == 65535)
			UINT16		m_Width = 0;			// Fixed Point (0.0 == 0, 1.0 == 65535)
			UINT8		m_ShiftX = 0;			// Shift X - in Pixels
			UINT8		m_ShiftY = 0;			// Shift Y - in Pixels
		};
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

struct PACKED I3FONT_GROUPINFO
{
	UINT16				m_StartCode = 0;
	UINT16				m_EndCode = 0;
	i3::pack::FONT_CHARINFO	*	m_pTable = nullptr;
	i3Texture *			m_pTexture = nullptr;
	char *				m_szTextureName = nullptr;
};

class I3_EXPORT_GFX i3SystemFont : public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE( i3SystemFont, i3NamedElement );
protected:
	REAL32				m_Height = 0.0f;
	UINT16				m_GroupCount = 0;
	I3FONT_GROUPINFO *	m_pGroups = nullptr;

public:
	virtual ~i3SystemFont(void);

	REAL32				GetHeight(void)					{ return m_Height; }

	UINT16				GetGroupCount(void)				{ return m_GroupCount; }
	void				SetGroupCount( UINT16 count);
	I3FONT_GROUPINFO *	GetGroup( UINT16 idx)			{ return & m_pGroups[ idx]; }

	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;

	UINT32				LoadFromFile( const char * pszFileName);
	UINT32				LoadFromFile( i3Stream * pStream);

	UINT32				SaveToFile( const char * pszFileName);
	UINT32				SaveToFile( i3Stream * pStream);
};

#endif //__I3G_SYSTEM_FONT_H__
