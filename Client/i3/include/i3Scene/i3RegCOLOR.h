#if !defined( __I3_REG_COLOR_H__)
#define __I3_REG_COLOR_H__

#include "i3RegData.h"

class I3_EXPORT_SCENE	i3RegCOLOR :	public i3RegData
{
	I3_EXPORT_CLASS_DEFINE( i3RegCOLOR, i3RegData);

protected:
	I3COLOR	m_Value;

public:
	inline	I3COLOR	&	operator = ( const I3COLOR & Value)
	{
		i3Color::Copy( &m_Value, (I3COLOR*)&Value);
		return m_Value;
	}

	inline i3RegCOLOR & operator = ( const i3RegCOLOR & Value)
	{
		i3Color::Copy( &m_Value, (I3COLOR*)&Value.m_Value);
		return *this;
	}

	i3RegCOLOR * get()	{	return this;}
	void		 set( i3RegCOLOR & Value)
	{
		*this = Value;
	}

	I3COLOR *	getValue()	{	return &m_Value;}
	void		setValue( I3COLOR * pCol)			{	i3Color::Set( &m_Value, pCol);}
	void		setValue( UINT8 r, UINT8 g, UINT8 b, UINT8 a)	{	i3Color::Set( &m_Value, r, g, b, a);}

public:
	i3RegCOLOR(void);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual void	GetValueByString( i3::wstring& out ) override;
};

#endif //__I3_REG_COLOR_H__