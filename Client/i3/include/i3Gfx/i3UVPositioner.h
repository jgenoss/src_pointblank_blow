#if !defined( __I3_GFX_TEXTURE_INDEXER_H)
#define __I3_GFX_TEXTURE_INDEXER_H


class	I3_EXPORT_GFX	i3UVPositioner	:
	public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3UVPositioner, i3ElementBase);
public:
	i3UVPositioner() {}

public:
	void			Set(const SIZE32 & Interval, const SIZE32 & TextureSize);

	POINT32			GetPositionIndex(UINT32 Index);
	bool			CalcuPosition(UINT32 Index, POINT32 & Pos);

	const SIZE32 &	GetInterval()							{return m_Interval;}
	const SIZE32 &	GetTextureSize()						{return m_TextureSize;}


private:
	SIZE32		m_Interval;
	SIZE32		m_TextureSize;
	POINT32		m_Count;
};

#endif	//__I3_GFX_TEXTURE_INDEXER_H
