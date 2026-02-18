#if !defined( __I3_OPT_MACRO_TEXTURE_H)
#define __I3_OPT_MACRO_TEXTURE_H

#include "i3SceneOptimizer.h"

typedef struct _tagTexMergeInfo
{
	bool		m_bPlaced;
	i3Texture * m_pTex;
	INT32		m_X;
	INT32		m_Y;
	INT32		m_Width;
	INT32		m_Height;
	i3List		m_GeoList;
	i3List		m_BindList;
} TEX_MERGE_INFO;

typedef struct _tagMactoTexInfo
{
	i3Texture * m_pMacroTex;
	i3List		m_MergeList;
} MACRO_TEX_INFO;

class I3_EXPORT_OPT i3OptMacroTexture : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptMacroTexture);
protected:
	i3List			m_MergeInfoList;
	i3List			m_PlacedInfoList;

	i3List			m_MacroTexList;

	INT32			m_TotalTexCount;

	INT32			m_TargetCX;
	INT32			m_TargetCY;

	bool			m_bDump;

protected:
	bool					_isTilling( TEX_MERGE_INFO * pInfo, i3GeometryAttr * pGeoAttr);
	bool					_checkTiledGeometry( TEX_MERGE_INFO * pInfo);
	i3TextureBindAttr *		_findLastTexture( i3Node * pNode);

	TEX_MERGE_INFO *		_addMergeInfo( i3Texture * pTex);
	TEX_MERGE_INFO *		_findMergeInfoByTex( i3Texture * pTex);
	void					_removeAllMergeInfo(void);
	
	void					_FilterTiledTextures(void);

	MACRO_TEX_INFO *		_addMacroTex( void);
	void					_removeAllMacroTex(void);
	MACRO_TEX_INFO *		_FindFittedMacroTex( TEX_MERGE_INFO * pInfo);
	bool					_TryToPlace( MACRO_TEX_INFO * pMacro, TEX_MERGE_INFO * pMerge);
	bool					_Rec_TryToPlace( MACRO_TEX_INFO * pMacro, RECT * pRect);
	bool					_canBePlace( MACRO_TEX_INFO * pMacro, I3RECT * pRect, bool bCheckForPlaced = false);

	void					_buildMactoTex(void);
	void					_copyToMacroTex( MACRO_TEX_INFO * pMacro);
	void					_adjustTexCoord(void);
	void					_removeOldTexBind(void);
	bool					_isAllPlaced(void);

public:
	i3OptMacroTexture(void);
	virtual ~i3OptMacroTexture(void);

	INT32			getMacroWidth(void)						{ return m_TargetCX; }
	void			setMacroWidth( INT32 w)					{ m_TargetCX = w; }

	INT32			getMacroHeight(void)					{ return m_TargetCY; }
	void			setMacroHeight( INT32 h)				{ m_TargetCY = h; }

	bool			getDumpState(void)						{ return m_bDump; }
	void			setDumpState( bool bFlag)				{ m_bDump = bFlag; }

	virtual BOOL	OnNode( i3Node * pNode);
	virtual void	Optimize( i3SceneGraphInfo * pSg);
};

#endif
