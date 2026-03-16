#pragma once

/************************************************************************/
/*							GameCharaBase.h		                        */
/************************************************************************/
//
//void AIP_Getter_CharaBase_StateKey(void* unobfdst, const void* obfdst , size_t sz);
//void AIP_Setter_CharaBase_StateKey(void* obfdst, const void* unobfsrc , size_t sz);

/************************************************************************/
/*                        GameCharaBoneContext.h                        */
/************************************************************************/

//void AIP_Getter_CharaBoneContext_Rho(void* unobfdst, const void* obfdst , size_t sz);
//void AIP_Setter_CharaBoneContext_Rho(void* obfdst, const void* unobfsrc , size_t sz);
//
//void AIP_Getter_CharaBoneContext_Theta(void* unobfdst, const void* obfdst , size_t sz);
//void AIP_Setter_CharaBoneContext_Theta(void* obfdst, const void* unobfsrc , size_t sz);

/************************************************************************/
/*                            GameP2PDef.h                              */
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
//							GAMEINFO_HIT								//
//////////////////////////////////////////////////////////////////////////

void AIP_Getter_HITINFO_FirePos(void* unobfdst, const void* obfdst , size_t sz);
void AIP_Setter_HITINFO_FirePos(void* obfdst, const void* unobfsrc , size_t sz);

void AIP_Getter_HITINFO_HitPos(void* unobfdst, const void* obfdst , size_t sz);
void AIP_Setter_HITINFO_HitPos(void* obfdst, const void* unobfsrc , size_t sz);

void AIP_Getter_THROW_Pos(void* unobfdst, const void* obfdst , size_t sz);
void AIP_Setter_THROW_Pos(void* obfdst, const void* unobfsrc , size_t sz);

void AIP_Getter_THROW_Dir(void* unobfdst, const void* obfdst , size_t sz);
void AIP_Setter_THROW_Dir(void* obfdst, const void* unobfsrc , size_t sz);

void AIP_Getter_THROW_State(void* unobfdst, const void* obfdst , size_t sz);
void AIP_Setter_THROW_State(void* obfdst, const void* unobfsrc , size_t sz);

void AIP_Getter_THROW_BoundType(void* unobfdst, const void* obfdst , size_t sz);
void AIP_Setter_THROW_BoundType(void* obfdst, const void* unobfsrc , size_t sz);

void AIP_Setter_THROW_ObjectPos(void* obfdst, const void* unobfsrc, size_t sz);
void AIP_Getter_THROW_ObjectPos(void* unobfdst, const void* obfdst, size_t sz);

//////////////////////////////////////////////////////////////////////////
//							GAMEINFO_MISSION							//
//////////////////////////////////////////////////////////////////////////

//void AIP_Getter_MISSION_State(void* unobfdst, const void* obfdst , size_t sz);
//void AIP_Setter_MISSION_State(void* obfdst, const void* unobfsrc , size_t sz);
//
//void AIP_Getter_MISSION_Area(void* unobfdst, const void* obfdst , size_t sz);
//void AIP_Setter_MISSION_Area(void* obfdst, const void* unobfsrc , size_t sz);
//
//void AIP_Getter_MISSION_StartTime(void* unobfdst, const void* obfdst , size_t sz);
//void AIP_Setter_MISSION_StartTime(void* obfdst, const void* unobfsrc , size_t sz);


//////////////////////////////////////////////////////////////////////////
//							CHARA_DAMAGEINFO							//
//////////////////////////////////////////////////////////////////////////

void AIP_Getter_CHARA_DAMAGEINFO_HitPart(void* unobfdst, const void* obfdst , size_t sz);
void AIP_Setter_CHARA_DAMAGEINFO_HitPart(void* obfdst, const void* unobfsrc , size_t sz);