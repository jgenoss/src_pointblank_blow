#if !defined( __I3_REG_VEC2D_H__)
#define __I3_REG_VEC2D_H__

#include "i3RegData.h"

class I3_EXPORT_SCENE i3RegVEC2D :	public i3RegData
{
	I3_CLASS_DEFINE( i3RegVEC2D);

protected:
	RT_VEC2D	m_Value;

public:
	inline	RT_VEC2D	&	operator = ( const RT_VEC2D & Value)
	{
		i3Vector::Copy( &m_Value, (RT_VEC2D*)&Value);
		return m_Value;
	}

	inline i3RegVEC2D	& operator = ( const i3RegVEC2D & Value)
	{
		i3Vector::Copy( &m_Value, (RT_VEC2D*)&Value.m_Value);
		return *this;
	}


public:
	i3RegVEC2D * get()	{	return this;}
	void		 set( i3RegVEC2D & Value)
	{
		*this = Value;
	}

	RT_VEC2D *	getValue()	{	return & m_Value;}
	void		setValue( RT_VEC2D * pVec)			{	i3Vector::Copy( &m_Value, pVec);}
	void		setValue( RT_REAL32 u, RT_REAL32 v)	{	i3Vector::Set( &m_Value, u, v);}

public:
	i3RegVEC2D(void);
	i3RegVEC2D( RT_REAL32 u, RT_REAL32 v);
	i3RegVEC2D( RT_VEC2D * pVec);

	virtual ~i3RegVEC2D(void);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif //__I3_REG_VEC2D_H__