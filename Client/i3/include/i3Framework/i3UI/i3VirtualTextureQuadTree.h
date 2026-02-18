#if !defined( __I3_VIRTUAL_TEXTURE_QUADTREE_H)
#define __I3_VIRTUAL_TEXTURE_QUADTREE_H

//
// VRAM 관리를 위해 사용되는 Quad-Tree의 Node Class.
class I3_VTS_QTNODE
{
protected:
	INT32		m_idxParent;
	INT32		m_idxChild[4];

	UINT64		m_Offset;
	char *		m_pData;


public:
	I3_VTS_QTNODE(void);
	
};

#endif
