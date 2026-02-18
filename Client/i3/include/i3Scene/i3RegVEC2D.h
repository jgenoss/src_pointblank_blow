#if !defined( __I3_REG_VEC2D_H__)
#define __I3_REG_VEC2D_H__

#include "i3RegData.h"

class I3_EXPORT_SCENE i3RegVEC2D :	public i3RegData
{
	I3_EXPORT_CLASS_DEFINE( i3RegVEC2D, i3RegData);

protected:
	VEC2D	m_Value;

public:
	inline	VEC2D	&	operator = ( const VEC2D & Value)
	{
		i3Vector::Copy( &m_Value, (VEC2D*)&Value);
		return m_Value;
	}

	inline i3RegVEC2D	& operator = ( const i3RegVEC2D & Value)
	{
		i3Vector::Copy( &m_Value, (VEC2D*)&Value.m_Value);
		return *this;
	}


public:
	i3RegVEC2D * get()	{	return this;}
	void		 set( i3RegVEC2D & Value)
	{
		*this = Value;
	}

	VEC2D *	getValue()	{	return & m_Value;}
	void		setValue( VEC2D * pVec)			{	i3Vector::Copy( &m_Value, pVec);}
	void		setValue( REAL32 u, REAL32 v)	{	i3Vector::Set( &m_Value, u, v);}

public:
	i3RegVEC2D(void);
	i3RegVEC2D( REAL32 u, REAL32 v);
	i3RegVEC2D( VEC2D * pVec);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual void GetValueByString( i3::wstring& out ) override;
};

#endif //__I3_REG_VEC2D_H__