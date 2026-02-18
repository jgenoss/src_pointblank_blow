#include "i3CommonType.h"
#include "i3RSABF.h"


#include "string/stack_string.h"
#include <strsafe.h>
#include <Mmsystem.h>
#include "../../Crypto/Crypto.h"
//
//  파일에 저장되는 마크문자열이 변경되지 않도록 주의..
//


namespace
{
	//
	struct mark_literal_object
	{
		mark_literal_object(const char* lit) : str(lit), len(strlen(lit)) {}
		const char* const	str;
		const size_t		len;
	private:
		void operator=(const mark_literal_object&);
	};

	bool check_mark(const mark_literal_object& mark_obj, const char* test_str)
	{
		return i3::char_traits<char>::compare(test_str, mark_obj.str, mark_obj.len) == 0;
	}

	mark_literal_object	mark_db_literal("RSA_Public_Private_Key_DB_File");
	mark_literal_object	mark_public_literal("RSA_Public_Key_File");
	mark_literal_object	mark_rsa_bf_literal("RSA_BF_Encrypt_File");

	const INT16 current_db_version_major = 1;
	const INT16 current_db_version_minor = 0;
	
	const INT16 current_public_version_major = 1;
	const INT16 current_public_version_minor = 0;

	const INT16 current_rsa_bf_version_major = 1;
	const INT16 current_rsa_bf_version_minor = 0;


	struct rsa_db_header
	{
		char  szMark[32] = { 0 };
		INT16 version_major = 0;
		INT16 version_minor = 0;

		struct init_tag{};
		rsa_db_header() {}			// 미초기화상태이다..
		rsa_db_header(init_tag);	// 
	};



	rsa_db_header::rsa_db_header(init_tag) : version_major(current_db_version_major), version_minor(current_db_version_minor)
	{
		::memset( szMark, 0, sizeof(szMark) );
		::strncpy(szMark, mark_db_literal.str, mark_db_literal.len);
	}
	
	struct rsa_db
	{
		typedef i3::unordered_map<i3::rc_string, i3::rc_string>::iterator	map_iterator;
		i3::unordered_map<i3::rc_string, i3::rc_string>		m_pub_priv_key_map;		
	};
	// 여기서 rc_string은 널문자에 대한 고려를 하지 않은 버퍼처럼 활용된다..

	bool load_public_private_key_db(const i3::rc_string& public_private_db_path, rsa_db& out)
	{
		HANDLE hfile = ::CreateFile(public_private_db_path.c_str(), GENERIC_READ, 0, 
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hfile == INVALID_HANDLE_VALUE)		// 여기서 없으면 파일 없는 것임..
			return false;
		
		DWORD file_size = ::GetFileSize(hfile, NULL);
		if (file_size < sizeof(rsa_db_header) )		// 헤더사이즈 오류.
		{
			CloseHandle(hfile);		return false;
		}

		DWORD result_len = 0;
		rsa_db_header dbh;
		::ReadFile( hfile, &dbh, sizeof(dbh), &result_len, NULL);
		if ( !check_mark( mark_db_literal, dbh.szMark )  )	// 마크 오류.
		{
			CloseHandle(hfile);		return false;	
		}
		
		//
		// 헤더에는 버전체크도 있는데, 우선 비워둔다..
		// 이후로는 데이터 유무와 관계없이 잘 읽어들인것으로 간주한다..
		//
		DWORD  key_size;
		i3::vector<unsigned char> public_key;
		i3::vector<unsigned char> private_key;

		for( ; ; )
		{
			::ReadFile( hfile, &key_size, sizeof(DWORD), &result_len, NULL);
			if ( result_len < sizeof(DWORD) )
				break;		// 이제 그만..

			public_key.resize(key_size);
			::ReadFile( hfile, &public_key[0], key_size, &result_len, NULL);
			if ( result_len < key_size )
				break;		// 이제 그만..
			
			::ReadFile( hfile, &key_size, sizeof(DWORD), &result_len, NULL);
			if ( result_len < sizeof(DWORD) )
				break;		// 이제 그만..

			private_key.resize(key_size);
			::ReadFile( hfile, &private_key[0], key_size, &result_len, NULL);
			if ( result_len < key_size )
				break;		// 이제 그만..
			
			i3::rc_string pub_key_string( (const char*)&public_key[0], public_key.size() );
			i3::rc_string priv_key_string( (const char*)&private_key[0], private_key.size() );
			
			i3::pair< rsa_db::map_iterator, bool> rp = 
				out.m_pub_priv_key_map.insert( i3::make_pair( pub_key_string, priv_key_string) );

			if ( !rp.second )		// public키가 같은데 priv가 다른 그런 건 오류이고, 다 다르다고 가정..
			{
				// 우선 비워둔다.. 
			}

		}

		CloseHandle(hfile);
	
		return true;
	}

	i3::rc_string  create_backup_data_file_string( const i3::rc_string& public_private_db_path )
	{
		size_t pos = public_private_db_path.find_last_of('.');	// pos == -1인 경우를 인터페이스가 견디는지 볼것..
			
		if (pos == i3::rc_string::npos)
			pos = public_private_db_path.size();
		
		i3::stack_string tempHead(public_private_db_path.c_str(), pos);
		i3::stack_string tempTail(public_private_db_path.c_str() + pos, public_private_db_path.size() - pos);	

		SYSTEMTIME		SystemTime;
		//Cal Local Time
		::GetLocalTime( &SystemTime );	
		INT32 Year		=	SystemTime.wYear;
		INT32 Month		=	SystemTime.wMonth; 
		INT32 Day		=	SystemTime.wDay; 	
		INT32 Hour		=	SystemTime.wHour; 
		INT32 Minute	=	SystemTime.wMinute; 		
		INT32 Second	=	SystemTime.wSecond;
		INT32 iDate	= (Year * 10000) + (Month) * 100 + Day;
		
		char tempDate[MAX_PATH];
		::StringCchPrintfA(tempDate, _countof(tempDate), "_%d_%02d_%02d_%02d_", iDate, Hour, Minute, Second);

		i3::stack_string result = tempHead;
		result += tempDate;
		result += tempTail;

		return i3::rc_string(result);
	}



	struct rsa_public_header
	{
		char  szMark[32] = { 0 };
		INT16 version_major = 0;
		INT16 version_minor = 0;

		struct init_tag{};
		rsa_public_header() : version_major(0), version_minor(0) { ::memset( szMark, 0, sizeof(szMark)); }			// 미초기화상태이다..
		rsa_public_header(init_tag);	// 
	};
	
	rsa_public_header::rsa_public_header(init_tag) : version_major(current_public_version_major), 
		version_minor(current_public_version_minor)
	{
		::memset( szMark, 0, sizeof(szMark) );
		::strncpy(szMark, mark_public_literal.str, mark_public_literal.len);
	}
	
	struct rsa_bf_header
	{
		char	szMark[32] = { 0 };
		INT16	version_major = 0;
		INT16	version_minor = 0;

		struct init_tag{};
		rsa_bf_header() : version_major(0), version_minor(0) { ::memset( szMark, 0, sizeof(szMark) ); }			// 미초기화상태이다..
		rsa_bf_header(init_tag);	// 
	};

	rsa_bf_header::rsa_bf_header(init_tag) : version_major(current_rsa_bf_version_major), 
		version_minor(current_rsa_bf_version_minor)
	{
		::memset( szMark, 0, sizeof(szMark) );
		::strncpy(szMark, mark_rsa_bf_literal.str, mark_rsa_bf_literal.len);
	}

	HANDLE try_force_create_file(const i3::rc_string& strFilePath)
	{
		HANDLE hfile = ::CreateFile( strFilePath.c_str(), GENERIC_WRITE, 0, 
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		// 실패시 디렉토리 존재여부를 본다..

		if (hfile == INVALID_HANDLE_VALUE)
		{
			size_t last_tok_pos = strFilePath.find_last_of("\\/");

			if (last_tok_pos == i3::rc_string::npos) 
			{
				return INVALID_HANDLE_VALUE;			// 이경우는 실패...
			}

			i3::string test_dir(strFilePath.c_str(), last_tok_pos+1);
			DWORD file_attr = ::GetFileAttributesA( test_dir.c_str());

			if ( file_attr != INVALID_FILE_ATTRIBUTES  )
			{
				return INVALID_HANDLE_VALUE;			// 이미 존재하는 파일이거나 디렉토리면 실패..
			}

			if (::CreateDirectory( test_dir.c_str(), NULL) == FALSE)
			{
				return INVALID_HANDLE_VALUE;
			}
			
			return ::CreateFile( strFilePath.c_str(), GENERIC_WRITE, 0, 
				NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		}

		return hfile;
	}


}


namespace i3
{
	
	bool save_public_private_key_to_db(const i3::rc_string& public_private_db_path, size_t key_length)
	{
		// 일단은 모두 읽고 난뒤에, 내용 추가하고 다시 전체 재저장해도 상관없을 것이다.
		rsa_db	dbd;

		load_public_private_key_db(public_private_db_path, dbd);		// 일단 읽을수 있으면 읽고..
		
		//
		// 키목록은 백업이 중요하기 때문에, 무조건 원본 보존을 처리한다..
		if (!dbd.m_pub_priv_key_map.empty())
		{
			i3::rc_string backup_path = create_backup_data_file_string(public_private_db_path);
			bool bBackupOK = ::CopyFile(  public_private_db_path.c_str(), backup_path.c_str(), FALSE) != FALSE;
		
			if (!bBackupOK)
			{
				// 백업파일 카피가 안된것임. 아직 미처리..
				DWORD err_code = ::GetLastError();
				I3PRINTLOG(I3LOG_WARN,  "Error = %d", err_code);
			}
		}
		//
		
		// RSA키 생성 처리..
		
		{
			Crypto::RSA* rsa = Crypto::GenerateRSAKey(key_length);	// 대충 128바이트로 정할것..
		
			INT32  pub_buff_len = Crypto::SizeOfConvertRSAPublicKeyToBuffer(rsa);

			i3::vector<unsigned char>	vec_pub_buff(pub_buff_len); 
			Crypto::ConvertRSAPublicKeyToBuffer(rsa, &vec_pub_buff[0]);
		
			i3::rc_string str_public_key( (const char*)&vec_pub_buff[0], vec_pub_buff.size() );
				
			INT32 priv_buff_len = Crypto::SizeOfConvertRSAPrivateKeyToBuffer(rsa);

			i3::vector<unsigned char> vec_pri_buff(priv_buff_len); 
			Crypto::ConvertRSAPrivateKeyToBuffer(rsa, &vec_pri_buff[0]);
			i3::rc_string str_private_key( (const char*)&vec_pri_buff[0], vec_pri_buff.size() );

			dbd.m_pub_priv_key_map.insert( i3::make_pair( str_public_key, str_private_key) );

			Crypto::DeleteRSA(rsa);
		}
			
	
		// 무조건 저장..
		HANDLE	hfile =	try_force_create_file( public_private_db_path );

		if (hfile == INVALID_HANDLE_VALUE)
		{
			CloseHandle(hfile);	return false;
		}

		rsa_db_header dbh = rsa_db_header(rsa_db_header::init_tag());
		DWORD num_written = 0;
		::WriteFile(hfile, &dbh, sizeof(dbh), &num_written, NULL);
		//

		// 사이즈 2개 및 각 버퍼로 아무렇게나 저장하면 된다.
		rsa_db::map_iterator it = dbd.m_pub_priv_key_map.begin();
		rsa_db::map_iterator end = dbd.m_pub_priv_key_map.end();
		
		DWORD size_data;
	
		for ( ; it != end ; ++it)
		{
			i3::rc_string pub_string = it->first;
			i3::rc_string priv_string = it->second;
	
			size_data = DWORD(pub_string.size());
			::WriteFile(hfile, &size_data, sizeof(DWORD), &num_written, NULL);
			::WriteFile(hfile, &pub_string[0], size_data, &num_written, NULL);
			size_data = DWORD(priv_string.size());
			::WriteFile(hfile, &size_data, sizeof(DWORD), &num_written, NULL);
			::WriteFile(hfile, &priv_string[0], size_data, &num_written, NULL);
			
		}
		 
		::CloseHandle(hfile);
		
		return true;
	}
	
	// 공개키 저장.. (우선은 게이머에게 작은 파일 그대로 노출 시킬 것이다..)

	bool save_public_key_from_db(const i3::rc_string& public_private_db_path, const i3::rc_string& dest_key_path)
	{
		// db파일이 존재하지 않거나, 유효하지 않으면 실패로 간주한다..
		rsa_db		dbd;
		bool bLoaded = load_public_private_key_db(public_private_db_path, dbd);		// 일단 읽을수 있으면 읽고..
		if (!bLoaded) 
			return false;
		
		//
		// 목록 중에서 아무거나 꺼낸다...(랜덤하면 좋을지도..)
		// 그리고, 파일 작성 갱신이 나쁜 영향을 미치지 않기 때문에 그냥 덮어씌운다..
		//
		::srand( ::timeGetTime()  );

		size_t num_key = dbd.m_pub_priv_key_map.size();
		size_t  result_idx	= size_t(::rand() % int(num_key));

		//
		rsa_db::map_iterator it = dbd.m_pub_priv_key_map.begin();
		i3::advance(it, result_idx);
		//
		
		i3::rc_string pub_key =	it->first;
		i3::rc_string priv_key = it->second;
		DWORD pub_size_key = DWORD(pub_key.size());
	
	
		HANDLE hfile = try_force_create_file( dest_key_path );
		if (hfile == INVALID_HANDLE_VALUE )
		{
			::CloseHandle(hfile);	return false;
		}
		
		rsa_public_header pbh = rsa_public_header(rsa_public_header::init_tag());
		DWORD num_written = 0;
		::WriteFile(hfile, &pbh, sizeof(pbh), &num_written, NULL);
		
		// 공개키만 넘기면 된다..
	
		::WriteFile(hfile, &pub_size_key, sizeof(DWORD), &num_written, NULL);
		::WriteFile(hfile, &pub_key[0], pub_size_key, &num_written, NULL);
		
		::CloseHandle(hfile);

		return true;
	}

	//
	// 게이머 입장에서는 db가 뭔지 관심 없다... public_key_path로부터 공개키를 만들고, 즉시 블로우피시키를 생성한 후,
	// 파일에는 암호화된 블로우피시키를 삽입한다..
	//

	rsa_bf_log_enc*		create_rsa_bf_enc(const i3::rc_string& public_key_path, const i3::rc_string& dest_file_path)	
	{
		HANDLE hfile = ::CreateFile(public_key_path.c_str(), GENERIC_READ, 0, 
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hfile == INVALID_HANDLE_VALUE)		// 여기서 없으면 파일 없는 것임..
			return nullptr;

		DWORD file_size = ::GetFileSize(hfile, NULL);
		if (file_size < sizeof(rsa_public_header) )		// 헤더사이즈 오류.
		{
			CloseHandle(hfile);		return nullptr;
		}

		DWORD result_len = 0;
		rsa_public_header pbh;
		::ReadFile( hfile, &pbh, sizeof(pbh), &result_len, NULL);
		if ( !check_mark(mark_public_literal, pbh.szMark)  )	// 마크 오류.
		{
			CloseHandle(hfile);		return nullptr;
		}

		DWORD buff_size;
		DWORD num_read;
		::ReadFile(hfile, &buff_size, sizeof(DWORD), &num_read, NULL);
		
		if ( num_read < sizeof(DWORD) )
		{
			CloseHandle(hfile); return nullptr;
		}
		
		i3::vector<unsigned char> vec_public_key( buff_size);
		
		::ReadFile(hfile, &vec_public_key[0], buff_size, &num_read, NULL);

		if ( num_read < buff_size)
		{
			CloseHandle(hfile); return nullptr;
		}
		
		CloseHandle(hfile);

		Crypto::RSA* rsa = Crypto::CreateEmptyRSA();
		Crypto::ConvertBufferToRSAPublicKey(rsa, &vec_public_key[0], vec_public_key.size());
		
	
		//
		// public_string_key는 뽑았다... 객체부터 먼저 생성하고, 
		// 생성된 객체의 블로우키값을 로그 파일로 기록해야한다..
		
		//
		// 블로우 키값은 여기서 생성한다.. (128바이트정도로 생각하자)
		// 
		
		size_t key_length = Crypto::GetRSAInputBuffSize(rsa);

		::srand( ::timeGetTime()  );
		
		i3::vector<unsigned char>	vec_bf_key( key_length);
		unsigned char				bf_ivec[8];			// 8바이트로 키와 비슷하게 운용..

		for ( size_t i = 0; i < key_length ; ++i)
		{
			vec_bf_key[i] = ::rand() % 256;
		}
		for ( size_t i = 0 ; i < 8 ; ++i)
		{
			bf_ivec[i] = ::rand() % 256;
		}


		Crypto::BF_KEY* bfk =	Crypto::CreateEmptyBlowFishKey();
		Crypto::GetBlowFishKey(bfk, &vec_bf_key[0], vec_bf_key.size() );

		i3::rsa_bf_log_enc* enc = new i3::rsa_bf_log_enc(bfk, bf_ivec, dest_file_path);

		NetworkDump_Malloc( enc, sizeof( i3::rsa_bf_log_enc), __FILE__, __LINE__);
				
		// enc생성자에 두지 않는 로그 헤더파일을 여기서 처리한다..
		// 로그파일의 이름은 이미 정해져있는 것으로 간주하므로, 파일이름을 별도 변경하지 않음..
		
		hfile = ::CreateFile( dest_file_path.c_str(), GENERIC_WRITE, 0, 
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		rsa_bf_header rbh = rsa_bf_header(rsa_bf_header::init_tag());
		DWORD num_written = 0;
		::WriteFile(hfile, &rbh, sizeof(rbh), &num_written, NULL);

		//
	
		//
		i3::vector<unsigned char> vec_enc_bf( Crypto::GetRSAOutputBuffSize(rsa) );
		size_t eff_len;
		Crypto::RSAPublicEncrypt(rsa, &vec_bf_key[0], vec_bf_key.size(), &vec_enc_bf[0], eff_len);
		vec_enc_bf.resize(eff_len);
		//
		
		//
		i3::vector<unsigned char> vec_enc_ivec( Crypto::GetRSAOutputBuffSize(rsa));
		Crypto::RSAPublicEncrypt(rsa, &bf_ivec[0], sizeof(unsigned char) * 8, &vec_enc_ivec[0], eff_len);
		vec_enc_ivec.resize(eff_len);
		//


		// 공개키부터 저장하고, 블로우암호키를 저장..
		DWORD pub_key_size = DWORD(vec_public_key.size());
		::WriteFile(hfile, &pub_key_size, sizeof(DWORD), &num_written, NULL);
		::WriteFile(hfile, &vec_public_key[0], pub_key_size, &num_written, NULL);
		
		DWORD enc_bf_size = DWORD(vec_enc_bf.size());
		::WriteFile(hfile, &enc_bf_size, sizeof(DWORD), &num_written, NULL);
		::WriteFile(hfile, &vec_enc_bf[0], enc_bf_size, &num_written, NULL);
		
		DWORD enc_ivec_size = DWORD(vec_enc_ivec.size());
		::WriteFile(hfile, &enc_ivec_size, sizeof(DWORD), &num_written, NULL);
		::WriteFile(hfile, &vec_enc_ivec[0], enc_ivec_size, &num_written, NULL);
		//

		// 이후부터는 순수로그내용을 암호화해서 기록하게 될것으로 기대됨..
		CloseHandle(hfile);
		Crypto::DeleteRSA(rsa);
		//
		return enc;
	}

	void				delete_rsa_bf_enc(rsa_bf_log_enc* p)
	{
		NetworkDump_Free( p, __FILE__, __LINE__);
		delete p;
	}


	rsa_bf_log_enc::rsa_bf_log_enc(Crypto::BF_KEY* k, const unsigned char(&ivec)[8], const i3::rc_string& log_path) : 
		m_blowfish_key(k), m_log_file_path(log_path), m_ivec_offset(0)
	{
		::memcpy(m_ivec, ivec, sizeof(unsigned char) * 8);
	}

	rsa_bf_log_enc::~rsa_bf_log_enc()
	{
		Crypto::DeleteBlowFishKey(m_blowfish_key);
	}

	void rsa_bf_log_enc::log(const char* str)			// 점진적으로 하나씩 처리..
	{
		// 끝에서부터 하나씩 저장해두어야한다..(저장할때, 버퍼 사이즈단위 잊지 말아야한다.)
		
		// 먼저 변환부터 처리해야한다..
		long size_input_str = ::strlen(str);
		i3::vector<unsigned char> enc(size_input_str);
		Crypto::BlowFishEncryt(m_blowfish_key, m_ivec, &m_ivec_offset, (const unsigned char*)str, &enc[0], size_input_str);
		//

		// 이제 저장..
		
		HANDLE hFile = ::CreateFile( m_log_file_path.c_str(), GENERIC_WRITE, 
			FILE_SHARE_DELETE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if( hFile == INVALID_HANDLE_VALUE)
		{
			I3TRACE( "_log : Could not open %s file.\n", m_log_file_path );
			return;
		}

		::SetFilePointer( hFile, 0, NULL, FILE_END);
		
		// 사이즈부터 저장..
		DWORD enc_size = DWORD(enc.size());
		DWORD num_written = 0;
		::WriteFile(hFile, &enc_size, sizeof(DWORD), &num_written, NULL);
		::WriteFile(hFile, &enc[0], enc_size, &num_written, NULL);

		::CloseHandle( hFile);
		//

	}

	// 해독하는 함수.. (사용자쪽이 아닌 회사쪽입장)

	bool decrypt_log_enc(const i3::rc_string& public_private_db_path, const i3::rc_string& src_file_path,
		const i3::rc_string& dest_file_path)
	{
		// 1. db 파일 읽고
		// 2. 암호화로그파일을 읽는데, 공개키를 검색어로 비공개키를 찾아냄..
		// 3. 비공개키로 암호화된 블로우피시키와 벡터값을 해독..
		// 4. 해독된 피시키와 벡터값으로 나머지를 해독한다..
		// 5. 해독된 버퍼를 통으로 파일에 저장한다..
		//
		rsa_db		dbd;

		bool bLoaded = load_public_private_key_db(public_private_db_path, dbd);		// 일단 읽을수 있으면 읽고..
		if (!bLoaded) 
			return false;

		HANDLE hsrcfile = ::CreateFile(src_file_path.c_str(), GENERIC_READ, 0, 
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hsrcfile == INVALID_HANDLE_VALUE)		// 여기서 없으면 파일 없는 것임..
		{
			CloseHandle(hsrcfile);	return false;
		}

		DWORD srcfile_size = ::GetFileSize(hsrcfile, NULL);
		if (srcfile_size < sizeof(rsa_bf_header) )		// 헤더사이즈 오류.
		{
			CloseHandle(hsrcfile);	return false;
		}
		
		DWORD result_len = 0;
		rsa_bf_header rbh;
		::ReadFile(hsrcfile, &rbh, sizeof(rbh), &result_len, NULL);
		if (!check_mark(mark_rsa_bf_literal, rbh.szMark))		// 마크 체크..
		{
			CloseHandle(hsrcfile); return false;
		}
		
	
		DWORD sizeof_data;
		i3::rc_string str_prviate_key;
		{
			::ReadFile(hsrcfile, &sizeof_data, sizeof(DWORD), &result_len, NULL);
			i3::vector<unsigned char>	vec_pub_key(sizeof_data);
			::ReadFile(hsrcfile, &vec_pub_key[0], sizeof_data, &result_len, NULL);
			
			// 공개키 문자열을 갖고 해시맵을 통해 비공개키 문자열을 조회한다..
			i3::rc_string str_pub_key( (const char*)&vec_pub_key[0], vec_pub_key.size());
			
			rsa_db::map_iterator it = dbd.m_pub_priv_key_map.find(str_pub_key);
			
			if (it == dbd.m_pub_priv_key_map.end())		// 못찾으면 해독불가..
			{
				CloseHandle(hsrcfile); return false;
			}

			str_prviate_key = it->second;
		}

		//
		dbd.m_pub_priv_key_map.clear();			// 이제 DB가 필요없으니 제거...
		//

		// 비 공개 키 생성..
		Crypto::RSA* rsa = Crypto::CreateEmptyRSA();
		Crypto::ConvertBufferToRSAPrivateKey(rsa, (const unsigned char*)str_prviate_key.c_str(), str_prviate_key.size()	);

		i3::vector<unsigned char>	vec_dec_fishkey(Crypto::GetRSAOutputBuffSize(rsa));	//해독되는 피시키..

		{
			::ReadFile(hsrcfile, &sizeof_data, sizeof(DWORD), &result_len, NULL);
			i3::vector<unsigned char>	vec_enc_fishkey(sizeof_data);
			::ReadFile(hsrcfile, &vec_enc_fishkey[0], sizeof_data, &result_len, NULL);
		
			size_t eff_len;
			Crypto::RSAPrivateDecrypt(rsa, &vec_enc_fishkey[0], vec_enc_fishkey.size(), &vec_dec_fishkey[0], eff_len);
			vec_dec_fishkey.resize(eff_len);
		}
		
		i3::vector<unsigned char>	vec_dec_ivec( Crypto::GetRSAOutputBuffSize(rsa));

		{
			::ReadFile(hsrcfile, &sizeof_data, sizeof(DWORD), &result_len, NULL);
			I3ASSERT( sizeof_data < 65535);
			i3::vector<unsigned char>	vec_enc_ivec(sizeof_data);
			::ReadFile(hsrcfile, &vec_enc_ivec[0], sizeof_data, &result_len, NULL);

			size_t eff_len;
			Crypto::RSAPrivateDecrypt(rsa, &vec_enc_ivec[0], vec_enc_ivec.size(), &vec_dec_ivec[0], eff_len);
			vec_dec_ivec.resize(eff_len);
		}
		
		Crypto::DeleteRSA(rsa);		// rsa는 이제 필요없으니 버림..
		//

		Crypto::BF_KEY* bfk =	Crypto::CreateEmptyBlowFishKey();
		Crypto::GetBlowFishKey(bfk, &vec_dec_fishkey[0], vec_dec_fishkey.size() );
		
		// 해독과정.. 얼마나 걸려서 끝날지 모르므로, while로 굴린다..
		// 파일을 따로 열기 전에 모두 버퍼에 보관한다...
		
		i3::vector<unsigned char>	m_result_buffer;
		i3::vector<unsigned char>	m_result_line;		// 라인단위 결과..
		i3::vector<unsigned char>	m_enc_line;			// 라인단위 입력.. 
		int							vec_offset = 0;		// 반드시 0으로 시작해서 진행..
		DWORD enc_size;
		 
		for( ; ; )
		{
			::ReadFile(hsrcfile, &enc_size, sizeof(DWORD), &result_len, NULL);

			if ( result_len < sizeof(DWORD) )
				break;

			m_enc_line.resize(enc_size);		// clear없이 이것만으로 충분할것임..

			::ReadFile(hsrcfile, &m_enc_line[0], enc_size, &result_len, NULL);

			if (result_len < enc_size )
				break;

			// 일단 한줄 읽어냄..
			m_result_line.resize(enc_size);		// clear없이 이것만으로 충분할것임..
			Crypto::BlowFishDecrypt(bfk, &vec_dec_ivec[0], &vec_offset, &m_enc_line[0], &m_result_line[0], enc_size);

			//
			m_result_buffer.insert(m_result_buffer.end(), m_result_line.begin(), m_result_line.end());
			//
		}

		// m_result_buffer에 결과물이 모였음...이제 피시키는 지우고, hsrcfile은 닫을 것..
		Crypto::DeleteBlowFishKey(bfk);

		::CloseHandle(hsrcfile);
		
		HANDLE hdestfile = try_force_create_file(dest_file_path);

		if (hdestfile == INVALID_HANDLE_VALUE)
		{
			::CloseHandle(hdestfile);		return false;
		}

		DWORD num_written = 0;
		::WriteFile(hdestfile, &m_result_buffer[0], m_result_buffer.size(), &num_written, NULL);
		
		::CloseHandle(hdestfile);
		
		return true;
	}

}
