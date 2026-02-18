#if !defined ( __I3_GAME_RES_BINARY_H_)
#define __I3_GAME_RES_BINARY_H_

#include "i3GameRes.h"
// 익스포트추가->다시 제거. (12.09.19.수빈)
class i3GameResBinary : public i3GameRes
{
	I3_CLASS_DEFINE( i3GameResBinary, i3GameRes);

protected:
	i3PersistantElement * m_pKeyObject = nullptr;

public:
	i3GameResBinary( void);
	virtual ~i3GameResBinary( void);

	bool			Load(const char * pszFile) { return false; }

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif