#if !defined( __I3LV_RES_TEXTURE_H)
#define __I3LV_RES_TEXTURE_H

#include "i3LevelRes.h"

class I3_EXPORT_TDK i3LevelResTexture : public i3LevelRes
{
	I3_EXPORT_CLASS_DEFINE( i3LevelResTexture, i3LevelRes);

protected:
	// Persistant members

	// Volatile members
	UINT32				m_Width = 0;
	UINT32				m_Height = 0;
	I3G_IMAGE_FORMAT	m_Format = I3G_IMAGE_FORMAT_NONE;
	UINT32				m_MipCount = 0;

	i3Texture *			m_pTexture = nullptr;			// Brief Load 시에는 생성하지 않는다.

	UINT32				m_IconWidth = 0;
	UINT32				m_IconHeight = 0;
	HBITMAP				m_hBitmap = nullptr;			// 

protected:
	void				_UpdateInfo( i3Texture * pTex);
	HBITMAP				_MakeThumbnail( i3Texture * pTex);

public:
	i3LevelResTexture(void);
	virtual ~i3LevelResTexture(void);

	UINT32				getWidth(void)						{ return m_Width; }
	UINT32				getHeight(void)						{ return m_Height; }
	I3G_IMAGE_FORMAT	getFormat(void)						{ return m_Format; }
	UINT32				getMipmapCount(void)				{ return m_MipCount; }

	i3Texture *			getTexture(void)					{ return m_pTexture; }
	void				setTexture( i3Texture * pTex)		{ I3_REF_CHANGE( m_pTexture, pTex); }

	UINT32				getIconWidth()						{ return m_IconWidth;}
	UINT32				getIconHeight()						{ return m_IconHeight;}
	HBITMAP				getBitmap(void)						{ return m_hBitmap; }

	// 간략한 정보를 Validate 시키기 위해 Scene에서 호출된다.
	// 실제 Resource를 위한 메모리를 할당하는 등의 처리는 하지 않는다.
	virtual bool		OnValidateBrief( bool bForce) override;

	// 실제 Resource를 메모리로 읽어 들이는 단계.
	virtual bool		OnValidate( i3LevelScene * pScene, bool bForce) override;
	virtual i3GameRes *	CreateGameRes(void) override;
	
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
