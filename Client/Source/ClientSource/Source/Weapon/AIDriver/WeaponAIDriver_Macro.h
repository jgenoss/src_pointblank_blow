#pragma once

#include "WeaponBase.h"

#define  AIPROC_WEAPON_IDLE_RUN( ctx, tm)		{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
													I3ASSERT( pWeapon != nullptr);		\
													pWeapon->OnIdle( ctx, tm); }

#define  AIPROC_WEAPON_IDLE_FIN( ctx, tm)		{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
													I3ASSERT( pWeapon != nullptr);		\
													pWeapon->OnEndIdle( ctx, tm); }

#define  AIPROC_WEAPON_LOAD_BULLET_RUN( ctx, tm)	{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
														I3ASSERT( pWeapon != nullptr);		\
														pWeapon->OnLoadBullet( ctx, tm); }

#define  AIPROC_WEAPON_LOAD_BULLET_FIN( ctx, tm)	{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
														I3ASSERT( pWeapon != nullptr);		\
														pWeapon->OnEndLoadBullet( ctx, tm); }

#define  AIPROC_WEAPON_LOAD_MAGAZINE_RUN( ctx, tm)	{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
															I3ASSERT( pWeapon != nullptr);		\
															pWeapon->OnLoadMag( ctx, tm); }

#define  AIPROC_WEAPON_LOAD_MAGAZINE_FIN( ctx, tm)	{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
															I3ASSERT( pWeapon != nullptr);		\
															pWeapon->OnEndLoadMag( ctx, tm); }

#define  AIPROC_WEAPON_FIRE_RUN( ctx, tm)	{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
														I3ASSERT( pWeapon != nullptr);		\
														pWeapon->OnFire( ctx, tm); }

#define  AIPROC_WEAPON_FIRE_FIN( ctx, tm)	{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
														I3ASSERT( pWeapon != nullptr);		\
														pWeapon->OnEndFire( ctx, tm); }

#define  AIPROC_WEAPON_EXTENSION_ACTIVATE_RUN( ctx, tm)	{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
																I3ASSERT( pWeapon != nullptr);		\
																pWeapon->OnExtensionActivate( ctx, tm); }

#define  AIPROC_WEAPON_EXTENSION_DEACTIVATE_RUN( ctx, tm)	{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
																I3ASSERT( pWeapon != nullptr);		\
																pWeapon->OnExtensionDeactivate( ctx, tm); }

#define AIPROC_WEAPON_QUICK_LOAD_MAGAZINE_RUN( ctx, tm )	{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
																I3ASSERT( pWeapon != nullptr );	\
																pWeapon->OnQuickLoadMag( ctx, tm ); }

#define AIPROC_WEAPON_QUICK_LOAD_MAGAZINE_FIN( ctx, tm )	{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
																I3ASSERT( pWeapon != nullptr ); \
																pWeapon->OnEndQuickLoadMag( ctx, tm ); }

#define AIPROC_WEAPON_FIRE_SHELL_RUN( ctx, tm)			{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
																I3ASSERT( pWeapon != nullptr); \
																pWeapon->OnShellFire( ctx, tm); }

#define AIPROC_WEAPON_FIRE_SHELL_FIN( ctx, tm)			{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
																I3ASSERT( pWeapon != nullptr); \
																pWeapon->OnEndShellFire( ctx, tm); }

#define AIPROC_WEAPON_RELOAD_SHELL_RUN( ctx, tm)			{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
																I3ASSERT( pWeapon != nullptr); \
																pWeapon->OnShellReload( ctx, tm); }

#define AIPROC_WEAPON_RELOAD_SHELL_FIN( ctx, tm)			{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
																I3ASSERT( pWeapon != nullptr); \
																pWeapon->OnEndShellReload( ctx, tm); }

#define AIPROC_WEAPON_REMOVE_SHELL_RUN( ctx, tm)			{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
																I3ASSERT( pWeapon != nullptr); \
																pWeapon->OnShellRemove( ctx, tm); }

#define  AIPROC_WEAPON_LOAD_MAGAZINE_READY_RUN( ctx, tm)	{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
																I3ASSERT( pWeapon != nullptr);		\
																pWeapon->OnLoadMag_Ready( ctx, tm); }

#define  AIPROC_WEAPON_FIRE_BULLET_RUN( ctx, tm)			{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
																I3ASSERT( pWeapon != nullptr);		\
																pWeapon->OnFireBullet( ctx, tm); }

#define  AIPROC_WEAPON_FIRE_BULLET_FIN( ctx, tm)			{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
																I3ASSERT( pWeapon != nullptr);		\
																pWeapon->OnEndFireBullet( ctx, tm); }

#define  AIPROC_WEAPON_SWAP_RUN( ctx, tm)					{ WeaponBase * pWeapon = (WeaponBase *) ctx->getOwner(); \
																I3ASSERT( pWeapon != nullptr);		\
																pWeapon->OnSwaping( ctx, tm); }