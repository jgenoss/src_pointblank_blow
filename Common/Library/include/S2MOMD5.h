/**
 * \file md5.h
 */
#ifndef _S2MOMD5_H
#define _S2MOMD5_H

/**
 * \brief          MD5 context structure
 */
typedef struct
{
    unsigned long total[2];     /*!< number of bytes processed  */
    unsigned long state[4];     /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */

    unsigned char ipad[64];     /*!< HMAC: inner padding        */
    unsigned char opad[64];     /*!< HMAC: outer padding        */
}
S2MO_md5_context;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          MD5 context setup
 *
 * \param ctx      context to be initialized
 */
void S2MO_md5_starts( S2MO_md5_context *ctx );

/**
 * \brief          MD5 process buffer
 *
 * \param ctx      MD5 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void S2MO_md5_update( S2MO_md5_context *ctx, unsigned char *input, int ilen );

/**
 * \brief          MD5 final digest
 *
 * \param ctx      MD5 context
 * \param output   MD5 checksum result
 */
void S2MO_md5_finish( S2MO_md5_context *ctx, unsigned char output[16] );

/**
 * \brief          Output = MD5( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   MD5 checksum result
 */
void S2MO_md5( unsigned char *input, int ilen, unsigned char output[16] );

/**
 * \brief          Output = MD5( file contents )
 *
 * \param path     input file name
 * \param output   MD5 checksum result
 *
 * \return         0 if successful, 1 if fopen failed,
 *                 or 2 if fread failed
 */
int S2MO_md5_file( char *path, unsigned char output[16] );

/**
 * \brief          MD5 HMAC context setup
 *
 * \param ctx      HMAC context to be initialized
 * \param key      HMAC secret key
 * \param keylen   length of the HMAC key
 */
void S2MO_md5_hmac_starts( S2MO_md5_context *ctx, unsigned char *key, int keylen );

/**
 * \brief          MD5 HMAC process buffer
 *
 * \param ctx      HMAC context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void S2MO_md5_hmac_update( S2MO_md5_context *ctx, unsigned char *input, int ilen );

/**
 * \brief          MD5 HMAC final digest
 *
 * \param ctx      HMAC context
 * \param output   MD5 HMAC checksum result
 */
void S2MO_md5_hmac_finish( S2MO_md5_context *ctx, unsigned char output[16] );

/**
 * \brief          Output = HMAC-MD5( hmac key, input buffer )
 *
 * \param key      HMAC secret key
 * \param keylen   length of the HMAC key
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   HMAC-MD5 result
 */
void S2MO_md5_hmac( unsigned char *key, int keylen,
               unsigned char *input, int ilen,
               unsigned char output[16] );

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int S2MO_md5_self_test( int verbose );

#ifdef __cplusplus
}
#endif

namespace S2MOMD5
{
	S2MO_EXPORT_BASE	void	S2MO_Gen( char *input, int ilen, char code[16] );
	S2MO_EXPORT_BASE	void	S2MO_GenHMAC( char * pKey, int lenKey, char * input, int len, char code[16]);
	S2MO_EXPORT_BASE	void	S2MO_HMAC_Start( S2MO_md5_context * pCtx, char * pKey, int keylen);
	S2MO_EXPORT_BASE	void	S2MO_HMAC_Update( S2MO_md5_context * pCtx, char * pData, int len);
	S2MO_EXPORT_BASE	void	S2MO_HMAC_Finish( S2MO_md5_context * pCtx, char code[16]);
	S2MO_EXPORT_BASE	void	S2MO_GetMD5String( char* pOutputString, INT32 i32Size, unsigned __int64*  Val);
	S2MO_EXPORT_BASE	int		S2MO_Getmd5_file( char *path, unsigned char output[16] );
};

#endif /* md5.h */
