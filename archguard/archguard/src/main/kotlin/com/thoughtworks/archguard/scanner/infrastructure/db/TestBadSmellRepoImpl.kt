package com.thoughtworks.archguard.scanner.infrastructure.db

import com.thoughtworks.archguard.scanner.domain.scanner.javaext.tbs.TestBadSmell
import com.thoughtworks.archguard.scanner.domain.scanner.javaext.tbs.TestBadSmellRepo
import org.springframework.beans.factory.annotation.Autowired
import org.springframework.stereotype.Repository
import java.util.UUID

@Repository
class TestBadSmellRepoImpl(
    @Autowired private val testBadSmellDao: TestBadSmellDao,
    @Autowired private val overviewDao: OverviewDao
) : TestBadSmellRepo {

    override fun save(testBadSmells: List<TestBadSmell>) {
        testBadSmellDao.deleteAll()
        testBadSmellDao.saveAll(testBadSmells)
    }

    override fun saveCount(count: Int) {
        overviewDao.deleteByType("test")
        overviewDao.save(Overview(UUID.randomUUID().toString(), "test", count.toString()))
    }
}
