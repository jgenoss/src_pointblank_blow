#pragma once

// i3Decal.h에서 옮겨왔습니다.  (2012.08.30.수빈.)

#define I3SG_DECAL_MAX_COUNT			64
#define I3SG_DECAL_MAX_VERTICES			128		//< 값을 작게 하면 데칼이 그려지지 않을 수 있습니다. by swoongoo
#define	I3SG_DECAL_ACTOR_GROUP			23

I3_EXPORT_SCENE void i3Decal_InitWorldHitPair();
I3_EXPORT_SCENE void i3Decal_ReleaseWorldHitPair();
