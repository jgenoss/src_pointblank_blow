#if !defined( _I3REG_STRING_H__)
#define _I3REG_STRING_H__

#include "i3RegData.h"

class I3_EXPORT_SCENE i3RegString : public	i3RegData
{
	I3_CLASS_DEFINE( i3RegString);

protected:
	char	*	m_szString;

public:

	inline	char *	operator = ( const char * Value)
	{
		setValue( Value);

		return m_szString;
	}

	inline BOOL operator ==( const i3RegString & value) const
	{
		return !i3String::Compare( value.m_szString, m_szString);
	}

	inline BOOL	operator != ( const i3RegString & value) const
	{
		return i3String::Compare( value.m_szString, m_szString);
	}

	inline i3RegString & operator = ( const i3RegString & Value)
	{
		if( this != &Value)
		{
			I3MEM_SAFE_FREE( m_szString);

			{
				INT32 nLength = i3String::Length( Value.m_szString);
				m_szString = (char*)i3MemAlloc( nLength + 1);
				i3String::NCopy( m_szString, Value.m_szString, nLength);
				m_szString[nLength] = 0;
			}
		}

		return *this;
	}

	void		Set( const char * pszString);
	char	*	get()		{	return m_szString;}

	char*		getValue()	{	return m_szString;}
	void		setValue(const char * Value)
	{
		I3MEM_SAFE_FREE( m_szString);	
		if( Value != NULL)
		{
			INT32 nLength = i3String::Length( Value);
			m_szString = (char*)i3MemAlloc( nLength + 1);
			i3String::NCopy( m_szString, Value, nLength);
			m_szString[nLength] = 0;
		}
	}

public:
	i3RegString();
	i3RegString( const char * szString);
	i3RegString( const i3RegString &OtherString);

	virtual ~i3RegString();
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif	// _I3REG_DATA_H__