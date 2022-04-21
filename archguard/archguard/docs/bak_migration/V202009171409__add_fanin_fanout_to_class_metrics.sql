drop table if exists metric_class;
create table metric_class
(
    id        bigint auto_increment
        primary key,
    system_id bigint       not null,
    class_id  varchar(255) not null,
    abc       int          null,
    noc       int          null,
    dit       int          null,
    lcom4     int          null,
    fanin     int          null,
    fanout    int          null
)
    collate = utf8mb4_unicode_ci;

create index idx_project_id on metric_class (system_id);
create index idx_class_id on code_class (id);


