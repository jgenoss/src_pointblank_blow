#ifndef _D_Z_OPENSSLRSA_H__
#define _D_Z_OPENSSLRSA_H__
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <openssl/rsa.h>
#include <openssl/pem.h>

class COpensslRSA
{
public:
	COpensslRSA():m_pRsa(NULL){}
	~COpensslRSA()
	{
		if(m_pRsa)
		{
			RSA_free(m_pRsa);
			m_pRsa = NULL;
		}
	}

public:
	bool Init( char* strFile );
	bool Encrypt(const char * pData , INT32 nDataLen,char * result); 
	bool Decrypt(const char * pData , INT32 nDataLen,char * result);
private:
	RSA *m_pRsa;
};

//extern COpensslRSA OpensslRSA;

#endif // _D_OPENSSLRSA_H__