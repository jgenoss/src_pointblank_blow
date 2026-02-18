#ifndef		__MemoryCore_h__
#define		__MemoryCore_h__

#if 0

//암호화, 복호화 해주는 템플릿 클래스

//암호화, 복호화가 없는 기본 클래스
template< class V >
class non_encrypt
{
public:
	V encode( V value ) const	{
		return value;
	}
	
	V decode(V value) const {
		return value;
	}
};

//암호화, 복호화 예제 클래스
template< class V >
class encrypt1
{
public:
	//암호화
	V encode( V value ) const {
		return value + 10;
	}
	//복원화
	V decode(V value) const {
		return value - 10;
	}
};

//주로 저장, 대입만 사용하면 좋겠습니다.
//사칙연산도 가능하지만 ...
template < class V, template<class> class FT>
//class F = non_encrypt<V>  >
class memcore : FT<V>
{
	typedef FT<V> base;
	V val;

public:
	//생성자
	memcore()  : val(0) {}

	memcore( const memcore& other )	{	
		set( other.get() );	
	}

	/*explicit*/ memcore( V value )	{	
		set( value );	
	}

	//소멸자
	~memcore()	{}

	//연산자
	void reset() {	
		val = 0;	
	}

	V get() const	{	
		return base::decode(val); 
	}

	void set(V value) {	
		val = base::encode(value);  
	}

	//반환 연산자
	operator V () const  { 	
		return get(); 
	}

	//대입 연산자
	memcore& operator=( const memcore & other)	{
		 if( this == &other ) return *this;
		set( other.get() );
		return *this;
	}

	memcore& operator=( V value )	{
		set( value );
		return *this;
	}

	//비교 연산자
	bool operator==(const memcore & other) const	{
		return( get() == other.get() );
	}
	bool operator==(V other) const	{
		return( get() == other );
	}
	bool operator!=(const memcore & other) const	{
		return !(*this == other);
	}
	bool operator!=(V other) const	{
		return !(get() == other);
	}
	bool operator < ( const memcore& other ) const	{
		return ( get() < other.get() );
	}
	bool operator < ( V other ) const	{
		return ( get() < other );
	}
	bool operator > ( const memcore& other ) const	{
		return !(*this < other);
	}
	bool operator > ( V other ) const	{
		return !(*this < other);
	}
	bool operator <= ( const memcore& other ) const	{
		 return ( get() <= other.get() );
	}
	bool operator <= ( V other ) const	{
		return( get() <= other );
	}
	bool operator >= ( const memcore& other ) const	{
		return !(*this <= other);
	}
	bool operator >= ( V other ) const	{
		return !(*this <= other);
	}

	/*
	//사칙 연산자
	memcore operator+( const memcore & other)	{
		return memcore( get() + other.get() );
	}

	memcore operator-( const memcore & other)	{
		return memcore( get() - other.get() );
	}

	memcore operator*( const memcore & other)	{
		return memcore( get() * other.get() );
	}

	memcore operator/( const memcore & other)	{
		V fInv = 1.0 / other.get();
		return memcore( get() * fInv );
	}

	//부호 변경
	memcore operator + () const	{
		return *this;
	}
	memcore operator - () const	{
		return memcore( -get() );
	}

	//단항 연산자
	memcore& operator += ( const memcore& other )	{
		set( get() + other.get() );
		return *this;
	}
	memcore& operator -= ( const memcore& other )	{
		set( get() - other.get() );
		return *this;
	}
	memcore& operator *= ( const memcore& other )	{
		set( get() * other.get() );
		return *this;
	}
	memcore& operator /= ( const memcore& other )	{
		V fInv = 1.0 / other.get();
		set( get() * fInv );
		return *this;
	}
	*/
};
template <class V, template<class> class FT>
bool operator==(V value, const memcore< V, FT >  & other) {
	return( other.get() == value );
}

#endif

#endif	// __CLIENT_DEF_H__