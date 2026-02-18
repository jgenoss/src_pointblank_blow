#if !defined( __I3UI_IMAGEBOXEX_H__)
#define __I3UI_IMAGEBOXEX_H__

#include "i3UIImageBox.h"

class i3UIShape;

struct SHAPETEXINFO
{
	i3UIShape * _pShape = nullptr;
	i3Texture * _pTex = nullptr;
};

typedef void (*I3UIIMAGEBOX_PROC)( i3UIControl * pThis, void * pData );

class I3_EXPORT_FRAMEWORK i3UIImageBoxEx : public i3UIImageBox
{
	I3_EXPORT_CLASS_DEFINE( i3UIImageBoxEx, i3UIImageBox);
private:
	i3::vector<SHAPETEXINFO*> m_ShapeList;

	INT32			m_CurrentImage = -1;

	i3Sprite2D *	m_pSprite = nullptr;

	// 불필요 문자열 m_strData 제거됨..
#if defined( I3_DEBUG)
	INT32			m_ImageCount = 1;
	INT32			m_PreImageCount = 0;
	i3::vector<char*> m_ImagePathList;
#endif

protected:
	void			_CalculateSizeByTexSize( REAL32 width, REAL32 height);

	INT32			_FindShapeByPath( const char * pszPath);

public:
	i3UIImageBoxEx();
	virtual ~i3UIImageBoxEx();

	virtual void	prepareSprites( void) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void	OnVisibleScene( void) override;
	virtual void	OnInvisibleScene( void) override;

	virtual void	UpdateRenderObjects() override;

	virtual void	EnableCtrl( bool bTrue, bool bRecursive = false) override;

	INT32			AddTexture( i3Texture * pTex, REAL32 w = 0.f, REAL32 h = 0.f);
	INT32			AddTexture( i3Texture * pTex, INT32 x, INT32 y, INT32 w, INT32 h);
	INT32			AddTextureMovie(i3Texture * pTex, i3::pack::RECT, i3::pack::RECT);
	void			SetCurrentImage( INT32 idx);

	INT32			FindShape( i3Texture * pTex, INT32 x, INT32 y, INT32 w, INT32 h);

	void			DeleteShape( INT32 idx);
	void			DeleteShapeAll();

	void			SetShapeSize( REAL32 w, REAL32 h);
	void			SetRotationRad( REAL32 val);
	void SetTextureCoord(REAL32 u1, REAL32 v1, REAL32 u2, REAL32 v2);

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

#if defined( I3_DEBUG)
	// PropertyGrid 관련..
	virtual void		GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List) override;
	virtual void		OnTDKPropertyChanged( const char* szname) override;

	void			ResetPathCount( void);
#endif

	static void		RegisterLuaFunction(LuaState * pState);
};

#endif
