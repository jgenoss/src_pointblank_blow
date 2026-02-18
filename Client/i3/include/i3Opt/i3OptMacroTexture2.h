#if !defined( __I3_OPT_MACRO_TEXTURE2_H)
#define __I3_OPT_MACRO_TEXTURE2_H

#include "i3SceneOptimizer.h"

// 각 Geometry마다 사용하는 Texture들을 모은 정보.
class GEO_TEXBIND_INFO
{
protected:
	INT32			m_TexCoordIndex;
	i3Texture *		m_pMap;
	i3::vector<i3Texture*> m_RelList;	
	i3::vector<i3Geometry*>	m_GeoList;

public:
	GEO_TEXBIND_INFO(void);
	~GEO_TEXBIND_INFO(void);

	INT32			getTexCoordIndex(void)				{ return m_TexCoordIndex; }
	void			setTexCoordIndex( INT32 val)		{ m_TexCoordIndex = val; }

	i3Texture *		getTexture(void)					{ return m_pMap; }
	void			setTexture( i3Texture * pTex)		{ I3_REF_CHANGE( m_pMap, pTex); }

	void			addRelTex( const i3::vector<i3Texture*>& List);
	INT32			getRelTexCount(void)				{ return (INT32)m_RelList.size(); }
	i3Texture *		getRelTex( INT32 idx)				{ return m_RelList[idx]; }
	void			removeAllRelTex(void);
	i3Texture *		getRelTexByFlag( UINT32 flag);

	INT32			getGeoCount(void)					{ return (INT32)m_GeoList.size(); }
	i3Geometry *	getGeo( INT32 idx)					{ return m_GeoList[idx]; }
	void			addGeo( i3Geometry * pGeo);
	void			removeAllGeo(void);

	void			reset(void);

};

// 배치 정보
struct TEX_PLACE_INFO
{
	i3Texture *		m_pTex = nullptr;
	i3Texture *		m_pTargetTex = nullptr;
	INT32			m_X = 0;
	INT32			m_Y = 0;
	INT32			m_Width = 0;
	INT32			m_Height = 0;
	INT32			m_idxMacro = 0;
} ;

// 교환 정보
struct TEX_EXCHANGE_INFO
{
	i3Texture *		m_pSrcTex = nullptr;
	REAL32			m_U = 0.0f;
	REAL32			m_V = 0.0f;
	INT32			m_idxMacro = 0;
} ;

// Macro Texture에 대한 정보
struct MACRO_TEX_INFO2
{
	i3Texture *		m_pMacro = nullptr;
	INT32			m_Width = 0;
	INT32			m_Height = 0;
} ;

class I3_EXPORT_OPT i3OptMacroTexture2 : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptMacroTexture2, i3SceneOptimizer);
protected:
	INT32			m_TotalTexCount = 0;

	INT32			m_TargetCX;					// [initialize at constructor]
	INT32			m_TargetCY;					// [initialize at constructor]

	i3::vector<GEO_TEXBIND_INFO*> m_GeoList;
	i3::vector<MACRO_TEX_INFO2*> m_MacroTexList;
	i3::vector<TEX_PLACE_INFO*> m_PlaceTexList;
	i3::vector<TEX_EXCHANGE_INFO*> m_ExchangeList;

	bool			m_bDump = false;
	INT32			m_TexCoordIndex = 0;
	INT32			m_Phase = 0;
	INT32			m_idxMacro = 0;

protected:
	i3TextureBindAttr *	_findTexBind( i3Node * pNode, i3ClassMeta * pMeta);

	GEO_TEXBIND_INFO  *	_addGeoBind( i3Texture * pTex, i3Geometry * pGeo, INT32 texIdx, const i3::vector<i3Texture*>& RelList);
	GEO_TEXBIND_INFO  *	_addGeoBind( i3Texture * pTex, i3Geometry * pGeo, INT32 texIdx);


	INT32				_getGeoBindCount(void)				{ return (INT32)m_GeoList.size(); }
	GEO_TEXBIND_INFO *	_getGeoBind( INT32 idx)				{ return m_GeoList[idx]; }
	void				_removeAllGeoBind(void);

	void				_FilterTiledTextures(void);
	bool				_checkTiledGeometry( GEO_TEXBIND_INFO * pInfo);
	bool				_isTilling( i3GeometryAttr * pGeoAttr, INT32 texCoord, INT32 texCX, INT32 texCY);

	void				_RemoveBigTextures(void);
	void				_BuildPlacementList(void);

	TEX_PLACE_INFO *	_addTexPlace( i3Texture * pTex);
	void				_removeAllTexPlace(void);
	bool				isAllTexPlaced(void);
	bool				_canBePlace( i3::pack::RECT * pRect);
	bool				_Rec_TryToPlace( RECT * pRect);
	TEX_PLACE_INFO *	_FindTexPlace( i3Texture * pTargetTex);

	INT32				getExchangeCount(void)				{ return (INT32)m_ExchangeList.size(); }
	TEX_EXCHANGE_INFO *	getExchange( INT32 idx)				{ return m_ExchangeList[idx]; }
	TEX_EXCHANGE_INFO *	findExchange( i3Texture * pTex);
	TEX_EXCHANGE_INFO *	addExchange( i3Texture * pTex, REAL32 u, REAL32 v, INT32 idxMacro);
	void				removeAllExchange(void);

	MACRO_TEX_INFO2 *	addMacroTex( INT32 idx, INT32 maxx, INT32 maxy);

	void				_MakeExchangeList(void);
	void				_CreateMacroTex(void);
	void				_RemoveAllMacroTex(void);



public:
	i3OptMacroTexture2(void);
	
	INT32			getMacroWidth(void)						{ return m_TargetCX; }
	void			setMacroWidth( INT32 w)					{ m_TargetCX = w; }

	INT32			getMacroHeight(void)					{ return m_TargetCY; }
	void			setMacroHeight( INT32 h)				{ m_TargetCY = h; }

	bool			getDumpState(void)						{ return m_bDump; }
	void			setDumpState( bool bFlag)				{ m_bDump = bFlag; }

	virtual bool	OnNode( i3Node * pNode, i3Node * pParent) override;
	virtual void	Optimize( i3SceneGraphInfo * pSg) override;
};

#endif
