#if !defined( __I3_OPT_CONVERT_TEXTURE_FORMAT_H)
#define __I3_OPT_CONVERT_TEXTURE_FORMAT_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptConvertTextureFormat : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptConvertTextureFormat, i3SceneOptimizer);

public:
	i3OptConvertTextureFormat(void);

protected:
	REAL32			m_rTextureCoordMin_U = 1.0f;
	REAL32			m_rTextureCoordMax_U = 0.0f;
	REAL32			m_rTextureCoordMin_V = 1.0f;
	REAL32			m_rTextureCoordMax_V = 0.0f;

public:
	void	FindTextureCoordMinMax( i3VertexArray * pDest, i3VertexArray * pSrc );
	void	ConvertTextureFormat( i3VertexArray * pDest, i3VertexArray * pSrc, INT32 which, INT32 index );
	void	SetTextureCoordScale( i3Node* pParentNode );
};

#endif // __I3_OPT_CONVERT_TEXTURE_FORMAT_H
