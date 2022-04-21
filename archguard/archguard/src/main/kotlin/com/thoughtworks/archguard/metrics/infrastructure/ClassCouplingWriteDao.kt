package com.thoughtworks.archguard.metrics.infrastructure

import org.jdbi.v3.sqlobject.config.RegisterBeanMapper
import org.jdbi.v3.sqlobject.customizer.Bind
import org.jdbi.v3.sqlobject.customizer.BindBean
import org.jdbi.v3.sqlobject.statement.GetGeneratedKeys
import org.jdbi.v3.sqlobject.statement.SqlUpdate

@RegisterBeanMapper(MetricClassCouplingWritePO::class)
interface ClassCouplingWriteDao {
    @GetGeneratedKeys
    @SqlUpdate(
        "insert into metric_class_coupling (" +
            "class_id, system_id, inner_fan_in, inner_fan_out, outer_fan_in, " +
            "outer_fan_out, inner_instability, inner_coupling, outer_instability, outer_coupling) " +
            "values(:classId, :systemId, :innerFanIn, :innerFanOut, :outerFanIn, " +
            ":outerFanOut, :innerInstability, :innerCoupling, :outerInstability, :outerCoupling)"
    )
    fun insert(@BindBean metricClassCouplingWritePO: MetricClassCouplingWritePO): Long

    @SqlUpdate("DELETE FROM metric_class_coupling where system_id = :systemId")
    fun deleteBy(@Bind("systemId") systemId: Long)
}
