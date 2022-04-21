package com.thoughtworks.archguard.scanner2.infrastructure.mysql

import com.thoughtworks.archguard.scanner2.domain.model.ModuleMetric
import com.thoughtworks.archguard.scanner2.domain.repository.ModuleMetricRepository
import org.slf4j.LoggerFactory
import org.springframework.stereotype.Repository

@Repository
class ModuleMetricRepositoryImpl(val moduleMetricsDao: ModuleMetricsDao) : ModuleMetricRepository {
    private val log = LoggerFactory.getLogger(ModuleMetricRepositoryImpl::class.java)

    override fun insertOrUpdateModuleMetric(systemId: Long, moduleMetrics: List<ModuleMetric>) {
        moduleMetricsDao.deleteBy(systemId)
        log.info("Delete system module metric old data with id: {}", systemId)
        if (moduleMetrics.isEmpty()) {
            log.warn("Insert system module metric new data with id is empty!: {}", systemId)
            return
        }
        moduleMetricsDao.insert(moduleMetrics)
        log.info("Insert system module metric new data with id: {}", systemId)
    }
}
