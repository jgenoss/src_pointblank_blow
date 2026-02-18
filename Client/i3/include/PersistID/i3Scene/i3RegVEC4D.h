#if !defined( __I3_REG_VEC4D_H__)
#define __I3_REG_VEC4D_H__

#include "i3RegData.h"

class I3_EXPORT_SCENE i3RegVEC4D :	public i3RegData
{
	I3_CLASS_DEFINE( i3RegVEC4D);

protected:
	RT_VEC4D	m_Value;

public:
	inline	RT_VEC4D	&	operator = ( const RT_VEC4D & Value)
	{
		i3Vector::Copy( &m_Value, (RT_VEC4D*)&Value);
		return m_Value;
	}

	inline i3RegVEC4D & operator = ( const i3RegVEC4D & Value)
	{
		i3Vector::Copy( &m_Value, (RT_VEC4D*)&Value.m_Value);
		return *this;
	}

	inline i3RegVEC4D & operator >> ( RT_VEC4D & Value)
	{
		i3Vector::Copy( &Value, &m_Value);
		return *this;
	}

	i3RegVEC4D * get()	{	return this;}
	void		 set( i3RegVEC4D & Value)
	{
		*this = Value;
	}

	RT_VEC4D	*	getValue()	{	return &m_Value;}
	void			setValue( RT_VEC4D * pVec)			{	i3Vector::Copy( &m_Value, pVec);}
	void			setValue( RT_REAL32 x, RT_REAL32 y, RT_REAL32 z, RT_REAL32 w)	{	i3Vector::Set( &m_Value, x, y, z, w);}

public:
	i3RegVEC4D(void);
	i3RegVEC4D( RT_REAL32 x, RT_REAL32 y, RT_REAL32 z, RT_REAL32 w);
	i3RegVEC4D( RT_VEC4D * pVec);
	i3RegVEC4D( i3RegVEC4D * pVec);

	virtual ~i3RegVEC4D(void);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif //__I3_REG_VEC4D_H__