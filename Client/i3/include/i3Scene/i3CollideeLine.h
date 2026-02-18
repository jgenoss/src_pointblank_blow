#if !defined( __I3_COLLIDEE_LINE_H)
#define __I3_COLLIDEE_LINE_H

#include "i3Base.h"
#include "i3Math.h"
#include "i3Collidee.h"
#include "i3Node.h"

class I3_EXPORT_SCENE i3CollideeLine : public i3Collidee
{
	I3_EXPORT_CLASS_DEFINE( i3CollideeLine, i3Collidee);

protected:
	VEC3D		m_Start;
	VEC3D		m_End;
	VEC3D		m_Dir;

public:
	VEC3D*		GetStart(void)					{ return &m_Start; }
	VEC3D*		GetEnd  (void)					{ return &m_End; }
	VEC3D*		GetDir  (void)					{ return &m_Dir; }

	void		SetStart(VEC3D* pVec)			{ m_Start = *pVec; }
	void		SetDir  (VEC3D* pVec)
	{
		m_Dir   = *pVec;
		i3Vector::Add(&m_End, &m_Start, &m_Dir);
	}
	void		SetEnd  (VEC3D* pVec)
	{
		m_End   = *pVec;
		i3Vector::Sub( &m_Dir, &m_End, &m_Start);
	}

	virtual void	Transform( MATRIX * pMatrix, i3Collidee * pDest) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
