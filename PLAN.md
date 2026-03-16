# Plan de Implementación - Piercing Blow Server Remake

> **Última actualización**: 2026-03-16 (post Batch 20)

---

## Estado General del Proyecto

| Servidor | Archivos | Líneas | Estado |
|----------|----------|--------|--------|
| **GameServer** | 56 | 31,686 | Activo - 443 dispatch cases |
| **DataServer** | 22 | 3,372 | Completo - PostgreSQL |
| **BattleServer** | 17 | 1,846 | Completo - UDP relay |
| **ConnectServer** | 10 | 1,068 | Completo - Auth + registry |
| **ServerCommon** | 12 | 2,054 | Completo - InterServerProtocol |
| **S2MO** | - | - | Completo - RSA/AES/XOR |
| **i3Server** | - | - | Completo - IOCP framework |
| **TOTAL** | **117+** | **~40,026** | |

### Cobertura de Protocolos

**443 dispatch cases implementados de ~529 relevantes (~84%)**

(605 totales en ProtocolDef.h - ~76 deprecated/server-only = ~529 relevantes)

| Categoría | Rango | En ProtocolDef | Implementados | % |
|-----------|-------|----------------|---------------|---|
| Login | 0x0100 | 1 | 1 | 100% |
| Base (info/channel/quest/medal/map/title/attendance) | 0x0200 | 86 | 91* | ~100% |
| Auth (social/friends/whisper/block) | 0x0300 | 78 | 53 | 68% |
| Shop (buy/gift/capsule/repair) | 0x0400 | 5+24** | 29 | 100% |
| Clan CS_* (create/join/member/mark) | 0x0700 | 87*** | 57 | 66% |
| Clan Match CS_MATCH_* | 0x0800 | 31 | 24 | 77% |
| Community | 0x0B00 | 2 | 1 | 50% |
| Lobby | 0x0C00 | 21 | 20 | 95% |
| Inventory | 0x0D00 | 3 | 3 | 100% |
| RS/IGS/FieldShop | 0x0E00 | 13 | 13 | 100% |
| Room | 0x0F00 | 46 | 42 | 91% |
| Battle | 0x1000 | 97 | 52 | 54% |
| Cheat | 0x1300 | 12 | 12 | 100% |
| Gacha | 0x1400 | 6 | 6 | 100% |
| QuickJoin | 0x1500 | 2 | 2 | 100% |
| Skill | 0x1700 | 3 | 3 | 100% |
| Char/New | 0x1800 | 14 | 6 | 43% |
| MyInfo | 0x1900 | 2 | 2 | 100% |
| GMChat | 0x1A00 | 5 | 4 | 80% |
| Clan War | 0x1B00 | 25 | 22 | 88% |
| Messenger | N/A | 38 | 24 | 63% |

\* Base tiene más dispatch cases que REQs porque incluye SET_*/GET_*/INIT_*/DELETE_*/PERIOD_* protocolos
\** Auth shop protocols están divididos entre PROTOCOL_AUTH_SHOP_* y PROTOCOL_SHOP_*
\*** CS_ total (118) menos CS_MATCH_ (31) = 87 clan-only

### Archivos GameServer más grandes

| Archivo | Líneas | Contenido |
|---------|--------|-----------|
| GameSession.cpp | 3,773 | Switch dispatch (443 cases) + base/login handlers |
| Room.cpp | 2,748 | Room logic, game modes, state machine |
| GameSessionClan.cpp | 2,117 | 40+ clan handlers (CS_* protocols) |
| GameSessionShop.cpp | 1,967 | Shop/buy/gift/roulette handlers |
| GameSessionBattle.cpp | 1,885 | Battle flow, death, respawn, mission |
| GameSessionClanWar.cpp | 1,640 | Clan war + 16 clan match handlers |
| GameSessionRoom.cpp | 1,592 | Room operations, kick vote, observer |
| GameSessionSocial.cpp | 1,582 | Friends, whisper, notes, messenger |
| GameSessionChannel.cpp | 1,313 | Channel/lobby, quickjoin, map data |
| GameSessionGM.cpp | 1,244 | GM commands, admin tools |
| GameSession.h | 932 | Session class declaration |

---

## Fases Completadas

### FASE 0: Infraestructura de Build [COMPLETADA]
- ServerCommon.props compartido para los 4 servidores
- Los 4 vcxproj linkan contra i3Server.dll y S2MO.dll
- Win32 + x64 configurations

### FASE 1: Battle Flow [COMPLETADA]
- Room state machine completa (COUNTDOWN_R → LOADING → RENDEZVOUS → PRE_BATTLE → COUNTDOWN_B → BATTLE → BATTLE_RESULT)
- Death/kill con multi-kill tracking, headshots, assists
- Respawn system con timers configurables
- Battle result screen con stats y recompensas
- BattleServer integration (IS_BATTLE_CREATE, UDP IP/port relay)

### FASE 2: Game Modes [COMPLETADA]
- Deathmatch, Bomb (install/defuse), Annihilation, Destroy, Defence
- Escape (VIP), CrossCount, Convoy, Challenge/AI
- Round-based logic con ATK/DEF swap

### FASE 3: Room Protocols [COMPLETADA]
- Room invite, observer, kick vote, ping
- Double-buffered room info, FillRoomInfoBasic completo

### FASE 4: Protocolos Base [COMPLETADA]
- Login flow, system info, options save/load
- Map/stage data (version, list, rules, matching)
- User info (record, basic, all, detail)
- Title system (equip/release/change)

### FASE 5: Shop System [COMPLETADA]
- Shop catalog, purchase, gift, capsule/gacha
- Roulette shop con weighted tiers
- Field shop, repair, durability

### FASE 6: Clan System [COMPLETADA]
- Clan create/dissolve, member management, mark/notice/intro
- Clan match: team create/join/fight request/accept/chat
- Clan war 1.5: seasons, scoring, mercenary stubs

### FASE 7: Social & Community [COMPLETADA]
- Friends (add/delete/accept/info), whisper, block
- Note system (send/receive/delete)
- Messenger protocol aliases
- GM chat + admin commands

### FASE 8: QuickJoin [COMPLETADA]
- Search compatible rooms, random selection

### FASE 9: Anti-Cheat & Security [COMPLETADA]
- Packet validation, replay detection, rate limiting
- State validation, timeouts por estado
- GameGuard stub, cheat detection handlers

### FASE 10: EXP/Point/Rank [COMPLETADA]
- Battle rewards (EXP/GP calculation, boost events)
- Rank-up system con items reward

### FASE 11: Admin/GM Tools [COMPLETADA]
- Kick, exit, block, destroy room, pause/resume
- Server announce, shutdown, config reload

### FASE 12: Performance [COMPLETADA]
- Per-channel mutexes, ring buffers, pre-allocated pools
- Battle logging, ZLog, performance metrics

### FASE 13: ConnectServer [COMPLETADA]
- Auth tokens, server list, load balancing, heartbeat

### FASE 14: Miscellaneous [COMPLETADA]
- Attendance/daily gift, boost events
- Item durability, random map rotation

---

## Trabajo Actual: Cobertura de Protocolos

### Historial de Batches (protocol handler coverage)

| Batch | Commit | Handlers | Lines | Total Cases | Cobertura |
|-------|--------|----------|-------|-------------|-----------|
| 15 | ec3a3f76 | +34 | +1,092 | ~310 | ~52% |
| 16 | 56ede0ab | +34 | +1,476 | ~344 | ~58% |
| 17 | 3b3082e8 | +35 | +1,255 | ~348 | ~59% |
| 18 | 5e18339c | +36 | +1,143 | ~385 | ~64% |
| 19 | 03164304 | +37 | +1,539 | 385 | ~65% |
| 20 | 7d187083 | +56 | +1,594 | 441→443 | ~84% |

### Protocolos Pendientes (~86 restantes)

#### Alta Prioridad - Battle (0x1000) ~45 pendientes
El gap más grande. Muchos son protocolos de sub-sistemas de batalla:
- `PROTOCOL_BATTLE_MISSION_*` (bomb, object, NPC sub-protocols)
- `PROTOCOL_BATTLE_WEAPON_*` (pickup, change, special)
- `PROTOCOL_BATTLE_ITEM_*` (use item in battle)
- `PROTOCOL_BATTLE_VEHICLE_*` (vehicle mount/dismount)
- `PROTOCOL_BATTLE_SYNC_*` (position sync, state sync)
- `PROTOCOL_BATTLE_SPECIAL_*` (special kills, streaks)

#### Media Prioridad - Auth (0x0300) ~25 pendientes
- `PROTOCOL_AUTH_COUPON_*` (coupon redemption)
- `PROTOCOL_AUTH_EVENT_*` (event participation)
- `PROTOCOL_AUTH_PCR_*` (PC room features)
- Algunos `PROTOCOL_AUTH_SHOP_*` variantes menores

#### Media Prioridad - Clan CS_* (0x0700) ~30 pendientes
- `PROTOCOL_CS_USER_*` (muchos son server-to-server internos)
- `PROTOCOL_CS_RECORD_*` (clan records, server-only)
- `PROTOCOL_CS_MEMBER_INFO_*` (server-only)
- Algunos client-facing restantes

#### Baja Prioridad - Messenger ~14 pendientes
- `PROTOCOL_MESSENGER_LOGIN/LOGOUT` (internal)
- `PROTOCOL_MESSENGER_ENTER_USER/LEAVE_USER` (internal)
- Algunos protocolos de estado y notificación

#### Baja Prioridad - Char/New (0x1800) ~8 pendientes
- `PROTOCOL_NEW_*` protocolos misceláneos
- `PROTOCOL_CHAR_*` character management adicional

#### Baja Prioridad - Room (0x0F00) ~4 pendientes
- `PROTOCOL_ROOM_GM_*` variantes adicionales

#### Baja Prioridad - CS_MATCH (0x0800) ~7 pendientes
- `PROTOCOL_CS_MATCH_TEAM_*` (context/all team variants)
- `PROTOCOL_CS_MATCH_FIGHT_ACCECT_*` (accept variants)
- `PROTOCOL_CS_MATCH_CHATING_*` (chat)

---

## Fase 8 del Plan Maestro (GameServer ↔ DataServer Sync)

> Del plan maestro `zazzy-mixing-creek.md` - pendientes de implementación

### 8.1 - Shop DB Table [PENDIENTE]
- Tabla `pb_shop_items` en DataServer
- Protocolo `IS_SHOP_LIST_REQ/ACK` inter-servidor
- `ModuleDBGameData::LoadShopItems()`

### 8.2 - ModuleDataServer Sync [PENDIENTE - CRÍTICO]
- 15 nuevos métodos Request en ModuleDataServer (equipment, medal, attendance, skill, quest, clan, friends, block, shop)
- 16 response handlers correspondientes
- Actualmente muchos subsistemas operan in-memory sin persistencia a DB

### 8.3 - Fix Nick Callbacks [PENDIENTE]
- `OnCreateNickAck()` / `OnCheckNickAck()` - callback a session con resultado
- Métodos `OnNickCreated()`, `SendCreateNickAck()`, `SendCheckNickAck()` en GameSession

### 8.4 - Battle End → Save Stats [PENDIENTE - CRÍTICO]
- `OnBattleEndNotify()` debe guardar stats via DataServer
- `RequestStatsSave()`, `RequestPlayerSave()` post-batalla

### 8.5 - BattleServer UDP Integration Fix [PENDIENTE]
- Room debe almacenar BattleServer UDP IP/port (`m_szBattleUdpIP`, `m_ui16BattleUdpPort`)
- `OnBattleCreateAck()` → `Room::SetBattleInfo()`
- `OnBattlePreStartBattleReq()` → usar datos de Room en vez de hardcoded

### 8.6 - Room Timer System [PENDIENTE]
- `Room::UpdateBattleTimer()` - check time limit expiry
- Auto-end battle cuando tiempo expira
- Integrar en `RoomManager::OnUpdate()`

### 8.7 - Shop Catalog Dynamic Loading [PENDIENTE]
- `ShopManager::LoadFromDataServer()` reemplaza static `s_ShopCatalog[]`
- `OnShopListReceived()` callback

### 8.8 - Config-Driven Settings [PENDIENTE]
- `config.ini` secciones [Economy], [Battle], [Channels]
- `GameContextMain` carga valores desde INI
- Reemplazar hardcoded values (GP costs, level requirements, time limits)

---

## Próximos Pasos Recomendados

### Opción A: Continuar cobertura de protocolos (Batch 21+)
- Foco en **Battle (0x1000)** - el gap más grande (~45 pendientes)
- Luego **Auth (0x0300)** y **Clan CS_* (0x0700)**
- Meta: llegar a 95%+ cobertura

### Opción B: Implementar Fase 8 (DataServer sync)
- Hacer que los datos realmente persistan en PostgreSQL
- Crítico para producción pero no para testing del cliente

### Opción C: Refinamiento de sistemas existentes
- Battle modes: agregar variantes faltantes (vehicle, special weapons)
- Clan match: completar mercenary invite/accept flow
- Shop: migrar de catálogo hardcoded a DB

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
     └────────┬────────┘    │     └────────┬────────┘
              │             │              │
              │    ┌────────▼────────┐     │
              │    │  DataServer     │     │
              ├────►  :40100 (TCP)   ◄─────┤
              │    │  (PostgreSQL)   │     │
              │    └─────────────────┘     │
              │                            │
     ┌────────▼────────┐         ┌────────▼────────┐
     │ BattleServer #1 │         │ BattleServer #N │
     │ :40200 (TCP)    │         │ :402XX (TCP)    │
     │ :41000+ (UDP)   │         │ :410XX+ (UDP)   │
     └─────────────────┘         └─────────────────┘
```

### Flujo de Conexión
1. Cliente → ConnectServer: RSA handshake + login
2. ConnectServer → Cliente: Server list + auth token
3. Cliente → GameServer: Validate token → enter channel → lobby → room
4. GameServer → BattleServer: Create battle room → UDP relay
5. GameServer → DataServer: Load/save player data
