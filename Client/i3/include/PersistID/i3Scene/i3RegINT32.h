#if !defined( __I3_REG_INT32_H__)
#define __I3_REG_INT32_H__

#include "i3RegData.h"

class I3_EXPORT_SCENE	i3RegINT32 :	public i3RegData
{
	I3_CLASS_DEFINE( i3RegINT32);

protected:
	INT32	m_Value;

public:
	inline i3RegINT32 & operator = ( const i3RegINT32 & Value)
	{
		m_Value = Value.m_Value;

		return *this;
	}

	inline	INT32	&	operator = ( const INT32 & Value)
	{
		m_Value		 = Value;

		return m_Value;
	}

	inline i3RegINT32 & operator = ( const REAL32 & Value)
	{
		m_Value = ( INT32)Value;
		return *this;
	}

	//
	inline i3RegINT32 operator - ( const i3RegINT32 & Value) const
	{
		i3RegINT32 nNew;
		nNew = m_Value - Value.m_Value;
		return nNew;
	}

	inline i3RegINT32 operator - ( const INT32 & Value) const
	{
		i3RegINT32 nNew;
		nNew = m_Value - Value;
		return nNew;
	}

	inline i3RegINT32 operator - ( const REAL32 & Value) const
	{
		i3RegINT32 nNew;
		nNew = (INT32)(m_Value - Value);
		return nNew;
	}

	//
	inline i3RegINT32 operator + ( const i3RegINT32 & Value) const
	{
		i3RegINT32 nNew;
		nNew = m_Value - Value.m_Value;
		return nNew;
	}

	inline i3RegINT32 operator + (const INT32 & Value) const
	{
		i3RegINT32 nNew;
		nNew = m_Value - Value;
		return nNew;
	}

	inline i3RegINT32 operator + ( const REAL32 & Value) const
	{
		i3RegINT32 nNew;
		nNew = (INT32)(m_Value - Value);
		return nNew;
	}

	//
	inline i3RegINT32 operator * ( const i3RegINT32 & Value) const
	{
		i3RegINT32 nNew;
		nNew = m_Value * Value.m_Value;
		return nNew;
	}

	inline i3RegINT32 operator * ( const INT32 & Value) const
	{
		i3RegINT32 nNew;
		nNew = m_Value * Value;
		return nNew;
	}

	inline i3RegINT32 operator * ( const REAL32 & Value) const
	{
		i3RegINT32 nNew;
		nNew = (INT32)(m_Value * Value);
		return nNew;
	}

	//
	inline i3RegINT32 operator /( const i3RegINT32 & Value) const
	{
		i3RegINT32 nNew;
		nNew = (1/Value.m_Value) * m_Value;
		return nNew;
	}

	inline i3RegINT32 operator /(const INT32 & Value) const
	{
		i3RegINT32 nNew;
		nNew = (1/Value) * m_Value;
		return nNew;
	}

	inline i3RegINT32 operator /(const REAL32 & Value) const
	{
		i3RegINT32 nNew;
		nNew = (INT32)((1/Value) * m_Value);
		return nNew;
	}

	//
	i3RegINT32 * get()	{	return this;}
	void		 set( i3RegINT32 & Value)
	{
		*this = Value;
	}

	INT32	* getValue()	{	return &m_Value;}
	void	setValue( INT32 nValue)	{	m_Value = nValue;}
public:
	i3RegINT32();
	virtual ~i3RegINT32();
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif //#if !defined( __I3_REG_INT32_H__)