#if !defined( __I3_OPT_MIPMAP_GEN_H)
#define __I3_OPT_MIPMAP_GEN_H

#include "i3SceneOptimizer.h"

typedef struct _tagTexRefInfo
{
	i3Texture *		m_pTex;
	i3Texture *		m_pNewTex;
} TEX_REF_INFO;

class I3_EXPORT_OPT i3OptMipmapGen : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptMipmapGen);
protected:
	INT32		m_MinCX;
	INT32		m_MinCY;
	i3List		m_TexBindList;

protected:
	void		_RemoveAllTexBindInfo(void);
	INT32		_FindTexRef( i3Texture * pTex);

public:
	i3OptMipmapGen(void);
	virtual ~i3OptMipmapGen(void);

	INT32		getMinWidth(void)				{ return m_MinCX; }
	void		setMinWidth( INT32 w)			{ m_MinCX = w; }

	INT32		getMinHeight(void)				{ return m_MinCY; }
	void		setMinHeight( INT32 h)			{ m_MinCY = h; }

	i3Texture *	GenMipmap( i3Texture * pSrcTex);

	virtual void		Trace( i3Node * pRoot);
	virtual void		SetProperty( char * pszFieldName, char * pszValue);
};

#endif
