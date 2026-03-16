# Plan de Implementación Completa - Piercing Blow Server Remake

## Estado Actual del Remake

### Lo que YA está implementado:
- **GameServer**: 46 archivos (~9,500 líneas) - Estado de sesión, Room, Battle básico, Shop, Clan, Medal, Quest, Skill, Roulette, Social, Equipment, Events
- **DataServer**: 22 archivos - PostgreSQL con auth, perfil, stats, inventario, social
- **BattleServer**: 17 archivos - Rooms UDP, relay, member tracking
- **ConnectServer**: 10 archivos - Registry, heartbeat, sesiones
- **S2MO**: Completo - RSA/AES/XOR encryption
- **i3Server**: Completo - Framework IOCP networking
- **ServerCommon**: InterServerProtocol (700 líneas), ModuleBase, BaseServer

### Lo que FALTA (organizado por prioridad):

---

## FASE 1: Battle Flow Completo (Crítico - el cliente se congela sin esto)

### 1A. Room State Machine Completa
**Archivos**: `Room.cpp`, `Room.h`
- [x] Implementar `OnUpdateRoom()` con timer-driven state machine (como original `_OnUpdateRoom_*_M`)
- [x] Estados faltantes: `ROOM_STATE_COUNTDOWN_R` (3s countdown), `ROOM_STATE_LOADING` (timeout 60s), `ROOM_STATE_RENDEZVOUS`, `ROOM_STATE_PRE_BATTLE` (hole punch), `ROOM_STATE_COUNTDOWN_B` (3s), `ROOM_STATE_BATTLE_RESULT` (5s scoreboard)
- [x] Transiciones automáticas por timeout entre estados
- [x] Agregar `m_dwStateStartTime` para tracking de cuando entró en cada estado
- [x] Loading timeout: si un jugador no carga en 60s, sacarlo

### 1B. Battle Flow Completo con BattleServer
**Archivos**: `GameSessionBattle.cpp`, `ModuleBattleServer.cpp`
- [x] En `OnBattleReadyBattleReq`: Solicitar creación de room al BattleServer via `IS_BATTLE_CREATE_REQ`
- [x] Esperar `IS_BATTLE_CREATE_ACK` antes de avanzar a LOADING
- [x] En `OnBattlePreStartBattleReq`: Enviar IP/Puerto UDP del BattleServer al cliente
- [x] Tracking per-slot de estado de loading (`SLOT_STATE_LOAD` → `SLOT_STATE_BATTLE_LOADOK` → `SLOT_STATE_BATTLE_READY`)
- [x] Cuando TODOS los slots están en `BATTLE_READY`, avanzar a `COUNTDOWN_B`
- [x] En `OnBattleStartBattleReq`: Solo avanzar a BATTLE si room está en COUNTDOWN_B

### 1C. Battle Result Screen
**Archivos**: `GameSessionBattle.cpp`, `Room.cpp`
- [x] Cuando el match termina, cambiar a `ROOM_STATE_BATTLE_RESULT` (no directamente a READY)
- [x] Enviar packet con estadísticas completas por jugador: kills, deaths, headshots, assists, exp ganada, puntos ganados
- [x] Timer de 5 segundos en BATTLE_RESULT
- [x] Al terminar timer, calcular recompensas, guardar stats en DataServer, y volver a READY
- [x] Implementar `SLOT_BONUS` tracking: EXP y POINT bonuses por performance

### 1D. Death & Kill Processing Completo
**Archivos**: `GameSessionBattle.cpp`
- [x] Parsear `DEATH_INFO_HEADER` completo: weaponId, hitPart, position(x,y,z), assistSlot
- [x] Multi-kill tracking (double, triple, multi, ultra, unbelievable)
- [x] Headshot tracking
- [x] Assist tracking (último jugador que dañó antes del kill)
- [x] Actualizar `m_stUsedWeapon` en sesión para tracking de arma usada
- [x] Broadcasting de death con toda la info a todos los jugadores

### 1E. Respawn System
**Archivos**: `GameSessionBattle.cpp`, `Room.cpp`
- [x] Implementar respawn timers configurables (NO=6s, SHORT_20=5s, SHORT_30=4s, MID_30=3s, MID_40=2s, MID_50=1s)
- [x] `m_bReSpawnFlag[SLOT_MAX_COUNT]` y `m_bDeathFlag[SLOT_MAX_COUNT]` tracking
- [x] `m_bAlive[SLOT_MAX_COUNT]` para saber quién está vivo
- [x] Validar respawn: no permitir si el round ya acabó

---

## FASE 2: Game Modes (Modos de Juego Específicos)

### 2A. Bomb Mode (Bomb Plant/Defuse)
**Archivos**: Crear `RoomModeBomb.h/.cpp` o integrar en `Room.cpp`
- [x] `MissionBombInstall()`: Instalar bomba, broadcast posición, iniciar timer de detonación (45s)
- [x] `MissionBombUnInstall()`: Desinstalar bomba, broadcast, award puntos
- [x] Round win conditions: bomba explota = ATK wins, bomba desinstalada = DEF wins, timer expira sin bomba = DEF wins, todos muertos = team opuesto wins
- [x] Atk/Def team swap entre rounds (`m_ui8AtkDefChange`)
- [x] Smoke/flash/grenade validation (ValidateGrenadeUse: type, position, alive check)

### 2B. Annihilation Mode (Eliminar a Todos)
**Archivos**: Integrar en `Room.cpp`
- [x] Round-based: todos los del equipo contrario muertos = round win
- [x] No respawn durante el round
- [x] Round timer + last man standing tracking

### 2C. Destroy Mode
**Archivos**: Integrar en `Room.cpp`
- [x] `MissionObjectDestroy()`: Destruir objetivos del mapa
- [x] HP tracking por objeto destructible
- [x] Win condition: objeto destruido o timer expira

### 2D. Defence Mode
**Archivos**: Integrar en `Room.cpp`
- [x] `MissionObjectDefence()`: Defender objetivos (uses generator HP system)
- [ ] Wave system con NPCs/bots
- [x] HP tracking del objeto a defender

### 2E. Escape Mode (VIP)
**Archivos**: Integrar en `Room.cpp`
- [x] VIP selection (aleatorio de ATK team)
- [x] `MissionTouchDown()`: VIP llega a zona de escape = win
- [x] VIP muerto = DEF wins
- [x] VIP tiene más HP (150%), restricciones de arma (ValidateDamage + IsVIPSlot)

### 2F. CrossCount Mode (Dino DM)
**Archivos**: Integrar en `Room.cpp`
- [ ] Kill counting con variantes dinosaurio
- [ ] Special spawn points

### 2G. Convoy Mode
- [ ] Convoy HP tracking, checkpoint system

### 2H. Challenge/AI Mode
- [ ] Server-side AI bot spawning
- [ ] Difficulty-based kill points (Easy/Normal/Hard arrays en ServerDef.cpp)
- [ ] Stage progression

---

## FASE 3: Protocolo Completo de Room

### 3A. Room Protocols Faltantes
**Archivos**: `GameSessionRoom.cpp`, `GameSessionBattle.cpp`
- [x] `PROTOCOL_ROOM_INVITE_REQ/ACK` - Invitar jugador a la sala
- [x] `PROTOCOL_ROOM_OBSERVER_REQ/ACK` - Modo observador (observer slot toggle with validation)
- [x] `PROTOCOL_BATTLE_SUGGEST_KICKVOTE_REQ/ACK` - Votación de kick
- [x] `PROTOCOL_BATTLE_VOTE_KICKVOTE_REQ/ACK` - Resultado de votación
- [x] `PROTOCOL_BATTLE_SENDPING_REQ/ACK` - Ping measurement
- [x] Lógica de votación avanzada: mínimo 3 jugadores, timeout 20s, cooldown 60s
- [x] Kick list tracking (vote state per slot in Room)

### 3B. Room State Sync
- [x] Double-buffered room info (Info0/Info1) para updates lock-free del room list
- [x] `FillRoomInfoBasic()` completo con todos los campos que espera el cliente
- [x] Room list update throttling (no más de 1 update por segundo)

---

## FASE 4: Protocolos Base Faltantes

### 4A. Login Flow Completo
**Archivos**: `GameSession.cpp`
- [x] `PROTOCOL_BASE_GET_SYSTEM_INFO_REQ/ACK` - Enviar versión del server, MD5 key, GameGuard type
- [x] `PROTOCOL_BASE_GET_OPTION_REQ/ACK` - Enviar opciones del jugador (controles, sensibilidad)
- [x] `PROTOCOL_BASE_OPTION_SAVE_REQ/ACK` - Guardar opciones
- [x] `PROTOCOL_BASE_CHECK_NICK_NAME_REQ/ACK` - Verificar nickname disponible
- [x] `PROTOCOL_BASE_CREATE_NICK_REQ/ACK` - Crear nickname (primera vez)
- [x] `PROTOCOL_BASE_RANK_UP_REQ/ACK` - Rank up automático al alcanzar EXP requerida

### 4B. Map/Stage Data
**Archivos**: `GameContextMain.cpp`, `GameSessionChannel.cpp`
- [x] `PROTOCOL_BASE_MAP_VERSION_REQ/ACK` - Versión del map list
- [x] `PROTOCOL_BASE_MAP_LIST_REQ/ACK` - Lista de mapas disponibles
- [x] `PROTOCOL_BASE_MAP_RULELIST_REQ/ACK` - Reglas por modo de juego
- [x] `PROTOCOL_BASE_MAP_MATCHINGLIST_REQ/ACK` - Matching mode↔stage
- [x] `PROTOCOL_BASE_MAP_RANDOM_LIST_ACK` - Mapas random
- [x] Cargar datos de mapas/stages desde config o DataServer

### 4C. User Info Completa
**Archivos**: `GameSession.cpp`
- [x] `PROTOCOL_BASE_GET_MYINFO_RECORD_REQ/ACK` - Record del jugador (K/D/Win/Loss)
- [x] `PROTOCOL_BASE_GET_MYINFO_BASIC_REQ/ACK` - Info básica
- [x] `PROTOCOL_BASE_GET_MYINFO_ALL_REQ/ACK` - Todo junto
- [x] `PROTOCOL_BASE_GET_RECORD_INFO_DB_REQ/ACK` - Record desde DB
- [x] `PROTOCOL_BASE_GET_USER_DETAIL_INFO_ACK` - Info detallada de otro jugador (lobby + room)

### 4D. Title System
- [x] `PROTOCOL_BASE_USER_TITLE_CHANGE_REQ/ACK`
- [x] `PROTOCOL_BASE_USER_TITLE_EQUIP_REQ/ACK`
- [x] `PROTOCOL_BASE_USER_TITLE_RELEASE_REQ/ACK`
- [x] `PROTOCOL_BASE_USER_TITLE_INFO_ACK`

---

## FASE 5: Shop System Completo

### 5A. Shop Version & Catalog
**Archivos**: `GameSessionShop.cpp`, `ShopManager.cpp`
- [x] `PROTOCOL_AUTH_SHOP_VERSION_REQ/ACK` - Shop version check
- [x] `PROTOCOL_AUTH_SHOP_LIST_REQ/ACK` - Shop list
- [x] `PROTOCOL_AUTH_SHOP_GOODSLIST_REQ/ACK` - Goods list
- [x] `PROTOCOL_AUTH_SHOP_ITEMLIST_REQ/ACK` - Item list
- [x] `PROTOCOL_AUTH_SHOP_MATCHINGLIST_REQ/ACK` - Matching list
- [x] `PROTOCOL_AUTH_SHOP_REPAIRLIST_REQ/ACK` - Repair data
- [x] Double-buffered shop data (UseShopData/NotUseShopData) para actualizaciones atómicas

### 5B. Purchase & Gift
- [x] `PROTOCOL_AUTH_SHOP_GOODS_BUY_REQ/ACK` - Compra con validación GP/Cash
- [x] `PROTOCOL_AUTH_SHOP_GOODS_GIFT_REQ/ACK` - Regalar items
- [x] `PROTOCOL_AUTH_SHOP_ITEM_AUTH_REQ/ACK` - Activar item
- [x] `PROTOCOL_AUTH_SHOP_INSERT_ITEM_REQ/ACK` - Agregar item al inventario
- [x] `PROTOCOL_AUTH_SHOP_DELETE_ITEM_REQ/ACK` - Eliminar item
- [x] Item expiration checking
- [x] Durability system

### 5C. Capsule/Gacha Shop
- [x] `PROTOCOL_AUTH_SHOP_CAPSULE_REQ/ACK`
- [x] `PROTOCOL_AUTH_SHOP_JACKPOT_REQ/ACK`
- [x] Random item selection con pesos

### 5D. Field Shop (Roulette)
- [x] RS_IGS protocols completos (Enter/Leave/ItemInfo/Start/JackpotNotify)
- [x] Enter/Leave/Start/Result (with weighted tiers, multi-spin, currency deduction)
- [x] Jackpot notifications
- [x] FieldShop Open/GoodsList (stub, empty goods list)

---

## FASE 6: Clan System Completo

### 6A. Clan Server Integration
**Archivos**: Crear `ModuleClanServer.h/.cpp`
- [x] Clan system in-memory (GameClanManager + ClanDef.h)
- [x] Clan creation/dissolution con validación (nivel mínimo, GP cost)
- [x] Member management: join, kick (deportation), promote/demote (master/staff/regular)
- [x] Clan mark/notice/intro updates (ReplaceNotice, ReplaceIntro, ReplaceMark)
- [x] Clan EXP and ranking (earn EXP from battles, auto rank-up, unit size upgrade)

### 6B. Clan Match
**Archivos**: Crear `GameSessionClanBattle.cpp`
- [ ] `PROTOCOL_VALUE_CLAN_MATCH (0x0800)` - Matchmaking
- [ ] Team registration, matching queue
- [ ] Clan match room creation with special rules
- [ ] Clan match results and point distribution

### 6C. Clan War (1.5)
**Archivos**: Crear `GameSessionClanWar.cpp`
- [ ] `PROTOCOL_VALUE_CLAN_WAR (0x1B00)` - War system
- [ ] War declaration, acceptance, scheduling
- [ ] War match rules
- [ ] Mercenary system (`T_MerID`, `m_bMerPenalty`)

---

## FASE 7: Social & Community Completo

### 7A. Messenger Server Integration
- [ ] Conexión al MessengerServer (o implementar directamente)
- [x] Friend online/offline notifications
- [x] Whisper message routing (same-server, FindSessionByNickname)
- [x] Note system (send/receive/delete/read - in-memory per session)
- [x] Lobby enter/leave notifications a amigos

### 7B. Lobby Chat Completo
**Archivos**: `GameSessionChannel.cpp`
- [x] `PROTOCOL_BASE_CHATTING_REQ/ACK` - Chat por canal
- [x] Lobby user list con double-buffering
- [x] User invite from lobby
- [x] Megaphone (broadcast chat)

### 7C. GM Chat
**Archivos**: Crear `GameSessionGMChat.cpp`
- [x] `PROTOCOL_VALUE_GMCHAT (0x1A00)` - GM → User messaging (Start/Send/End/Penalty)
- [x] Admin commands via chat (/announce, /kick, /ban, /info, /ccu, /help)

---

## FASE 8: QuickJoin System

**Archivos**: `GameSessionChannel.cpp` (OnQuickJoinRoomReq ya existe, expandir)
- [x] `PROTOCOL_VALUE_QUICKJOIN (0x1500)` handlers
- [x] `SearchQuickJoinRoom()`: buscar rooms compatibles (modo, mapa, jugadores)
- [x] Criterios: mode match, not full, not password-protected, not in battle (unless inter-enter)
- [x] Random selection entre candidatos

---

## FASE 9: Anti-Cheat & Security

### 9A. Packet Validation
**Archivos**: `GameSession.cpp`
- [x] Packet encryption validation con XOR key (BitRotateDecript)
- [x] Packet replay detection (hash-based duplicate detection within 100ms window)
- [x] Protocol rate limiting (max packets per second)
- [x] State validation: rechazar packets que no corresponden al GAME_TASK actual
- [x] Timeout por estado: 30s para NOT_LOGIN, 600s para LOGIN, 120s para NORMAL, 3600s para CHANNEL

### 9B. GameGuard Stub
- [x] Stub para GameGuard handshake (para que el cliente no se desconecte)
- [x] `PROTOCOL_BASE_GAMEGUARD_REQ/ACK` - Responder con datos dummy válidos

### 9C. Cheat Detection
**Archivos**: Crear `GameSessionCheat.cpp`
- [x] `PROTOCOL_VALUE_CHEAT (0x1300)` handlers (IncreaseKill, PlaySolo, ReduceRoundTime, Teleport)
- [x] Speed hack detection (comparing client-reported time vs server time)
- [x] Damage validation
- [x] Position validation

---

## FASE 10: EXP/Point/Rank System

### 10A. Battle Rewards
**Archivos**: `Room.cpp`, `GameSessionBattle.cpp`
- [x] EXP calculation: base + kills*multiplier + headshot bonus + win bonus
- [x] Point (GP) calculation: base + kills*multiplier + win bonus
- [x] Boost event support: multiply EXP/GP by event multiplier
- [x] Bonus items: EXP boost items, Point boost items from inventory
- [x] Save results to DataServer after each match

### 10B. Rank System
**Archivos**: `GameSession.cpp`
- [x] Rank-up check after EXP update
- [x] Rank-up items reward (from `m_aRankUpItem[MAX_RANK_COUNT]`)
- [x] `PROTOCOL_BASE_RANK_UP_ACK` notification
- [x] Initial rank-up for new players

---

## FASE 11: Admin/GM Tools

### 11A. GM Commands
**Archivos**: `GameSessionGM.cpp`
- [x] `GM_KickUser_U` - Kick player from room
- [x] `GM_ExitUser_U` - Force disconnect
- [x] `GM_BlockUser_U` - Ban with comment (OnGMBlockUserReq)
- [x] `GM_DestroyRoom_U` - Force destroy room
- [x] `GMPause` / `GMResume` - Pause/resume battle (broadcast to room)
- [x] Damage console for debugging
- [x] Server announce message (`SendServerAnnounce`)
- [x] Lobby GM exit user

### 11B. Control Server Integration
- [x] Server announce (BroadcastToAll, BroadcastAnnounce in GameSessionManager)
- [x] Server start/stop commands (/shutdown GM command with broadcast notice)
- [x] Config reload (hot reload economy/battle via /reload GM command)
- [x] User kick via admin tool (/kick, /ban GM commands)
- [x] Room list/user list queries (/rooms, /users GM commands)

---

## FASE 12: Performance & Robustness

### 12A. Thread Safety
- [ ] Per-channel mutexes para room lists y user lists
- [ ] Ring buffers para comunicación inter-thread (como NSM_CRingBuffer del original)
- [ ] Announce message system con ring buffers

### 12B. Memory Management
- [ ] Pre-allocated room pools (no dynamic allocation durante gameplay)
- [ ] Session pool pre-allocation
- [ ] Object pooling para packets

### 12C. Logging
- [x] Battle log con unique number tracking (BattleID=ServerID+Timestamp+Counter)
- [ ] ZLog integration para persistent logging
- [x] Performance metrics: packet count, average CCU, room count, peak tracking, periodic logging

---

## FASE 13: ConnectServer Completo

- [x] Auth token generation y validación (ModuleConnectServer validates, fallback dev mode)
- [x] Server list transmission al cliente (ConnectSession::SendServerList)
- [x] Load balancing: server select uses GetLeastLoadedServer() with client-requested ID fallback
- [x] Server heartbeat monitoring (GameServer sends actual player count, ConnectServer removes dead servers)

---

## FASE 14: Miscellaneous

### 14A. Attendance/Daily Gift System
- [x] Daily login tracking
- [x] Reward items por día consecutivo (milestone items at day 3/7/14/21/28)
- [x] Monthly reset (auto-reset on month change)

### 14B. Boost Events
- [x] Event schedule loading
- [x] EXP/Point multipliers durante eventos
- [x] `PROTOCOL_BASE_BOOSTEVENT_INFO_ACK`

### 14C. Item Durability
- [x] Durability decrease per battle
- [x] Repair cost calculation
- [x] Item break notification
- [x] Repair single/all items via shop

### 14D. Random Map System
- [x] Random map selection per channel
- [x] Map rotation config (SetMapRotation + AdvanceMapRotation on battle end)

---

## Orden de Implementación Recomendado

1. **FASE 1** (Battle Flow) - Sin esto el juego no funciona
2. **FASE 4A-4B** (Login + Maps) - Sin datos de mapas no se pueden crear rooms
3. **FASE 3** (Room Protocols) - Para que el lobby funcione bien
4. **FASE 10** (EXP/Rank) - Para que haya progresión
5. **FASE 2A-2B** (Bomb + Annihilation) - Los 2 modos más jugados
6. **FASE 5** (Shop) - Para que los jugadores compren items
7. **FASE 9** (Security) - Antes de hacer público
8. **FASE 6-7** (Clan + Social) - Features sociales
9. **FASE 8, 11-14** (El resto) - Completar features

## Métricas de Completitud Actual

| Categoría | Protocolos Original | Implementados | % |
|-----------|---------------------|---------------|---|
| Login (0x0100) | 3 | 1 | 33% |
| Base (0x0200) | ~80 | 30 | 38% |
| Auth (0x0300) | ~80 | 11 | 14% |
| Shop (0x0400) | ~40 | 16 | 40% |
| Admin (0x0500) | ~10 | 0 | 0% |
| Clan (0x0700) | ~60 | 20 | 33% |
| Clan Match (0x0800) | ~20 | 0 | 0% |
| Server Msg (0x0A00) | ~10 | 4 | 40% |
| Lobby (0x0C00) | ~10 | 7 | 70% |
| Inventory (0x0D00) | ~10 | 3 | 30% |
| RS/IGS (0x0E00) | ~10 | 5 | 50% |
| Room (0x0F00) | ~30 | 20 | 67% |
| Battle (0x1000) | ~40 | 20 | 50% |
| Medal (0x1200) | ~10 | 7 | 70% |
| Cheat (0x1300) | ~10 | 7 | 70% |
| Gacha (0x1400) | ~10 | 5 | 50% |
| QuickJoin (0x1500) | ~5 | 3 | 60% |
| Skill (0x1700) | ~5 | 4 | 80% |
| Char (0x1800) | ~5 | 3 | 60% |
| MyInfo (0x1900) | ~5 | 3 | 60% |
| GMChat (0x1A00) | ~5 | 5 | 100% |
| ClanWar (0x1B00) | ~20 | 0 | 0% |
| **TOTAL** | **~460** | **~175** | **~38%** |
