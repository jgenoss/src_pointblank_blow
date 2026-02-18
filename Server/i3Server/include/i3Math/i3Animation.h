#if !defined( __I3_ANIMATION_H)
#define __I3_ANIMATION_H

#include "i3Base.h"
#include "i3TransformSequence.h"

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

class I3_EXPORT_MATH i3Animation : public i3ResourceObject
{
	I3_CLASS_DEFINE( i3Animation);

protected:
	i3MemoryBuffer	*			m_pBindBuf;
	INT32						m_offMemory;
	I3ANIMTRACKINFO *			m_pTracks;
	INT32						m_TrackCount;
	REAL32						m_Duration;

public:
	i3Animation(void);
	virtual ~i3Animation(void);

	virtual BOOL		Create( INT32		TrackCount);

	void				Bind( I3ANIMTRACKINFO * pTracks, INT32 Count, REAL32 Duration, i3MemoryBuffer * pBuf, INT32 offset);

	bool							isBinded(void)						{ return (m_pBindBuf != NULL); }

	i3TransformSequence *			GetTrack( INT32 idx)				{ return m_pTracks[idx].m_pSeq; }
	void							SetTrack( INT32 idx, i3TransformSequence * pSeq, char * pszBoneName);
	INT32							AddTrack( i3TransformSequence * pSeq, char * pszBoneName);
	INT32							GetTrackCount(void)					{ return m_TrackCount; }
	void							SetTrackCount( INT32 cnt)			{ m_TrackCount = cnt; }
	char *							GetTrackBoneName( INT32 idx)		{ return m_pTracks[idx].m_szBone; }

	I3ANIMTRACKINFO *				getTrackArray(void)					{ return m_pTracks; }
	void							setTrackArray(I3ANIMTRACKINFO * pInfo)
	{
		I3MEM_SAFE_FREE( m_pTracks);
		m_pTracks = pInfo;
	}

	REAL32							GetDuration(void)					{ return m_Duration; }

	virtual UINT32		GetInterpolatedKeyframe( INT32 idxTrack, I3MATRIXINFO * pInfo, BOOL bFirst);
	virtual void		GetKeyframe( INT32 idxTrack, INT32 idxKey, MATRIX * pMtx);

	virtual void		OnBuildObjectList( i3::vector<i3PersistantElement*>& List);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);

	BOOL						SaveToFile( const char * pszPath);
	static i3Animation *		LoadFromFile( const char * pszPath);

	BOOL						SaveToFile( i3Stream * pStream);
	static i3Animation *		LoadFromFile( i3Stream * pStream);
};

#endif
