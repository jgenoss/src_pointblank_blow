#ifndef __S2_MO_STRUCT_LOGIN_H__
#define __S2_MO_STRUCT_LOGIN_H__



#define MAX_HACK_TYPE		8
#define MAX_USG_DX_FUNC		2
#define DX_DLL_MD5_SIZE		33

#pragma pack( push, 1)
struct HACK_TYPE_BASE
{
	UINT16	m_ui8Type[MAX_HACK_TYPE];
};

struct HACK_CHECK_LOGIN : public HACK_TYPE_BASE
{
	char	m_strDXMD5[DX_DLL_MD5_SIZE];
};

struct HACK_CHECK_REALTIME : public HACK_TYPE_BASE
{
	UINT64	m_ui64DXFunction[MAX_USG_DX_FUNC];
	UINT64	m_ui64DXDevice[MAX_USG_DX_FUNC];
};
#pragma pack(pop)


struct PACKET_LOGIN_REQ : public S2MOPacketBaseT<PROTOCOL_LOGIN_REQ>
{
	S2MOValue<UINT8>								m_ui8PBVer;	// PB_VER
	S2MOStringA<NET_ID_SIZE>						m_strID;
	S2MOStringA<NET_PW_SIZE>						m_strPasswd;

	S2MOValue<char,NET_ENCRYPTED_TOKEN_SIZE>		m_pSessionKey;

	S2MOValue<SC_VERSION>							m_stVersion;
	S2MOValue<UINT8>								m_ui8LocalCode;
	S2MOValue<INT64>								m_i64MacAddress;
	S2MOValue<UINT64>								m_ui64EncryptKey;
	S2MOStringA<NET_MD5_KEY_SIZE>					m_strMD5;
	S2MOValue<INT64>								m_i64ActiveMacAddress;

	S2MOValue<HACK_CHECK_LOGIN>						m_stHackCheckLogin;
	S2MOValue<UINT8>								m_ui8LinkMethod;

	// 2016-11-02 User PC Specs 추가
	S2MOValue<USER_INFO_PCSPECS>					m_stPCSpec;
	
	PACKET_LOGIN_REQ()
	{
		_SetValue( m_ui8PBVer );
		_SetValue( m_strID );
		_SetValue( m_strPasswd );
		_SetValue( m_pSessionKey );
		_SetValue( m_stVersion );
		_SetValue( m_ui8LocalCode );
		_SetValue( m_i64MacAddress );
		_SetValue( m_ui64EncryptKey );
		_SetValue( m_strMD5 );
		_SetValue( m_i64ActiveMacAddress );

		_SetValue(m_stHackCheckLogin);
		_SetValue(m_ui8LinkMethod);
		_SetValue(m_stPCSpec);
	}
};

struct PACKET_LOGIN_ACK : public S2MOPacketBaseT<PROTOCOL_LOGIN_ACK>
{
	S2MOValue<T_RESULT>							m_TResult;
	S2MOValue<T_UID>							m_TUID;
	S2MOStringA<NET_ID_SIZE>					m_strID;
	S2MOStringA<NORTH_CUSTOM_MSG_SIZE>			m_strCustom;
	S2MOStringA<BILLING_ID_SIZE>				m_strUID;
	S2MOValue<UINT32>							m_ui32BlockDate;
	S2MOStringW<BLOCK_COMMENT_SIZE> 			m_strUserNoticeComment; // 추가
	S2MOValue<INT32>							m_i32PenaltyAmount;          //추가
	S2MOStringA<EXT_UID_SIZE>					m_strExtUID;
	S2MOStringA<WEB_LOGINKEY_SIZE>				m_strWebLoginKey;

	PACKET_LOGIN_ACK()
	{
		_SetValue( m_TResult );
		_SetValue( m_TUID );
		_SetValue( m_strID );
		_SetValue( m_strCustom );
		_SetValue( m_strUID );
		_SetValue( m_ui32BlockDate );
		_SetValue( m_strUserNoticeComment);
		_SetValue( m_i32PenaltyAmount);

		_SetValue(m_strExtUID);
		_SetValue(m_strWebLoginKey);
	}
};

struct PACKET_BASE_LOGOUT_REQ : public S2MOPacketBaseT<PROTOCOL_BASE_LOGOUT_REQ>
{
};

struct PACKET_BASE_LOGOUT_ACK : public S2MOPacketBaseT<PROTOCOL_BASE_LOGOUT_ACK>
{
};

#endif