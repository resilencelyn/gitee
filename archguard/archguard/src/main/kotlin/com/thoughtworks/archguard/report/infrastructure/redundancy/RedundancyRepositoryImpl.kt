package com.thoughtworks.archguard.report.infrastructure.redundancy

import com.thoughtworks.archguard.report.domain.models.ClassVO
import com.thoughtworks.archguard.report.domain.redundancy.RedundancyRepository
import com.thoughtworks.archguard.report.infrastructure.ClassPO
import org.jdbi.v3.core.Jdbi
import org.springframework.stereotype.Repository

@Repository
class RedundancyRepositoryImpl(val jdbi: Jdbi) : RedundancyRepository {
    private val table = "select sum(1) sum, m.clzname as name, m.module as module " +
        "from code_method m, code_class c, method_access ma, code_class_access ca " +
        "where ca.class_id=c.id  and ca.is_interface = false and ca.is_abstract = false " +
        "and ma.method_id = m.id and m.name not like '%$%'" +
        "and m.name != '<clinit>' and m.name != 'main' and m.name != '<init>'" +
        "and m.name != 'toString' and m.name != 'equals' and m.name != 'hashCode' and m.name != 'clone'" +
        "and m.clzname = c.name and c.module=m.module and c.is_thirdparty = false and c.is_test != true " +
        "and m.system_id=:system_id and c.system_id=:system_id and ma.system_id=:system_id " +
        "group by m.clzname, m.module having sum = 1"

    override fun getOneMethodClassCount(systemId: Long): Long {
        return jdbi.withHandle<Long, Exception> {
            it.createQuery("select count(1) from ($table) a")
                .bind("system_id", systemId)
                .mapTo<Long>(Long::class.java).one()
        }
    }

    override fun getOneMethodClass(systemId: Long, limit: Long, offset: Long): List<ClassVO> {
        return jdbi.withHandle<List<ClassVO>, Exception> {
            it.createQuery("$table order by clzname LIMIT :limit OFFSET :offset ")
                .bind("system_id", systemId)
                .bind("limit", limit)
                .bind("offset", offset)
                .mapTo(ClassPO::class.java).list()
                .map { p -> ClassVO.create(p.name, p.module) }
        }
    }
}
