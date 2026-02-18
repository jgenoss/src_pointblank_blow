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
struct I3SND_BUFINFO
{
	LPDIRECTSOUNDBUFFER lpBuffer = nullptr;
	bool				isUsed = false;
	bool				isStopping = false;
	UINT32				nCreateFlag = 0;
};
#else
struct I3SND_BUFINFO
{
	char		*		pBuffer = nullptr;
	bool				isUsed = false;
	bool				isStopping = false;
	UINT32				nCreateFlag = 0;
};
#endif
//-----------------------------------------------------------------------------
// Name: i3Sound : public i3ResourceObject
// Desc: Resource Class
//-----------------------------------------------------------------------------
class I3_EXPORT_SOUND i3Sound : public i3ResourceObject
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE(i3Sound, i3ResourceObject);
//-----------------------------------------------------------------------------
//Value
//-----------------------------------------------------------------------------
protected:
	// persistent Members
	UINT32				m_nPersistFormat = I3SND_CMN_FORMAT_UNKNOWN;
	UINT32				m_nChannels = 1;
	UINT32				m_nSampleRate = 44100;
	UINT32				m_nBitsPerSample = 16;
	UINT32				m_nDataSize = 0;
	
	// volatile Members
	UINT32				m_nFormat = I3SND_CMN_FORMAT_UNKNOWN;
	UINT32				m_nBlockAlign = 4;
	UINT32				m_nAvgBytesPerSec;

	char *				m_pData = nullptr;
//-----------------------------------------------------------------------------
//Function
//-----------------------------------------------------------------------------
public:
	i3Sound(void); 
	virtual ~i3Sound(void); 	

	virtual	bool		Create( UINT32	nFormat, UINT32	nChannels, UINT32	nSampleRate, UINT32	nBitsPerSample, UINT32	nSampleSize);
	virtual bool		Create( char * pszPath);

	virtual void		ClearBuffers(void) {}

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
	virtual	bool		MakeRuntimeForm(char * pDstBuf, char * pSrcBuf, INT32	nSrcLength );

	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
	virtual void		OnBuildObjectList(i3::vector<i3PersistantElement*>& List);

	//추가적인 버퍼가 필요한 경우에 추가 버퍼를 미리 생성하여둔다.
	virtual bool		PrepareBuffers(){	return false;}
};

#endif // I3SOUND_H
