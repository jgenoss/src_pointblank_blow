#ifndef __S2MO_DATA_TYPE_H__
#define __S2MO_DATA_TYPE_H__

class S2MO_EXPORT_BASE S2MOPackable 
{
	S2MOPackable*			m_pNext;

protected:

	BOOL					_PackingSize( char* pBuffer, int i32Count, int i32MaxSize, int& i32Size );
	BOOL					_UnPackingSize( char* pBuffer, int& i32Count, int i32MaxSize, int& i32Size );
	
public:
	S2MOPackable()																{	m_pNext = NULL;					}
	virtual ~S2MOPackable()														{}

	virtual void			Copy( const S2MOPackable* pOrg )					{}

	S2MOPackable*			GetNext()											{	return m_pNext;					}
	void					SetNext( S2MOPackable* pPackable )					{	m_pNext = pPackable;			}

	virtual BOOL			Packing( char* pBuffer, int& i32Size )				{	return TRUE;					}
	virtual BOOL			UnPacking( char* pBuffer, int& i32Size )			{	return TRUE;					}
};

template <typename T, int N=1>
class S2MOValue : public S2MOPackable 
{
protected:
	int						m_i32Count;
	T						m_TData[ N ];

public:
	S2MOValue()				{	m_i32Count = 0; }
	virtual ~S2MOValue()	{}
	
	virtual void			Copy( const S2MOPackable* pOrg )
	{
		const S2MOValue* pValue = (S2MOValue*)pOrg;
		m_i32Count = pValue->m_i32Count;
		memcpy( m_TData, pValue->m_TData, sizeof(T)*m_i32Count );
	}

	virtual BOOL			Packing( char* pBuffer, int& i32Size )
	{
		if( N > 1 )
		{
			if( FALSE == _PackingSize( pBuffer, m_i32Count, N, i32Size ) )		return FALSE;
			if( 0 < m_i32Count )
			{
				int i32FullSize = sizeof( T )*m_i32Count;
				memcpy( &pBuffer[ i32Size ], m_TData, i32FullSize );
				i32Size += i32FullSize;
			}
		}
		else
		{
			T* pValue	= (T*)pBuffer;
			*pValue		= m_TData[0];
			i32Size		= sizeof(T);
		}
		return TRUE;
	}
	virtual BOOL			UnPacking( char* pBuffer, int& i32Size )
	{
		if( N > 1 )
		{
			if( FALSE == _UnPackingSize( pBuffer, m_i32Count, N, i32Size ) )	return FALSE;
			if( N < m_i32Count )												return FALSE;
			if( 0 < m_i32Count )
			{
				int i32FullSize = sizeof( T )*m_i32Count;
				memcpy( m_TData, &pBuffer[ i32Size ], i32FullSize );
				i32Size += i32FullSize;
			}
		}
		else
		{
			T* pValue	= (T*)pBuffer;
			m_TData[0]	= *pValue;
			i32Size		= sizeof(T);
			m_i32Count	= 1;
		}
		return TRUE;
	}

	virtual void			GetValue( T* pTData, const int i32CountofBuffer, int* pi32Count = NULL )
	{
		int i32CopyCount = m_i32Count;
		if( i32CountofBuffer < m_i32Count )
		{
#ifdef _DEBUG
			_ASSERT( FALSE );
#else
			i32CopyCount = i32CountofBuffer;
#endif
		}
		if( pi32Count )
		{
			*pi32Count = i32CopyCount;
		}
		if( 0 < i32CopyCount )
		{
			memcpy( pTData, m_TData, sizeof( T )*i32CopyCount );
		}
	}

	void					SetValue( const T* pTData, const int i32StartIdx, const int i32CountofBuffer )
	{
		int i32CopyCount = i32CountofBuffer;
		if( 0 >= i32CountofBuffer )											return;
		int i32Idx = i32StartIdx + i32CountofBuffer;
		if( N < i32Idx )
		{
#ifdef _DEBUG
			_ASSERT( FALSE );
#else
			i32CopyCount = i32CountofBuffer - (i32Idx-N);
#endif
		}

		memcpy( &m_TData[ i32StartIdx ], pTData, sizeof( T )*i32CopyCount );
		if( m_i32Count < i32Idx ) m_i32Count = i32Idx;
	}

	INT32&					GetCount()
	{
		return m_i32Count;
	}

	void					SetCount( INT32 i32Count )
	{
		m_i32Count = i32Count;
	}

	operator T() const
	{
		return *m_TData;
	}
	T*						operator&()
	{
		return m_TData;
	}

	virtual S2MOValue&		operator=( const S2MOValue& Org )
	{
		Copy( &Org );
		return *this;
	}
	virtual void			operator=( const T& TData )
	{
		if( m_i32Count < 1 )			m_i32Count = 1;
		*m_TData = TData;
	}
	virtual void			operator=( const T* TData )
	{
		memcpy( m_TData, TData, sizeof( T )*N );

		m_i32Count = N;
	}
	T&						operator[]( const int i32Idx )
	{
		if( N <= i32Idx )					_ASSERT( FALSE );
		if( m_i32Count < ( i32Idx + 1 ) )	m_i32Count = i32Idx + 1;
		return m_TData[ i32Idx ];
	}
};

template <typename T, int N>
class S2MOString : public S2MOValue<T,N>
{

public:
	S2MOString()			{}
	virtual ~S2MOString()	{}

	virtual void			Copy( const S2MOPackable* pOrg )
	{
		S2MOValue::Copy( pOrg );
		m_TData[ m_i32Count ] = '\0';
	}
	
	virtual BOOL			UnPacking( char* pBuffer, int& i32Size )
	{
		if( FALSE == S2MOValue::UnPacking( pBuffer, i32Size ) )						return FALSE;
		m_TData[ m_i32Count ] = '\0';

		return TRUE;
	}

	virtual void			GetValue( T* strData, const int i32CountofBuffer, int* pi32Count = NULL )
	{
		int i32CopyCount = m_i32Count;
		if( i32CountofBuffer < (m_i32Count+1) )
		{
#ifdef _DEBUG
			_ASSERT( FALSE );
#else
			i32CopyCount = i32CountofBuffer-1;
#endif
		}
		if( pi32Count )
		{
			*pi32Count = i32CopyCount;
		}
		if( 0 < i32CopyCount )
		{
			memcpy( strData, m_TData, sizeof(T)*i32CopyCount );
		}
		strData[ i32CopyCount ] = '\0';
	}

	void					SetValue( const T* pTData )
	{
		int i32Count = N-2;
		m_i32Count = 0;
		while( '\0' != pTData[ m_i32Count ] )
		{
			m_TData[ m_i32Count ] = pTData[ m_i32Count ];
			m_i32Count++;
			if( i32Count < m_i32Count ) break;
		}
		m_TData[ m_i32Count ] = '\0';
	}

	virtual void			operator=( const T* strData )
	{
		SetValue( strData );
	}
};

template <int N>
class S2MOStringA : public S2MOString<char,N>
{

public:
	S2MOStringA()			{}
	virtual ~S2MOStringA()	{}

	virtual S2MOStringA&	operator=( const S2MOStringA& Org )
	{
		S2MOString::Copy( &Org );
		return *this;
	}
	virtual void			operator=( const char* strData )
	{
		S2MOString::operator=( strData );
	}
};

template <int N>
class S2MOStringW : public S2MOString<wchar_t,N>
{

public:
	S2MOStringW()			{}
	virtual ~S2MOStringW()	{}

	virtual S2MOStringW&	operator=( const S2MOStringW& Org )
	{
		S2MOString::Copy( &Org );
		return *this;
	}
	virtual void			operator=( const wchar_t* strData )
	{
		S2MOString::operator=( strData );
	}
};

typedef S2MOValue<INT8>			S2INT8;
typedef S2MOValue<UINT8>		S2UINT8;
typedef S2MOValue<INT16>		S2INT16;
typedef S2MOValue<UINT16>		S2UINT16;
typedef S2MOValue<INT32>		S2INT32;
typedef S2MOValue<UINT32>		S2UINT32;
typedef S2MOValue<INT64>		S2INT64;
typedef S2MOValue<UINT64>		S2UINT64;

#endif