#if !defined( __I3_OPT_MACRO_TEXTURE_H)
#define __I3_OPT_MACRO_TEXTURE_H

#include "i3SceneOptimizer.h"

struct TEX_MERGE_INFO
{
	bool		m_bPlaced = false;
	i3Texture * m_pTex = nullptr;
	INT32		m_X = 0;
	INT32		m_Y = 0;
	INT32		m_Width = 0;
	INT32		m_Height = 0;
	i3::vector<i3Geometry*>		m_GeoList;
	i3::vector<i3TextureBindAttr*>	m_BindList;
} ;

struct MACRO_TEX_INFO
{
	i3Texture * m_pMacroTex = nullptr;
	i3::vector<TEX_MERGE_INFO*>	m_MergeList;
	INT32		m_Width = 0;
	INT32		m_Height = 0;
} ;

class I3_EXPORT_OPT i3OptMacroTexture : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptMacroTexture, i3SceneOptimizer);
protected:
	i3::vector<TEX_MERGE_INFO*>	m_MergeInfoList;
	i3::vector<MACRO_TEX_INFO*> m_MacroTexList;

	INT32			m_TargetCX = 2048;
	INT32			m_TargetCY = 2048;

	i3ClassMeta *	m_pBindMeta = nullptr;
	INT32			m_idxTexCoord = 0;
	char			m_szPrefix[256] = { 0 };

	bool			m_bDump = false;

protected:
	bool					_isTilling( TEX_MERGE_INFO * pInfo, i3GeometryAttr * pGeoAttr);
	bool					_checkTiledGeometry( TEX_MERGE_INFO * pInfo);
	i3TextureBindAttr *		_findLastTexture( i3Node * pNode, i3ClassMeta * pBindMeta);

	TEX_MERGE_INFO *		_addMergeInfo( i3Texture * pTex);
	TEX_MERGE_INFO *		_findMergeInfoByTex( i3Texture * pTex);
	void					_removeAllMergeInfo(void);
	
	void					_FilterTiledTextures(void);

	MACRO_TEX_INFO *		_addMacroTex( void);
	void					_createMacroTex( MACRO_TEX_INFO * pInfo);
	void					_removeAllMacroTex(void);
	MACRO_TEX_INFO *		_FindFittedMacroTex( TEX_MERGE_INFO * pInfo);
	bool					_TryToPlace( MACRO_TEX_INFO * pMacro, TEX_MERGE_INFO * pMerge);
	bool					_Rec_TryToPlace( MACRO_TEX_INFO * pMacro, RECT * pRect);
	bool					_canBePlace( MACRO_TEX_INFO * pMacro, i3::pack::RECT * pRect, bool bCheckForPlaced = false);

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

	virtual bool	OnNode( i3Node * pNode, i3Node * pParent) override;
	virtual void	Optimize( i3SceneGraphInfo * pSg) override;
	virtual bool		isSupportProgress(void) override;
	virtual const char *	getDesc(void) override;
};

#endif
