#if !defined( __VTX_BUFFER_H)
#define __VTX_BUFFER_H

struct _VTX
{
	REAL32	x = 0.0f, y = 0.0f, z = 0.0f;
	UINT32	c = 0;
} ;

class VtxBuffer : public i3ElementBase
{
	I3_CLASS_DEFINE( VtxBuffer, i3ElementBase);
protected:
	_VTX *		m_pBuff;
	INT32		m_MaxCount;
	INT32		m_Count;

public:
	VtxBuffer(void);
	virtual ~VtxBuffer(void);

	INT32		getMaxCount(void)						{ return m_MaxCount; }
	INT32		getCount(void)							{ return m_Count; }

	INT32		getVertexStride(void)					{ return sizeof(_VTX); }
	_VTX *		getVertex( INT32 idx = 0)				{ return & m_pBuff[idx]; }

	_VTX *		alloc( INT32 cnt = 1);
	
	void		reset(void);
};

#endif
