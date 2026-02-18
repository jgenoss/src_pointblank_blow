#if !defined( __I3_GFX_VERTEX_BANK_H)
#define __I3_GFX_VERTEX_BANK_H

#include "i3GfxResource.h"

#define DEFAULT_VERTEXBANK_VERTEX_COUNT_MAX	20000

typedef struct
{
	i3VertexArray * m_pVA;
	UINT32			m_nCurIdx;
	UINT32			m_nNextIdx;
}VERTEX_BANK_ELEMENT;

enum VB_TYPE
{
	VB_TYPE_DEFAULT = 0,
	VB_TYPE_TRANSFORMED,
};

class I3_EXPORT_GFX i3GfxVertexBank : public i3GfxResource
{
	I3_CLASS_DEFINE( i3GfxVertexBank);

public:
	i3List			m_VertextBankList;	

public:
	i3GfxVertexBank(void);
	virtual ~i3GfxVertexBank(void);

	VERTEX_BANK_ELEMENT*	CreateVertexBank( i3VertexFormat* pformat, UINT32 nSize);

	VERTEX_BANK_ELEMENT*	GetVertexArray( i3VertexFormat* pformat, UINT32 nSize);
	VERTEX_BANK_ELEMENT*	FindVertexArray( i3VertexFormat* pformat, UINT32 nSize);
	void					ResetIndex(void);

	virtual BOOL			Destroy( bool bLostDevice);
	virtual BOOL			Revive( i3RenderContext * pCtx);
};

#endif	// __I3_GFX_VERTEX_BANK_H
