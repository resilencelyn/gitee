ALTER TABLE `code_framework_dubbo_module`
    ADD COLUMN `project_id` bigint(20) NOT NULL AFTER `id`,
    ADD INDEX `idx_project_id`(`project_id`) USING BTREE;

ALTER TABLE `code_framework_dubbo_bean`
    ADD COLUMN `project_id` bigint(20) NOT NULL AFTER `id`,
    ADD INDEX `idx_project_id`(`project_id`) USING BTREE;

ALTER TABLE `code_framework_dubbo_reference_config`
    ADD COLUMN `project_id` bigint(20) NOT NULL AFTER `id`,
    ADD INDEX `idx_project_id`(`project_id`) USING BTREE;

ALTER TABLE `code_framework_dubbo_service_config`
    ADD COLUMN `project_id` bigint(20) NOT NULL AFTER `id`,
    ADD INDEX `idx_project_id`(`project_id`) USING BTREE;
