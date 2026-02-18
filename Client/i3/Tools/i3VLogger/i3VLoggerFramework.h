#if !defined( __I3_VLOGGER_FRAMEWORK_H)
#define __I3_VLOGGER_FRAMEWORK_H

#include "VtxBuffer.h"

class i3VLoggerFramework : public i3ViewerFramework
{
	I3_CLASS_DEFINE( i3VLoggerFramework, i3ViewerFramework);
protected:

	VtxBuffer *		m_pLineVB;
	VtxBuffer *		m_pTriVB;
	CWnd *			m_pViewerCtrl;

	//
	UINT32			m_Color;
	VEC3D			m_LastPos;

	i3BoundBox		m_BBox;

	REAL32			m_BoundSize;

protected:

	void			_DrawAxis(void);
	void			_DrawBox( VEC3D * pCenter, REAL32 sz);

public:
	i3VLoggerFramework(void);
	virtual ~i3VLoggerFramework(void);

	void			setViewerCtrl( CWnd * pCtrl)		{ m_pViewerCtrl = pCtrl; }

	void			PushCmd( I3VLOG_HEADER * pHead, INT32 sz);

	virtual void	OnRender(void) override;
	virtual	void	OnKeyDown( UINT32 nKey) override;
};

#endif
