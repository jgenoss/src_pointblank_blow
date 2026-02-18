#if !defined( __I3_EVENT_DEF_H)
#define __I3_EVENT_DEF_H

#define I3_EVT_MASK_COLLISION			0x80000000

// 
// Collision Test에 충돌이 발생했음을 알리는 Event이다.
// 
// Param1 : i3ColliderSet *
// Param2 : Not used
//
#define	I3_EVT_HIT						(0x00000001 | I3_EVT_MASK_COLLISION)

//
// Game 상의 다양한 Action.
//
// Param1 : i3Action *
// Param2 : N/A
#define	I3_EVT_ACTION					(0x00000002)

//
// Physix Shape Contact event
//
// Param1 : i3PhysixShapeSet *
// Param2 : i3EventReceiver *
#define I3_EVT_CONTACT					(0x00000004)

//
// Input
//
// Param1 : i3InputDeviceManager *
// Param2 : REAL32 rDeltaSeconds
#define I3_EVT_INPUT					(0x00000008)

//
// 사용자 정의 Event의 시작
#define	I3_EVT_USER						(0x00000FFF)

#endif
