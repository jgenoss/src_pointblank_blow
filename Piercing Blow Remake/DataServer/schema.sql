-- ============================================
-- Piercing Blow Remake - DataServer PostgreSQL Schema
-- ============================================
-- Ejecutar: psql -U pb_server -d piercing_blow -f schema.sql
-- ============================================

-- Crear base de datos (ejecutar como superuser si no existe)
-- CREATE DATABASE piercing_blow OWNER pb_server;

-- ============================================
-- Tabla principal de usuarios
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
    rank_id         INTEGER         DEFAULT 0,
    clan_id         INTEGER         DEFAULT 0,
    active_class    SMALLINT        DEFAULT 0,           -- GameCharaClass (0=Assault)
    created_at      TIMESTAMP       DEFAULT NOW(),
    last_login      TIMESTAMP
);

-- ============================================
-- Inventario de jugadores
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
-- Estadisticas de jugadores
-- ============================================
CREATE TABLE IF NOT EXISTS pb_stats (
    uid             BIGINT          PRIMARY KEY REFERENCES pb_users(uid) ON DELETE CASCADE,
    kills           INTEGER         DEFAULT 0,
    deaths          INTEGER         DEFAULT 0,
    headshots       INTEGER         DEFAULT 0,
    wins            INTEGER         DEFAULT 0,
    losses          INTEGER         DEFAULT 0,
    total_exp_earned BIGINT         DEFAULT 0,
    total_gp_earned  BIGINT         DEFAULT 0
);

-- ============================================
-- Equipamiento de personaje (5 character slots x 15 equip slots)
-- ============================================
CREATE TABLE IF NOT EXISTS pb_equipment (
    uid             BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    chara_slot      SMALLINT        NOT NULL DEFAULT 0,  -- 0-4 (MAX_CHARA_SLOT)
    slot_state      SMALLINT        DEFAULT 0,           -- MultiSlotState
    chara_id        INTEGER         DEFAULT 0,           -- Character ItemID
    -- Weapon slots (5)
    weapon_primary  INTEGER         DEFAULT 0,
    weapon_secondary INTEGER        DEFAULT 0,
    weapon_melee    INTEGER         DEFAULT 0,
    weapon_throw1   INTEGER         DEFAULT 0,
    weapon_throw2   INTEGER         DEFAULT 0,
    -- Parts slots (10)
    parts_chara     INTEGER         DEFAULT 0,
    parts_head      INTEGER         DEFAULT 0,
    parts_face      INTEGER         DEFAULT 0,
    parts_upper     INTEGER         DEFAULT 0,
    parts_lower     INTEGER         DEFAULT 0,
    parts_glove     INTEGER         DEFAULT 0,
    parts_belt      INTEGER         DEFAULT 0,
    parts_holster   INTEGER         DEFAULT 0,
    parts_skin      INTEGER         DEFAULT 0,
    parts_beret     INTEGER         DEFAULT 0,
    PRIMARY KEY (uid, chara_slot)
);

-- ============================================
-- Medallas de jugadores
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
-- Asistencia diaria (attendance)
-- ============================================
CREATE TABLE IF NOT EXISTS pb_attendance (
    uid             BIGINT          PRIMARY KEY REFERENCES pb_users(uid) ON DELETE CASCADE,
    total_days      INTEGER         DEFAULT 0,
    current_streak  INTEGER         DEFAULT 0,
    last_attend_date INTEGER        DEFAULT 0,           -- YYYYMMDD format
    attend_data     BYTEA                                -- 30 bytes, ui8Days[30]
);

-- ============================================
-- Skills de personaje (por clase)
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
-- Quest progress (cardsets activos)
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
-- Clanes
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
    unit            SMALLINT        DEFAULT 0,           -- GameClanUnit
    mark_id         SMALLINT        DEFAULT 0,
    mark_color      SMALLINT        DEFAULT 0,
    is_active       BOOLEAN         DEFAULT TRUE,
    created_at      TIMESTAMP       DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_clans_master ON pb_clans(master_uid);

-- ============================================
-- Miembros de clan
-- ============================================
CREATE TABLE IF NOT EXISTS pb_clan_members (
    clan_id         INTEGER         NOT NULL REFERENCES pb_clans(clan_id) ON DELETE CASCADE,
    uid             BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    member_level    SMALLINT        DEFAULT 3,           -- GameClanMemberLevel: 1=Master,2=Staff,3=Regular
    joined_at       TIMESTAMP       DEFAULT NOW(),
    PRIMARY KEY (clan_id, uid)
);

CREATE INDEX IF NOT EXISTS idx_clan_members_uid ON pb_clan_members(uid);

-- ============================================
-- Lista de amigos
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
-- Lista de bloqueo
-- ============================================
CREATE TABLE IF NOT EXISTS pb_blocks (
    uid             BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    blocked_uid     BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    created_at      TIMESTAMP       DEFAULT NOW(),
    PRIMARY KEY (uid, blocked_uid)
);

CREATE INDEX IF NOT EXISTS idx_blocks_uid ON pb_blocks(uid);

-- ============================================
-- Tienda (shop catalog)
-- ============================================
CREATE TABLE IF NOT EXISTS pb_shop_items (
    goods_id        SERIAL          PRIMARY KEY,
    item_id         INTEGER         NOT NULL,
    item_type       SMALLINT        DEFAULT 0,           -- GameItemAttrType
    price_gp        INTEGER         DEFAULT 0,           -- 0 = not available for GP
    price_cash      INTEGER         DEFAULT 0,           -- 0 = not available for Cash
    duration        INTEGER         DEFAULT 0,           -- 0 = permanent, seconds
    level_req       INTEGER         DEFAULT 0,           -- Minimum level required
    rank_req        INTEGER         DEFAULT 0,           -- Minimum rank required
    is_active       BOOLEAN         DEFAULT TRUE,
    category        SMALLINT        DEFAULT 0            -- 0=Primary,1=Secondary,2=Melee,3=Character,4=Parts
);

-- ============================================
-- Notas/mensajes (mail system)
-- ============================================
CREATE TABLE IF NOT EXISTS pb_notes (
    id              BIGSERIAL       PRIMARY KEY,
    sender_uid      BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    receiver_uid    BIGINT          NOT NULL REFERENCES pb_users(uid) ON DELETE CASCADE,
    subject         VARCHAR(128)    DEFAULT '',
    body            VARCHAR(512)    DEFAULT '',
    note_type       SMALLINT        DEFAULT 0,           -- 0=normal, 1=notice, 2=gift
    is_read         BOOLEAN         DEFAULT FALSE,
    gift_item_id    INTEGER         DEFAULT 0,           -- Item attached as gift
    created_at      TIMESTAMP       DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_notes_receiver ON pb_notes(receiver_uid, is_read);

-- ============================================
-- Trigger: crear stats y datos iniciales al crear usuario
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
-- ============================================
-- Ban List
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
-- Verificar
-- ============================================
-- SELECT * FROM pb_users;
-- SELECT * FROM pb_stats;
-- SELECT * FROM pb_inventory;
-- SELECT * FROM pb_equipment;
-- SELECT * FROM pb_medals;
-- SELECT * FROM pb_attendance;
-- SELECT * FROM pb_skills;
-- SELECT * FROM pb_quests;
-- SELECT * FROM pb_clans;
-- SELECT * FROM pb_clan_members;
-- SELECT * FROM pb_friends;
-- SELECT * FROM pb_blocks;
-- SELECT * FROM pb_notes;
-- SELECT * FROM pb_ban_list;
