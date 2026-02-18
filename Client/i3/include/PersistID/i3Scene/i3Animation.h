#if !defined( __I3_ANIMATION_H)
#define __I3_ANIMATION_H

#include "i3Base.h"
#include "i3Math.h"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

typedef struct PACKED _tagI3AnimTrackInfo
{
	i3TransformSequence * m_pSeq;
	char					m_szBone[MAX_NAME];
} I3ANIMTRACKINFO;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

class I3_EXPORT_SCENE i3Animation : public i3ResourceObject
{
	I3_CLASS_DEFINE( i3Animation);

protected:
	i3MemoryBuffer	*			m_pBindBuf;
	I3ANIMTRACKINFO *			m_pTracks;
	INT32						m_TrackCount;
	RT_REAL32						m_Duration;

public:
	i3Animation(void);
	virtual ~i3Animation(void);

	virtual BOOL		Create( INT32		TrackCount);

	void				Bind( I3ANIMTRACKINFO * pTracks, INT32 Count, REAL32 Duration, i3MemoryBuffer * pBuf);

	bool							isBinded(void)						{ return (m_pBindBuf != NULL); }

	i3TransformSequence *			GetTrack( INT32 idx)				{ return m_pTracks[idx].m_pSeq; }
	void							SetTrack( INT32 idx, i3TransformSequence * pSeq, char * pszBoneName);
	INT32							GetTrackCount(void)					{ return m_TrackCount; }
	void							SetTrackCount( INT32 cnt)			{ m_TrackCount = cnt; }
	char *							GetTrackBoneName( INT32 idx)		{ return m_pTracks[idx].m_szBone; }

	I3ANIMTRACKINFO *				getTrackArray(void)					{ return m_pTracks; }
	void							setTrackArray(I3ANIMTRACKINFO * pInfo)
	{
		I3MEM_SAFE_FREE( m_pTracks);
		m_pTracks = pInfo;
	}

	RT_REAL32							GetDuration(void)					{ return m_Duration; }

	virtual void		OnBuildObjectList( i3List * pList);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);

	BOOL						SaveToFile( const char * pszPath);
	static i3Animation *		LoadFromFile( const char * pszPath);

	BOOL						SaveToFile( i3Stream * pStream);
	static i3Animation *		LoadFromFile( i3Stream * pStream);
};

#endif
