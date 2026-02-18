#if !defined( __OBJECT_MARK_OUTPOST_H__)
#define __OBJECT_MARK_OUTPOST_H__

//////////////////////////////////////////////////////////
// OutPost에서의 헬기 탑승의 화살표 마크용 오브젝트		//
//////////////////////////////////////////////////////////

class CGameObjectMark_OutPost : public i3Object
{
	I3_CLASS_DEFINE( CGameObjectMark_OutPost, i3Object);
protected:

public:
	CGameObjectMark_OutPost();
	virtual ~CGameObjectMark_OutPost();
};

#endif
