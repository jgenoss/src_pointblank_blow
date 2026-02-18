#if !defined( __I3_REG_VEC3D_H__)
#define __I3_REG_VEC3D_H__

#include "i3RegData.h"

class I3_EXPORT_SCENE i3RegVEC3D :	public i3RegData
{
	I3_CLASS_DEFINE( i3RegVEC3D);

protected:
	RT_VEC3D	m_Value;

public:
	inline	RT_VEC3D	&	operator = ( const RT_VEC3D & Value)
	{
		i3Vector::Copy( &m_Value, (RT_VEC3D*)&Value);
		return m_Value;
	}

	inline i3RegVEC3D & operator = ( const i3RegVEC3D & Value)
	{
		i3Vector::Copy( &m_Value, (RT_VEC3D*)&Value.m_Value);
		return *this;
	}

	i3RegVEC3D * get()	{	return this;}
	void		 set( i3RegVEC3D & Value)
	{
		*this = Value;
	}

	RT_VEC3D	*	getValue()	{	return &m_Value;}
	void			setValue( RT_VEC3D * pVec)			{	i3Vector::Copy( &m_Value, pVec);}
	void			setValue( RT_REAL32 x, RT_REAL32 y, RT_REAL32 z)	{	i3Vector::Set( &m_Value, x, y, z);}

public:
	i3RegVEC3D(void);
	i3RegVEC3D( RT_REAL32 x, RT_REAL32 y, RT_REAL32 z);
	i3RegVEC3D( RT_VEC3D * pVec);

	virtual ~i3RegVEC3D(void);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif //__I3_REG_VEC3D_H__