#if !defined( __I3_REG_MATRIX_H__)
#define __I3_REG_MATRIX_H__

#include "i3RegData.h"

class I3_EXPORT_SCENE i3RegMATRIX :	public i3RegData
{
	I3_EXPORT_CLASS_DEFINE( i3RegMATRIX, i3RegData);

protected:
	MATRIX	m_Value;

public:
	inline	MATRIX	&	operator = ( const MATRIX & Value)
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

	MATRIX *	getValue()	{	return &m_Value;}

public:
	i3RegMATRIX(void);
	i3RegMATRIX( MATRIX * pMtx);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual void GetValueByString( i3::wstring& out ) override;
};

#endif //__I3_REG_MATRIX_H__