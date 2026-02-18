#include "pch.h"
#include "S2CriticalSection.h"
#include "WeaponTable.h"
#include "hms.h"
#include "DediRoom.h"

NxBounds3 g_StageBounds[ STAGE_UID_MAX ]; // boundary table

CHMS::CHMS() {}
CHMS::~CHMS() {}

// 2011.09.09 안국정
// 데키페이지: http://deki.zepetto.com/index.php?title=Studio:Point_Blank/Point_Blank/QA/PB_Cheat/%EC%84%9C%EB%B2%84_%EC%A4%91%EB%A0%A5%ED%95%B5_%EC%A2%8C%ED%91%9C 의 최신 중력핵 감지 좌표데이터를
// PointBlank > Document > Programming > GravityHackMapBoundry > "g_CommonGetMapBounds()전역함수 소스 자동화.ods" 파일로 쉽게 반영할수 있습니다.
void g_CommonGetMapBounds( STAGE_UID OrdStageID, REAL32* pBound )
{
	switch( OrdStageID )
	{
	case STAGE_UID_BLACKPANTHER:
		pBound[ X_START_BOUNDRY] = -43.102f;	pBound[ X_END_BOUNDRY] = 43.102f;
		pBound[ Y_START_BOUNDRY] = -21.502f;	pBound[ Y_END_BOUNDRY] = 5.902f; //(x=3.345703,y=-20.125000,z=-1.617188)
		pBound[ Z_START_BOUNDRY] = -54.102f;	pBound[ Z_END_BOUNDRY] = 48.402f;
		return;
	case STAGE_UID_BLOWCITY:
		pBound[ X_START_BOUNDRY] = -78.182f;	pBound[ X_END_BOUNDRY] = 48.002f;
		pBound[ Y_START_BOUNDRY] = -0.702f;		pBound[ Y_END_BOUNDRY] = 20.422f;
		pBound[ Z_START_BOUNDRY] = -47.322f;	pBound[ Z_END_BOUNDRY] = 51.562f;
		return;
	case STAGE_UID_BREAKDOWN:
		pBound[ X_START_BOUNDRY] = -86.582f;	pBound[ X_END_BOUNDRY] = 89.202f;
		pBound[ Y_START_BOUNDRY] = -0.902f;		pBound[ Y_END_BOUNDRY] = 14.202f;
		pBound[ Z_START_BOUNDRY] = -38.302f;	pBound[ Z_END_BOUNDRY] = 38.202f;
		return;
	case STAGE_UID_BREEDING_NEST:
		pBound[ X_START_BOUNDRY] = -48.702f;	pBound[ X_END_BOUNDRY] = 48.702f;
		pBound[ Y_START_BOUNDRY] = -3.802f;		pBound[ Y_END_BOUNDRY] = 16.362f;
		pBound[ Z_START_BOUNDRY] = -47.202f;	pBound[ Z_END_BOUNDRY] = 33.202f;
		return;
	case STAGE_UID_BURNINGHALL:
		pBound[ X_START_BOUNDRY] = -38.302f;	pBound[ X_END_BOUNDRY] = 38.302f;
		pBound[ Y_START_BOUNDRY] = -0.902f;		pBound[ Y_END_BOUNDRY] = 13.202f;
		pBound[ Z_START_BOUNDRY] = -19.402f;	pBound[ Z_END_BOUNDRY] = 19.402f;
		return;
	case STAGE_UID_RED_ROCK:
		pBound[ X_START_BOUNDRY] = -19.202f;	pBound[ X_END_BOUNDRY] = 19.202f;
		pBound[ Y_START_BOUNDRY] = -0.502f;		pBound[ Y_END_BOUNDRY] = 6.902f;
		pBound[ Z_START_BOUNDRY] = -51.302f;	pBound[ Z_END_BOUNDRY] = 52.702f;
		return;
	case STAGE_UID_MIDNIGHT_ZONE:
		pBound[ X_START_BOUNDRY] = -12.902f;	pBound[ X_END_BOUNDRY] = 12.902f;
		pBound[ Y_START_BOUNDRY] = -0.902f;		pBound[ Y_END_BOUNDRY] = 9.402f;
		pBound[ Z_START_BOUNDRY] = -51.102f;	pBound[ Z_END_BOUNDRY] = 52.602f;
		return;
	case STAGE_UID_CRACKDOWN:
		pBound[ X_START_BOUNDRY] = -17.402f;	pBound[ X_END_BOUNDRY] = 17.102f;
		pBound[ Y_START_BOUNDRY] = 0.698f;		pBound[ Y_END_BOUNDRY] = 8.001f;
		pBound[ Z_START_BOUNDRY] = -17.002f;	pBound[ Z_END_BOUNDRY] = 17.502f;
		return;
	case STAGE_UID_DINOBREAKDOWN:
		pBound[ X_START_BOUNDRY] = -88.702f;	pBound[ X_END_BOUNDRY] = 91.102f;
		pBound[ Y_START_BOUNDRY] = -0.902f;		pBound[ Y_END_BOUNDRY] = 15.001f;
		pBound[ Z_START_BOUNDRY] = -29.702f;	pBound[ Z_END_BOUNDRY] = 39.702f;
		return;
	case STAGE_UID_DINORUINS:
		pBound[ X_START_BOUNDRY] = -52.001f;	pBound[ X_END_BOUNDRY] = 56.001f;
		pBound[ Y_START_BOUNDRY] = -1.001f;		pBound[ Y_END_BOUNDRY] = 20.001f;
		pBound[ Z_START_BOUNDRY] = -35.001f;	pBound[ Z_END_BOUNDRY] = 35.001f;
		return;
	case STAGE_UID_DUPTOWN:
		pBound[ X_START_BOUNDRY] = -31.302f;	pBound[ X_END_BOUNDRY] = 32.102f;
		pBound[ Y_START_BOUNDRY] = -0.902f;		pBound[ Y_END_BOUNDRY] = 11.402f;
		pBound[ Z_START_BOUNDRY] = -64.702f;	pBound[ Z_END_BOUNDRY] = 69.002f;
		return;
	case STAGE_UID_DESERTCAMP:
		pBound[ X_START_BOUNDRY] = -43.402f;	pBound[ X_END_BOUNDRY] = 42.902f;
		pBound[ Y_START_BOUNDRY] = -2.502f;		pBound[ Y_END_BOUNDRY] = 8.802f;
		pBound[ Z_START_BOUNDRY] = -24.702f;	pBound[ Z_END_BOUNDRY] = 20.302f;
		return;
	case STAGE_UID_DOWNTOWN:
		pBound[ X_START_BOUNDRY] = -50.202f;	pBound[ X_END_BOUNDRY] = 54.702f;
		pBound[ Y_START_BOUNDRY] = 1.098f;		pBound[ Y_END_BOUNDRY] = 18.502f;
		pBound[ Z_START_BOUNDRY] = -58.202f;	pBound[ Z_END_BOUNDRY] = 46.502f;
		return;
	case STAGE_UID_DOWNTOWN2:
		pBound[ X_START_BOUNDRY] = -31.602f;	pBound[ X_END_BOUNDRY] = 49.602f;
		pBound[ Y_START_BOUNDRY] = -12.802f;	pBound[ Y_END_BOUNDRY] = 14.7f;
		pBound[ Z_START_BOUNDRY] = -47.402f;	pBound[ Z_END_BOUNDRY] = 54.402f;
		return;
	case STAGE_UID_DSQUAD:
		pBound[ X_START_BOUNDRY] = -30.302f;	pBound[ X_END_BOUNDRY] = 29.902f;
		pBound[ Y_START_BOUNDRY] = -0.902f;		pBound[ Y_END_BOUNDRY] = 9.002f;
		pBound[ Z_START_BOUNDRY] = -45.502f;	pBound[ Z_END_BOUNDRY] = 45.502f;
		return;
	case STAGE_UID_EASTERNROAD:
		pBound[ X_START_BOUNDRY] = -27.002f;	pBound[ X_END_BOUNDRY] = 33.702f;
		pBound[ Y_START_BOUNDRY] = -1.002f;		pBound[ Y_END_BOUNDRY] = 21.402f;
		pBound[ Z_START_BOUNDRY] = -27.602f;	pBound[ Z_END_BOUNDRY] = 31.902f;
		return;
	case STAGE_UID_FACEROCK:
		pBound[ X_START_BOUNDRY] = -50.702f;	pBound[ X_END_BOUNDRY] = 38.202f;
		pBound[ Y_START_BOUNDRY] = -25.502f;	pBound[ Y_END_BOUNDRY] = 22.402f;
		pBound[ Z_START_BOUNDRY] = -41.702f;	pBound[ Z_END_BOUNDRY] = 41.302f;
		return;
	case STAGE_UID_GIRAN:
	case STAGE_UID_GIRAN2:
		pBound[ X_START_BOUNDRY] = -37.402f;	pBound[ X_END_BOUNDRY] = 118.502f;
		pBound[ Y_START_BOUNDRY] = 7.698f;		pBound[ Y_END_BOUNDRY] = 18.702f;
		pBound[ Z_START_BOUNDRY] = -102.702f;	pBound[ Z_END_BOUNDRY] = 61.602f;
		return;
	case STAGE_UID_HOSPITAL:
		pBound[ X_START_BOUNDRY] = -44.702f;	pBound[ X_END_BOUNDRY] = 45.002f;
		pBound[ Y_START_BOUNDRY] = -4.402f;		pBound[ Y_END_BOUNDRY] = 11.302f;
		pBound[ Z_START_BOUNDRY] = -31.902f;	pBound[ Z_END_BOUNDRY] = 55.002f;
		return;
	case STAGE_UID_KICKPOINT:
		pBound[ X_START_BOUNDRY] = -28.602f;	pBound[ X_END_BOUNDRY] = 28.602f;
		pBound[ Y_START_BOUNDRY] = -0.902f;		pBound[ Y_END_BOUNDRY] = 10.602f;
		pBound[ Z_START_BOUNDRY] = -28.502f;	pBound[ Z_END_BOUNDRY] = 28.502f;
		return;
	case STAGE_UID_LIBRARY:
		pBound[ X_START_BOUNDRY] = -36.902f;	pBound[ X_END_BOUNDRY] = 37.302f;
		pBound[ Y_START_BOUNDRY] = -0.902f;		pBound[ Y_END_BOUNDRY] = 15.202f;
		pBound[ Z_START_BOUNDRY] = -17.502f;	pBound[ Z_END_BOUNDRY] = 23.202f;
		return;
	case STAGE_UID_LUXVILLE:
		pBound[ X_START_BOUNDRY] = -39.902f;	pBound[ X_END_BOUNDRY] = 39.602f;
		pBound[ Y_START_BOUNDRY] = 6.698f;		pBound[ Y_END_BOUNDRY] = 19.602f;
		pBound[ Z_START_BOUNDRY] = -35.902f;	pBound[ Z_END_BOUNDRY] = 31.602f;
		return;
	case STAGE_UID_PORT_AKABA:
		pBound[ X_START_BOUNDRY] = -24.702f;	pBound[ X_END_BOUNDRY] = 24.702f;
		pBound[ Y_START_BOUNDRY] = -0.902f;		pBound[ Y_END_BOUNDRY] = 7.102f;
		pBound[ Z_START_BOUNDRY] = -35.702f;	pBound[ Z_END_BOUNDRY] = 35.702f;
		return;
	case STAGE_UID_MSTATION:
		pBound[ X_START_BOUNDRY] = -12.702f;	pBound[ X_END_BOUNDRY] = 18.402f;
		pBound[ Y_START_BOUNDRY] = -1.602f;		pBound[ Y_END_BOUNDRY] = 3.902f;
		pBound[ Z_START_BOUNDRY] = -37.502f;	pBound[ Z_END_BOUNDRY] = 37.502f;
		return;
	case STAGE_UID_SAFARI:
		pBound[ X_START_BOUNDRY] = -44.202f;	pBound[ X_END_BOUNDRY] = 44.102f;
		pBound[ Y_START_BOUNDRY] = -1.102f;		pBound[ Y_END_BOUNDRY] = 8.502f;
		pBound[ Z_START_BOUNDRY] = -16.302f;	pBound[ Z_END_BOUNDRY] = 15.702f;
		return;
	case STAGE_UID_SAINTMANSION:
		pBound[ X_START_BOUNDRY] = -17.702f;	pBound[ X_END_BOUNDRY] = 17.702f;
		pBound[ Y_START_BOUNDRY] = -0.902f;		pBound[ Y_END_BOUNDRY] = 15.902f;
		pBound[ Z_START_BOUNDRY] = -22.202f;	pBound[ Z_END_BOUNDRY] = 22.202f;
		return;
	case STAGE_UID_SANDSTORM:
		pBound[ X_START_BOUNDRY] = -41.702f;	pBound[ X_END_BOUNDRY] = 40.702f;
		pBound[ Y_START_BOUNDRY] = -3.702f;		pBound[ Y_END_BOUNDRY] = 5.902f;
		pBound[ Z_START_BOUNDRY] = -58.702f;	pBound[ Z_END_BOUNDRY] = 38.302f;
		return;
	case STAGE_UID_SENTRYBASE:
		pBound[ X_START_BOUNDRY] = -30.902f;	pBound[ X_END_BOUNDRY] = 35.702f;
		pBound[ Y_START_BOUNDRY] = -0.902f;		pBound[ Y_END_BOUNDRY] = 13.502f;
		pBound[ Z_START_BOUNDRY] = -48.702f;	pBound[ Z_END_BOUNDRY] = 48.102f;
		return;
	case STAGE_UID_SHOPPINGCENTER:
		pBound[ X_START_BOUNDRY] = -45.202f;	pBound[ X_END_BOUNDRY] = 41.202f;
		pBound[ Y_START_BOUNDRY] = 1.202f;		pBound[ Y_END_BOUNDRY] = 16.002f;
		pBound[ Z_START_BOUNDRY] = -47.502f;	pBound[ Z_END_BOUNDRY] = 40.002f;
		return;
	case STAGE_UID_STORMTUBE:
		pBound[ X_START_BOUNDRY] = -70.302f;	pBound[ X_END_BOUNDRY] = 48.602f;
		pBound[ Y_START_BOUNDRY] = -1.202f;		pBound[ Y_END_BOUNDRY] = 11.202f;
		pBound[ Z_START_BOUNDRY] = -67.502f;	pBound[ Z_END_BOUNDRY] = 64.002f;
		return;
	case STAGE_UID_SUPPLYBASE:
		pBound[ X_START_BOUNDRY] = -16.202f;	pBound[ X_END_BOUNDRY] = 16.202f;
		pBound[ Y_START_BOUNDRY] = 2.799f;		pBound[ Y_END_BOUNDRY] = 12.702f;
		pBound[ Z_START_BOUNDRY] = -40.002f;	pBound[ Z_END_BOUNDRY] = 40.002f;
		return;
	case STAGE_UID_TRAININGCAMP:
		pBound[ X_START_BOUNDRY] = -27.002f;	pBound[ X_END_BOUNDRY] = 28.302f;
		pBound[ Y_START_BOUNDRY] = -0.902f;		pBound[ Y_END_BOUNDRY] = 10.102f;
		pBound[ Z_START_BOUNDRY] = -50.002f;	pBound[ Z_END_BOUNDRY] = 49.902f;
		return;
	case STAGE_UID_UPTOWN:
		pBound[ X_START_BOUNDRY] = -31.302f;	pBound[ X_END_BOUNDRY] = 32.102f;
		pBound[ Y_START_BOUNDRY] = -1.002f;		pBound[ Y_END_BOUNDRY] = 9.102f;
		pBound[ Z_START_BOUNDRY] = -64.802f;	pBound[ Z_END_BOUNDRY] = 67.202f;
		return;
	case STAGE_UID_DRAGONALLEY:
		pBound[ X_START_BOUNDRY] = -55.001f;	pBound[ X_END_BOUNDRY] = 50.001f;
		pBound[ Y_START_BOUNDRY] = -0.501f;		pBound[ Y_END_BOUNDRY] = 11.501f;
		pBound[ Z_START_BOUNDRY] = -25.001f;	pBound[ Z_END_BOUNDRY] = 25.001f;
		return;
	case STAGE_UID_MACHUPICHU:
		pBound[ X_START_BOUNDRY] = -60.001f;	pBound[ X_END_BOUNDRY] = 60.001f;
		pBound[ Y_START_BOUNDRY] = -0.001f;		pBound[ Y_END_BOUNDRY] = 30.001f;
		pBound[ Z_START_BOUNDRY] = -60.001f;	pBound[ Z_END_BOUNDRY] = 60.001f;
		return;
	case STAGE_UID_CARGOSHIP:
		pBound[ X_START_BOUNDRY] = -16.501f;	pBound[ X_END_BOUNDRY] = 16.501f;
		pBound[ Y_START_BOUNDRY] = -13.501f;	pBound[ Y_END_BOUNDRY] = 7.501f;
		pBound[ Z_START_BOUNDRY] = -58.501f;	pBound[ Z_END_BOUNDRY] = 56.501f;
		return;
	case STAGE_UID_TWOTOWERS:
		pBound[ X_START_BOUNDRY] = -28.821f;	pBound[ X_END_BOUNDRY] = 29.361f;
		pBound[ Y_START_BOUNDRY] = -76.421f;	pBound[ Y_END_BOUNDRY] = 40.001f;
		pBound[ Z_START_BOUNDRY] = -41.251f;	pBound[ Z_END_BOUNDRY] = 41.431f; // 북미
		return;
	case STAGE_UID_ANGKOR:
		pBound[ X_START_BOUNDRY] = -55.001f;	pBound[ X_END_BOUNDRY] = 55.001f;
		pBound[ Y_START_BOUNDRY] =  -3.201f;	pBound[ Y_END_BOUNDRY] = 20.001f; // (-35.1, -2.0, 10.7 ) 가능
		pBound[ Z_START_BOUNDRY] = -35.001f;	pBound[ Z_END_BOUNDRY] = 30.001f;
		return;
	case STAGE_UID_GHOSTTOWN:
		pBound[ X_START_BOUNDRY] = -18.001f;	pBound[ X_END_BOUNDRY] = 22.001f;
		pBound[ Y_START_BOUNDRY] =  -8.601f;	pBound[ Y_END_BOUNDRY] = 10.001f;
		pBound[ Z_START_BOUNDRY] = -50.001f;	pBound[ Z_END_BOUNDRY] = 51.001f;
		return;
	case STAGE_UID_GRANDBAZAAR:
		pBound[ X_START_BOUNDRY] = -43.001f;	pBound[ X_END_BOUNDRY] = 44.001f;
		pBound[ Y_START_BOUNDRY] =  -0.001f;	pBound[ Y_END_BOUNDRY] = 7.001f; 
		pBound[ Z_START_BOUNDRY] = -28.001f;	pBound[ Z_END_BOUNDRY] = 25.001f;
		return;
	case STAGE_UID_DINOLAB:
		pBound[ X_START_BOUNDRY] = -50.001f;	pBound[ X_END_BOUNDRY] = 50.001f;
		pBound[ Y_START_BOUNDRY] =   4.472f;	pBound[ Y_END_BOUNDRY] = 20.001f; 
		pBound[ Z_START_BOUNDRY] = -27.001f;	pBound[ Z_END_BOUNDRY] = 27.001f;
		return;
	case STAGE_UID_UNDER23:
		pBound[ X_START_BOUNDRY] = -20.001f;	pBound[ X_END_BOUNDRY] = 21.001f;
		pBound[ Y_START_BOUNDRY] =  -2.001f;	pBound[ Y_END_BOUNDRY] = 12.001f; 
		pBound[ Z_START_BOUNDRY] = -51.851f;	pBound[ Z_END_BOUNDRY] = 51.001f;
		return;
	case STAGE_UID_TAIPEICITYMALL:
		pBound[ X_START_BOUNDRY] = -46.001f;	pBound[ X_END_BOUNDRY] = 46.001f;
		pBound[ Y_START_BOUNDRY] =  -3.001f;	pBound[ Y_END_BOUNDRY] = 8.171f; 
		pBound[ Z_START_BOUNDRY] = -23.001f;	pBound[ Z_END_BOUNDRY] = 21.001f;
		return;
	case STAGE_UID_SAFARI2:
		pBound[ X_START_BOUNDRY] = -50.001f;	pBound[ X_END_BOUNDRY] = 50.001f;
		pBound[ Y_START_BOUNDRY] =  -1.001f;	pBound[ Y_END_BOUNDRY] = 10.001f; 
		pBound[ Z_START_BOUNDRY] = -30.001f;	pBound[ Z_END_BOUNDRY] = 30.001f;
		return;
	case STAGE_UID_AIRPORT:
		pBound[ X_START_BOUNDRY] = -51.001f;	pBound[ X_END_BOUNDRY] = 52.241f;
		pBound[ Y_START_BOUNDRY] =  -0.001f;	pBound[ Y_END_BOUNDRY] = 15.001f; 
		pBound[ Z_START_BOUNDRY] = -71.001f;	pBound[ Z_END_BOUNDRY] = 60.001f;
		return;
	case STAGE_UID_METRO:
		pBound[ X_START_BOUNDRY] = -63.551f;	pBound[ X_END_BOUNDRY] = 70.471f;
		pBound[ Y_START_BOUNDRY] = 1.999f;		pBound[ Y_END_BOUNDRY] = 10.501f; 
		pBound[ Z_START_BOUNDRY] = -29.001f;	pBound[ Z_END_BOUNDRY] = 29.001f;
		return;
	case STAGE_UID_RUSHHOUR:
		pBound[ X_START_BOUNDRY] = -33.351f;	pBound[ X_END_BOUNDRY] = 32.501f;
		pBound[ Y_START_BOUNDRY] =  -0.501f;	pBound[ Y_END_BOUNDRY] = 10.001f; 
		pBound[ Z_START_BOUNDRY] = -47.501f;	pBound[ Z_END_BOUNDRY] = 46.501f;
		return;
	case STAGE_UID_CARGOPORT:
		pBound[ X_START_BOUNDRY] = -62.069f;	pBound[ X_END_BOUNDRY] = 62.001f;
		pBound[ Y_START_BOUNDRY] = -0.501f;		pBound[ Y_END_BOUNDRY] = 20.001f; 
		pBound[ Z_START_BOUNDRY] = -17.001f;	pBound[ Z_END_BOUNDRY] = 17.427f;
		return;
	case STAGE_UID_BLACKMAMBA:
		pBound[ X_START_BOUNDRY] = -43.616f;	pBound[ X_END_BOUNDRY] = 43.789f;
		pBound[ Y_START_BOUNDRY] = 1.832f;		pBound[ Y_END_BOUNDRY] = 21.001f; 
		pBound[ Z_START_BOUNDRY] = -31.421f;	pBound[ Z_END_BOUNDRY] = 29.828f;
		return;
	case STAGE_UID_SAFEHOUSE:
		pBound[ X_START_BOUNDRY] = -41.807f;	pBound[ X_END_BOUNDRY] = 41.888f;
		pBound[ Y_START_BOUNDRY] = -0.001f;		pBound[ Y_END_BOUNDRY] = 11.201f; 
		pBound[ Z_START_BOUNDRY] = -52.275f;	pBound[ Z_END_BOUNDRY] = 53.501f;
		return;
	case STAGE_UID_WESTSTATION:
		pBound[X_START_BOUNDRY] = -39.479f;		pBound[X_END_BOUNDRY] = 41.422f;
		pBound[Y_START_BOUNDRY] = 0.264f;		pBound[Y_END_BOUNDRY] = 7.901f;
		pBound[Z_START_BOUNDRY] = -28.021f;		pBound[Z_END_BOUNDRY] = 25.333f;
		return;
	case STAGE_UID_HARDROCK:
		pBound[X_START_BOUNDRY] = -51.319f;	pBound[X_END_BOUNDRY] = 51.349f;
		pBound[Y_START_BOUNDRY] = 0.499f;	pBound[Y_END_BOUNDRY] = 9.546f;
		pBound[Z_START_BOUNDRY] = -31.596f;	pBound[Z_END_BOUNDRY] = 31.572f;
		return;
	case STAGE_UID_HOUSEMUSEUM:
		pBound[X_START_BOUNDRY] = -23.001f;	pBound[X_END_BOUNDRY] = 23.771f;
		pBound[Y_START_BOUNDRY] = 0.261f;	pBound[Y_END_BOUNDRY] = 8.311f;
		pBound[Z_START_BOUNDRY] = -43.441f;	pBound[Z_END_BOUNDRY] = 42.711f;
		return;
	case STAGE_UID_HOTELCAMOUFLAGE:
		pBound[X_START_BOUNDRY] = -48.431f;	pBound[X_END_BOUNDRY] = 48.232f;
		pBound[Y_START_BOUNDRY] = 0.499f;	pBound[Y_END_BOUNDRY] = 17.634f;
		pBound[Z_START_BOUNDRY] = -23.711f;	pBound[Z_END_BOUNDRY] = 23.801f;
		return;
	case STAGE_UID_WATERCOMPLEX:
		pBound[X_START_BOUNDRY] = -47.091f;	pBound[X_END_BOUNDRY] = 46.601f;
		pBound[Y_START_BOUNDRY] = 0.051f;	pBound[Y_END_BOUNDRY] = 19.991f;
		pBound[Z_START_BOUNDRY] = -26.541f;	pBound[Z_END_BOUNDRY] = 29.671f;
		return;
	case STAGE_UID_HOLIDAY:
		pBound[X_START_BOUNDRY] = -39.781f;	pBound[X_END_BOUNDRY] = 38.727f;
		pBound[Y_START_BOUNDRY] = 1.083f;	pBound[Y_END_BOUNDRY] = 18.162f;
		pBound[Z_START_BOUNDRY] = -34.929f;	pBound[Z_END_BOUNDRY] = 37.232f;
		return;
	case STAGE_UID_PUMPKINHOLLOW:
		pBound[X_START_BOUNDRY] = -48.031f;	pBound[X_END_BOUNDRY] = 48.148f;
		pBound[Y_START_BOUNDRY] = -0.498f;	pBound[Y_END_BOUNDRY] = 6.001f;
		pBound[Z_START_BOUNDRY] = -24.371f;	pBound[Z_END_BOUNDRY] = 26.721f;
		return;
	case STAGE_UID_BATTLESHIP:
		pBound[X_START_BOUNDRY] = -76.181f;		pBound[X_END_BOUNDRY] = 129.871f;
		pBound[Y_START_BOUNDRY] = 6.413f;		pBound[Y_END_BOUNDRY] = 17.651f;
		pBound[Z_START_BOUNDRY] = -16.323f;		pBound[Z_END_BOUNDRY] = 16.323f;
		return;
	case STAGE_UID_BALLISTIC:
		pBound[X_START_BOUNDRY] = -40.221f;	pBound[X_END_BOUNDRY] = 43.351f;
		pBound[Y_START_BOUNDRY] = 0.499f;	pBound[Y_END_BOUNDRY] = 16.351f;
		pBound[Z_START_BOUNDRY] = -47.281f;	pBound[Z_END_BOUNDRY] = 47.876f;
		return;
	case STAGE_UID_RAMPARTTOWN:
		pBound[X_START_BOUNDRY] = -20.111f;	pBound[X_END_BOUNDRY] = 16.471f;
		pBound[Y_START_BOUNDRY] = 0.501f;	pBound[Y_END_BOUNDRY] = 13.733f;
		pBound[Z_START_BOUNDRY] = -48.457f;	pBound[Z_END_BOUNDRY] = 43.034f;
		return;
	case STAGE_UID_ROTHENVILLAGE:
		pBound[X_START_BOUNDRY] = -42.421f;	pBound[X_END_BOUNDRY] = 41.537f;
		pBound[Y_START_BOUNDRY] = 0.901f;	pBound[Y_END_BOUNDRY] = 13.311f;
		pBound[Z_START_BOUNDRY] = -35.685f;	pBound[Z_END_BOUNDRY] = 32.703f;
		return;
	case STAGE_UID_HINDRANCE:
		pBound[X_START_BOUNDRY] = -26.035f;	pBound[X_END_BOUNDRY] = 26.035f;
		pBound[Y_START_BOUNDRY] = -0.801f;	pBound[Y_END_BOUNDRY] = 7.308f;
		pBound[Z_START_BOUNDRY] = -44.991f;	pBound[Z_END_BOUNDRY] = 44.991f;
		return;
	case STAGE_UID_FALLUCITY:
		pBound[X_START_BOUNDRY] = -53.932f;	pBound[X_END_BOUNDRY] = 41.632f;
		pBound[Y_START_BOUNDRY] = -1.474f;	pBound[Y_END_BOUNDRY] = 18.001f;
		pBound[Z_START_BOUNDRY] = -48.796f;	pBound[Z_END_BOUNDRY] = 51.572f;
		return;
	case STAGE_UID_SLUMAREA:
		pBound[X_START_BOUNDRY] = -48.001f;	pBound[X_END_BOUNDRY] = 47.601f;
		pBound[Y_START_BOUNDRY] = -0.501f;	pBound[Y_END_BOUNDRY] = 8.001f;
		pBound[Z_START_BOUNDRY] = -25.001f;	pBound[Z_END_BOUNDRY] = 22.001f;
		return;
	case STAGE_UID_NEWLIBRARY:
		pBound[X_START_BOUNDRY] = -36.901f;	pBound[X_END_BOUNDRY] = 37.301f;
		pBound[Y_START_BOUNDRY] = -0.901f;	pBound[Y_END_BOUNDRY] = 15.201f;
		pBound[Z_START_BOUNDRY] = -17.501f;	pBound[Z_END_BOUNDRY] = 23.201f;
		return;
	case STAGE_UID_FATALZONE:
		pBound[X_START_BOUNDRY] = -45.501f;	pBound[X_END_BOUNDRY] = 45.501f;
		pBound[Y_START_BOUNDRY] = 1.899f;	pBound[Y_END_BOUNDRY] = 13.001f;
		pBound[Z_START_BOUNDRY] = -23.903f;	pBound[Z_END_BOUNDRY] = 18.071f;
		return;
	case STAGE_UID_MARINEWAVE:
		pBound[ X_START_BOUNDRY] = -37.341f;	pBound[ X_END_BOUNDRY] = 38.191f;
		pBound[ Y_START_BOUNDRY] = 4.501f;		pBound[ Y_END_BOUNDRY] = 14.371f;
		pBound[ Z_START_BOUNDRY] = -46.191f;	pBound[ Z_END_BOUNDRY] = 43.091f;
		return;
	case STAGE_UID_STILLRAID:
		pBound[X_START_BOUNDRY] = -46.501f;		pBound[X_END_BOUNDRY] = 35.001f;
		pBound[Y_START_BOUNDRY] = -3.001f;		pBound[Y_END_BOUNDRY] = 8.001f;
		pBound[Z_START_BOUNDRY] = -40.001f;		pBound[Z_END_BOUNDRY] = 63.001f;
		return;
	case STAGE_UID_OLDDOCK:
		pBound[X_START_BOUNDRY] = -38.001f;		pBound[X_END_BOUNDRY] = 39.001f;
		pBound[Y_START_BOUNDRY] = -0.501f;		pBound[Y_END_BOUNDRY] = 10.001f;
		pBound[Z_START_BOUNDRY] = -57.001f;		pBound[Z_END_BOUNDRY] = 47.501f;
		return;
	case STAGE_UID_BIOLAB:
		pBound[X_START_BOUNDRY] = -41.001f;		pBound[X_END_BOUNDRY] = 40.301f;
		pBound[Y_START_BOUNDRY] = -0.201f;		pBound[Y_END_BOUNDRY] = 5.601f;
		pBound[Z_START_BOUNDRY] = -27.501f;		pBound[Z_END_BOUNDRY] = 35.601f;
		return;
	case STAGE_UID_BROKENALLEY:
		pBound[X_START_BOUNDRY] = -65.001f;		pBound[X_END_BOUNDRY] = 57.001f;
		pBound[Y_START_BOUNDRY] = -0.201f;		pBound[Y_END_BOUNDRY] = 12.001f;
		pBound[Z_START_BOUNDRY] = -47.001f;		pBound[Z_END_BOUNDRY] = 55.001f;
		return;
	case STAGE_UID_BANKHALL:
		pBound[X_START_BOUNDRY] = -32.001f;		pBound[X_END_BOUNDRY] = 32.501f;
		pBound[Y_START_BOUNDRY] = -0.201f;		pBound[Y_END_BOUNDRY] = 8.001f;
		pBound[Z_START_BOUNDRY] = -42.861f;		pBound[Z_END_BOUNDRY] = 44.501f;
		return;
	case STAGE_UID_PROVENCE:
		pBound[X_START_BOUNDRY] = -55.001f;		pBound[X_END_BOUNDRY] = 55.001f;
		pBound[Y_START_BOUNDRY] = -9.001f;		pBound[Y_END_BOUNDRY] = 12.001f;
		pBound[Z_START_BOUNDRY] = -65.001f;		pBound[Z_END_BOUNDRY] = 65.001f;
		return;
	case STAGE_UID_MIHAWK:
		pBound[X_START_BOUNDRY] = -45.001f;		pBound[X_END_BOUNDRY] = 40.001f;
		pBound[Y_START_BOUNDRY] = -1.001f;		pBound[Y_END_BOUNDRY] = 10.001f;
		pBound[Z_START_BOUNDRY] = -52.001f;		pBound[Z_END_BOUNDRY] = 43.001f;
		return;
	case STAGE_UID_DESERTCANYON:
		pBound[X_START_BOUNDRY] = -65.001f;		pBound[X_END_BOUNDRY] = 65.001f;
		pBound[Y_START_BOUNDRY] = -35.001f;		pBound[Y_END_BOUNDRY] = 28.001f;
		pBound[Z_START_BOUNDRY] = -35.001f;		pBound[Z_END_BOUNDRY] = 35.001f;
		return;
	case STAGE_UID_S_TWOTOWERS:
		pBound[X_START_BOUNDRY] = -28.821f;		pBound[X_END_BOUNDRY] = 29.361f;
		pBound[Y_START_BOUNDRY] = -6.741f;		pBound[Y_END_BOUNDRY] = 40.001f;
		pBound[Z_START_BOUNDRY] = -41.251f;		pBound[Z_END_BOUNDRY] = 41.431f; 
		return;	
	case STAGE_UID_THAISTADIUM:
		pBound[X_START_BOUNDRY] = -37.331f;		pBound[X_END_BOUNDRY] = 35.301f;
		pBound[Y_START_BOUNDRY] = -0.571f;		pBound[Y_END_BOUNDRY] = 13.001f;
		pBound[Z_START_BOUNDRY] = -35.531f;		pBound[Z_END_BOUNDRY] = 35.801f;
		return;
	case STAGE_UID_M_BRIDGE:
		pBound[X_START_BOUNDRY] = -10.951f;		pBound[X_END_BOUNDRY] = 10.731f;
		pBound[Y_START_BOUNDRY] = -0.471f;		pBound[Y_END_BOUNDRY] = 8.021f;
		pBound[Z_START_BOUNDRY] = -34.581f;		pBound[Z_END_BOUNDRY] = 32.531f;
		return;
	default:
		pBound[ X_START_BOUNDRY] = -999.0f;		pBound[ X_END_BOUNDRY] = 999.0f;
		pBound[ Y_START_BOUNDRY] = -999.0f;		pBound[ Y_END_BOUNDRY] = 999.0f;
		pBound[ Z_START_BOUNDRY] = -999.0f;		pBound[ Z_END_BOUNDRY] = 999.0f;
		return;
	}
}

//-------------------------------------------
// private functions
//-------------------------------------------
void CHMS::_ResetHackData()
{
	i3mem::FillZero( m_aui8WallShotCount,			sizeof(m_aui8WallShotCount));	
	i3mem::FillZero( m_aui8SpeedHackCount,			sizeof(m_aui8SpeedHackCount));
	i3mem::FillZero( m_aui8RecallHackCount,			sizeof(m_aui8RecallHackCount));
	i3mem::FillZero( m_aui8RespawnPosHackCount,		sizeof(m_aui8RespawnPosHackCount));
	i3mem::FillZero( m_aui8RespawnTimeHackCount,	sizeof(m_aui8RespawnTimeHackCount));
	i3mem::FillZero( m_aaui16RemainBullets,			sizeof(m_aaui16RemainBullets ));
	//i3mem::FillZero( m_aaui16RemainDualBullets,	sizeof(m_aaui16RemainDualBullets));
}

void CHMS::_ResetHackData( UINT32 ui32SlotIdx )
{
	if( ui32SlotIdx < SLOT_MAX_COUNT )
	{
		m_aui8WallShotCount			[ ui32SlotIdx ] = 0;
		m_aui8SpeedHackCount		[ ui32SlotIdx ] = 0;
		m_aui8RecallHackCount		[ ui32SlotIdx ] = 0;
		m_aui8RespawnPosHackCount	[ ui32SlotIdx ] = 0;
		m_aui8RespawnTimeHackCount	[ ui32SlotIdx ]	= 0;
		return;
	}
	I3ASSERT(0);
}

//-------------------------------------------
// reset functions
//-------------------------------------------

void CHMS::ResetOnRoomCreate( UINT32 ui32StageID, UINT8 ui8AvailableWeapon  )
{
	I3ASSERT( ui32StageID != 0 );
	I3ASSERT( ui8AvailableWeapon != 0 );
	
	_ResetHackData();

	// set: room info
	m_ui32StageID				= ui32StageID;
	m_ui8Mode					= StageID::GetStageMode( ui32StageID );
	m_ui8AvailableWeapon		= ui8AvailableWeapon;
}

//난입
void CHMS::ResetOnEnter( UINT32 ui32SlotIdx )
{
	I3ASSERT( ui32SlotIdx < SLOT_MAX_COUNT );
	if( ui32SlotIdx < SLOT_MAX_COUNT ) 
	{
		_ResetHackData( ui32SlotIdx );
	}
}

void CHMS::ResetOnLeave( UINT32 ui32SlotIdx )
{
	I3ASSERT( ui32SlotIdx < SLOT_MAX_COUNT );
	if( ui32SlotIdx < SLOT_MAX_COUNT ) 
	{
		_ResetHackData( ui32SlotIdx );
	}
}

void g_InitMapBounds()
{
	NxReal Boundry[ MAX_BOUNDRY_NAME ];

	for(  UINT32 ui32StageUID = (UINT32)STAGE_UID_NONE ; ui32StageUID < STAGE_UID_MAX; ui32StageUID++ )
	{
		g_CommonGetMapBounds( static_cast<STAGE_UID>(ui32StageUID),	Boundry );
		g_StageBounds[ ui32StageUID ].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	}

	return; 
}

char* HACK_LOG::MakeLogString( CDediRoom*  pRoom, UINT32 ui32SlotIdx, UINT8 ui8HackType )
{
	INT32 i32Len = 0;
	
	if( m_i32AddInfoLen > 0 )
	{
		i32Len = sprintf_s( m_szOutputBuffer, sizeof( m_szOutputBuffer ), 
										"HMS:UID:%I64u,S:%d,G:%d,R:%d,U:%d,C:%d,%s\n", 
										pRoom->GetUID( ui32SlotIdx ), 
										pRoom->GetStageUID(), 
										pRoom->GetGroupIndex(),
										pRoom->GetRoundNumber(),
										ui32SlotIdx,
										ui8HackType, 
										m_szAddInfo
						);

	}
	else
	{
		i32Len = sprintf_s( m_szOutputBuffer, sizeof( m_szOutputBuffer ), 
										"HMS:UID:%I64u,S:%d,G:%d,R:%d,U:%d,C:%d\n", 
										pRoom->GetUID( ui32SlotIdx ), 
										pRoom->GetStageUID(), 
										pRoom->GetGroupIndex(),
										pRoom->GetRoundNumber(),
										ui32SlotIdx,
										ui8HackType 
						);
	}

	if( i32Len == sizeof( m_szOutputBuffer ) ) 
	{
		m_szOutputBuffer[ sizeof(m_szOutputBuffer) - 2 ] = '\n';
		m_szOutputBuffer[ sizeof(m_szOutputBuffer) - 1 ] = '\0';
	}
	
	if( i32Len > 0 ) 
		return m_szOutputBuffer;
	else
		return NULL;
}