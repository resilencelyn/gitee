drop table if exists metric_dataclass;
create table metric_dataclass
(
    id        bigint auto_increment
        primary key,
    class_id  char(36) not null,
    field_id  char(36) not null,
    system_id bigint   not null,
    constraint class_id_field_id_UNIQUE
        unique (class_id, field_id)
)
    collate = utf8mb4_unicode_ci;

create index idx_class_id
    on metric_dataclass (class_id);

create index idx_system_id
    on metric_dataclass (system_id);

