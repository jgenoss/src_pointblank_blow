#if !defined( __I3_ANIMATION_H)
#define __I3_ANIMATION_H

#include "i3Base.h"
#include "i3TransformSequence.h"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace i3
{
	namespace pack
	{
		namespace animation
		{
			struct PACKED TRACK_INFO
			{
				i3TransformSequence * m_pSeq = nullptr;
				char					m_szBone[MAX_NAME] = { 0 };
			};
		}
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

class I3_EXPORT_MATH i3Animation : public i3ResourceObject
{
	I3_EXPORT_CLASS_DEFINE( i3Animation, i3ResourceObject);

protected:
	i3MemoryBuffer	*			m_pBindBuf = nullptr;
	INT32						m_offMemory = 0;
	i3::pack::animation::TRACK_INFO *			m_pTracks = nullptr;
	INT32						m_TrackCount = 0;
	REAL32						m_Duration = 0.0f;

public:
	virtual ~i3Animation(void);

	virtual bool		Create( INT32		TrackCount);

	void				Bind( i3::pack::animation::TRACK_INFO * pTracks, INT32 Count, REAL32 Duration, i3MemoryBuffer * pBuf, INT32 offset);

	bool							isBinded(void)						{ return (m_pBindBuf != nullptr); }

	i3TransformSequence *			GetTrack( INT32 idx)				{ return m_pTracks[idx].m_pSeq; }
	void							SetTrack( INT32 idx, i3TransformSequence * pSeq, const char * pszBoneName);
	INT32							AddTrack( i3TransformSequence * pSeq, char * pszBoneName);
	INT32							GetTrackCount(void)					{ return m_TrackCount; }
	void							SetTrackCount( INT32 cnt)			{ m_TrackCount = cnt; }
	char *							GetTrackBoneName( INT32 idx)		{ return m_pTracks[idx].m_szBone; }

	i3::pack::animation::TRACK_INFO *				getTrackArray(void)					{ return m_pTracks; }
	void							setTrackArray(i3::pack::animation::TRACK_INFO * pInfo)
	{
		I3MEM_SAFE_FREE( m_pTracks);
		m_pTracks = pInfo;
	}

	REAL32							GetDuration(void)					{ return m_Duration; }

	virtual UINT32		GetInterpolatedKeyframe( INT32 idxTrack, I3MATRIXINFO * pInfo, bool bFirst);
	virtual void		GetKeyframe( INT32 idxTrack, INT32 idxKey, MATRIX * pMtx);

	virtual void		OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;

	bool						SaveToFile( const char * pszPath);
	static i3Animation *		LoadFromFile( const char * pszPath, UINT32 * pSize = nullptr);

	bool						SaveToFile( i3Stream * pStream);
	static i3Animation *		LoadFromFile( i3Stream * pStream);
};

#endif
