#if !defined( __I3_CLUT_H)
#define __I3_CLUT_H

#include "i3Base.h"
#include "i3GfxDefine.h"
#include "i3Math.h"
#include "i3RenderState.h"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

typedef struct PACKED _tagI3_CLUT_FILE_HEADER
{
	UINT8				m_ColorCount;
	I3G_IMAGE_FORMAT	m_Format;
} I3_CLUT_FILE_HEADER;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif



class I3_EXPORT_GFX i3Clut : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3Clut);

protected:
	INT32				m_Count;
	INT32				m_PixelBits;
	I3G_IMAGE_FORMAT	m_Format;

#if defined( I3G_DX)
	PALETTEENTRY *		m_pBuff;

#elif defined( I3G_XBOX)
	PALETTEENTRY *		m_pBuff;

#elif defined( I3G_PSP)	
	void *				m_pBuff;

#elif defined( I3G_PS2)
#elif defined( I3G_OGL)
#elif defined( I3G_OGLES)
	void *				m_pBuff;	

#endif	

public:
	i3Clut(void);
	virtual ~i3Clut(void);

	BOOL	Create( INT32 ColorCount, INT32 PixelBits = 32, INT32 AlphaBits = 8);

	// Pixel Format을 반환한다.
	I3G_IMAGE_FORMAT	GetFormat(void)								{ return m_Format; }

	char *				GetData(void)								{ return (char *) m_pBuff; }
	char *				GetPixel( INT32 Index);

	// 색상수를 반환한다.
	INT32				GetColorCount(void)							{ return m_Count; }

	INT32				FindNearestColor( COLOR * pColor);

	void				GetColor( INT32 Index, COLOR * pColor);
	void				GetColor( INT32 Index, COLORREAL * pColor);

	UINT32				GetColorRGBA32( INT32 idx);
	UINT32				GetColorBGRA32( INT32 idx);

	void				SetColor( INT32 Index, COLOR * pColor);
	void				SetColor( INT32 Index, COLORREAL * pColor);



	UINT32				Load( i3Stream * pStream);
	UINT32				Save( i3Stream * pStream);

	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
};

#endif
