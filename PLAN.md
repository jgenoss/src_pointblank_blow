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
- [ ] Implementar `OnUpdateRoom()` con timer-driven state machine (como original `_OnUpdateRoom_*_M`)
- [ ] Estados faltantes: `ROOM_STATE_COUNTDOWN_R` (3s countdown), `ROOM_STATE_LOADING` (timeout 60s), `ROOM_STATE_RENDEZVOUS`, `ROOM_STATE_PRE_BATTLE` (hole punch), `ROOM_STATE_COUNTDOWN_B` (3s), `ROOM_STATE_BATTLE_RESULT` (5s scoreboard)
- [ ] Transiciones automáticas por timeout entre estados
- [ ] Agregar `m_dwStateStartTime` para tracking de cuando entró en cada estado
- [ ] Loading timeout: si un jugador no carga en 60s, sacarlo

### 1B. Battle Flow Completo con BattleServer
**Archivos**: `GameSessionBattle.cpp`, `ModuleBattleServer.cpp`
- [ ] En `OnBattleReadyBattleReq`: Solicitar creación de room al BattleServer via `IS_BATTLE_CREATE_REQ`
- [ ] Esperar `IS_BATTLE_CREATE_ACK` antes de avanzar a LOADING
- [ ] En `OnBattlePreStartBattleReq`: Enviar IP/Puerto UDP del BattleServer al cliente
- [ ] Tracking per-slot de estado de loading (`SLOT_STATE_LOAD` → `SLOT_STATE_BATTLE_LOADOK` → `SLOT_STATE_BATTLE_READY`)
- [ ] Cuando TODOS los slots están en `BATTLE_READY`, avanzar a `COUNTDOWN_B`
- [ ] En `OnBattleStartBattleReq`: Solo avanzar a BATTLE si room está en COUNTDOWN_B

### 1C. Battle Result Screen
**Archivos**: `GameSessionBattle.cpp`, `Room.cpp`
- [ ] Cuando el match termina, cambiar a `ROOM_STATE_BATTLE_RESULT` (no directamente a READY)
- [ ] Enviar packet con estadísticas completas por jugador: kills, deaths, headshots, assists, exp ganada, puntos ganados
- [ ] Timer de 5 segundos en BATTLE_RESULT
- [ ] Al terminar timer, calcular recompensas, guardar stats en DataServer, y volver a READY
- [ ] Implementar `SLOT_BONUS` tracking: EXP y POINT bonuses por performance

### 1D. Death & Kill Processing Completo
**Archivos**: `GameSessionBattle.cpp`
- [ ] Parsear `DEATH_INFO_HEADER` completo: weaponId, hitPart, position(x,y,z), assistSlot
- [ ] Multi-kill tracking (double, triple, multi, ultra, unbelievable)
- [ ] Headshot tracking
- [ ] Assist tracking (último jugador que dañó antes del kill)
- [ ] Actualizar `m_stUsedWeapon` en sesión para tracking de arma usada
- [ ] Broadcasting de death con toda la info a todos los jugadores

### 1E. Respawn System
**Archivos**: `GameSessionBattle.cpp`, `Room.cpp`
- [ ] Implementar respawn timers configurables (NO=6s, SHORT_20=5s, SHORT_30=4s, MID_30=3s, MID_40=2s, MID_50=1s)
- [ ] `m_bReSpawnFlag[SLOT_MAX_COUNT]` y `m_bDeathFlag[SLOT_MAX_COUNT]` tracking
- [ ] `m_bAlive[SLOT_MAX_COUNT]` para saber quién está vivo
- [ ] Validar respawn: no permitir si el round ya acabó

---

## FASE 2: Game Modes (Modos de Juego Específicos)

### 2A. Bomb Mode (Bomb Plant/Defuse)
**Archivos**: Crear `RoomModeBomb.h/.cpp` o integrar en `Room.cpp`
- [ ] `MissionBombInstall()`: Instalar bomba, broadcast posición, iniciar timer de detonación (45s)
- [ ] `MissionBombUnInstall()`: Desinstalar bomba, broadcast, award puntos
- [ ] Round win conditions: bomba explota = ATK wins, bomba desinstalada = DEF wins, timer expira sin bomba = DEF wins, todos muertos = team opuesto wins
- [ ] Atk/Def team swap entre rounds (`m_ui8AtkDefChange`)
- [ ] Smoke/flash/position validation

### 2B. Annihilation Mode (Eliminar a Todos)
**Archivos**: Integrar en `Room.cpp`
- [ ] Round-based: todos los del equipo contrario muertos = round win
- [ ] No respawn durante el round
- [ ] Round timer + last man standing tracking

### 2C. Destroy Mode
**Archivos**: Integrar en `Room.cpp`
- [ ] `MissionObjectDestroy()`: Destruir objetivos del mapa
- [ ] HP tracking por objeto destructible
- [ ] Win condition: objeto destruido o timer expira

### 2D. Defence Mode
**Archivos**: Integrar en `Room.cpp`
- [ ] `MissionObjectDefence()`: Defender objetivos
- [ ] Wave system con NPCs/bots
- [ ] HP tracking del objeto a defender

### 2E. Escape Mode (VIP)
**Archivos**: Integrar en `Room.cpp`
- [ ] VIP selection (aleatorio de ATK team)
- [ ] `MissionTouchDown()`: VIP llega a zona de escape = win
- [ ] VIP muerto = DEF wins
- [ ] VIP tiene más HP, restricciones de arma

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
**Archivos**: `GameSessionRoom.cpp`
- [ ] `PROTOCOL_ROOM_INVITE_REQ/ACK` - Invitar jugador a la sala
- [ ] `PROTOCOL_ROOM_OBSERVER_REQ/ACK` - Modo observador
- [ ] `PROTOCOL_ROOM_DECIDE_VOTE_REQ/ACK` - Votación de kick
- [ ] `PROTOCOL_ROOM_DECIDE_VOTE_RESULT_ACK` - Resultado de votación
- [ ] `PROTOCOL_ROOM_GET_PING_REQ/ACK` - Ping measurement
- [ ] Lógica de votación: mínimo 3 jugadores, timeout 20s, cooldown 60s
- [ ] Kick list tracking (`m_aKickUserList[]`)

### 3B. Room State Sync
- [ ] Double-buffered room info (Info0/Info1) para updates lock-free del room list
- [ ] `FillRoomInfoBasic()` completo con todos los campos que espera el cliente
- [ ] Room list update throttling (no más de 1 update por segundo)

---

## FASE 4: Protocolos Base Faltantes

### 4A. Login Flow Completo
**Archivos**: `GameSession.cpp`
- [ ] `PROTOCOL_BASE_GET_SYSTEM_INFO_REQ/ACK` - Enviar versión del server, MD5 key, GameGuard type
- [ ] `PROTOCOL_BASE_GET_OPTION_REQ/ACK` - Enviar opciones del jugador (controles, sensibilidad)
- [ ] `PROTOCOL_BASE_OPTION_SAVE_REQ/ACK` - Guardar opciones
- [ ] `PROTOCOL_BASE_CHECK_NICK_NAME_REQ/ACK` - Verificar nickname disponible
- [ ] `PROTOCOL_BASE_CREATE_NICK_REQ/ACK` - Crear nickname (primera vez)
- [ ] `PROTOCOL_BASE_RANK_UP_REQ/ACK` - Rank up automático al alcanzar EXP requerida

### 4B. Map/Stage Data
**Archivos**: `GameContextMain.cpp`, nuevo `GameSessionChannel.cpp`
- [ ] `PROTOCOL_BASE_MAP_VERSION_REQ/ACK` - Versión del map list
- [ ] `PROTOCOL_BASE_MAP_LIST_REQ/ACK` - Lista de mapas disponibles
- [ ] `PROTOCOL_BASE_MAP_RULELIST_REQ/ACK` - Reglas por modo de juego
- [ ] `PROTOCOL_BASE_MAP_MATCHINGLIST_REQ/ACK` - Matching mode↔stage
- [ ] `PROTOCOL_BASE_MAP_RANDOM_LIST_ACK` - Mapas random
- [ ] Cargar datos de mapas/stages desde config o DataServer

### 4C. User Info Completa
**Archivos**: `GameSession.cpp`
- [ ] `PROTOCOL_BASE_GET_MYINFO_RECORD_REQ/ACK` - Record del jugador (K/D/Win/Loss)
- [ ] `PROTOCOL_BASE_GET_MYINFO_BASIC_REQ/ACK` - Info básica
- [ ] `PROTOCOL_BASE_GET_MYINFO_ALL_REQ/ACK` - Todo junto
- [ ] `PROTOCOL_BASE_GET_RECORD_INFO_DB_REQ/ACK` - Record desde DB
- [ ] `PROTOCOL_BASE_GET_USER_DETAIL_INFO_ACK` - Info detallada de otro jugador

### 4D. Title System
- [ ] `PROTOCOL_BASE_USER_TITLE_CHANGE_REQ/ACK`
- [ ] `PROTOCOL_BASE_USER_TITLE_EQUIP_REQ/ACK`
- [ ] `PROTOCOL_BASE_USER_TITLE_RELEASE_REQ/ACK`
- [ ] `PROTOCOL_BASE_USER_TITLE_INFO_ACK`

---

## FASE 5: Shop System Completo

### 5A. Shop Version & Catalog
**Archivos**: `GameSessionShop.cpp`, `ShopManager.cpp`
- [ ] `PROTOCOL_AUTH_SHOP_VERSION_REQ/ACK` - Shop version check
- [ ] `PROTOCOL_AUTH_SHOP_LIST_REQ/ACK` - Shop list
- [ ] `PROTOCOL_AUTH_SHOP_GOODSLIST_REQ/ACK` - Goods list
- [ ] `PROTOCOL_AUTH_SHOP_ITEMLIST_REQ/ACK` - Item list
- [ ] `PROTOCOL_AUTH_SHOP_MATCHINGLIST_REQ/ACK` - Matching list
- [ ] `PROTOCOL_AUTH_SHOP_REPAIRLIST_REQ/ACK` - Repair data
- [ ] Double-buffered shop data (UseShopData/NotUseShopData) para actualizaciones atómicas

### 5B. Purchase & Gift
- [ ] `PROTOCOL_AUTH_SHOP_GOODS_BUY_REQ/ACK` - Compra con validación GP/Cash
- [ ] `PROTOCOL_AUTH_SHOP_GOODS_GIFT_REQ/ACK` - Regalar items
- [ ] `PROTOCOL_AUTH_SHOP_ITEM_AUTH_REQ/ACK` - Activar item
- [ ] `PROTOCOL_AUTH_SHOP_INSERT_ITEM_REQ/ACK` - Agregar item al inventario
- [ ] `PROTOCOL_AUTH_SHOP_DELETE_ITEM_REQ/ACK` - Eliminar item
- [ ] Item expiration checking
- [ ] Durability system

### 5C. Capsule/Gacha Shop
- [ ] `PROTOCOL_AUTH_SHOP_CAPSULE_REQ/ACK`
- [ ] `PROTOCOL_AUTH_SHOP_JACKPOT_REQ/ACK`
- [ ] Random item selection con pesos

### 5D. Field Shop (Roulette)
- [ ] RS_IGS protocols completos
- [ ] Enter/Leave/Start/Result
- [ ] Jackpot notifications

---

## FASE 6: Clan System Completo

### 6A. Clan Server Integration
**Archivos**: Crear `ModuleClanServer.h/.cpp`
- [ ] Conexión al ClanServer (o implementar directamente en DataServer)
- [ ] Clan creation/dissolution con validación (nivel mínimo, GP cost)
- [ ] Member management: join requests, accept/deny, kick, authority levels
- [ ] Clan mark/name/notice updates
- [ ] Clan EXP and ranking

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
- [ ] Friend online/offline notifications
- [ ] Whisper message routing entre servidores
- [ ] Note system (send/receive/delete/read)
- [ ] Lobby enter/leave notifications a amigos

### 7B. Lobby Chat Completo
**Archivos**: `GameSessionChannel.cpp`
- [ ] `PROTOCOL_BASE_CHATTING_REQ/ACK` - Chat por canal
- [ ] Lobby user list con double-buffering
- [ ] User invite from lobby
- [ ] Megaphone (broadcast chat)

### 7C. GM Chat
**Archivos**: Crear `GameSessionGMChat.cpp`
- [ ] `PROTOCOL_VALUE_GMCHAT (0x1A00)` - GM → User messaging
- [ ] Admin commands via chat

---

## FASE 8: QuickJoin System

**Archivos**: `GameSessionChannel.cpp` (OnQuickJoinRoomReq ya existe, expandir)
- [ ] `PROTOCOL_VALUE_QUICKJOIN (0x1500)` handlers
- [ ] `SearchQuickJoinRoom()`: buscar rooms compatibles (modo, mapa, jugadores)
- [ ] Criterios: mode match, not full, not password-protected, not in battle (unless inter-enter)
- [ ] Random selection entre candidatos

---

## FASE 9: Anti-Cheat & Security

### 9A. Packet Validation
**Archivos**: `GameSession.cpp`
- [ ] Packet encryption validation con XOR key
- [ ] Packet replay detection
- [ ] Protocol rate limiting (max packets per second)
- [ ] State validation: rechazar packets que no corresponden al GAME_TASK actual
- [ ] Timeout por estado: 30s para NOT_LOGIN, 600s para LOGIN, 120s para NORMAL, 3600s para CHANNEL

### 9B. GameGuard Stub
- [ ] Stub para GameGuard handshake (para que el cliente no se desconecte)
- [ ] `PROTOCOL_BASE_GAMEGUARD_REQ/ACK` - Responder con datos dummy válidos

### 9C. Cheat Detection
**Archivos**: Crear `GameSessionCheat.cpp`
- [ ] `PROTOCOL_VALUE_CHEAT (0x1300)` handlers
- [ ] Speed hack detection (comparing client-reported time vs server time)
- [ ] Damage validation
- [ ] Position validation

---

## FASE 10: EXP/Point/Rank System

### 10A. Battle Rewards
**Archivos**: `Room.cpp`, `GameSessionBattle.cpp`
- [ ] EXP calculation: base + kills*multiplier + headshot bonus + win bonus
- [ ] Point (GP) calculation: base + kills*multiplier + win bonus
- [ ] Boost event support: multiply EXP/GP by event multiplier
- [ ] Bonus items: EXP boost items, Point boost items from inventory
- [ ] Save results to DataServer after each match

### 10B. Rank System
**Archivos**: `GameSession.cpp`
- [ ] Rank-up check after EXP update
- [ ] Rank-up items reward (from `m_aRankUpItem[MAX_RANK_COUNT]`)
- [ ] `PROTOCOL_BASE_RANK_UP_ACK` notification
- [ ] Initial rank-up for new players

---

## FASE 11: Admin/GM Tools

### 11A. GM Commands
**Archivos**: Crear `GameSessionGMChat.cpp`
- [ ] `GM_KickUser_U` - Kick player from room
- [ ] `GM_ExitUser_U` - Force disconnect
- [ ] `GM_BlockUser_U` - Ban with comment
- [ ] `GM_DestroyRoom_U` - Force destroy room
- [ ] `GMPause` / `GMResume` - Pause/resume battle
- [ ] Damage console for debugging

### 11B. Control Server Integration
- [ ] Server announce (normal, emergency, sliding, chat notices)
- [ ] Server start/stop commands
- [ ] Config reload
- [ ] User kick via admin tool
- [ ] Room list/user list queries

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
- [ ] Battle log con unique number tracking
- [ ] ZLog integration para persistent logging
- [ ] Performance metrics: packet count, average CCU, room count

---

## FASE 13: ConnectServer Completo

- [ ] Auth token generation y validación
- [ ] Server list transmission al cliente
- [ ] Load balancing: redirect clientes al server con menos carga
- [ ] Server heartbeat monitoring

---

## FASE 14: Miscellaneous

### 14A. Attendance/Daily Gift System
- [ ] Daily login tracking
- [ ] Reward items por día consecutivo
- [ ] Monthly reset

### 14B. Boost Events
- [ ] Event schedule loading
- [ ] EXP/Point multipliers durante eventos
- [ ] `PROTOCOL_BASE_BOOSTEVENT_INFO_ACK`

### 14C. Item Durability
- [ ] Durability decrease per battle
- [ ] Repair cost calculation
- [ ] Item break notification

### 14D. Random Map System
- [ ] Random map selection per channel
- [ ] Map rotation config

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
| Base (0x0200) | ~80 | 8 | 10% |
| Auth (0x0300) | ~80 | 7 | 9% |
| Shop (0x0400) | ~40 | 5 | 13% |
| Clan (0x0700) | ~60 | 9 | 15% |
| Clan Match (0x0800) | ~20 | 0 | 0% |
| Lobby (0x0C00) | ~10 | 5 | 50% |
| Inventory (0x0D00) | ~10 | 3 | 30% |
| RS/IGS (0x0E00) | ~10 | 5 | 50% |
| Room (0x0F00) | ~30 | 14 | 47% |
| Battle (0x1000) | ~40 | 10 | 25% |
| Medal (0x1200) | ~10 | 7 | 70% |
| Cheat (0x1300) | ~10 | 0 | 0% |
| Gacha (0x1400) | ~10 | 5 | 50% |
| QuickJoin (0x1500) | ~5 | 1 | 20% |
| Skill (0x1700) | ~5 | 4 | 80% |
| Char (0x1800) | ~5 | 3 | 60% |
| MyInfo (0x1900) | ~5 | 0 | 0% |
| GMChat (0x1A00) | ~5 | 0 | 0% |
| ClanWar (0x1B00) | ~20 | 0 | 0% |
| **TOTAL** | **~460** | **~87** | **~19%** |
