#pragma once

//
// 우선 사용자 입장에서 처리하는 것 작성..
// 

struct bf_key_st;
namespace Crypto {  	typedef bf_key_st	BF_KEY; }

namespace i3
{
	class rsa_bf_log_enc;

	I3_EXPORT_BASE rsa_bf_log_enc*		create_rsa_bf_enc(const i3::rc_string& public_key_path, 
							const i3::rc_string& dest_file_path); // 공개키부터 읽음..없으면 비활성화 수준으로 널리턴.

	I3_EXPORT_BASE void				delete_rsa_bf_enc(rsa_bf_log_enc* p);

	class I3_EXPORT_BASE rsa_bf_log_enc
	{
	public:
		rsa_bf_log_enc(Crypto::BF_KEY* k, const unsigned char(&ivec)[8], const i3::rc_string& log_path );
		~rsa_bf_log_enc();
		void log(const char* str);			// 점진적으로 하나씩 처리..
		const i3::rc_string&	get_log_path() const { return m_log_file_path; }
	private:
		Crypto::BF_KEY*		m_blowfish_key;			// 프로그램 실행단위로는 랜덤해야한다..
		unsigned char		m_ivec[8];				// 변경이 수시로 일어날 것으로 보인다..
		int					m_ivec_offset;
		i3::rc_string		m_log_file_path;
	};
	
	// 존재하지 않으면 생성하게끔 해준다..
	
	I3_EXPORT_BASE bool save_public_key_from_db(const i3::rc_string& public_private_db_path, const i3::rc_string& dest_key_path);

	I3_EXPORT_BASE bool save_public_private_key_to_db(const i3::rc_string& public_private_db_path, size_t key_length);		//	db파일에 공개/비공개키쌍을 누적시킨다...

	// 디코드 파일은 여러개가 될수도 있다는 생각도 좀 해볼것..
	I3_EXPORT_BASE bool decrypt_log_enc(const i3::rc_string& public_private_db_path, const i3::rc_string& src_file_path,
						const i3::rc_string& dest_file_path);
	
	
	
}
