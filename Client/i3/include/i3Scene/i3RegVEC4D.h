#if !defined( __I3_REG_VEC4D_H__)
#define __I3_REG_VEC4D_H__

#include "i3RegData.h"

class I3_EXPORT_SCENE i3RegVEC4D :	public i3RegData
{
	I3_EXPORT_CLASS_DEFINE( i3RegVEC4D, i3RegData);

protected:
	VEC4D	m_Value;

public:
	inline	VEC4D	&	operator = ( const VEC4D & Value)
	{
		i3Vector::Copy( &m_Value, (VEC4D*)&Value);
		return m_Value;
	}

	inline i3RegVEC4D & operator = ( const i3RegVEC4D & Value)
	{
		i3Vector::Copy( &m_Value, (VEC4D*)&Value.m_Value);
		return *this;
	}

	inline i3RegVEC4D & operator >> ( VEC4D & Value)
	{
		i3Vector::Copy( &Value, &m_Value);
		return *this;
	}

	i3RegVEC4D * get()	{	return this;}
	void		 set( i3RegVEC4D & Value)
	{
		*this = Value;
	}

	VEC4D	*	getValue()	{	return &m_Value;}
	void			setValue( VEC4D * pVec)			{	i3Vector::Copy( &m_Value, pVec);}
	void			setValue( REAL32 x, REAL32 y, REAL32 z, REAL32 w)	{	i3Vector::Set( &m_Value, x, y, z, w);}

public:
	i3RegVEC4D(void);
	i3RegVEC4D( REAL32 x, REAL32 y, REAL32 z, REAL32 w);
	i3RegVEC4D( VEC4D * pVec);
	i3RegVEC4D( i3RegVEC4D * pVec);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual void GetValueByString( i3::wstring& out) override;
};

#endif //__I3_REG_VEC4D_H__