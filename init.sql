BEGIN;

CREATE TABLE IF NOT EXISTS "ports"
(
    "id" SERIAL PRIMARY KEY,
    "name" varchar
);
INSERT INTO ports (id, name) VALUES
                                 (1, 'St. Petersburg'),
                                 (2, 'Murmansk'),
                                 (3, 'Arkhangelsk'),
                                 (4, 'Vladivostok'),
                                 (5, 'Magadan')
    ON CONFLICT DO NOTHING;

CREATE TABLE IF NOT EXISTS "ship_purposes"
(
    "id" SERIAL PRIMARY KEY,
    "name" varchar
);
INSERT INTO ship_purposes (id, name) VALUES
                                         (1, 'Military'),
                                         (2, 'Transport'),
                                         (3, 'Fishing')
    ON CONFLICT DO NOTHING;

CREATE TABLE IF NOT EXISTS "ships"
(
    "id" SERIAL PRIMARY KEY,
    "name" varchar,
    "carry_capacity" float4,
    "homeport_id" int,
    "purpose_id" int,
    "location" point,
    "overhaul_start_date" date,
    CONSTRAINT ships_homeport_id_fkey FOREIGN KEY(homeport_id) REFERENCES ports(id),
    CONSTRAINT ships_purpose_id_fkey FOREIGN KEY(purpose_id) REFERENCES ship_purposes(id)
);

CREATE TABLE IF NOT EXISTS "crew_roles"
(
    "id" SERIAL PRIMARY KEY,
    "title" varchar
);
INSERT INTO crew_roles (id, title) VALUES
                                       (1, 'Master'),
                                       (2, 'Mate'),
                                       (3, '2nd Mate'),
                                       (4, '3rd Mate'),
                                       (5, 'Bosun'),
                                       (6, 'Sailor'),
                                       (7, 'Chief Engineer'),
                                       (8, '2nd Engineer'),
                                       (9, '3rd Engineer'),
                                       (10, '4th Engineer'),
                                       (11, 'Donkeyman'),
                                       (12, 'Watchkeeping Greaser'),
                                       (13, 'Chief cook'),
                                       (14, 'Crew cook'),
                                       (15, 'Steward')
    ON CONFLICT DO NOTHING;

CREATE TABLE IF NOT EXISTS "crew_members"
(
    "id" SERIAL PRIMARY KEY,
    "ship_id" int,
    "first_name" varchar,
    "last_name" varchar,
    "patronymic" varchar,
    "birth_date" date,
    "role_id" int,
    "experience" int,
    "salary" int,
    CONSTRAINT crew_members_ship_id_fkey FOREIGN KEY(ship_id) REFERENCES ships(id) ON DELETE SET NULL,
    CONSTRAINT crew_members_role_id_fkey FOREIGN KEY(role_id) REFERENCES crew_roles(id)
);

CREATE TABLE IF NOT EXISTS "general_cargo_types"
(
    "id" SERIAL PRIMARY KEY,
    "name" varchar
);
INSERT INTO general_cargo_types (id, name) VALUES
                                               (1, 'Drums'),
                                               (2, 'Bags'),
                                               (3, 'Pallets'),
                                               (4, 'Boxes'),
                                               (5, 'Lumber'),
                                               (6, 'Paper'),
                                               (7, 'Steel'),
                                               (8, 'Vehicles'),
                                               (9, 'Containers')
    ON CONFLICT DO NOTHING;

CREATE TABLE IF NOT EXISTS "charterers"
(
    "id" SERIAL PRIMARY KEY,
    "name" varchar,
    "address" varchar,
    "phonenumber" varchar,
    "fax" varchar,
    "email" varchar,
    "bankdetails_bank_name" varchar,
    "bankdetails_city" varchar,
    "bankdetails_TIN" varchar,
    "bankdetails_account_number" varchar
);

CREATE TABLE IF NOT EXISTS "cruises"
(
    "id" SERIAL PRIMARY KEY,
    "ship_id" int,
    "general_cargo_package_id" int,
    "departure_port_id" int,
    "destination_port_id" int,
    "charterer_id" int,
    CONSTRAINT cruises_ship_id_fkey FOREIGN KEY(ship_id) REFERENCES ships(id),
    CONSTRAINT cruises_general_cargo_package_id_fkey FOREIGN KEY(general_cargo_package_id) REFERENCES general_cargo_types(id),
    CONSTRAINT cruises_departure_port_id_fkey FOREIGN KEY(departure_port_id) REFERENCES ports(id),
    CONSTRAINT cruises_destination_port_id_fkey FOREIGN KEY(destination_port_id) REFERENCES ports(id),
    CONSTRAINT cruises_charterer_id_fkey FOREIGN KEY(charterer_id) REFERENCES charterers(id)
);

CREATE TABLE IF NOT EXISTS "ship_port_entries"
(
    "id" SERIAL PRIMARY KEY,
    "cruise_id" int,
    "port_id" int,
    "destination_ts_planned" timestamp,
    "destination_ts_actual" timestamp,
    "departure_ts_planned" timestamp,
    "departure_ts_actual" timestamp,
    "destination_delay_reason" varchar,
    "departure_delay_reason" varchar,
    CONSTRAINT ship_port_entries_cruise_id_fkey FOREIGN KEY(cruise_id) REFERENCES cruises(id),
    CONSTRAINT ship_port_entries_port_id_fkey FOREIGN KEY(port_id) REFERENCES ports(id)
);

COMMIT;