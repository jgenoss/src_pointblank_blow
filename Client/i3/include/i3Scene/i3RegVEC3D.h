#if !defined( __I3_REG_VEC3D_H__)
#define __I3_REG_VEC3D_H__

#include "i3RegData.h"

class I3_EXPORT_SCENE i3RegVEC3D :	public i3RegData
{
	I3_EXPORT_CLASS_DEFINE( i3RegVEC3D, i3RegData);

protected:
	VEC3D	m_Value;

public:
	inline	VEC3D	&	operator = ( const VEC3D & Value)
	{
		i3Vector::Copy( &m_Value, (VEC3D*)&Value);
		return m_Value;
	}

	inline i3RegVEC3D & operator = ( const i3RegVEC3D & Value)
	{
		i3Vector::Copy( &m_Value, (VEC3D*)&Value.m_Value);
		return *this;
	}

	i3RegVEC3D * get()	{	return this;}
	void		 set( i3RegVEC3D & Value)
	{
		*this = Value;
	}

	VEC3D	*	getValue()	{	return &m_Value;}
	void			setValue( VEC3D * pVec)			{	i3Vector::Copy( &m_Value, pVec);}
	void			setValue( REAL32 x, REAL32 y, REAL32 z)	{	i3Vector::Set( &m_Value, x, y, z);}

public:
	i3RegVEC3D(void);
	i3RegVEC3D( REAL32 x, REAL32 y, REAL32 z);
	i3RegVEC3D( VEC3D * pVec);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual void GetValueByString( i3::wstring& out) override;
};

#endif //__I3_REG_VEC3D_H__