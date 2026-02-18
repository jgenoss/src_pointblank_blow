#if !defined( __I3_GIC_SHAPE_RENDER_TEXTURE_H__)
#define __I3_GIC_SHAPE_RENDER_TEXTURE_H__

#include "i3GICShapeRect.h"

#if defined( I3_DEBUG)

class I3_EXPORT_GIC i3GICShapeRenderTexture : public i3GICShapeRect
{
	I3_EXPORT_CLASS_DEFINE( i3GICShapeRenderTexture, i3GICShapeRect);

private:
	// persist member
	i3Texture	*	m_pTexture = nullptr;

	// Volatile member
	HDC				m_hdcMem = nullptr;
	HBITMAP			m_hBitmap = nullptr, m_hOldBitmap = nullptr;

protected:
	void			_CreateImage( void);
	void			_DeleteImage( void);

	void			_CreateGDIOldBitmap( void);
	void			_DeleteGDIOldBitmap( void);

public:
	i3Texture *		getTexture( void)						{ return m_pTexture; }


public:
	i3GICShapeRenderTexture();
	virtual ~i3GICShapeRenderTexture();

	void			SetTexture( i3Texture * pTex);

	virtual void	OnDraw( i3GICDiagramContext * pCtx);

	/** \brief Diagram을 Bind/Unbind할시에 호출된다.
		\note Diagram이 보여지면 GDI Object를 생성하고
			그렇지 않으면 삭제한다.
		\param[in] bBind Bind 유무
			*/
	virtual void	OnBindDiagram( bool bBind);

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);

	virtual void	OnProperty(CMFCPropertyGridCtrl * pCtrl);
};

#endif	// I3_DEBUG

#endif
