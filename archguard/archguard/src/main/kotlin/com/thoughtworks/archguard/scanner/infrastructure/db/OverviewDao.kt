package com.thoughtworks.archguard.scanner.infrastructure.db

import org.jdbi.v3.sqlobject.customizer.Bind
import org.jdbi.v3.sqlobject.customizer.BindBean
import org.jdbi.v3.sqlobject.statement.SqlUpdate

interface OverviewDao {
    @SqlUpdate(
        "insert into system_overview (id, overview_type, overview_value) " +
            "values (:overview.id, :overview.type, :overview.value)"
    )
    fun save(@BindBean("overview") overview: Overview)

    @SqlUpdate("delete from system_overview where overview_type= :type")
    fun deleteByType(@Bind("type") type: String)
}
