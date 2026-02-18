#if !defined( __I3_UI_FONT_CACHE2_H)
#define __I3_UI_FONT_CACHE2_H

#include "i3UIGlyph.h"

#define		NUM_FCN_CHILD		4

struct I3UI_FONT_CACHE_NODE
{
protected:
	INT32	m_ChildID[NUM_FCN_CHILD] = { 0 };

public:
	INT32	m_X = 0;
	INT32	m_Y = 0;

	INT32	m_ID = 0;
	INT32	m_Depth = 0;
	i3UIGlyph *	m_pGlyph = nullptr;

	INT32	m_ParentID = -1;

	void	reset(void)
	{
		m_X = m_Y = 0;
		
		m_Depth = 0;
		
		m_pGlyph = nullptr;
		
		m_ParentID = -1;

		for( INT32 i = 0; i < NUM_FCN_CHILD; i++)
		{
			m_ChildID[i] = -1;
			
		}
	}

	INT32	getChild( INT32 idx)					{ return m_ChildID[idx]; }
	void	setChild( INT32 idx, INT32 childID)		{	m_ChildID[idx] = childID;	}

	void	setParent( INT32 parentID)				{	m_ParentID = parentID;		}
};

class I3_EXPORT_FRAMEWORK i3UIFontCache: public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3UIFontCache, i3ElementBase);
protected:
	I3UI_FONT_CACHE_NODE *	m_pNodes = nullptr;
	
	INT32					m_MaxNodes = 0;

	INT32					m_FreeHeadID = -1;
	INT32					m_RootID = -1;

	INT32					m_SpaceWidth = 0;
	INT32					m_SpaceHeight = 0;

protected:
	void					_ResizeNodes( INT32 cnt);

	void					getSplittedSpace( I3UI_FONT_CACHE_NODE * pNode, i3Rect * pRectSpace, i3Rect * pRects);

	INT32					Alloc( INT32 x, INT32 y, INT32 depth);
	void					Free( INT32	nodeID);

	INT32					_Rec_AddGlyph( INT32	idNode, i3UIGlyph * pGlyph, i3Rect * pSpace = nullptr);

	void					_Rotate( I3UI_FONT_CACHE_NODE * pNode);
	void					_LeftRotate( I3UI_FONT_CACHE_NODE * pNode);
	void					_RightRotate( I3UI_FONT_CACHE_NODE * pNode);

	void					_AddChild( INT32 parentID, INT32 childID, INT32 idx, bool bRotate = false);
public:
	virtual ~i3UIFontCache(void);

	bool						Create( INT32 spaceW, INT32 spaceH);
	void						Reset(void);
	void						ChangeSpace( INT32 spaceW, INT32 spaceH);

	I3UI_FONT_CACHE_NODE *		AddGlyph( i3UIGlyph * pGlyph);

};

#endif
