#include "pch.h"
#include "SkillSystemInfo.h"

//----------------------------------------------------------------------------------//
//									SKILL SYSTEM Interface							//
void SKILLSYSTEM_APPLY_INFO::Reset()
{
	setSkillType( SKILLLIST_UNKNOWN);
	setWeaponType( SKILL_WEAPONTYPE_UNKNOWN);
	setWeaponMoveSpeed(0.f);
	setWeaponFireSpeed(0.f);
	setWeaponReloadTime(0.f);
	setWeaponHorizontalReaction(0.f);
	setWeaponVerticalReaction(0.f);
	setWeaponDeclination(0.f);
	setWeaponAttackRange(0.f);
	setWeaponDamage(0.f);
	setWeaponScoopeRange(0.f);
	setWeaponCrossHairRestore(0.f);
	setWeaponBombTime(0.f);
	setWeaponBombRange(0.f);
	setWeaponBombLength(0.f);
	setWeaponChange(0.f);
	setWeaponPenetrate(0.f);
	setWeaponReaction(0.f);
	
	setCharaWalkSpeed(0.f);
	setCharaMoveSpeed(0.f);
	setCharaAddSpeed(0.f);
	setCharaJump(0.f);
	setCharaFallDown(0.f);
	setCharaDefense(0.f);
	setCharaHelmetDefense(0.f);
	setFirstShotRate(0.f);
	setDPDRate(0.f);
}

INT32 SKILLSYSTEM_APPLY_INFO::getCategory( void) const
{
	return _iCategoryIdx;
}

void SKILLSYSTEM_APPLY_INFO::setCategory( INT32 idx)
{
	_iCategoryIdx = idx;
}

INT32 SKILLSYSTEM_APPLY_INFO::getIndex( void) const
{
	return _iSkillIdx;
}

void SKILLSYSTEM_APPLY_INFO::setIndex( INT32 idx)
{
	_iSkillIdx = idx;
}

SKILL_TYPE SKILLSYSTEM_APPLY_INFO::getSkillType( void) const
{
	return (SKILL_TYPE)_iSkillType;
}

void SKILLSYSTEM_APPLY_INFO::getSkillTypeStr(char* strtemp) const
{
	switch( getSkillType())
	{
	case SKILLLIST_RUN:
		i3::safe_string_copy( strtemp ,"RUN", MAX_STRING_COUNT);
		break;
	case SKILLLIST_WALK:
		i3::safe_string_copy( strtemp ,"WALK", MAX_STRING_COUNT);
		break;
	case SKILLLIST_DEFENSE:
		i3::safe_string_copy( strtemp ,"DEFENSE", MAX_STRING_COUNT);
		break;
	case SKILLLIST_HELMETDEFENSE:
		i3::safe_string_copy( strtemp ,"HELMETDEFENSE", MAX_STRING_COUNT);
		break;
	case SKILLLIST_JUMP:
		i3::safe_string_copy( strtemp ,"JUMP", MAX_STRING_COUNT);
		break;
	case SKILLLIST_FIRESPEED:
		i3::safe_string_copy( strtemp ,"FIRESPEED", MAX_STRING_COUNT);
		break;
	case SKILLLIST_DAMAGE:
		i3::safe_string_copy( strtemp ,"DAMAGE", MAX_STRING_COUNT);
		break;
	case SKILLLIST_ACCURACY:
		i3::safe_string_copy( strtemp ,"ACCURACY", MAX_STRING_COUNT);
		break;
	case SKILLLIST_RANGE:
		i3::safe_string_copy( strtemp ,"RANGE", MAX_STRING_COUNT);
		break;
	case SKILLLIST_PENETRATE:
		i3::safe_string_copy( strtemp ,"PENETRATE", MAX_STRING_COUNT);
		break;
	case SKILLLIST_RELOAD:
		i3::safe_string_copy( strtemp ,"RELOAD", MAX_STRING_COUNT);
		break;
	case SKILLLIST_CHANGE:
		i3::safe_string_copy( strtemp ,"CHANGE", MAX_STRING_COUNT);
		break;
	case SKILLLIST_BOOMRANGE:
		i3::safe_string_copy( strtemp ,"BOOMRANGE", MAX_STRING_COUNT);
		break;
	case SKILLLIST_BOOMLENGTH:
		i3::safe_string_copy( strtemp ,"BOOMLENGTH", MAX_STRING_COUNT);
		break;
	case SKILLLIST_BOOMTIME:
		i3::safe_string_copy( strtemp ,"BOOMTIME", MAX_STRING_COUNT);
		break;
	default:
		i3::safe_string_copy( strtemp ,"NUNOWN", MAX_STRING_COUNT);
	}
}

void SKILLSYSTEM_APPLY_INFO::setSkillType( SKILL_TYPE idx)
{
	_iSkillType = idx;
}

INT32 SKILLSYSTEM_APPLY_INFO::getLevel( void) const
{
	return _iSkillLevel;
}

void SKILLSYSTEM_APPLY_INFO::setLevel( INT32 idx)
{
	_iSkillLevel = idx;
}

bool SKILLSYSTEM_APPLY_INFO::getActivate( void) const
{
	return _bActivate;
}

void SKILLSYSTEM_APPLY_INFO::seActivate( bool idx)
{
	_bActivate = (bool)idx;
}

void SKILLSYSTEM_APPLY_INFO::setSkillData( INT32 _category, INT32 _idx, INT32 _level)
{
	if(_level < 0)
	{
		setCategory(-1);
		setIndex(-1);
		setLevel(0);
		seActivate(false);
	}
	else if(_level <= 10)
	{
		setCategory(_category);
		setIndex(_idx);
		setLevel(_level);
		seActivate(true);
	}
}

REAL32 SKILLSYSTEM_APPLY_INFO::getSkillValue(void) const
{
	return getDataValue( getSkillType());
}

REAL32 SKILLSYSTEM_APPLY_INFO::getSkillValue( WEAPON_CLASS_TYPE weapontype, SKILL_TYPE skilltype) const
{
	REAL32 _value = 0.f;
	if (( isWeaponType(weapontype)) && (skilltype == getSkillType()))
		_value = getDataValue(skilltype);

	return _value;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getSkillValue(  SKILL_TYPE skilltype) const
{
	REAL32 _value = 0.f;
	if(skilltype == getSkillType())
		_value = getDataValue(skilltype);

	return _value;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getDataValue(SKILL_TYPE skilltype) const
{
	REAL32 _value = 0.f;

	switch(skilltype)
	{
	case SKILLLIST_RUN:				_value = getCharaMoveSpeed();		break;
	case SKILLLIST_WALK:			_value = getCharaWalkSpeed();		break;
	case SKILLLIST_DEFENSE:			_value = getCharaDefense();			break;
	case SKILLLIST_HELMETDEFENSE:	_value = getCharaHelmetDefense();	break;
	case SKILLLIST_JUMP:			_value = getCharaJump();			break;
	case SKILLLIST_FIRESPEED:		_value = getWeaponFireSpeed();		break;
	case SKILLLIST_DAMAGE:			_value = getWeaponDamage();			break;
	case SKILLLIST_ACCURACY:		_value = getWeaponDeclination();	break;
	case SKILLLIST_RANGE:			_value = getWeaponAttackRange();	break;
	case SKILLLIST_PENETRATE:		_value = getWeaponPenetrate();		break;
	case SKILLLIST_RELOAD:			_value = getWeaponReloadTime();		break;
	case SKILLLIST_CHANGE:			_value = getWeaponChange();			break;
	case SKILLLIST_BOOMRANGE:		_value = getWeaponBombRange();		break;
	case SKILLLIST_BOOMLENGTH:		_value = getWeaponBombLength();		break;
	case SKILLLIST_BOOMTIME:		_value = getWeaponBombTime();		break;
	}
	return _value;
}

SKILL_WEAPONTYPE SKILLSYSTEM_APPLY_INFO::getSkillWeaponType( void)
{
	return (SKILL_WEAPONTYPE) _iWeaponType;
}

bool SKILLSYSTEM_APPLY_INFO::isWeaponType( WEAPON_CLASS_TYPE _weapon) const
{
	SKILL_WEAPONTYPE tempskillweapon = SKILL_WEAPONTYPE_UNKNOWN;

	switch(_weapon)
	{
	case WEAPON_CLASS_KNIFE:
	case WEAPON_CLASS_DUALKNIFE:
	case WEAPON_CLASS_KNUCKLE:
		tempskillweapon = SKILL_WEAPONTYPE_KNIFE;
		break;
	case WEAPON_CLASS_HANDGUN:
	case WEAPON_CLASS_DUALHANDGUN:
		tempskillweapon = SKILL_WEAPONTYPE_HANDGUN;
		break;
	case WEAPON_SUBCLASS_ASSAULT:
	case WEAPON_CLASS_ASSAULT:
		tempskillweapon = SKILL_WEAPONTYPE_ASSAULT;
		break;
	case WEAPON_SUBCLASS_SMG:
	case WEAPON_CLASS_SMG:
	case WEAPON_CLASS_DUALSMG:
		tempskillweapon = SKILL_WEAPONTYPE_SMG;
		break;
	case WEAPON_SUBCLASS_SNIPER:
	case WEAPON_CLASS_SNIPER:
		tempskillweapon = SKILL_WEAPONTYPE_RIFLE;
		break;
	case WEAPON_SUBCLASS_SHOTGUN:
	case WEAPON_CLASS_SHOTGUN:
	case WEAPON_CLASS_DUALSHOTGUN :
		tempskillweapon = SKILL_WEAPONTYPE_SHOTGUN;
		break;
	case WEAPON_CLASS_THROWING_GRENADE:
		tempskillweapon = SKILL_WEAPONTYPE_THROWING;
		break;
	case WEAPON_CLASS_MG:
		tempskillweapon = SKILL_WEAPONTYPE_MG;
		break;
	default:
		tempskillweapon = SKILL_WEAPONTYPE_UNKNOWN;
	}

	return (_iWeaponType == tempskillweapon);
}

void SKILLSYSTEM_APPLY_INFO::getWeaponTypeStr( char* strtemp) const
{
	switch(_iWeaponType)
	{
	case SKILL_WEAPONTYPE_KNIFE:
		i3::safe_string_copy( strtemp, "KNIFE", MAX_STRING_COUNT);
		break;
	case SKILL_WEAPONTYPE_HANDGUN:
		i3::safe_string_copy( strtemp, "HANDGUN", MAX_STRING_COUNT);
		break;
	case SKILL_WEAPONTYPE_ASSAULT:
		i3::safe_string_copy( strtemp, "ASSAULT", MAX_STRING_COUNT);
		break;
	case SKILL_WEAPONTYPE_SMG:
		i3::safe_string_copy( strtemp, "SMG", MAX_STRING_COUNT);
		break;
	case SKILL_WEAPONTYPE_RIFLE:
		i3::safe_string_copy( strtemp, "SNIPER", MAX_STRING_COUNT);
		break;
	case SKILL_WEAPONTYPE_SHOTGUN:
		i3::safe_string_copy( strtemp, "SHOTGUN", MAX_STRING_COUNT);
		break;
	case SKILL_WEAPONTYPE_THROWING:
		i3::safe_string_copy( strtemp, "THROWING", MAX_STRING_COUNT);
		break;
	case SKILL_WEAPONTYPE_MG:
		i3::safe_string_copy( strtemp, "MG", MAX_STRING_COUNT);
		break;
	default:
		i3::safe_string_copy( strtemp, "UNNOWN", MAX_STRING_COUNT);
	}
}

void SKILLSYSTEM_APPLY_INFO::setWeaponType( SKILL_WEAPONTYPE idx)
{
	_iWeaponType = idx;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getWeaponMoveSpeed( void) const
{
	return _fWeaponMoveSpeed;
}

void SKILLSYSTEM_APPLY_INFO::setWeaponMoveSpeed( REAL32 rVal)
{
	_fWeaponMoveSpeed = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getWeaponFireSpeed( void) const
{
	return _fWeaponFireSpeed;
}

void SKILLSYSTEM_APPLY_INFO::setWeaponFireSpeed( REAL32 rVal)
{
	_fWeaponFireSpeed = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getWeaponReloadTime( void) const
{
	return _fWeaponReloadTime;
}

void SKILLSYSTEM_APPLY_INFO::setWeaponReloadTime( REAL32 rVal)
{
	_fWeaponReloadTime = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getWeaponHorizontalReaction( void) const
{
	return _fWeaponHorizontalReaction;
}

void SKILLSYSTEM_APPLY_INFO::setWeaponHorizontalReaction( REAL32 rVal)
{
	_fWeaponHorizontalReaction = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getWeaponVerticalReaction( void) const
{
	return _fWeaponVerticalReaction;
}

void SKILLSYSTEM_APPLY_INFO::setWeaponVerticalReaction( REAL32 rVal)
{
	_fWeaponVerticalReaction = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getWeaponDeclination( void) const
{
	return _fWeaponDeclination;
}

void SKILLSYSTEM_APPLY_INFO::setWeaponDeclination( REAL32 rVal)
{
	_fWeaponDeclination = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getWeaponAttackRange( void) const
{
	return _fWeaponAttackRange;
}

void SKILLSYSTEM_APPLY_INFO::setWeaponAttackRange( REAL32 rVal)
{
	_fWeaponAttackRange = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getWeaponDamage( void) const
{
	return _fWeaponDamage;
}

void SKILLSYSTEM_APPLY_INFO::setWeaponDamage( REAL32 rVal)
{
	_fWeaponDamage = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getWeaponScoopeRange( void) const
{
	return _fWeaponScoopeRange;
}

void SKILLSYSTEM_APPLY_INFO::setWeaponScoopeRange( REAL32 rVal)
{
	_fWeaponScoopeRange = rVal;
}


REAL32 SKILLSYSTEM_APPLY_INFO::getWeaponCrossHairRestore( void) const
{
	return _fWeaponCrossHairRestore;
}

void SKILLSYSTEM_APPLY_INFO::setWeaponCrossHairRestore( REAL32 rVal)
{
	_fWeaponCrossHairRestore = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getWeaponBombTime( void) const
{
	return _fWeaponBombTime;
}

void SKILLSYSTEM_APPLY_INFO::setWeaponBombTime( REAL32 rVal)
{
	_fWeaponBombTime = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getWeaponBombRange( void) const
{
	return _fWeaponBombRange;
}

void SKILLSYSTEM_APPLY_INFO::setWeaponBombRange( REAL32 rVal)
{
	_fWeaponBombRange = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getWeaponChange( void) const
{
	return _fWeaponChange;
}

void SKILLSYSTEM_APPLY_INFO::setWeaponChange( REAL32 rVal)
{
	_fWeaponChange = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getWeaponPenetrate( void) const
{
	return _fWeaponPenetrate;
}

void SKILLSYSTEM_APPLY_INFO::setWeaponPenetrate( REAL32 rVal)
{
	_fWeaponPenetrate = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getWeaponBombLength( void) const
{
	return _fWeaponBombLength;
}

void SKILLSYSTEM_APPLY_INFO::setWeaponBombLength( REAL32 rVal)
{
	_fWeaponBombLength = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getCharaMoveSpeed( void) const
{
	return _fCharaMoveSpeed;
}

void SKILLSYSTEM_APPLY_INFO::setCharaMoveSpeed( REAL32 rVal)
{
	_fCharaMoveSpeed = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getCharaWalkSpeed( void) const
{
	return _fCharaWalkSpeed;
}

void SKILLSYSTEM_APPLY_INFO::setCharaWalkSpeed( REAL32 rVal)
{
	_fCharaWalkSpeed = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getCharaAddSpeed( void) const
{
	return _fCharaAddSpeed;
}

void SKILLSYSTEM_APPLY_INFO::setCharaAddSpeed( REAL32 rVal)
{
	_fCharaAddSpeed = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getCharaJump( void) const
{
	return _fCharaJump;
}

void SKILLSYSTEM_APPLY_INFO::setCharaJump( REAL32 rVal)
{
	_fCharaJump = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getCharaDefense( void) const
{
	return _fCharaDefense;
}

void SKILLSYSTEM_APPLY_INFO::setCharaDefense( REAL32 rVal)
{
	_fCharaDefense = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getCharaHelmetDefense( void) const
{
	return _fCharaHelmetDefense;
}

void SKILLSYSTEM_APPLY_INFO::setCharaHelmetDefense( REAL32 rVal)
{
	_fCharaHelmetDefense = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getCharaFallDown( void) const
{
	return _fCharaFallDown;
}

void SKILLSYSTEM_APPLY_INFO::setCharaFallDown( REAL32 rVal)
{
	_fCharaFallDown = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getWeaponReaction( void) const
{
	return _fWeaponReaction;
}

void SKILLSYSTEM_APPLY_INFO::setWeaponReaction( REAL32 rVal)
{
	_fWeaponReaction = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getFirstShotRate( void) const
{
	return _fFirstShot_Rate;
}

void SKILLSYSTEM_APPLY_INFO::setFirstShotRate( REAL32 rVal)
{
	 _fFirstShot_Rate = rVal;
}

REAL32 SKILLSYSTEM_APPLY_INFO::getDPDRate( void) const
{
	return _fDPD_Rate;
}

void SKILLSYSTEM_APPLY_INFO::setDPDRate( REAL32 rVal)
{
	_fDPD_Rate = rVal;
}
