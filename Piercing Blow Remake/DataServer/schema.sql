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
-- Trigger: crear stats automaticamente al crear usuario
-- ============================================
CREATE OR REPLACE FUNCTION fn_create_user_stats()
RETURNS TRIGGER AS $$
BEGIN
    INSERT INTO pb_stats (uid) VALUES (NEW.uid);
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

DROP TRIGGER IF EXISTS trg_create_user_stats ON pb_users;
CREATE TRIGGER trg_create_user_stats
    AFTER INSERT ON pb_users
    FOR EACH ROW
    EXECUTE FUNCTION fn_create_user_stats();

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

-- ============================================
-- Verificar
-- ============================================
-- SELECT * FROM pb_users;
-- SELECT * FROM pb_stats;
-- SELECT * FROM pb_inventory;
