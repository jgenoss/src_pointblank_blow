#ifndef _I3_OPT_ANIMATION_H__
#define _I3_OPT_ANIMATION_H__

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptAnimation :	public i3SceneOptimizer
{
	I3_CLASS_DEFINE(i3OptAnimation);
public:
	i3OptAnimation(void);
	~i3OptAnimation(void);
public:

	//넘겨진 노드 및 하위 노드에 대해 애니메이션 최적화 수행
	virtual void	Trace(i3Node * pRoot );

	void	OptimizeAnim( i3Animation * pAnim);

private:
	////////////////////////////////////////////////////////////////////////////////////////////////

	//모든 키프레임의 Scale의 값이 모두 1인경우 스케일에대한 보간을 할필요가 없음을 명시하여줍니다.
	i3TransformSource	 * OptScaleInterPolate(i3TransformSource * pAnim ,REAL32	ApproximateValue);

	//키프레임중에 연속된시간에 동일한 값을 가지고있는 데이터들을 삭제하여줍니다.
	i3TransformSource *	OptEraseSameKeyFrame(i3TransformSource * pAnim ,REAL32	ApproximateValue);

	//모든 키프레임이 동일한 값을 가질경우 FIXED 플래그를 주어 첫번째 데이터만을 사용하게되고 
	//첫번째 데이터 이후의 키프레임을 삭제한다.
	i3TransformSource *	OptEraseSameKeyFrameAll(i3TransformSource * pAnim ,REAL32	ApproximateValue);

	//모든 키프레임들의 Time Step 이 균등한 경우에 Time Key의 값을 계산할 필요가 없음을 지정
	i3TransformSource *	OptTimeStep(i3TransformSource * pAnim,REAL32	ApproximateValue); 

	//
	BOOL	Vec3dApproximatelySame(VEC3D * pVec1 , VEC3D * pVec2 , REAL32	ApproximateValue);
	BOOL	QuaternionApproximatelySame(QUATERNION * pQuat1 , QUATERNION * pQuat2 , REAL32	ApproximateValue);

	void	LogAnimation(i3TransformSource * pAnim , char * szFileName);

	i3TransformSource * _OptTransSource( i3TransformSource * pSrc);	
};

#endif//