//-----------------------------------------------------------------------------
// File: i3Sound.h
//
// Desc: 
//		
// Date: 2005/11/10		by: raja
//-----------------------------------------------------------------------------
#ifndef I3_SOUND_H__
#define I3_SOUND_H__

#include "i3SoundConfig.h"

#if defined( I3SND_DSOUND)
typedef struct _tagi3SoundBufferInfo
{
	LPDIRECTSOUNDBUFFER lpBuffer;
	BOOL				isUsed;
	BOOL				isStopping;
	UINT32				nCreateFlag;
}I3SND_BUFINFO;
#else
typedef struct _tagi3SoundBufferInfo
{
	char		*		pBuffer;
	BOOL				isUsed;
	BOOL				isStopping;
	UINT32				nCreateFlag;
}I3SND_BUFINFO;
#endif
//-----------------------------------------------------------------------------
// Name: i3Sound : public i3ResourceObject
// Desc: Resource Class
//-----------------------------------------------------------------------------
class I3_EXPORT_SOUND i3Sound : public i3ResourceObject
{
	I3_ABSTRACT_CLASS_DEFINE(i3Sound);
//-----------------------------------------------------------------------------
//Value
//-----------------------------------------------------------------------------
protected:
	// persistent Members
	UINT32				m_nPersistFormat;
	UINT32				m_nChannels;
	UINT32				m_nSampleRate;
	UINT32				m_nBitsPerSample;
	UINT32				m_nDataSize;
	
	// volatile Members
	UINT32				m_nFormat;
	UINT32				m_nBlockAlign;
	UINT32				m_nAvgBytesPerSec;

	char *				m_pData;
//-----------------------------------------------------------------------------
//Function
//-----------------------------------------------------------------------------
public:
	i3Sound(void); 
	virtual ~i3Sound(void); 	

	virtual	BOOL		Create( UINT32	nFormat, UINT32	nChannels, UINT32	nSampleRate, UINT32	nBitsPerSample, UINT32	nSampleSize);
	virtual BOOL		Create( char * pszPath);

	virtual void		ClearBuffers(void);

public:
	UINT32				GetFormat()				{	return m_nFormat;}
	UINT32				GetChannels()			{	return m_nChannels;}
	UINT32				GetSampleRate()			{	return m_nSampleRate;}
	UINT32				GetBitsPerSample()		{	return m_nBitsPerSample;}
	UINT32				GetAvgBytesPerSec()		{	return m_nAvgBytesPerSec;}

	UINT32				getBlockAlign()			{	return m_nBlockAlign;}
	virtual REAL32		GetDuration();
	//Sound Info
	UINT32				GetDataLength()			{	return m_nDataSize;}

	UINT32				GetPersistFormat()		{	return m_nPersistFormat;}
	void				SetPersistFormat( UINT32	nFormat )
	{
		m_nPersistFormat = nFormat;
	}

	void				SetData( char * pData);
	char*				getData( void)			{	return m_pData;}

public:
	virtual	BOOL		MakeRuntimeForm(char * pDstBuf, char * pSrcBuf, INT32	nSrcLength );

	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
	virtual void		OnBuildObjectList(i3List * pList);

	//추가적인 버퍼가 필요한 경우에 추가 버퍼를 미리 생성하여둔다.
	virtual BOOL		PrepareBuffers(){	return FALSE;}
};

#endif // I3SOUND_H
