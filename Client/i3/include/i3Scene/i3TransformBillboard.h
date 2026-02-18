#if !defined( __I3_TRANSFORM_BILLBOARD_H)
#define __I3_TRANSFORM_BILLBOARD_H

#include "i3Base.h"
#include "i3Math.h"
#include "i3Transform.h"
#include "i3ModelViewMatrixAttr.h"

class I3_EXPORT_SCENE i3TransformBillboard : public i3Transform
{
	I3SG_NODE_CLASS_DEFINE( i3TransformBillboard, i3Transform);

protected:
	bool					m_bAnchorBillboard = true;
	VEC3D					m_vAnchorAxis = I3_YAXIS;

public:
	i3TransformBillboard(void);

	void			setAnchorEnable( bool bEnable = true) { m_bAnchorBillboard = bEnable;}
	void			SetAhchorAxis( VEC3D *pvAxis);

	void			GetBillboardMatrix( MATRIX * pOut, MATRIX * pIn, MATRIX * pView);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	PreTrace(i3SceneTracer * pTracer) override;
	virtual void	OnUpdate( i3SceneTracer * pTracer) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
