-- ============================================
-- Piercing Blow Remake - DataServer PostgreSQL Schema
-- ============================================
-- Ejecutar: psql -U pb_server -d piercing_blow -f schema.sql
-- ============================================
-- Sistemas incluidos:
--   1.  pb_users (extendido)          - User Info completo
--   2.  pb_inventory                  - Inventario
--   3.  pb_stats                      - Estadisticas globales
--   4.  pb_equipment (extendido)      - Equipamiento con IDX linking
--   5.  pb_medals                     - Medallas individuales
--   6.  pb_medal_sets                 - Medal Sets completos
--   7.  pb_attendance                 - Asistencia diaria
--   8.  pb_skills                     - Skills por clase
--   9.  pb_quests                     - Quest cardsets
--   10. pb_account_cosmetics          - Cosmeticos de cuenta
--   11. pb_mode_records               - Records por modo de juego
--   12. pb_daily_records              - Records diarios
--   13. pb_maps / pb_map_modes        - Mapas desde DB
--   14. pb_map_stats                  - Estadisticas de mapas
--   15. pb_rankings_individual        - Rankings periodicos (individual)
--   16. pb_rankings_clan              - Rankings periodicos (clan)
--   17. pb_server_config              - Configuracion del servidor
--   18. pb_notices                    - Avisos/noticias del servidor
--   19. pb_boost_events               - Eventos de boost EXP/GP
--   20. pb_shop_items (extendido)     - Tienda extendida
--   21. pb_shop_prices                - Precios multiples por item
--   22. pb_shop_matching              - Matching goods->items
--   23. pb_shop_coupons               - Sistema de cupones
--   24. pb_gifts                      - Gift Delivery System
--   25. pb_clans (extendido)          - Clanes extendidos
--   26. pb_clan_members (extendido)   - Miembros con stats
--   27. pb_clan_requests              - Solicitudes de ingreso
--   28. pb_clan_items                 - Items de clan
--   29. pb_clan_season                - Clan Season/Rating
--   30. pb_mercenary                  - Sistema de mercenarios
--   31. pb_friends                    - Amigos
--   32. pb_blocks                     - Bloqueos
--   33. pb_notes                      - Notas/correo
--   34. pb_ban_list                   - Lista de bans
-- ============================================

-- Crear base de datos (ejecutar como superuser si no existe)
-- CREATE DATABASE piercing_blow OWNER pb_server;

-- ============================================
-- 1. Tabla principal de usuarios (EXTENDIDA)
-- Basado en: PB_GT_UUserInfo + AccountItemData del original
-- ============================================
CREATE TABLE IF NOT EXISTS pb_users (
    uid             BIGSERIAL       PRIMARY KEY,
    username        VARCHAR(64)     UNIQUE NOT NULL,
    password_hash   VARCHAR(128)    NOT NULL,
    nickname        VARCHAR(64)     UNIQUE,
    auth_level      SMALLINT        DEFAULT 0,          -- 0=normal, 1=GM, 2=admin
    is_banned       BOOLEAN         DEFAULT FALSE,
    level           INTEGER         DEFAULT 1,
    exp             BIGINT          DEFAULT 0,
    cash            INTEGER         DEFAULT 0,           -- Moneda premium
    gp              INTEGER         DEFAULT 10000,       -- Game Points
    coin            INTEGER         DEFAULT 0,           -- Moneda terciaria (original: i32Coin)
    rank_id         INTEGER         DEFAULT 0,
    keep_rank       INTEGER         DEFAULT 0,           -- Rango mantenido (original: i32KeepRank)
    clan_id         INTEGER         DEFAULT 0,
    active_class    SMALLINT        DEFAULT 0,           -- GameCharaClass (0=Assault)
    -- Campos extendidos del original (PB_GT_UUserInfo)
    connect_count   INTEGER         DEFAULT 0,           -- Veces conectado (original: i32ConnectCNT)
    connect_time    INTEGER         DEFAULT 0,           -- Tiempo total conectado en seg (original: i32ConnectTime)
    total_battle_time INTEGER       DEFAULT 0,           -- Tiempo total en batalla (original: i32TotalBattleTime)
    esport_level    SMALLINT        DEFAULT 0,           -- Nivel e-sport (original: ui8ESportLevel)
    tutorial_done   SMALLINT        DEFAULT 0,           -- Tutorial completado (original: ui8Tutorialing)
    guide_complete  BIGINT          DEFAULT 0,           -- Bitmask guias completadas (original: ui64GuideComplete)
    report_count    SMALLINT        DEFAULT 0,           -- Reportes recibidos (original: ui8ReportCount)
    abusing_count   INTEGER         DEFAULT 0,           -- Infracciones (original: i32AbusingCNT)
    special_ability SMALLINT        DEFAULT 0,           -- Habilidad especial (original: ui8SpecialAbility)
    vote_count      SMALLINT        DEFAULT 0,           -- Votos disponibles (original: VoteCount)
    -- Timestamps extendidos
    created_at      TIMESTAMP       DEFAULT NOW(),
    last_login      TIMESTAMP,
    last_logout     TIMESTAMP,
    last_buy_cash   TIMESTAMP,                           -- Ultima compra con cash (original: sdtLastBuyCash)
    chat_block_date TIMESTAMP                            -- Fecha hasta bloqueado de chat (original: sdtChatBlockDate)
);

-- ============================================
-- 10. Cosmeticos de cuenta
-- Basado en: AccountItemData del original
-- ============================================
CREATE TABLE IF NOT EXISTS pb_account_cosmetics (
    uid                 BIGINT      PRIMARY KEY REFERENCES pb_users(uid) ON DELETE CASCADE,
    nickname_color      SMALLINT    DEFAULT 0,       -- Color del nickname (original: NicknameColor)
    crosshair_color     SMALLINT    DEFAULT 0,       -- Color del crosshair (original: CrossHairColor)
    chatting_color      SMALLINT    DEFAULT 0,       -- Color del chat (original: ChattingColor)
    disguise_rank       INTEGER     DEFAULT 0,       -- Rango disfrazado (original: DisguiseRank)
    fake_nick           VARCHAR(50) DEFAULT ''       -- Nick falso (original: FakeNick)
);

-- ============================================
-- 2. Inventario de jugadores
-- ============================================
CREATE TABLE IF NOT EXISTS pb_inventory (
    id              BIGSERIAL       PRIMARY KEY,
    uid             BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    item_id         INTEGER         NOT NULL,
    item_count      INTEGER         DEFAULT 1,
    slot_idx        INTEGER         DEFAULT -1,
    item_type       SMALLINT        DEFAULT 0,           -- GameItemAttrType: 0=permanent, 1=count, 2=period
    item_arg        INTEGER         DEFAULT 0,            -- For count: remaining. For period: expire timestamp
    expire_date     TIMESTAMP,                           -- NULL = permanente
    is_equipped     BOOLEAN         DEFAULT FALSE
);

CREATE INDEX IF NOT EXISTS idx_inventory_uid ON pb_inventory(uid);
CREATE INDEX IF NOT EXISTS idx_inventory_item ON pb_inventory(uid, item_id);

-- ============================================
-- 3. Estadisticas globales de jugadores
-- ============================================
CREATE TABLE IF NOT EXISTS pb_stats (
    uid             BIGINT          PRIMARY KEY REFERENCES pb_users(uid) ON DELETE CASCADE,
    kills           INTEGER         DEFAULT 0,
    deaths          INTEGER         DEFAULT 0,
    headshots       INTEGER         DEFAULT 0,
    wins            INTEGER         DEFAULT 0,
    losses          INTEGER         DEFAULT 0,
    draws           INTEGER         DEFAULT 0,           -- Empates (nuevo, del original)
    disconnects     INTEGER         DEFAULT 0,           -- Desconexiones (nuevo, del original)
    total_exp_earned BIGINT         DEFAULT 0,
    total_gp_earned  BIGINT         DEFAULT 0
);

-- ============================================
-- 4. Equipamiento de personaje (EXTENDIDO con IDX linking)
-- Basado en: PB_GT_UCharaInfo del original
-- Mejora: cada slot tiene IDX (link a inventario) + ID (tipo de item)
-- ============================================
CREATE TABLE IF NOT EXISTS pb_equipment (
    uid             BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    chara_slot      SMALLINT        NOT NULL DEFAULT 0,  -- 0-4 (MAX_CHARA_SLOT)
    slot_state      SMALLINT        DEFAULT 0,           -- MultiSlotState
    chara_id        INTEGER         DEFAULT 0,           -- Character ItemID
    -- Weapon slots: IDX (inventory link) + ID (item type)
    -- IDX=0 significa "sin item especifico del inventario" (usa el ID directo)
    weapon_primary_idx  BIGINT      DEFAULT 0,           -- Inventario IDX (original: i64MainWeaponIDX)
    weapon_primary      INTEGER     DEFAULT 0,           -- Item ID (original: i32MainWeaponID)
    weapon_secondary_idx BIGINT     DEFAULT 0,
    weapon_secondary    INTEGER     DEFAULT 0,
    weapon_melee_idx    BIGINT      DEFAULT 0,
    weapon_melee        INTEGER     DEFAULT 0,
    weapon_throw1_idx   BIGINT      DEFAULT 0,
    weapon_throw1       INTEGER     DEFAULT 0,
    weapon_throw2_idx   BIGINT      DEFAULT 0,
    weapon_throw2       INTEGER     DEFAULT 0,
    -- Parts slots: IDX + ID
    parts_chara_idx     BIGINT      DEFAULT 0,
    parts_chara         INTEGER     DEFAULT 0,
    parts_head_idx      BIGINT      DEFAULT 0,
    parts_head          INTEGER     DEFAULT 0,
    parts_face_idx      BIGINT      DEFAULT 0,
    parts_face          INTEGER     DEFAULT 0,
    parts_upper_idx     BIGINT      DEFAULT 0,
    parts_upper         INTEGER     DEFAULT 0,
    parts_lower_idx     BIGINT      DEFAULT 0,
    parts_lower         INTEGER     DEFAULT 0,
    parts_glove_idx     BIGINT      DEFAULT 0,
    parts_glove         INTEGER     DEFAULT 0,
    parts_belt_idx      BIGINT      DEFAULT 0,
    parts_belt          INTEGER     DEFAULT 0,
    parts_holster_idx   BIGINT      DEFAULT 0,
    parts_holster       INTEGER     DEFAULT 0,
    parts_skin_idx      BIGINT      DEFAULT 0,
    parts_skin          INTEGER     DEFAULT 0,
    parts_beret_idx     BIGINT      DEFAULT 0,
    parts_beret         INTEGER     DEFAULT 0,
    -- Campos adicionales del original
    battle_time         INTEGER     DEFAULT 0,           -- Tiempo de batalla con este personaje
    play_time           INTEGER     DEFAULT 0,           -- Tiempo de juego total
    play_time_point     SMALLINT    DEFAULT 0,           -- Puntos por tiempo jugado
    PRIMARY KEY (uid, chara_slot)
);

-- ============================================
-- 5. Medallas individuales de jugadores
-- ============================================
CREATE TABLE IF NOT EXISTS pb_medals (
    uid             BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    medal_idx       SMALLINT        NOT NULL,            -- Medal unique index
    action_id       SMALLINT        DEFAULT 0,           -- GameMedalActionId
    current_count   INTEGER         DEFAULT 0,           -- Progress count
    get_reward      SMALLINT        DEFAULT 0,           -- Bitmask: bit N = level N reward claimed
    PRIMARY KEY (uid, medal_idx)
);

CREATE INDEX IF NOT EXISTS idx_medals_uid ON pb_medals(uid);

-- ============================================
-- 6. Medal Sets completos
-- Basado en: PB_GT_UCompleteMedalSet + PB_GT_UCurrentMedalSet + PB_GT_UMedalBasicInfo
-- ============================================

-- Medal set actual equipado
CREATE TABLE IF NOT EXISTS pb_medal_sets_current (
    uid             BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    set_type        SMALLINT        NOT NULL DEFAULT 0,  -- Tipo de set (original: i8Type)
    medal_set_idx   SMALLINT        DEFAULT 0,           -- Index del set activo
    recv_date       TIMESTAMP,                           -- Cuando se recibio
    medal1_count    SMALLINT        DEFAULT 0,           -- Progreso medalla 1 del set
    medal2_count    SMALLINT        DEFAULT 0,
    medal3_count    SMALLINT        DEFAULT 0,
    medal4_count    SMALLINT        DEFAULT 0,
    medal5_count    SMALLINT        DEFAULT 0,
    medal6_count    SMALLINT        DEFAULT 0,
    get_reward      SMALLINT        DEFAULT 0,           -- Bitmask recompensas
    PRIMARY KEY (uid, set_type)
);

-- Medal sets completados (historico)
CREATE TABLE IF NOT EXISTS pb_medal_sets_complete (
    uid             BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    set_type        SMALLINT        NOT NULL,
    medal_set_idx   SMALLINT        NOT NULL,
    recv_date       TIMESTAMP,
    complete_date   TIMESTAMP,
    medal1_count    SMALLINT        DEFAULT 0,
    medal2_count    SMALLINT        DEFAULT 0,
    medal3_count    SMALLINT        DEFAULT 0,
    medal4_count    SMALLINT        DEFAULT 0,
    medal5_count    SMALLINT        DEFAULT 0,
    medal6_count    SMALLINT        DEFAULT 0,
    get_reward      SMALLINT        DEFAULT 0,
    PRIMARY KEY (uid, set_type, medal_set_idx)
);

-- Medal basic info (notificaciones de medallas y master medal)
CREATE TABLE IF NOT EXISTS pb_medal_info (
    uid             BIGINT          PRIMARY KEY REFERENCES pb_users(uid) ON DELETE CASCADE,
    notify_type1    SMALLINT        DEFAULT 0,
    notify_idx1     SMALLINT        DEFAULT 0,
    notify_count1   SMALLINT        DEFAULT 0,
    notify_type2    SMALLINT        DEFAULT 0,
    notify_idx2     SMALLINT        DEFAULT 0,
    notify_count2   SMALLINT        DEFAULT 0,
    notify_type3    SMALLINT        DEFAULT 0,
    notify_idx3     SMALLINT        DEFAULT 0,
    notify_count3   SMALLINT        DEFAULT 0,
    notify_type4    SMALLINT        DEFAULT 0,
    notify_idx4     SMALLINT        DEFAULT 0,
    notify_count4   SMALLINT        DEFAULT 0,
    master_medal    SMALLINT        DEFAULT 0             -- Master medal index
);

-- ============================================
-- 7. Asistencia diaria (attendance)
-- ============================================
CREATE TABLE IF NOT EXISTS pb_attendance (
    uid             BIGINT          PRIMARY KEY REFERENCES pb_users(uid) ON DELETE CASCADE,
    total_days      INTEGER         DEFAULT 0,
    current_streak  INTEGER         DEFAULT 0,
    last_attend_date INTEGER        DEFAULT 0,           -- YYYYMMDD format
    attend_data     BYTEA                                -- 30 bytes, ui8Days[30]
);

-- ============================================
-- 8. Skills de personaje (por clase)
-- ============================================
CREATE TABLE IF NOT EXISTS pb_skills (
    uid             BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    class_id        SMALLINT        NOT NULL,            -- GameCharaClass (0-4)
    main_levels     BYTEA           NOT NULL DEFAULT E'\\x0000000000',   -- 5 bytes, level per main skill
    assist_levels   BYTEA           NOT NULL DEFAULT E'\\x00000000',     -- 4 bytes, level per assist skill
    common_levels   BYTEA           NOT NULL DEFAULT E'\\x0000000000',   -- 5 bytes, level per common skill
    skill_points    SMALLINT        DEFAULT 10,          -- Available skill points
    PRIMARY KEY (uid, class_id)
);

-- ============================================
-- 9. Quest progress (cardsets activos)
-- ============================================
CREATE TABLE IF NOT EXISTS pb_quests (
    uid             BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    set_index       SMALLINT        NOT NULL,            -- 0-3 (MAX_CARDSET_PER_USER)
    set_type        SMALLINT        DEFAULT 0,           -- GameQuestCardSetType
    active_card     SMALLINT        DEFAULT 0,           -- Active card index (0-9)
    quest_data      BYTEA,                               -- Serialized quest progress
    PRIMARY KEY (uid, set_index)
);

CREATE INDEX IF NOT EXISTS idx_quests_uid ON pb_quests(uid);

-- ============================================
-- 11. Records por Modo de Juego
-- Basado en: PB_GT_UUserRecord del original
-- Cada modo de juego tiene stats separados
-- record_type: 0=Deathmatch, 1=Bomb, 2=Destruction, 3=Annihilation,
--              4=Sniper, 5=Shotgun, 6=AI, 7=Knuckle, 8=HeadHunter, 9=HeadKiller
-- ============================================
CREATE TABLE IF NOT EXISTS pb_mode_records (
    uid             BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    record_type     SMALLINT        NOT NULL,            -- Modo de juego (0-9)
    matches         INTEGER         DEFAULT 0,           -- Partidas jugadas
    wins            INTEGER         DEFAULT 0,
    losses          INTEGER         DEFAULT 0,
    draws           INTEGER         DEFAULT 0,
    kills           INTEGER         DEFAULT 0,
    deaths          INTEGER         DEFAULT 0,
    headshots       INTEGER         DEFAULT 0,
    disconnects     INTEGER         DEFAULT 0,           -- Desconexiones en este modo
    daily_matches   INTEGER         DEFAULT 0,           -- Matches del dia (original: i32dMatch)
    daily_kills     INTEGER         DEFAULT 0,           -- Kills del dia (original: i32dKill)
    PRIMARY KEY (uid, record_type)
);

CREATE INDEX IF NOT EXISTS idx_mode_records_uid ON pb_mode_records(uid);

-- ============================================
-- 12. Records Diarios
-- Basado en: PB_GT_UUserDailyRecord del original
-- Se resetea cada dia, usado para daily rewards y limites
-- ============================================
CREATE TABLE IF NOT EXISTS pb_daily_records (
    uid             BIGINT          PRIMARY KEY REFERENCES pb_users(uid) ON DELETE CASCADE,
    record_date     INTEGER         DEFAULT 0,           -- YYYYMMDD, cuando se actualizo por ultima vez
    wins            INTEGER         DEFAULT 0,
    losses          INTEGER         DEFAULT 0,
    draws           INTEGER         DEFAULT 0,
    kills           INTEGER         DEFAULT 0,
    deaths          INTEGER         DEFAULT 0,
    headshots       INTEGER         DEFAULT 0,
    exp_earned      INTEGER         DEFAULT 0,           -- EXP ganada hoy
    gp_earned       INTEGER         DEFAULT 0,           -- GP ganado hoy
    play_time       INTEGER         DEFAULT 0,           -- Segundos jugados hoy
    gift_play_time  INTEGER         DEFAULT 0,           -- Tiempo jugado para daily gift
    got_daily_item  SMALLINT        DEFAULT 0            -- Ya recibio item diario? (original: GetItem)
);

-- ============================================
-- 13. Sistema de Mapas (desde DB, no hardcodeado)
-- Basado en: Map, MapModeMatching, Mode, MapDefault del original
-- ============================================

-- Definicion de mapas
CREATE TABLE IF NOT EXISTS pb_maps (
    stage_uid       SMALLINT        PRIMARY KEY,         -- Stage unique ID
    map_name        VARCHAR(50)     NOT NULL DEFAULT '',
    is_active       BOOLEAN         DEFAULT TRUE
);

-- Modos de juego definidos
CREATE TABLE IF NOT EXISTS pb_game_modes (
    mode_id         SMALLINT        PRIMARY KEY,
    mode_name       VARCHAR(50)     NOT NULL DEFAULT '',
    default_stage   SMALLINT        DEFAULT 0            -- Mapa default para este modo
);

-- Matching: que mapa soporta que modo (con limites)
-- Basado en: MapModeMatching del original
CREATE TABLE IF NOT EXISTS pb_map_mode_matching (
    stage_key       SMALLINT        PRIMARY KEY,         -- Clave unica del matching
    is_open         SMALLINT        DEFAULT 1,           -- 0=cerrado, >0=abierto
    stage_uid       SMALLINT        NOT NULL,            -- FK a pb_maps
    mode_id         SMALLINT        NOT NULL DEFAULT 1,  -- Modo de juego
    max_players     SMALLINT        DEFAULT 16,          -- Max jugadores en esta combinacion
    mark            SMALLINT        DEFAULT 0,           -- Clasificacion/mark
    special_mode    SMALLINT        DEFAULT 0            -- Flag de modo especial
);

CREATE INDEX IF NOT EXISTS idx_map_mode_stage ON pb_map_mode_matching(stage_uid);

-- ============================================
-- 14. Estadisticas de Mapas
-- Basado en: MapStat del original (ZLog DB)
-- ============================================
CREATE TABLE IF NOT EXISTS pb_map_stats (
    reg_date        DATE            NOT NULL DEFAULT CURRENT_DATE,
    map_id          INTEGER         NOT NULL,
    stage_type      INTEGER         NOT NULL DEFAULT 0,  -- Modo de juego
    play_time       BIGINT          DEFAULT 0,           -- Tiempo total jugado (segundos)
    play_rounds     INTEGER         DEFAULT 0,           -- Rondas jugadas
    PRIMARY KEY (reg_date, map_id, stage_type)
);

-- ============================================
-- 15. Rankings Periodicos (Individual)
-- Basado en: RankStatIndividual del original
-- term_type: 0=diario, 1=semanal, 2=mensual
-- ============================================
CREATE TABLE IF NOT EXISTS pb_rankings_individual (
    rank_date       DATE            NOT NULL DEFAULT CURRENT_DATE,
    term_type       SMALLINT        NOT NULL DEFAULT 0,  -- 0=daily, 1=weekly, 2=monthly
    rank_position   SMALLINT        NOT NULL,
    uid             BIGINT          NOT NULL,
    nickname        VARCHAR(50)     DEFAULT '',
    level           INTEGER         DEFAULT 0,
    exp             BIGINT          DEFAULT 0,
    kills           INTEGER         DEFAULT 0,
    deaths          INTEGER         DEFAULT 0,
    headshots       INTEGER         DEFAULT 0,
    created_at      TIMESTAMP       DEFAULT NOW(),
    is_sent         BOOLEAN         DEFAULT FALSE,       -- Ya se notifico al jugador?
    PRIMARY KEY (rank_date, term_type, rank_position)
);

CREATE INDEX IF NOT EXISTS idx_rankings_ind_uid ON pb_rankings_individual(uid);

-- ============================================
-- 16. Rankings Periodicos (Clan)
-- Basado en: RankStatClan del original
-- ============================================
CREATE TABLE IF NOT EXISTS pb_rankings_clan (
    rank_date       DATE            NOT NULL DEFAULT CURRENT_DATE,
    term_type       SMALLINT        NOT NULL DEFAULT 0,
    rank_position   SMALLINT        NOT NULL,
    clan_id         INTEGER         NOT NULL,
    clan_name       VARCHAR(50)     DEFAULT '',
    clan_exp        BIGINT          DEFAULT 0,
    wins            INTEGER         DEFAULT 0,
    losses          INTEGER         DEFAULT 0,
    member_count    INTEGER         DEFAULT 0,
    created_at      TIMESTAMP       DEFAULT NOW(),
    is_sent         BOOLEAN         DEFAULT FALSE,
    PRIMARY KEY (rank_date, term_type, rank_position)
);

CREATE INDEX IF NOT EXISTS idx_rankings_clan_id ON pb_rankings_clan(clan_id);

-- ============================================
-- 17. Configuracion del Servidor
-- Basado en: TPBC_Basic + TPBF_Basic del original
-- Una sola fila con toda la configuracion del server
-- ============================================
CREATE TABLE IF NOT EXISTS pb_server_config (
    config_key      VARCHAR(64)     PRIMARY KEY,
    config_value    VARCHAR(255)    DEFAULT '',
    config_type     SMALLINT        DEFAULT 0,           -- 0=string, 1=int, 2=bool
    description     VARCHAR(128)    DEFAULT ''
);

-- Insertar configuracion default
INSERT INTO pb_server_config (config_key, config_value, config_type, description) VALUES
    ('new_user_gp', '10000', 1, 'GP inicial para nuevos usuarios'),
    ('clan_create_rank', '5', 1, 'Rango minimo para crear clan'),
    ('clan_create_cost', '50000', 1, 'Costo GP para crear clan'),
    ('clan_default_max_members', '10', 1, 'Miembros max default de clan'),
    ('clan_max_members_cap', '100', 1, 'Maximo absoluto de miembros'),
    ('quest_event_enabled', '0', 1, 'Evento de quest activo'),
    ('field_shop_enabled', '1', 1, 'Field shop habilitado'),
    ('local_code', '0', 1, 'Codigo de region/localizacion')
ON CONFLICT (config_key) DO NOTHING;

-- ============================================
-- 18. Avisos/Noticias del servidor
-- Basado en: TPBC_Notice del original
-- ============================================
CREATE TABLE IF NOT EXISTS pb_notices (
    id              SERIAL          PRIMARY KEY,
    is_active       BOOLEAN         DEFAULT TRUE,
    repeat_interval INTEGER         DEFAULT 300,         -- Segundos entre repeticiones (0=una vez)
    notice_text     VARCHAR(255)    NOT NULL,
    begin_time      TIMESTAMP       DEFAULT NOW(),
    end_time        TIMESTAMP       DEFAULT NOW() + INTERVAL '7 days'
);

-- ============================================
-- 19. Eventos de Boost (EXP/GP multiplicadores)
-- Basado en: BoostEvent del original
-- ============================================
CREATE TABLE IF NOT EXISTS pb_boost_events (
    id              SERIAL          PRIMARY KEY,
    event_type      INTEGER         DEFAULT 0,           -- Tipo de evento
    start_date      TIMESTAMP       NOT NULL,
    end_date        TIMESTAMP       NOT NULL,
    exp_multiplier  INTEGER         DEFAULT 100,         -- Porcentaje (100=normal, 200=doble)
    gp_multiplier   INTEGER         DEFAULT 100,
    local_code      SMALLINT        DEFAULT 0            -- Region
);

-- ============================================
-- 20. Tienda (EXTENDIDA)
-- Basado en: PB_BT_SShopGoods + PB_BT_SShopItem del original
-- ============================================
CREATE TABLE IF NOT EXISTS pb_shop_items (
    goods_id        SERIAL          PRIMARY KEY,
    item_id         INTEGER         NOT NULL,
    item_type       SMALLINT        DEFAULT 0,           -- GameItemAttrType
    price_gp        INTEGER         DEFAULT 0,           -- 0 = not available for GP
    price_cash      INTEGER         DEFAULT 0,           -- 0 = not available for Cash
    price_coin      INTEGER         DEFAULT 0,           -- Moneda terciaria (nuevo)
    duration        INTEGER         DEFAULT 0,           -- 0 = permanent, seconds
    level_req       INTEGER         DEFAULT 0,           -- Minimum level required
    rank_req        INTEGER         DEFAULT 0,           -- Minimum rank required
    is_active       BOOLEAN         DEFAULT TRUE,
    category        SMALLINT        DEFAULT 0,           -- 0=Primary,1=Secondary,2=Melee,3=Character,4=Parts
    -- Campos extendidos del original
    sale_type       SMALLINT        DEFAULT 0,           -- Tipo de venta (original: SaleType)
    goods_name      VARCHAR(32)     DEFAULT '',          -- Nombre del producto
    description     VARCHAR(255)    DEFAULT '',          -- Descripcion
    mark            SMALLINT        DEFAULT 0            -- Clasificacion/tag visual
);

-- ============================================
-- 21. Precios multiples por item (tiers de precio)
-- Basado en: PB_GT_SShopGoodsPrice del original
-- Permite tener multiples opciones de compra (1 dia, 7 dias, 30 dias, permanente)
-- ============================================
CREATE TABLE IF NOT EXISTS pb_shop_prices (
    id              SERIAL          PRIMARY KEY,
    goods_id        INTEGER         NOT NULL,            -- FK a pb_shop_items
    price_type      SMALLINT        DEFAULT 0,           -- Tipo de precio/duracion
    price_gp        INTEGER         DEFAULT 0,
    price_cash      INTEGER         DEFAULT 0,
    price_coin      INTEGER         DEFAULT 0,
    duration        INTEGER         DEFAULT 0,           -- Duracion en segundos (0=permanente)
    mark            SMALLINT        DEFAULT 0
);

CREATE INDEX IF NOT EXISTS idx_shop_prices_goods ON pb_shop_prices(goods_id);

-- ============================================
-- 22. Matching Goods->Items (bundles, item compuesto)
-- Basado en: PB_BT_SShopMatching del original
-- Un goods_id puede dar multiples items
-- ============================================
CREATE TABLE IF NOT EXISTS pb_shop_matching (
    id              SERIAL          PRIMARY KEY,
    goods_id        INTEGER         NOT NULL,            -- FK a pb_shop_items
    item_id         INTEGER         NOT NULL,            -- Item que se entrega
    item_arg        INTEGER         DEFAULT 0,           -- Argumento del item (cantidad, duracion)
    bonus_arg       INTEGER         DEFAULT 0            -- Bonus adicional
);

CREATE INDEX IF NOT EXISTS idx_shop_matching_goods ON pb_shop_matching(goods_id);

-- ============================================
-- 23. Sistema de Cupones
-- Basado en: PB_BT_SShopCoupon del original
-- ============================================
CREATE TABLE IF NOT EXISTS pb_shop_coupons (
    id              SERIAL          PRIMARY KEY,
    coupon_name     VARCHAR(32)     DEFAULT '',
    coupon_code     VARCHAR(32)     UNIQUE,              -- Codigo que ingresa el jugador
    coupon_type     SMALLINT        DEFAULT 0,
    start_date      INTEGER         DEFAULT 0,           -- YYYYMMDD
    end_date        INTEGER         DEFAULT 0,
    period_arg      INTEGER         DEFAULT 0,           -- Duracion del item entregado
    discount_type   SMALLINT        DEFAULT 0,           -- 0=fixed, 1=percent
    discount_value  INTEGER         DEFAULT 0,
    item_class      SMALLINT        DEFAULT 0,           -- Categoria de item aplicable
    limit_type      SMALLINT        DEFAULT 0,           -- 0=unlimited, 1=per_user, 2=global
    limit_value     INTEGER         DEFAULT 0,           -- Cantidad limite
    times_used      INTEGER         DEFAULT 0            -- Veces usado (para limit global)
);

-- Registro de cupones usados por usuario
CREATE TABLE IF NOT EXISTS pb_shop_coupon_usage (
    uid             BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    coupon_id       INTEGER         NOT NULL,
    used_at         TIMESTAMP       DEFAULT NOW(),
    PRIMARY KEY (uid, coupon_id)
);

-- ============================================
-- 24. Gift Delivery System
-- Basado en: PB_GT_SGiftDeliver + PB_GT_SUserGift del original
-- ============================================
CREATE TABLE IF NOT EXISTS pb_gifts (
    id              BIGSERIAL       PRIMARY KEY,
    receiver_uid    BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    sender_uid      BIGINT          DEFAULT 0,           -- 0 = sistema/admin
    sender_nick     VARCHAR(50)     DEFAULT '',
    goods_id        INTEGER         NOT NULL,            -- ID del producto regalado
    item_id         INTEGER         DEFAULT 0,           -- Item ID directo (si no usa goods)
    is_delivered     BOOLEAN        DEFAULT FALSE,       -- Ya recogido por el jugador?
    gift_type       SMALLINT        DEFAULT 0,           -- 0=player_gift, 1=system, 2=web, 3=event
    message         VARCHAR(125)    DEFAULT '',           -- Mensaje del regalo
    comment         VARCHAR(255)    DEFAULT '',           -- Comentario admin (original: strComment)
    expire_date     TIMESTAMP,                           -- NULL = no expira
    created_at      TIMESTAMP       DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_gifts_receiver ON pb_gifts(receiver_uid, is_delivered);

-- ============================================
-- 25. Clanes (EXTENDIDO)
-- Basado en: TClan del original con season/rating
-- ============================================
CREATE TABLE IF NOT EXISTS pb_clans (
    clan_id         SERIAL          PRIMARY KEY,
    name            VARCHAR(32)     UNIQUE NOT NULL,
    notice          VARCHAR(256)    DEFAULT '',
    intro           VARCHAR(128)    DEFAULT '',
    master_uid      BIGINT          NOT NULL REFERENCES pb_users(uid),
    master_nickname VARCHAR(64)     DEFAULT '',
    member_count    INTEGER         DEFAULT 1,
    max_members     INTEGER         DEFAULT 10,
    clan_exp        INTEGER         DEFAULT 0,
    clan_rank       INTEGER         DEFAULT 0,
    wins            INTEGER         DEFAULT 0,
    losses          INTEGER         DEFAULT 0,
    draws           INTEGER         DEFAULT 0,           -- Empates (nuevo)
    unit            SMALLINT        DEFAULT 0,           -- GameClanUnit
    mark_id         SMALLINT        DEFAULT 0,
    mark_color      SMALLINT        DEFAULT 0,
    is_active       BOOLEAN         DEFAULT TRUE,
    created_at      TIMESTAMP       DEFAULT NOW(),
    -- Campos extendidos para season
    season_wins     INTEGER         DEFAULT 0,           -- Wins de la temporada actual
    season_losses   INTEGER         DEFAULT 0,
    season_draws    INTEGER         DEFAULT 0,
    season_rating   INTEGER         DEFAULT 1000,        -- ELO/rating de temporada
    season_rank     INTEGER         DEFAULT 0            -- Posicion en ranking de temporada
);

CREATE INDEX IF NOT EXISTS idx_clans_master ON pb_clans(master_uid);

-- ============================================
-- 26. Miembros de clan (EXTENDIDO con stats)
-- Basado en: TClanMem del original con stats de batalla
-- ============================================
CREATE TABLE IF NOT EXISTS pb_clan_members (
    clan_id         INTEGER         NOT NULL REFERENCES pb_clans(clan_id) ON DELETE CASCADE,
    uid             BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    member_level    SMALLINT        DEFAULT 3,           -- GameClanMemberLevel: 1=Master,2=Staff,3=Regular
    joined_at       TIMESTAMP       DEFAULT NOW(),
    -- Stats del miembro dentro del clan
    clan_kills      INTEGER         DEFAULT 0,           -- Kills en clan wars
    clan_deaths     INTEGER         DEFAULT 0,
    clan_wins       INTEGER         DEFAULT 0,           -- Partidas de clan ganadas
    clan_losses     INTEGER         DEFAULT 0,
    clan_exp_contrib INTEGER        DEFAULT 0,           -- EXP contribuida al clan
    last_active     TIMESTAMP       DEFAULT NOW(),       -- Ultima vez activo
    PRIMARY KEY (clan_id, uid)
);

CREATE INDEX IF NOT EXISTS idx_clan_members_uid ON pb_clan_members(uid);

-- ============================================
-- 27. Solicitudes de ingreso a clan
-- Basado en: TClanReq del original
-- ============================================
CREATE TABLE IF NOT EXISTS pb_clan_requests (
    id              BIGSERIAL       PRIMARY KEY,
    clan_id         INTEGER         NOT NULL REFERENCES pb_clans(clan_id) ON DELETE CASCADE,
    uid             BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    nickname        VARCHAR(64)     DEFAULT '',
    level           INTEGER         DEFAULT 0,
    rank_id         INTEGER         DEFAULT 0,
    message         VARCHAR(128)    DEFAULT '',           -- Mensaje de solicitud
    status          SMALLINT        DEFAULT 0,           -- 0=pending, 1=accepted, 2=rejected
    created_at      TIMESTAMP       DEFAULT NOW(),
    UNIQUE (clan_id, uid)
);

CREATE INDEX IF NOT EXISTS idx_clan_requests_clan ON pb_clan_requests(clan_id, status);

-- ============================================
-- 28. Items de Clan (inventario del clan)
-- Basado en: TClanItem del original
-- ============================================
CREATE TABLE IF NOT EXISTS pb_clan_items (
    id              SERIAL          PRIMARY KEY,
    clan_id         INTEGER         NOT NULL REFERENCES pb_clans(clan_id) ON DELETE CASCADE,
    item_id         INTEGER         NOT NULL,
    item_count      INTEGER         DEFAULT 1,
    item_type       SMALLINT        DEFAULT 0,           -- Tipo de item de clan
    expire_date     TIMESTAMP,                           -- NULL = permanente
    acquired_at     TIMESTAMP       DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_clan_items_clan ON pb_clan_items(clan_id);

-- ============================================
-- 29. Clan Season/Rating (historico por temporada)
-- Basado en: ClanSeasonRecord del original
-- ============================================
CREATE TABLE IF NOT EXISTS pb_clan_season (
    id              SERIAL          PRIMARY KEY,
    clan_id         INTEGER         NOT NULL,            -- No FK para preservar historial
    season_id       INTEGER         NOT NULL,            -- Numero de temporada
    final_rank      INTEGER         DEFAULT 0,           -- Posicion final en la temporada
    final_rating    INTEGER         DEFAULT 1000,        -- Rating final
    wins            INTEGER         DEFAULT 0,
    losses          INTEGER         DEFAULT 0,
    draws           INTEGER         DEFAULT 0,
    season_exp      INTEGER         DEFAULT 0,
    member_count    INTEGER         DEFAULT 0,
    created_at      TIMESTAMP       DEFAULT NOW(),
    UNIQUE (clan_id, season_id)
);

CREATE INDEX IF NOT EXISTS idx_clan_season_clan ON pb_clan_season(clan_id);

-- ============================================
-- 30. Sistema de Mercenarios
-- Basado en: Mercenary table del original (PBClan DB)
-- Los mercenarios son jugadores contratados temporalmente por un clan
-- ============================================
CREATE TABLE IF NOT EXISTS pb_mercenary (
    id              BIGSERIAL       PRIMARY KEY,
    clan_id         INTEGER         NOT NULL REFERENCES pb_clans(clan_id) ON DELETE CASCADE,
    uid             BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    nickname        VARCHAR(64)     DEFAULT '',
    -- Stats de mercenario durante el contrato
    merc_kills      INTEGER         DEFAULT 0,
    merc_deaths     INTEGER         DEFAULT 0,
    merc_wins       INTEGER         DEFAULT 0,
    merc_losses     INTEGER         DEFAULT 0,
    merc_disconnects INTEGER        DEFAULT 0,
    -- Contrato
    hire_date       TIMESTAMP       DEFAULT NOW(),
    expire_date     TIMESTAMP       NOT NULL,            -- Cuando expira el contrato
    is_active       BOOLEAN         DEFAULT TRUE,
    UNIQUE (clan_id, uid)
);

CREATE INDEX IF NOT EXISTS idx_mercenary_clan ON pb_mercenary(clan_id, is_active);
CREATE INDEX IF NOT EXISTS idx_mercenary_uid ON pb_mercenary(uid);

-- ============================================
-- 31. Lista de amigos
-- ============================================
CREATE TABLE IF NOT EXISTS pb_friends (
    uid             BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    friend_uid      BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    accepted        BOOLEAN         DEFAULT FALSE,       -- FALSE = pending request
    created_at      TIMESTAMP       DEFAULT NOW(),
    PRIMARY KEY (uid, friend_uid)
);

CREATE INDEX IF NOT EXISTS idx_friends_uid ON pb_friends(uid);
CREATE INDEX IF NOT EXISTS idx_friends_friend ON pb_friends(friend_uid);

-- ============================================
-- 32. Lista de bloqueo
-- ============================================
CREATE TABLE IF NOT EXISTS pb_blocks (
    uid             BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    blocked_uid     BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    created_at      TIMESTAMP       DEFAULT NOW(),
    PRIMARY KEY (uid, blocked_uid)
);

CREATE INDEX IF NOT EXISTS idx_blocks_uid ON pb_blocks(uid);

-- ============================================
-- 33. Notas/mensajes (mail system)
-- ============================================
CREATE TABLE IF NOT EXISTS pb_notes (
    id              BIGSERIAL       PRIMARY KEY,
    sender_uid      BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    receiver_uid    BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    sender_nick     VARCHAR(64)     DEFAULT '',
    subject         VARCHAR(128)    DEFAULT '',
    body            VARCHAR(512)    DEFAULT '',
    note_type       SMALLINT        DEFAULT 0,           -- 0=normal, 1=notice, 2=gift
    is_read         BOOLEAN         DEFAULT FALSE,
    gift_item_id    INTEGER         DEFAULT 0,           -- Item attached as gift
    created_at      TIMESTAMP       DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_notes_receiver ON pb_notes(receiver_uid, is_read);

-- ============================================
-- 34. Ban List
-- ============================================
CREATE TABLE IF NOT EXISTS pb_ban_list (
    id              BIGSERIAL       PRIMARY KEY,
    uid             BIGINT          NOT NULL REFERENCES pb_users(uid),
    reason          VARCHAR(128)    DEFAULT '',
    duration        INTEGER         DEFAULT 0,          -- 0 = permanent, >0 = seconds
    banned_by_uid   BIGINT          DEFAULT 0,
    created_at      TIMESTAMP       DEFAULT NOW(),
    expire_at       TIMESTAMP       GENERATED ALWAYS AS (
        CASE WHEN duration = 0 THEN '2099-12-31'::TIMESTAMP
             ELSE created_at + (duration || ' seconds')::INTERVAL
        END
    ) STORED
);
CREATE INDEX IF NOT EXISTS idx_ban_uid ON pb_ban_list(uid);

-- ============================================
-- Trigger: crear stats y datos iniciales al crear usuario (EXTENDIDO)
-- ============================================
CREATE OR REPLACE FUNCTION fn_create_user_data()
RETURNS TRIGGER AS $$
BEGIN
    -- Create stats
    INSERT INTO pb_stats (uid) VALUES (NEW.uid);
    -- Create attendance
    INSERT INTO pb_attendance (uid) VALUES (NEW.uid);
    -- Create default equipment (slot 0 = active)
    INSERT INTO pb_equipment (uid, chara_slot, slot_state) VALUES (NEW.uid, 0, 20);
    -- Create default skills for all 5 classes
    INSERT INTO pb_skills (uid, class_id) VALUES (NEW.uid, 0);
    INSERT INTO pb_skills (uid, class_id) VALUES (NEW.uid, 1);
    INSERT INTO pb_skills (uid, class_id) VALUES (NEW.uid, 2);
    INSERT INTO pb_skills (uid, class_id) VALUES (NEW.uid, 3);
    INSERT INTO pb_skills (uid, class_id) VALUES (NEW.uid, 4);
    -- Create account cosmetics (defaults)
    INSERT INTO pb_account_cosmetics (uid) VALUES (NEW.uid);
    -- Create daily records
    INSERT INTO pb_daily_records (uid) VALUES (NEW.uid);
    -- Create medal info
    INSERT INTO pb_medal_info (uid) VALUES (NEW.uid);
    -- Create default mode records for all 10 modes
    INSERT INTO pb_mode_records (uid, record_type) VALUES
        (NEW.uid, 0), (NEW.uid, 1), (NEW.uid, 2), (NEW.uid, 3), (NEW.uid, 4),
        (NEW.uid, 5), (NEW.uid, 6), (NEW.uid, 7), (NEW.uid, 8), (NEW.uid, 9);
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

DROP TRIGGER IF EXISTS trg_create_user_stats ON pb_users;
DROP TRIGGER IF EXISTS trg_create_user_data ON pb_users;
CREATE TRIGGER trg_create_user_data
    AFTER INSERT ON pb_users
    FOR EACH ROW
    EXECUTE FUNCTION fn_create_user_data();

-- ============================================
-- Funciones auxiliares para rankings
-- ============================================

-- Funcion para calcular rankings individuales diarios
CREATE OR REPLACE FUNCTION fn_calculate_daily_rankings()
RETURNS void AS $$
BEGIN
    INSERT INTO pb_rankings_individual (rank_date, term_type, rank_position, uid, nickname, level, exp, kills, deaths, headshots)
    SELECT CURRENT_DATE, 0, ROW_NUMBER() OVER (ORDER BY s.kills DESC, s.headshots DESC),
           u.uid, u.nickname, u.level, u.exp, s.kills, s.deaths, s.headshots
    FROM pb_users u
    JOIN pb_stats s ON s.uid = u.uid
    WHERE u.is_banned = FALSE AND u.nickname IS NOT NULL
    ORDER BY s.kills DESC, s.headshots DESC
    LIMIT 100
    ON CONFLICT (rank_date, term_type, rank_position) DO NOTHING;
END;
$$ LANGUAGE plpgsql;

-- Funcion para calcular rankings de clan
CREATE OR REPLACE FUNCTION fn_calculate_clan_rankings()
RETURNS void AS $$
BEGIN
    INSERT INTO pb_rankings_clan (rank_date, term_type, rank_position, clan_id, clan_name, clan_exp, wins, losses, member_count)
    SELECT CURRENT_DATE, 0, ROW_NUMBER() OVER (ORDER BY c.clan_exp DESC, c.wins DESC),
           c.clan_id, c.name, c.clan_exp, c.wins, c.losses, c.member_count
    FROM pb_clans c
    WHERE c.is_active = TRUE
    ORDER BY c.clan_exp DESC, c.wins DESC
    LIMIT 50
    ON CONFLICT (rank_date, term_type, rank_position) DO NOTHING;
END;
$$ LANGUAGE plpgsql;

-- Funcion para resetear daily records (llamar a medianoche)
CREATE OR REPLACE FUNCTION fn_reset_daily_records()
RETURNS void AS $$
DECLARE
    v_today INTEGER;
BEGIN
    v_today := TO_CHAR(CURRENT_DATE, 'YYYYMMDD')::INTEGER;
    UPDATE pb_daily_records SET
        record_date = v_today, wins = 0, losses = 0, draws = 0,
        kills = 0, deaths = 0, headshots = 0,
        exp_earned = 0, gp_earned = 0, play_time = 0,
        gift_play_time = 0, got_daily_item = 0
    WHERE record_date < v_today;
    -- Reset daily counters in mode records
    UPDATE pb_mode_records SET daily_matches = 0, daily_kills = 0;
END;
$$ LANGUAGE plpgsql;

-- Funcion para actualizar map stats al terminar una partida
CREATE OR REPLACE FUNCTION fn_update_map_stats(p_map_id INTEGER, p_stage_type INTEGER, p_play_time INTEGER)
RETURNS void AS $$
BEGIN
    INSERT INTO pb_map_stats (reg_date, map_id, stage_type, play_time, play_rounds)
    VALUES (CURRENT_DATE, p_map_id, p_stage_type, p_play_time, 1)
    ON CONFLICT (reg_date, map_id, stage_type) DO UPDATE SET
        play_time = pb_map_stats.play_time + p_play_time,
        play_rounds = pb_map_stats.play_rounds + 1;
END;
$$ LANGUAGE plpgsql;

-- ============================================
-- Datos de prueba
-- ============================================
INSERT INTO pb_users (username, password_hash, nickname, auth_level, gp)
VALUES ('testuser', 'testpass', 'TestPlayer', 0, 50000)
ON CONFLICT (username) DO NOTHING;

INSERT INTO pb_users (username, password_hash, nickname, auth_level, gp)
VALUES ('admin', 'adminpass', 'Admin', 2, 999999)
ON CONFLICT (username) DO NOTHING;

-- Items de prueba para testuser (si existe)
INSERT INTO pb_inventory (uid, item_id, item_count, slot_idx, is_equipped)
SELECT uid, 1001, 1, 0, TRUE FROM pb_users WHERE username = 'testuser'
ON CONFLICT DO NOTHING;

INSERT INTO pb_inventory (uid, item_id, item_count, slot_idx, is_equipped)
SELECT uid, 2001, 1, 1, TRUE FROM pb_users WHERE username = 'testuser'
ON CONFLICT DO NOTHING;

INSERT INTO pb_inventory (uid, item_id, item_count, slot_idx, is_equipped)
SELECT uid, 3001, 1, 2, FALSE FROM pb_users WHERE username = 'testuser'
ON CONFLICT DO NOTHING;

-- Default medals for testuser
INSERT INTO pb_medals (uid, medal_idx, action_id, current_count, get_reward)
SELECT uid, 0, 1, 0, 0 FROM pb_users WHERE username = 'testuser'
ON CONFLICT DO NOTHING;

INSERT INTO pb_medals (uid, medal_idx, action_id, current_count, get_reward)
SELECT uid, 1, 2, 0, 0 FROM pb_users WHERE username = 'testuser'
ON CONFLICT DO NOTHING;

INSERT INTO pb_medals (uid, medal_idx, action_id, current_count, get_reward)
SELECT uid, 2, 3, 0, 0 FROM pb_users WHERE username = 'testuser'
ON CONFLICT DO NOTHING;

INSERT INTO pb_medals (uid, medal_idx, action_id, current_count, get_reward)
SELECT uid, 3, 5, 0, 0 FROM pb_users WHERE username = 'testuser'
ON CONFLICT DO NOTHING;

-- Shop items (matching the hardcoded s_ShopCatalog)
-- Primary weapons (category 0)
INSERT INTO pb_shop_items (goods_id, item_id, item_type, price_gp, price_cash, duration, category) VALUES
(1001, 4097,  0, 5000,    0, 0, 0),   -- M4A1       (ASSAULT/1)
(1002, 4098,  0, 8000,    0, 0, 0),   -- AK-47      (ASSAULT/2)
(1003, 4099,  0, 12000,   0, 0, 0),   -- SG550      (ASSAULT/3)
(1004, 4100,  0, 3000,    0, 0, 0),   -- K-2        (ASSAULT/4)
(1005, 4353,  0, 4000,    0, 0, 0),   -- P90        (SMG/1)
(1006, 4354,  0, 6000,    0, 0, 0),   -- MP5        (SMG/2)
(1007, 4609,  0, 10000,   0, 0, 0),   -- AWP        (SNIPER/1)
(1008, 4610,  0, 15000,   0, 0, 0),   -- PSG-1      (SNIPER/2)
(1009, 4865,  0, 7000,    0, 0, 0),   -- Benelli    (SHOTGUN/1)
(1010, 5121,  0, 20000,   0, 0, 0)    -- MG36       (MG/1)
ON CONFLICT (goods_id) DO NOTHING;

-- Secondary weapons (category 1)
INSERT INTO pb_shop_items (goods_id, item_id, item_type, price_gp, price_cash, duration, category) VALUES
(2001, 8449,  0, 2000,    0, 0, 1),   -- Glock      (HANDGUN/1)
(2002, 8450,  0, 3000,    0, 0, 1),   -- Desert Eagle (HANDGUN/2)
(2003, 8451,  0, 1500,    0, 0, 1)    -- K5         (HANDGUN/3)
ON CONFLICT (goods_id) DO NOTHING;

-- Melee weapons (category 2)
INSERT INTO pb_shop_items (goods_id, item_id, item_type, price_gp, price_cash, duration, category) VALUES
(3001, 12545, 0, 1000,    0, 0, 2),   -- M7         (KNIFE/1)
(3002, 12546, 0, 5000,    0, 0, 2)    -- Kukri      (KNIFE/2)
ON CONFLICT (goods_id) DO NOTHING;

-- Characters (category 3)
INSERT INTO pb_shop_items (goods_id, item_id, item_type, price_gp, price_cash, duration, category) VALUES
(5001, 53505, 0, 10000,   0, 0, 3),   -- Character 1
(5002, 53506, 0, 15000,   0, 0, 3)    -- Character 2
ON CONFLICT (goods_id) DO NOTHING;

-- ============================================
-- Mapas (basados en STAGE_UID de CommonDef_Stage.h)
-- STAGE_UID enum: 1=PortAkaba, 2=RedRock, 3=Library, ...
-- ============================================
INSERT INTO pb_maps (stage_uid, map_name) VALUES
-- STAGE_UID = enum value from CommonDef_Stage.h
(1,  'Port Akaba'),
(2,  'Red Rock'),
(3,  'Library'),
(4,  'MStation'),
(5,  'Midnight Zone'),
(6,  'Uptown'),
(7,  'Burning Hall'),
(8,  'D Squad'),
(9,  'Crackdown'),
(10, 'Saint Mansion'),
(11, 'Eastern Road'),
(12, 'Downtown'),
(13, 'Luxville'),
(14, 'Blow City'),
(15, 'Storm Tube'),
(16, 'Giran 2'),
(17, 'Breakdown'),
(18, 'Training Camp'),
(19, 'Sentry Base'),
(20, 'Desert Camp'),
(21, 'Kick Point'),
(22, 'Face Rock'),
(23, 'Supply Base'),
(24, 'Sand Storm'),
(25, 'Shopping Center'),
(26, 'Safari'),
(27, 'Dragon Alley'),
(28, 'Machu Pichu'),
(29, 'Two Towers'),
(30, 'Angkor Ruins'),
(31, 'Ghost Town'),
(32, 'Grand Bazaar'),
(33, 'Under 23'),
(34, 'Taipei City Mall'),
(35, 'Safari 2'),
(36, 'Metro'),
(37, 'Rush Hour'),
(38, 'Cargo Port'),
(39, 'Black Mamba'),
(40, 'Holiday'),
(41, 'West Station'),
(42, 'House Museum'),
(43, 'Outpost'),
(44, 'Hospital'),
(45, 'Downtown 2'),
(46, 'Cargo Ship'),
(47, 'Airport'),
(48, 'Safe House'),
(49, 'Hard Rock'),
(50, 'Giran'),
(51, 'Helispot'),
(52, 'Black Panther'),
(53, 'Breeding Nest'),
(54, 'D Uptown'),
(55, 'Dino Breakdown'),
(56, 'Dino Lab'),
(57, 'Tutorial'),
(58, 'Random Stage'),
(59, 'Factory Ruins'),
(60, 'Military Base'),
(61, 'Downtown Square'),
(62, 'Stony Desert'),
(63, 'Dragon Nest'),
(64, 'DM8 Unknown'),
(65, 'Water Complex'),
(66, 'Hotel Camouflage'),
(67, 'Pumpkin Hollow'),
(68, 'Test Map'),
(69, 'Battle Ship'),
(70, 'Rampart Town'),
(71, 'Ballistic'),
(72, 'Test'),
(73, 'Holiday 2'),
(74, 'Rothen Village'),
(75, 'Merry White Village'),
(76, 'Fallu City'),
(77, 'Hindrance'),
(78, 'Sewerage'),
(79, 'Slum Area'),
(80, 'New Library'),
(81, 'Sand Storm 2'),
(82, 'Dino Ruins'),
(83, 'Fatal Zone'),
(84, 'Marine Wave')
ON CONFLICT (stage_uid) DO NOTHING;

-- Modos de juego (basados en STAGE_MODE de CommonDef_Stage.h)
-- STAGE_MODE: 1=Deathmatch, 2=Bomb, 3=Destroy, 4=Annihilation, 5=Defence,
--             7=Escape, 10=Tutorial, 12=CrossCount(DinoDM), 13=Convoy, 14=RunAway
INSERT INTO pb_game_modes (mode_id, mode_name, default_stage) VALUES
(1,  'Team Deathmatch', 12),    -- Default: Downtown
(2,  'Bomb Mission', 13),       -- Default: Luxville
(3,  'Destruction', 17),        -- Default: Breakdown
(4,  'Annihilation', 12),       -- Default: Downtown
(5,  'Defence', 52),            -- Default: Black Panther
(7,  'Escape', 53),             -- Default: Breeding Nest
(10, 'Tutorial', 57),           -- Default: Tutorial
(12, 'Cross Counter', 9),       -- Default: Crackdown
(13, 'Convoy', 12),             -- Default: Downtown
(14, 'Run Away', 12)            -- Default: Downtown
ON CONFLICT (mode_id) DO NOTHING;

-- ============================================
-- Map-Mode matching (que mapa soporta que modo)
-- Basado en UpdateMapInfo.sql MapState y analisis del original
-- mode_id: 1=TD, 2=BB, 3=DS, 4=AN, 5=DF, 7=ES, 12=CC
-- ============================================
INSERT INTO pb_map_mode_matching (stage_key, is_open, stage_uid, mode_id, max_players, mark, special_mode) VALUES
-- === TEAM DEATHMATCH (mode_id=1) ===
(1,   1, 1,  1, 16, 0, 0),   -- Port Akaba + TD
(2,   1, 2,  1, 16, 0, 0),   -- Red Rock + TD
(3,   1, 3,  1, 16, 0, 0),   -- Library + TD
(4,   1, 4,  1, 16, 0, 0),   -- MStation + TD
(5,   1, 5,  1, 16, 0, 0),   -- Midnight Zone + TD
(6,   1, 6,  1, 16, 0, 0),   -- Uptown + TD
(7,   1, 7,  1, 16, 0, 0),   -- Burning Hall + TD
(8,   1, 8,  1, 16, 0, 0),   -- D Squad + TD
(9,   1, 9,  1, 16, 0, 0),   -- Crackdown + TD
(10,  1, 10, 1, 16, 0, 0),   -- Saint Mansion + TD
(11,  1, 11, 1, 16, 0, 0),   -- Eastern Road + TD
(12,  1, 12, 1, 16, 0, 0),   -- Downtown + TD
(13,  1, 13, 1, 16, 0, 0),   -- Luxville + TD
(14,  1, 14, 1, 16, 0, 0),   -- Blow City + TD
(15,  1, 15, 1, 16, 0, 0),   -- Storm Tube + TD
(16,  1, 16, 1, 16, 0, 0),   -- Giran 2 + TD
(17,  1, 17, 1, 16, 0, 0),   -- Breakdown + TD
(18,  1, 18, 1, 16, 0, 0),   -- Training Camp + TD
(19,  1, 19, 1, 16, 0, 0),   -- Sentry Base + TD
(20,  1, 20, 1, 16, 0, 0),   -- Desert Camp + TD
(21,  1, 21, 1, 16, 0, 0),   -- Kick Point + TD
(22,  1, 22, 1, 16, 0, 0),   -- Face Rock + TD
(23,  1, 23, 1, 16, 0, 0),   -- Supply Base + TD
(24,  1, 24, 1, 16, 0, 0),   -- Sand Storm + TD
(25,  1, 25, 1, 16, 0, 0),   -- Shopping Center + TD
(26,  1, 26, 1, 16, 0, 0),   -- Safari + TD
(27,  1, 27, 1, 16, 0, 0),   -- Dragon Alley + TD
(28,  1, 28, 1, 16, 0, 0),   -- Machu Pichu + TD
(29,  1, 29, 1, 16, 0, 0),   -- Two Towers + TD
(30,  1, 30, 1, 16, 0, 0),   -- Angkor Ruins + TD
(31,  1, 31, 1, 16, 0, 0),   -- Ghost Town + TD
(32,  1, 32, 1, 16, 0, 0),   -- Grand Bazaar + TD
(33,  1, 33, 1, 16, 0, 0),   -- Under 23 + TD
(34,  1, 34, 1, 16, 0, 0),   -- Taipei City Mall + TD
(35,  1, 35, 1, 16, 0, 0),   -- Safari 2 + TD
(36,  1, 36, 1, 16, 0, 0),   -- Metro + TD
(37,  1, 37, 1, 16, 0, 0),   -- Rush Hour + TD
(38,  1, 38, 1, 16, 0, 0),   -- Cargo Port + TD
(39,  1, 39, 1, 16, 0, 0),   -- Black Mamba + TD
(40,  1, 40, 1, 16, 0, 0),   -- Holiday + TD
(41,  1, 41, 1, 16, 0, 0),   -- West Station + TD
(42,  1, 42, 1, 16, 0, 0),   -- House Museum + TD
(43,  1, 65, 1, 16, 0, 0),   -- Water Complex + TD
(44,  1, 66, 1, 16, 0, 0),   -- Hotel Camouflage + TD
(45,  1, 67, 1, 16, 0, 0),   -- Pumpkin Hollow + TD
(46,  1, 69, 1, 16, 0, 0),   -- Battle Ship + TD
(47,  1, 70, 1, 16, 0, 0),   -- Rampart Town + TD
(48,  1, 74, 1, 16, 0, 0),   -- Rothen Village + TD
(49,  1, 79, 1, 16, 0, 0),   -- Slum Area + TD
(50,  1, 83, 1, 16, 0, 0),   -- Fatal Zone + TD

-- === BOMB MISSION (mode_id=2) ===
(101, 1, 12, 2, 10, 0, 0),   -- Downtown + BB
(102, 1, 13, 2, 10, 0, 0),   -- Luxville + BB
(103, 1, 43, 2, 10, 0, 0),   -- Outpost + BB
(104, 1, 14, 2, 10, 0, 0),   -- Blow City + BB
(105, 1, 15, 2, 10, 0, 0),   -- Storm Tube + BB
(106, 1, 19, 2, 10, 0, 0),   -- Sentry Base + BB
(107, 1, 44, 2, 10, 0, 0),   -- Hospital + BB
(108, 1, 45, 2, 10, 0, 0),   -- Downtown 2 + BB
(109, 1, 25, 2, 10, 0, 0),   -- Shopping Center + BB
(110, 1, 24, 2, 10, 0, 0),   -- Sand Storm + BB
(111, 1, 46, 2, 10, 0, 0),   -- Cargo Ship + BB
(112, 1, 47, 2, 10, 0, 0),   -- Airport + BB
(113, 1, 48, 2, 10, 0, 0),   -- Safe House + BB
(114, 1, 49, 2, 10, 0, 0),   -- Hard Rock + BB
(115, 1, 70, 2, 10, 0, 0),   -- Rampart Town + BB
(116, 1, 71, 2, 10, 0, 0),   -- Ballistic + BB
(117, 1, 76, 2, 10, 0, 0),   -- Fallu City + BB

-- === DESTRUCTION (mode_id=3) ===
(201, 1, 17, 3, 16, 0, 0),   -- Breakdown + DS
(202, 1, 50, 3, 16, 0, 0),   -- Giran + DS
(203, 1, 16, 3, 16, 0, 0),   -- Giran 2 + DS
(204, 1, 51, 3, 16, 0, 0),   -- Helispot + DS

-- === ANNIHILATION (mode_id=4) ===
(301, 1, 12, 4, 16, 0, 0),   -- Downtown + AN
(302, 1, 13, 4, 16, 0, 0),   -- Luxville + AN
(303, 1, 7,  4, 16, 0, 0),   -- Burning Hall + AN
(304, 1, 21, 4, 16, 0, 0),   -- Kick Point + AN
(305, 1, 9,  4, 16, 0, 0),   -- Crackdown + AN
(306, 1, 18, 4, 16, 0, 0),   -- Training Camp + AN

-- === DEFENCE (mode_id=5) ===
(401, 1, 52, 5, 8, 0, 0),    -- Black Panther + DF

-- === ESCAPE (mode_id=7) ===
(501, 1, 53, 7, 8, 0, 0),    -- Breeding Nest + ES
(502, 1, 54, 7, 8, 0, 0),    -- D Uptown + ES
(503, 1, 55, 7, 8, 0, 0),    -- Dino Breakdown + ES
(504, 1, 82, 7, 8, 0, 0),    -- Dino Ruins + ES

-- === CROSS COUNTER / DINO DM (mode_id=12) ===
(601, 1, 9,  12, 10, 0, 0),  -- Crackdown + CC
(602, 1, 10, 12, 10, 0, 0),  -- Saint Mansion + CC
(603, 1, 8,  12, 10, 0, 0),  -- D Squad + CC
(604, 1, 1,  12, 10, 0, 0),  -- Port Akaba + CC
(605, 1, 4,  12, 10, 0, 0),  -- MStation + CC
(606, 1, 18, 12, 10, 0, 0),  -- Training Camp + CC
(607, 1, 2,  12, 10, 0, 0),  -- Red Rock + CC
(608, 1, 26, 12, 10, 0, 0),  -- Safari + CC
(609, 1, 35, 12, 10, 0, 0),  -- Safari 2 + CC
(610, 1, 56, 12, 10, 0, 0),  -- Dino Lab + CC

-- === TUTORIAL (mode_id=10) ===
(701, 1, 57, 10, 1, 0, 0)    -- Tutorial
ON CONFLICT (stage_key) DO NOTHING;

-- Aviso default del servidor
INSERT INTO pb_notices (notice_text)
VALUES ('Welcome to Piercing Blow Remake Server!')
ON CONFLICT DO NOTHING;

-- ============================================
-- Verificar
-- ============================================
-- SELECT * FROM pb_users;
-- SELECT * FROM pb_stats;
-- SELECT * FROM pb_inventory;
-- SELECT * FROM pb_equipment;
-- SELECT * FROM pb_medals;
-- SELECT * FROM pb_medal_sets_current;
-- SELECT * FROM pb_medal_sets_complete;
-- SELECT * FROM pb_medal_info;
-- SELECT * FROM pb_attendance;
-- SELECT * FROM pb_skills;
-- SELECT * FROM pb_quests;
-- SELECT * FROM pb_account_cosmetics;
-- SELECT * FROM pb_mode_records;
-- SELECT * FROM pb_daily_records;
-- SELECT * FROM pb_maps;
-- SELECT * FROM pb_game_modes;
-- SELECT * FROM pb_map_mode_matching;
-- SELECT * FROM pb_map_stats;
-- SELECT * FROM pb_rankings_individual;
-- SELECT * FROM pb_rankings_clan;
-- SELECT * FROM pb_server_config;
-- SELECT * FROM pb_notices;
-- SELECT * FROM pb_boost_events;
-- SELECT * FROM pb_shop_items;
-- SELECT * FROM pb_shop_prices;
-- SELECT * FROM pb_shop_matching;
-- SELECT * FROM pb_shop_coupons;
-- SELECT * FROM pb_gifts;
-- SELECT * FROM pb_clans;
-- SELECT * FROM pb_clan_members;
-- SELECT * FROM pb_clan_requests;
-- SELECT * FROM pb_clan_items;
-- SELECT * FROM pb_clan_season;
-- SELECT * FROM pb_mercenary;
-- SELECT * FROM pb_friends;
-- SELECT * FROM pb_blocks;
-- SELECT * FROM pb_notes;
-- SELECT * FROM pb_ban_list;
