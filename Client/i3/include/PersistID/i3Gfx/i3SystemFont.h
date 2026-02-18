#ifndef __I3G_SYSTEM_FONT_H__
#define __I3G_SYSTEM_FONT_H__

#include "i3Texture.h"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

typedef struct PACKED _tagi3FontCharInfo
{
	UINT16		m_U;				// Fixed Point (0.0 == 0, 1.0 == 65535)
	UINT16		m_V;				// Fixed Point (0.0 == 0, 1.0 == 65535)
	UINT16		m_Width;			// Fixed Point (0.0 == 0, 1.0 == 65535)
	UINT8		m_ShiftX;			// Shift X - in Pixels
	UINT8		m_ShiftY;			// Shift Y - in Pixels
} I3FONTCHARINFO;

typedef struct PACKED _tagi3FontGroupInfo
{
	UINT16				m_StartCode;
	UINT16				m_EndCode;
	I3FONTCHARINFO	*	m_pTable;
	i3Texture *			m_pTexture;
	char *				m_szTextureName;
} I3FONTGROUPINFO;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

class I3_EXPORT_GFX i3SystemFont : public i3NamedElement
{
	I3_CLASS_DEFINE( i3SystemFont );
protected:
	REAL32				m_Height;
	UINT16				m_GroupCount;
	I3FONTGROUPINFO *	m_pGroups;

public:
	i3SystemFont(void);
	virtual ~i3SystemFont(void);

	REAL32				GetHeight(void)					{ return m_Height; }

	UINT16				GetGroupCount(void)				{ return m_GroupCount; }
	void				SetGroupCount( UINT16 count);
	I3FONTGROUPINFO *	GetGroup( UINT16 idx)			{ return & m_pGroups[ idx]; }

	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);

	UINT32				LoadFromFile( const char * pszFileName);
	UINT32				LoadFromFile( i3Stream * pStream);

	UINT32				SaveToFile( const char * pszFileName);
	UINT32				SaveToFile( i3Stream * pStream);
};

#endif //__I3G_SYSTEM_FONT_H__
