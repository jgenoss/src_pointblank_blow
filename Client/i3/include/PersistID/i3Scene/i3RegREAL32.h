#if !defined( __I3_REG_REAL32_H__)
#define __I3_REG_REAL32_H__

#include "i3RegData.h"

class I3_EXPORT_SCENE i3RegREAL32 :	public i3RegData
{
	I3_CLASS_DEFINE( i3RegREAL32);

protected:
	RT_REAL32	m_Value;

public:
	inline	RT_REAL32	&	operator = ( const RT_REAL32 & Value)
	{
		m_Value		 = Value;

		return m_Value;
	}

	inline i3RegREAL32 & operator = ( const i3RegREAL32 & Value)
	{
		m_Value = Value.m_Value;

		return *this;
	}

	i3RegREAL32 * get()	{	return this;}
	void		 set( i3RegREAL32 & Value)
	{
		*this = Value;
	}

	REAL32		* getValue()	{	return &m_Value;}
	void		  setValue(RT_REAL32 fValue)	{	m_Value = fValue;		}

public:
	i3RegREAL32(void);
	virtual ~i3RegREAL32(void);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif //__I3_REG_REAL32_H__