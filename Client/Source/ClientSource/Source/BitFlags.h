#ifndef _BIT_FLAGS_H_
#define _BIT_FLAGS_H_

namespace bit_flags
{

#define FLAG(x) (1<<(x))
#define TEST_BIT(flag, bit) (((flag.m_value) & FLAG((bit))) !=0)
#define TEST_ALL(flag, set) (((flag.m_value) & (set)) == (set))
#define TEST_ANY(flag, set) (((flag.m_value) & (set)) !=0)
#define SET_BIT(flag, bit) ((flag.m_value) |= FLAG((bit)))
#define CLEAR_BIT(flag, bit) ((flag.m_value) &= ~FLAG((bit)))
#define SET_ALL(flag, set) ((flag.m_value) |= (set))
#define CLEAR_ALL(flag, set) ((flag.m_value) &= ~(set))

template<class T>
class BitFlags
{
public:
	T m_value;

	BitFlags();
	BitFlags(T data);
	BitFlags(const BitFlags& src);
	~BitFlags();

	BitFlags& operator=(const BitFlags& src);
	BitFlags& operator=(T src);
	operator T() const { return m_value; }
	bool operator==(const BitFlags& src) const;
	bool operator!=(const BitFlags& src) const;

	void	Set(T settings);
	void	Clear();
	void	SetFlags(T settings);
	void	ClearFlags(T settings);
	void	SetBit(INT32 bit);
	void	ClearBit(INT32 bit);
	void	SetBit(INT32 bit, bool setting);

	bool	IsEmpty() const;
	bool	TestBit(INT32 bit) const;
	bool	TestFlags(T test) const;
	bool	TestAny(T test) const;

	INT32	TotalBits() const;
	INT32	TotalSet() const;
};

typedef BitFlags<UINT8>		U8Flags;	// 8 bits of flags
typedef BitFlags<UINT16>	U16Flags;	// 16 bits of flags
typedef BitFlags<UINT32>	U32Flags;	// 32 bits of flags
typedef BitFlags<UINT64>	U64Flags;

template <class T>
inline BitFlags<T>::BitFlags() : m_value(0)
{
}

template <class T>
inline BitFlags<T>::BitFlags(T settings) : m_value(settings)
{
}

template <class T>
inline BitFlags<T>::BitFlags(const BitFlags& src) : m_value(src.m_value)
{
}

template <class T>
inline BitFlags<T>::~BitFlags()
{
}

template <class T>
inline BitFlags<T>& BitFlags<T>::operator=(const BitFlags<T>& src)
{
	m_value = src.m_value;
	return(*this);
}

template <class T>
inline BitFlags<T>& BitFlags<T>::operator=(T src)
{
	m_value = src;
	return(*this);
}

template <class T>
bool BitFlags<T>::operator==(const BitFlags& src)const
{
	return(m_value == src.m_value);
}

template <class T>
bool BitFlags<T>::operator!=(const BitFlags& src)const
{
	return(m_value != src.m_value);
}


template <class T>
inline void BitFlags<T>::Set(T settings)
{
	m_value = settings;
}

template <class T>
inline void BitFlags<T>::Clear()
{
	m_value = 0;
}

template <class T>
inline void BitFlags<T>::SetFlags(T settings)
{
	m_value |= settings;
}

template <class T>
inline void BitFlags<T>::ClearFlags(T settings)
{
	m_value &= ~settings;
}


template <class T>
inline void BitFlags<T>::SetBit(INT32 bit)
{
	m_value |= (1<<bit);
}

template <class T>
inline void BitFlags<T>::SetBit(INT32 bit, bool setting)
{
	if (setting)
	{
		m_value |= (1<<bit);
	}
	else
	{
		ClearBit(bit);
	}
}

template <class T>
inline void BitFlags<T>::ClearBit(INT32 bit)
{
	m_value &= (~(1<<bit));
}

template <class T>
inline bool BitFlags<T>::IsEmpty()const
{
	return (m_value == static_cast<T>(0));
}

template <class T>
inline bool BitFlags<T>::TestBit(INT32 bit) const
{
	return (m_value & (1<<bit)) != 0;
}


template <class T>
inline bool BitFlags<T>::TestFlags(T test)const
{
	return ((m_value & test) == test);
}

template <class T>
inline bool BitFlags<T>::TestAny(T test)const
{
	return m_value & test;
}

template <class T>
inline INT32 BitFlags<T>::TotalBits()const
{
	return sizeof(T)<<3;
}

template <class T>
inline INT32 BitFlags<T>::TotalSet()const
{
	INT32 count = 0;
	INT32 total = TotalBits();
	T testvalue = m_value;
	
	for (INT32 i=total; i; --i)
	{
		count += (testvalue & 1);
		testvalue >>= 1;
	}
	return count;
}

}

#pragma warning(default : 4800)

#endif