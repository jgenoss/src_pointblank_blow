# Plan de Implementación - Piercing Blow Server Remake

> **Última actualización**: 2026-03-17 (Phase 10-12 + BattleServer systems: HMSParser, WeaponSystem, GameCharacter, UDPChecker, ServerStatistics, TaskProcessor, Modules)

---

## BUG CRÍTICO: Fechas > 2015 crashean el cliente

### Estado: PENDIENTE (documentado, no se arregla aún)

### Root Cause

Hay **3 problemas de fechas** que se combinan para causar el crash:

#### Problema 1: DATE32.GetDateTimeYYMMDDHHMI() está HARDCODED a 2015
**Archivo**: `Piercing Blow Remake/i3Server/src/i3Base/DATE32.cpp:107-109`
```cpp
UINT32 DATE32::GetDateTimeYYMMDDHHMI()
{
    return (2015 % 100) * 100000000 + 1 * 1000000 + 1 * 10000 + 0 * 100 + 0;  // 1501010000
}
```
El código original (comentado debajo) era:
```cpp
return (GetYear() % 100) * 100000000 + GetMonth() * 1000000 + GetDay() * 10000 + GetHour() * 100 + GetMinute();
```
**Impacto**: El servidor SIEMPRE envía `1501010000` (2015-01-01 00:00) como InvenServerTime.

#### Problema 2: YYMMDDHHmm overflow en INT32 para año >= 2022
**Archivo**: `Client/Source/ClientSource/Source/TimeUtil.cpp:130`
```cpp
UINT32 CalcTimeDifference(INT32 tmAbs0, INT32 tmAbs1)  // <-- INT32 params!
```
El formato YYMMDDHHmm para año 2022+:
| Año  | Valor YYMMDDHHmm | > INT32_MAX (2,147,483,647)? |
|------|------------------|------------------------------|
| 2015 | 1,512,312,359    | No - OK                      |
| 2021 | 2,112,312,359    | No - OK                      |
| 2022 | 2,212,312,359    | **SÍ - OVERFLOW**            |
| 2026 | 2,612,312,359    | **SÍ - Se convierte en -1,682,654,937** |

Cuando el valor overflow INT32, `CalcTimeDifference` calcula diferencias negativas → crash.

#### Problema 3: DATE32 bit-packed soporta hasta año 2033
**Archivo**: `Piercing Blow Remake/i3Server/include/i3Base/DATE32.h:9-10`
```cpp
#define DATE32_YEAR_INIT  1970
#define DATE32_YEAR_MAX   (DATE32_YEAR_INIT + 63)  // = 2033
```
El campo Year tiene **6 bits** (max 63). Año 2034+ corrompe los otros campos (month, day, hour).

### Flujo del Bug

```
Servidor envía PROTOCOL_INVENTORY_ENTER_ACK
  → original: i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() = fecha real
  → remake:   DATE32 hardcoded → siempre 1501010000 (2015-01-01)

Cliente recibe en ClientTCPSocket_Parser_Inventory.cpp:89:
  → UserInfoContext::i()->SetInvenServerTime(nReturnCode);
  → m_InvenServerTime = 1501010000  (ó valor real si se descomenta)

Items con periodo (tipo 2) tienen _ui32ItemArg = ExpireTime en formato YYMMDDHHmm

Cliente calcula tiempo restante en UserInfoContext.cpp:139:
  → TimeUtil::CalcTimeDifference(m_InvenServerTime, ExpireTime)
  → Si ExpireTime >= 2022 → INT32 overflow → resultado negativo → crash

También en InvenList.cpp:850:
  → nCouponLimitTime - nCurServerTime <= 0 (ambos INT32)
  → Si cualquiera overflow → comparación incorrecta
```

### Archivos Afectados

| Archivo | Línea | Problema |
|---------|-------|----------|
| `i3Server/src/i3Base/DATE32.cpp` | 107-109 | GetDateTimeYYMMDDHHMI() hardcoded 2015 |
| `i3Server/src/i3Base/DATE32.cpp` | 102-105 | GetDateTimeYYYYMMDD() hardcoded 2015 |
| `i3Server/include/i3Base/DATE32.h` | 10 | DATE32_YEAR_MAX = 2033 (6 bits) |
| `Client/Source/ClientSource/Source/TimeUtil.cpp` | 130 | CalcTimeDifference usa INT32 |
| `Client/Source/ClientSource/Source/TimeUtil.h` | 6 | Firma usa INT32 |
| `Client/Source/ClientSource/Source/InvenList.cpp` | 842,850 | INT32 comparación |
| `Common/CommonSource/SIA_Define.h` | 397 | `T_ItemArg _ui32ItemArg` (UINT32 para expire) |
| Todos los `S2MOStruct*.h` | varios | `m_ui32InvenTime` (UINT32 en protocolo) |

### Solución Propuesta (NO implementar aún)

**Opción A: Mantener año < 2022 (workaround simple)**
- Enviar fechas del servidor con año fijo (ej: 2015-2020)
- Pro: No modifica cliente
- Con: Items solo pueden expirar hasta ~2021

**Opción B: Cambiar CalcTimeDifference a UINT32 (fix cliente)**
- Cambiar `INT32 tmAbs0, INT32 tmAbs1` → `UINT32 tmAbs0, UINT32 tmAbs1`
- Extender rango hasta año 2042 (UINT32 max = 4,294,967,295 → YY=42)
- Pro: Extiende soporte 20 años más
- Con: Requiere recompilar cliente

**Opción C: Cambiar formato a epoch seconds (fix completo)**
- Usar Unix timestamps (UINT32) en lugar de YYMMDDHHmm
- Pro: Soporte hasta 2038 (o 2106 con uint32 unsigned)
- Con: Requiere cambiar cliente Y servidor, rompe compatibilidad

**Opción D: DATE32 con más bits para año**
- Expandir campo Year de 6 a 7+ bits, o cambiar base epoch
- Pro: Fix para DATE32 bit-packed
- Con: Rompe formato binario existente

**Recomendación**: Opción B (cambiar cliente a UINT32) + descomentar DATE32 en servidor.

---

## Estado General del Proyecto

| Servidor | Archivos | Líneas | Estado |
|----------|----------|--------|--------|
| **GameServer** | 58 | ~31,900 | Activo - 443 dispatch + MedalLoader |
| **DataServer** | 22 | 3,372 | Funcional - PostgreSQL sync |
| **BattleServer** | 57 | ~6,800 | **Full: Physics, Collision, GameObjects, WeaponSystem, HMSParser, UDPChecker, Character, Modules** |
| **ConnectServer** | 10 | 1,068 | Funcional - Auth + registry |
| **ServerCommon** | 12 | 2,054 | Completo - InterServerProtocol |
| **S2MO** | - | - | Completo - RSA/AES/XOR |
| **i3Server** | - | - | Completo - IOCP framework |
| **TOTAL** | **135+** | **~42,200** | |

---

## Análisis: Original vs Remake

### Escala de Código

| Componente | Original | Remake | % Cubierto |
|------------|----------|--------|------------|
| GameServer (UserSession + Room + Context + Modules) | ~30,000 líneas | ~31,900 líneas | ~106%* |
| BattleServer (DediServer) | ~40,139 líneas, 82 archivos | ~6,800 líneas, 57 archivos | **16.9%** |
| DataServer (TransServer) | ~29,931 líneas, 90+ archivos | 3,372 líneas, 22 archivos | **11.3%** |
| **Total servidor** | **~100,070 líneas** | **~42,072 líneas** | **~42%** |

\* El GameServer del remake tiene más líneas que el original porque incluye lógica que en el original se distribuía entre Auth/Clan/Messenger servers.

### Cobertura de Protocolos (Precisa)

**578 REQ protocols definidos → 443 implementados → 135 faltantes**

De los 135 faltantes:
- 20 son deprecated (사용안함) → ignorar
- **115 son server-to-server internos** (no llegan del cliente)
- **~10 son client-facing verdaderos** que faltan

| Categoría Faltante | Count | Tipo | Prioridad |
|---------------------|-------|------|-----------|
| ASC_ADMIN (0x0500) | 25 | Control Server → GameServer | Baja (admin tool) |
| GC_INTERNAL | 22 | GameServer → ClanServer | N/A (interno) |
| BATTLE_SERVER (0x0900) | 17 | GameServer ↔ BattleServer | Media (inter-server) |
| CS_USER_* | 15 | GameServer → ClanServer state | N/A (interno) |
| AUTH_INTERNAL | 14 | GameServer → AuthServer | N/A (interno) |
| MESSENGER_INTERNAL | 12 | GameServer → MessengerServer | N/A (interno) |
| SS_GACHA | 5 | Server-to-Server gacha | N/A (interno) |
| SKILL duplicates | 5 | Alias de handlers existentes | Baja |
| CLAN_WAR client | 3 | Client-facing | Media |
| AUTH_CLIENT | 3 | Client-facing | Media |
| ROOM | 2 | Client-facing | Media |
| CS_MATCH | 1 | Client-facing | Baja |
| Otros (BASE, LOGIN, etc.) | 6 | Mixto | Baja |

**Conclusión de protocolos: La cobertura CLIENT-FACING es ~98%. Los faltantes son mayormente inter-server.**

---

## Gaps Críticos por Servidor

### GameServer - Gaps vs Original

#### Lo que TIENE el remake (y el original):
- [x] Session state machine (GAME_TASK: CONNECT→LOGIN→INFO→CHANNEL→LOBBY→ROOM→BATTLE)
- [x] Channel system con tipos (Normal, Beginner, Clan, Expert)
- [x] Room system con 17 game modes
- [x] Room state machine (WAITING→COUNTDOWN→LOADING→BATTLE→RESULT)
- [x] Battle flow completo (ready, loading, start, death, respawn, end)
- [x] Shop con compra/regalo/reparación/gacha
- [x] Clan system (crear, miembros, rangos, mark, chat)
- [x] Clan match + clan war
- [x] Friends/whisper/block/notes
- [x] Medal/quest/skill/title systems
- [x] Equipment/inventory/character management
- [x] GM commands (kick, ban, pause, announce)
- [x] Anti-cheat stubs (GameGuard, cheat detection)
- [x] EXP/GP calculation con boost events
- [x] Attendance/daily gifts
- [x] Double-buffered room/lobby lists
- [x] Per-channel mutexes

#### Lo que FALTA en el GameServer remake:

**1. Callbacks Asíncronos del Original (*_A methods)**
En el original, CUserSession tiene ~30 callback methods (`*_A` suffix) que se llaman cuando respuestas llegan de AuthServer/TransServer/ClanServer:
```
LogIn_MAU(), GetMyInfo_A(), ServerEnter_A(), ServerLeave_A()
CreateNickName_A(), CreateChar_A(), DeleteChar_A(), ChangeStateChar_A()
SetClan_C(), SetBirthDay_A(), SendAttendanceItemResult()
OnGoodsBuy_A(), OnGiftAuth_A(), OnInsertItem_A(), OnDeleteItem_A()
OnItemAuth_A(), OnMedalReward_A(), SetRankUp()
```
**En el remake**: Estos callbacks se manejan directamente en ModuleDataServer response handlers. La mayoría están como TODOs (Fase 8.2 del plan maestro).

**2. Equipment Validation Completa**
El original tiene ~17 métodos de validación de equipamiento:
```
_CheckWeaponEquip(), _SetWeaponEquip(), _CheckCharEquip(), _SetCharEquip()
_CheckItemAuth(), _CheckItemPositionCommon(), _SyncEquipIdx()
_CheckEquipment(), _CheckCharEquipment(), _CheckCommonEquipment()
_ExceptionChara(), _ExceptionSpecial(), _ExceptionEquip()
```
**En el remake**: Equipamiento básico funciona pero sin validación profunda de restricciones por tipo/clase/nivel.

**3. Item System Completo**
```
_BuyGoods() (con multi-basket), _CheckBuyRestriction(), _CheckBuyGoods()
_ExtendItem(), _RepairItem(), _SetUsedItem()
_SetItemAbilityAuth(), _SetItemAbilityChange()
_LastQuestItem(), _DeleteQuestCardSet(), _SetQuestEvent()
```
**En el remake**: Compra básica funciona. Faltan: extend items, item ability auth/change, quest item rewards completos.

**4. Abuse/Hack Detection Avanzada**
```
_CheckUlsanGuard() - USG anti-cheat integration
CheckRespawnHacking() - Respawn speed validation
m_ui8AbusingLevel, m_ui32AbusingPopupTime - Abuse tracking
m_bHSCheck, m_bHackKill - Hack detection flags
```
**En el remake**: Stubs básicos. Sin integración real de anti-cheat.

**5. ContextMain Data Loading**
El original CContextMain (462 líneas) carga masivamente:
```
Map/Stage data: m_aStageData[STAGE_TOTAL_MAX], m_aStageMatching[], m_aRuleData[]
Quest data: m_pQuest[QUEST_CARDSET_TYPE_COUNT][], m_pQuest_ItemInfo[]
Medal data: m_MedalSystemInfo, m_MQFMedal[], m_MQFAllDailyMedalSet
Shop: CShop*, FieldShopInfo
Clan: CLAN_CONTEXT[], CLAN_LIST_BASIC_INFO*
Events: m_stBoostEvent[], m_DailyGift, m_stTSEvent[]
GameGuard: HMODULE, hack check functions
Default items: m_TBasicWeapon[][], m_TBasicEquip[]
```
**En el remake**: GameContextMain tiene lo básico (session count, channels, server ID). Muchos datos se manejan inline o con stubs.

**6. Error Message System**
```
_SendErrorMessage() (2 overloads) - Unified error reporting
DisconnectUserDelay() (2 overloads) - Delayed disconnect with reason
EVENT_ERROR enum - Structured error codes
```
**En el remake**: Errores se manejan con SendSimpleAck(proto, errorCode). No hay sistema unificado de error messages.

---

### BattleServer - Gaps vs Original DediServer (MÁS GRANDE)

El original DediServer es **22x más grande** que el remake. Es el gap más significativo.

#### Estado de porting del BattleServer desde DediServer original:

| Sistema | Original | Remake | Estado |
|---------|----------|--------|--------|
| **HMSParser (Anti-Cheat)** | 1,612 líneas, 20+ hack types | HMSParser.h/.cpp (~420 líneas) | ✅ Implementado |
| **PhysX Engine** | NxGlobal, NxScene, NxActor, raycasts | CollisionSystem (Möller-Trumbore CPU) | ✅ Reemplazado |
| **Game Objects** | DSObject, DSObjectManager, COctreeNode | GameObject + GameObjectManager | ✅ Implementado |
| **Weapon System** | CWeapon, WeaponSlot, WeaponTable, bullet tracking | WeaponSystem.h/.cpp (~640 líneas) | ✅ Implementado |
| **Character System** | CCharacter (HP, position, speed, weapons) | GameCharacter.h/.cpp (~470 líneas) | ✅ Implementado |
| **Grenade/Throwables** | ThrowWeaponMgr, GrenadeState | ThrowWeaponMgr en WeaponSystem | ✅ Implementado |
| **Respawn Manager** | RespawnMgr, RespawnState queue | RespawnManager.h/.cpp | ✅ Implementado |
| **Dropped Weapons** | DroppedWeaponMgr (pickup mechanics) | DroppedWeaponMgr en WeaponSystem | ✅ Implementado |
| **Speed Validation** | SpeedState (anti-speed-hack) | SpeedState.h/.cpp (~90 líneas) | ✅ Implementado |
| **TaskProcessor** | 2,800 líneas de game logic por room | TaskProcessor.h/.cpp (~130 líneas, framework) | ⚠️ Framework (logic pendiente) |
| **Map System** | MapManager, MapData (stage geometry) | MapData + MapManager + ConfigXML | ✅ Implementado |
| **Battle Events (ex-ModuleCast)** | TCP to GameServer (kills, rounds, hacks) | BattleSession live event senders (direct, ~120 líneas) | ✅ Integrado en InterServerProtocol |
| **Event Receivers (GameServer)** | Recibe kills, hacks, rounds desde BattleServer | ModuleBattleServer handlers (~120 líneas) | ✅ Implementado |
| **Logging** | Logging to log server | printf-based (enterprise: integrate with syslog/ELK) | ⚠️ Simplificado |
| **Packet Validation** | DediUdpChecker (~20 parse methods) | UDPChecker.h/.cpp (~550 líneas) | ✅ Implementado |
| **Statistics** | ServerStatistics profiling | ServerStatistics.h/.cpp (~230 líneas) | ✅ Implementado |
| **HitValidator** | Line-of-sight, range check | HitValidator.h/.cpp | ✅ Implementado |
| **IOCP Workers** | iocpServer, iocpWorker (multi-thread) | Single-thread (i3Server IOCP framework) | ⚠️ Framework handles |
| **NxuLib (PhysX support)** | 60+ archivos | N/A (reemplazado por CollisionSystem CPU) | ✅ No necesario |
| **Thread-safe containers** | InterlockedList, PacketLocker | Standard C++ containers | ⚠️ Simplificado |

**Resumen**: 15/20 sistemas ✅ implementados, 5/20 ⚠️ stubs/framework (se completarán con integración de protocolo).

**Arquitectura mejorada vs original** (sin servidores intermediarios):
- **Sin CastServer**: Los battle events (kills, rounds, hacks, missions) se envían directamente BattleServer→GameServer via InterServerProtocol TCP
- **Sin LogServer/ControlServer**: Logging via printf (producción: syslog/ELK). Control via admin commands en GameServer
- **TaskProcessor**: El framework de procesamiento está listo; la lógica por modo de juego se añade incrementalmente

---

### DataServer - Gaps vs Original TransServer

El original TransServer es **9x más grande** que el remake.

#### Sistemas FALTANTES del DataServer remake:

| Sistema | Original | Remake | Impacto |
|---------|----------|--------|---------|
| **TaskProcessor completo** | 13 sub-processors, 10,000+ líneas | 1 processor sync, ~200 líneas | Alto |
| **ModuleDBShop_SP** | 4,154 líneas (compras, cupones, promos) | Tabla básica pb_shop_items | Alto |
| **Async Ring Buffers** | LocalRingBuffer para DB non-blocking | Sync directo (bloquea) | Medio |
| **User Management** | UserManager, UserFinder, UserEvent | Sin tracking de sesiones | Medio |
| **GeoIP/IPChecker** | Geolocalización, filtrado IP | NINGUNO | Bajo |
| **Payment (SIA)** | ModuleSIA (pagos reales) | NINGUNO | Bajo* |
| **Ranking System** | GeneralRankup (leaderboards) | NINGUNO | Medio |
| **Capsule/Gacha DB** | CapsuleItem, CapsuleRand | NINGUNO (se hace en GameServer) | Bajo |
| **Design System** | DesignV1, DesignV2 (progression curves) | NINGUNO | Bajo |
| **Kick List** | ModuleDBKickList (ban persistence) | NINGUNO | Medio |
| **ModuleDBNick** | 387 líneas (validación, historial) | Básico (check+create) | Bajo |
| **ModuleDBStats** | 234 líneas (stats detallados) | Básico | Bajo |

\* Payment no aplica para server privado

---

## Protocolos Client-Facing Verdaderamente Faltantes (~10)

Estos son los únicos REQ que un cliente podría enviar y NO tienen handler:

| Protocolo | Categoría | Impacto |
|-----------|-----------|---------|
| `PROTOCOL_AUTH_SHOP_SEND_GIFT_REQ` | Shop gift variante | Bajo (ya hay GOODS_GIFT) |
| `PROTOCOL_AUTH_SHOP_ITEM_AUTH_DATA_REQ` | Item auth data | Bajo |
| `PROTOCOL_AUTH_RS_JACKPOT_USER_NICK_INSERT_REQ` | Roulette jackpot nick | Bajo |
| `PROTOCOL_CLAN_WAR_MERCENARY_PENALTY_SAVE_REQ` | Save merc penalty | Bajo |
| `PROTOCOL_CLAN_WAR_RELEASE_INVITE_TEAM_REQ` | Release team invite | Bajo |
| `PROTOCOL_CLAN_WAR_START_GAME_SERVER_REQ` | Start clan war game | Medio |
| `PROTOCOL_ROOM_FRIEND_INVITE_LOBBY_USER_REQ` | Invite friend to room | Bajo (ya hay INVITE) |
| `PROTOCOL_ROOM_UNREADY_4VS4_REQ` | Unready 4v4 | Bajo |
| `PROTOCOL_CS_MATCH_ALL_TEAM_CONTEXT_REQ` | All team context | Bajo |
| `PROTOCOL_BASE_FIND_UID_GET_USER_DETAIL_INFO_REQ` | Find by UID detail | Bajo |

---

## Protocolos Inter-Server Faltantes (115) - Diseño del Remake

En el original, estos protocolos van ENTRE servidores (Game↔Auth, Game↔Clan, Game↔Messenger, Game↔BattleServer). En el remake distribuido, se manejan diferente:

| Comunicación Original | Solución en Remake |
|-----------------------|-------------------|
| GameServer → AuthServer (14 protos) | Via ModuleDataServer (InterServerProtocol.h) |
| GameServer → ClanServer (15 CS_USER_* + 22 GC_*) | In-memory GameClanManager (no server separado) |
| GameServer → MessengerServer (12 protos) | In-memory via GameSessionManager |
| GameServer ↔ BattleServer (17 protos) | Via ModuleBattleServer (InterServerProtocol.h) |
| ControlServer → GameServer (25 ASC_* protos) | GM commands via chat + futuro admin panel |
| GameServer → GachaServer (5 SS_GACHA_*) | In-memory en GameServer |

**Estos 115 protocolos NO necesitan dispatch en GameSession.cpp** porque no vienen del cliente. Se manejan en los módulos inter-servidor o in-memory.

---

## Prioridades de Desarrollo Pendiente

### P0 - Crítico para Funcionamiento (Fase 8 del plan maestro)

| Task | Descripción | Estado |
|------|-------------|--------|
| **8.2 ModuleDataServer Sync** | 15 Request + 16 Response handlers para persistir datos | PENDIENTE |
| **8.3 Fix Nick Callbacks** | Callbacks de CreateNick/CheckNick a session | PENDIENTE |
| **8.4 Battle End → Save Stats** | Guardar stats post-batalla via DataServer | PENDIENTE |
| **8.5 BattleServer UDP Fix** | Room almacena IP/port del BattleServer | PENDIENTE |
| **8.9 Date Fix (Server)** | Descomentar DATE32::GetDateTimeYYMMDDHHMI() para usar fecha real; enviar InvenServerTime correcto en INVENTORY_ENTER_ACK y shop packets | PENDIENTE |
| **8.10 Date Fix (Cliente)** | Cambiar TimeUtil::CalcTimeDifference() de INT32 a UINT32; fix InvenList::DeleteTimeOverCouponBuffers() INT32 comparación | PENDIENTE |
| **8.11 Date Fix (DATE32 range)** | Ampliar DATE32_YEAR_MAX o cambiar base epoch para soportar > 2033 | PENDIENTE |

### P1 - Importante para Experiencia

| Task | Descripción | Estado |
|------|-------------|--------|
| **8.1 Shop DB Table** | `pb_shop_items` + IS protocols + DataServer handler | PENDIENTE |
| **8.6 Room Timer System** | Auto-end batalla cuando tiempo expira | PENDIENTE |
| **8.7 Shop Dynamic Loading** | Catálogo desde DataServer en vez de hardcoded | PENDIENTE |
| **8.8 Config-Driven Settings** | Valores de config.ini en vez de hardcoded | PENDIENTE |
| **10 client-facing protos** | Los ~10 protocolos client-facing faltantes | PENDIENTE |

### P2 - Mejoras de Calidad

| Task | Descripción | Estado |
|------|-------------|--------|
| Equipment validation | _CheckWeaponEquip, _CheckCharEquip completos | PENDIENTE |
| Item abilities | _SetItemAbilityAuth/Change | PENDIENTE |
| Error message system | _SendErrorMessage unificado | PENDIENTE |
| Ranking/Leaderboard | GeneralRankup en DataServer | PENDIENTE |
| Ban persistence | ModuleDBKickList en DataServer | PENDIENTE |
| GetTickCount overflow | Reemplazar GetTickCount() con timer de 64-bit para uptimes > 49.7 días | PENDIENTE |

### P3 - Producción (si se va a hacer público)

| Task | Descripción | Estado |
|------|-------------|--------|
| BattleServer anti-cheat | HMSParser básico (speed, damage, position) | NO SE IMPLEMENTARÁ |
| BattleServer character state | CCharacter con HP y position tracking | ✅ COMPLETADO (Phase 11) |
| Async DB operations | Ring buffers para DataServer non-blocking | PENDIENTE |
| Admin panel | ASC_* protocol handlers (o web panel) | PENDIENTE |
| IP filtering | IPChecker, GeoIP | PENDIENTE |

### P4 - Nice-to-have

| Task | Descripción | Estado |
|------|-------------|--------|
| PhysX integration | Server-side hit detection | ✅ COMPLETADO (CollisionSystem CPU, Phase 11) |
| Weapon/bullet tracking | WeaponSlot, bullet state en BattleServer | PENDIENTE |
| Game objects | DSObject, DSObjectManager en BattleServer | ✅ COMPLETADO (Phase 12) |
| Payment system | SIA module (solo para producción comercial) | N/A |
| PC Cafe features | PCCafeInfo, Thai PC module | N/A |

---

## Archivos del Original - Referencia Completa

### GameServer Original (`Server/Source/Game/Game/`)

| Archivo | Líneas | Clase Principal | Portado a Remake? |
|---------|--------|-----------------|-------------------|
| ServerDef.h | 706 | Enums, macros, constantes | Parcial (RoomDef.h, BattleDef.h) |
| ServerContext.h/cpp | ~200 | CServerContext | ✅ GameServerContext |
| UserSession.h | 1,200 | CUserSession | ✅ GameSession.h (932 líneas) |
| UserSession*.cpp | 17,542 | Handlers | ✅ GameSession*.cpp (31,686 líneas) |
| UserManager.h/cpp | ~500 | CUserSessionManager | ✅ GameSessionManager |
| Room.h/cpp | ~3,000 | CRoom (12 mode pointers) | ✅ Room.h/cpp (2,748 líneas) |
| RoomManager.h/cpp | ~400 | CRoomManager | ✅ RoomManager |
| ContextMain.h/cpp | ~1,500 | CContextMain | Parcial (GameContextMain) |
| RoomInDeathmatch.h | 35 | CRoomInDeathmatch | ✅ Inline en Room.cpp |
| RoomInBomb.h | 57 | CRoomInBomb | ✅ Inline en Room.cpp |
| RoomInAnnihilation.h | 46 | CRoomInAnnihilation | ✅ Inline en Room.cpp |
| RoomInDestroy.h | 59 | CRoomInDestroy | ✅ Inline en Room.cpp |
| RoomInDefence.h | 73 | CRoomInDefence | ✅ Inline en Room.cpp |
| RoomInEscape.h | 129 | CRoomInEscape | ✅ Inline en Room.cpp |
| RoomInCrossCount.h | 71 | CRoomInCrossCount | ✅ Inline en Room.cpp |
| RoomInConvoy.h | 55 | CRoomInConvoy | ✅ Inline en Room.cpp |
| RoomInChallenge.h | 36 | CRoomInChallenge | ✅ Inline en Room.cpp |
| RoomInTutorial.h | 50 | CRoomInTutorial | ✅ Inline en Room.cpp |
| RoomInRunAway.h | 56 | CRoomInRunAway | ✅ Inline en Room.cpp |
| RoomInSpace.h | 13 | CRoomInSpace | ✅ Inline en Room.cpp |
| ModuleAuth.h | ~300 | CModuleAuth | ⚠️ Parcial (ModuleDataServer) |
| ModuleTrans.h | ~200 | CModuleTrans | ⚠️ Parcial (ModuleDataServer) |
| ModuleClan.h | ~200 | CModuleClan | ✅ In-memory (GameClanManager) |
| ModuleMessenger.h | ~200 | CModuleMessenger | ✅ In-memory (GameSessionManager) |
| ModuleControl.h | ~200 | CModuleControl | ❌ No (GM commands via chat) |
| ModuleGIP.h | ~200 | CModuleGIP | ⚠️ Parcial (ModuleBattleServer) |
| ModuleLog.h | ~200 | CModuleLog | ✅ ServerLog inline |
| ModuleZLog.h | ~200 | CModuleZLog | ✅ ServerLog inline |

### DediServer Original (`Server/Source/Dedication/Dedi/`) - 82 archivos, 40,139 líneas

| Área | Archivos | Líneas | Portado? |
|------|----------|--------|----------|
| Core (DediRoom, DediMember) | 4 | 7,075 | ⚠️ Simplificado (BattleRoom/Member) |
| Anti-Cheat (HMSParser) | 3 | 2,652 | ❌ |
| Physics (NxGlobal, NxShape) | 5 | 1,024 | ✅ CollisionSystem (Möller-Trumbore CPU) |
| Game Objects (DSObject) | 4 | 1,218 | ✅ GameObject + GameObjectManager |
| Characters (Character, Skill) | 6 | 1,390 | ⚠️ BattleMember (position/HP/weapon) |
| Weapons (Weapon, WeaponSlot) | 7 | 1,573 | ❌ |
| Grenades/Thrown | 2 | 329 | ❌ |
| Respawn | 2 | 269 | ✅ RespawnManager |
| Speed State | 1 | 185 | ✅ HitValidator (speed hack detection) |
| TaskProcessor | 3 | 6,840 | ⚠️ UdpRelay con packet parsing |
| UDP Parser/Builder | 4 | 1,347 | ✅ UdpRelay (parse + relay) |
| Networking (Modules) | 11 | 2,846 | ⚠️ Básico |
| IOCP Framework | 4 | 800 | ❌ (usa i3Server) |
| Maps/Config | 5 | 1,325 | ✅ MapData + MapManager + ConfigXML |
| Utilities | 8 | 1,510 | ❌ |
| NxuLib (PhysX) | 60+ | 3,000+ | ✅ Reemplazado por CollisionSystem |

### TransServer Original (`Server/Source/Trans/Trans/`) - 90+ archivos, 29,931 líneas

| Área | Archivos | Líneas | Portado? |
|------|----------|--------|----------|
| TaskProcessor (13 sub-procs) | 13 | 10,903 | ⚠️ Simplificado (1 proc sync) |
| DB Modules (13 modules) | 26 | 7,195 | ⚠️ 6 módulos básicos |
| ModuleDBShop_SP | 1 | 4,154 | ❌ |
| User Management | 4 | 879 | ❌ |
| Networking | 8 | 2,063 | ⚠️ Básico |
| Data Structures | 5 | 925 | ❌ |
| Specialized (GeoIP, Ranking) | 6 | 1,900 | ❌ |
| Capsule/Gacha | 2 | 383 | ❌ |
| Design System | 3 | 500 | ❌ |
| Utilities | 8 | 1,029 | ⚠️ Parcial |

---

## Arquitectura del Sistema

```
                    ┌─────────────────┐
                    │   Game Client   │
                    └────────┬────────┘
                             │ TCP (RSA+XOR)
                    ┌────────▼────────┐
                    │ ConnectServer   │ :40001
                    │ (Auth + List)   │
                    └────────┬────────┘
                             │ Auth Token
              ┌──────────────┼──────────────┐
              │              │              │
     ┌────────▼────────┐    │     ┌────────▼────────┐
     │  GameServer #1  │    │     │  GameServer #N  │
     │  :40000 (TCP)   │    │     │  :400XX (TCP)   │
     │  443 dispatch   │    │     │                 │
     │  56 archivos    │    │     │                 │
     └────────┬────────┘    │     └────────┬────────┘
              │             │              │
              │    ┌────────▼────────┐     │
              │    │  DataServer     │     │
              ├────►  :40100 (TCP)   ◄─────┤
              │    │  PostgreSQL     │     │
              │    │  22 archivos    │     │
              │    └─────────────────┘     │
              │                            │
     ┌────────▼────────┐         ┌────────▼────────┐
     │ BattleServer #1 │         │ BattleServer #N │
     │ :40200 (TCP)    │         │ :402XX (TCP)    │
     │ :41000+ (UDP)   │         │ :410XX+ (UDP)   │
     │ Physics+Objects │         │                 │
     │ CollisionSystem │         │                 │
     │ HitValidator    │         │                 │
     └─────────────────┘         └─────────────────┘
```

### Diferencia Arquitectural vs Original

```
ORIGINAL (Monolítico):
  GameServer ←→ AuthServer ←→ TransServer (DB)
      ↕              ↕
  ClanServer    MessengerServer
      ↕              ↕
  ControlServer  CastServer
      ↕
  DediServer (BattleServer con PhysX)

REMAKE (Distribuido simplificado):
  ConnectServer → GameServer → DataServer (PostgreSQL)
                      ↕
                  BattleServer (Physics + Collision + GameObjects)

  Clan/Messenger/Control → In-memory en GameServer
  Auth → Manejado por ConnectServer + DataServer
```

---

## Historial de Batches

| Batch | Commit | Handlers | Lines | Total Cases |
|-------|--------|----------|-------|-------------|
| 15 | ec3a3f76 | +34 | +1,092 | ~310 |
| 16 | 56ede0ab | +34 | +1,476 | ~344 |
| 17 | 3b3082e8 | +35 | +1,255 | ~348 |
| 18 | 5e18339c | +36 | +1,143 | ~385 |
| 19 | 03164304 | +37 | +1,539 | 385 |
| 20 | 7d187083 | +56 | +1,594 | 443 |

### Phase Commits

| Phase | Commit | Description |
|-------|--------|-------------|
| 8 | da73adaf | DataServer sync, nick callbacks, BattleServer migration |
| 8.2 | bd6f5b36 | Complete DataServer protocol gaps |
| 8.9-8.11 | 95cd1588 | Fix DATE32 year overflow and client INT32 overflow |
| 9.1-9.7 | 8d786138 | Persistence gaps, gift delivery, ban wiring, clan slots |
| 9.3-9.9 | 54f720dd | ConnectServer auth, token transfer, config loading, color nick |
| 10-12 | (pending) | Media loading, physics, collision, game objects, quest loading |
