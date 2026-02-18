#include "pch.h"
#include "z_OpensslRSA.h"
#include <openssl/applink.c>

#define GWS_RSA_PADDING						RSA_PKCS1_PADDING
#define RSA_PADDING_LENGTH					12

//COpensslRSA OpensslRSA;

bool COpensslRSA::Init( char* strFile )
{
#ifdef WIN32
	BIO * key = BIO_new(BIO_s_file()); 
	BIO_read_filename( key, strFile );
	m_pRsa = PEM_read_bio_RSA_PUBKEY(key, NULL, NULL, NULL); 
	BIO_free_all(key);
	
	if(!m_pRsa)
	{
		//printf("OpensslRSA init failed.");
		return false;
	}
#endif

	return true;
}

bool COpensslRSA::Encrypt( const char * pData, INT32 nDataLen, char * dataResult )
{
	int nKeyLen = RSA_size(m_pRsa);
	int nBlockLen = nKeyLen - RSA_PADDING_LENGTH;
	
	int nOffset = 0;
	int nLen = (nDataLen - nOffset) > nBlockLen? nBlockLen: (nDataLen - nOffset);
	while(nLen > 0)
	{
		unsigned char dataBuf[NET_ENCRYPTED_TOKEN_SIZE+1] = {0,};

		int nRet = RSA_private_encrypt(nLen, (const unsigned char*)&pData[nOffset], &dataBuf[0], m_pRsa, GWS_RSA_PADDING);
		if (nRet <= 0)
		{
			return false;
		}
		else 
		{
			if( NET_ENCRYPTED_TOKEN_SIZE < nOffset + nLen )
				return false;

			i3mem::Copy( &dataResult[nOffset], dataBuf, nRet );

			nOffset += nLen;
			nLen = (nDataLen - nOffset) > nBlockLen? nBlockLen: (nDataLen - nOffset);
		}
	}
	
	return true;
}

bool COpensslRSA::Decrypt( const char * pData, int nDataLen, char * dataResult )
{
	int nKeyLen = RSA_size(m_pRsa);
	
	int nOffset = 0;
	int nLen = (nDataLen - nOffset) > nKeyLen? nKeyLen: (nDataLen - nOffset);
	while(nLen > 0)
	{
		unsigned char dataBuf[NET_ENCRYPTED_TOKEN_SIZE+1] = {0,};

		int nRet = RSA_public_decrypt(nLen, (const unsigned char*)&pData[nOffset], (unsigned char*)dataBuf, m_pRsa, GWS_RSA_PADDING);
		if (nRet <= 0)
		{
			return false;
		}
		else 
		{
			if( NET_ENCRYPTED_TOKEN_SIZE < nOffset + nLen )
				return false;

			i3mem::Copy( &dataResult[nOffset], dataBuf, nRet );

			nOffset += nLen;
			nLen = (nDataLen - nOffset) > nKeyLen? nKeyLen: (nDataLen - nOffset);
		}
	}
	
	return true;
}