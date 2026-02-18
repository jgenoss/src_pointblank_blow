#if !defined( __I3_REG_MATRIX_H__)
#define __I3_REG_MATRIX_H__

#include "i3RegData.h"

class I3_EXPORT_SCENE i3RegMATRIX :	public i3RegData
{
	I3_CLASS_DEFINE( i3RegMATRIX);

protected:
	RT_MATRIX	m_Value;

public:
	inline	RT_MATRIX	&	operator = ( const RT_MATRIX & Value)
	{
		i3Matrix::Copy( &m_Value, &Value);

		return m_Value;
	}

	inline i3RegMATRIX & operator = ( const i3RegMATRIX & Value)
	{
		i3Matrix::Copy( &m_Value, &Value.m_Value);

		return *this;
	}

	i3RegMATRIX * get()	{	return this;}
	void		 set( i3RegMATRIX & Value)
	{
		*this = Value;
	}

	RT_MATRIX *	getValue()	{	return &m_Value;}

public:
	i3RegMATRIX(void);
	i3RegMATRIX( RT_MATRIX * pMtx);

	virtual ~i3RegMATRIX(void);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif //__I3_REG_MATRIX_H__