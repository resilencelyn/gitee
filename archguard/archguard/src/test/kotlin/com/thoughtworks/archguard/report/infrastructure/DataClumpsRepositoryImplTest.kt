package com.thoughtworks.archguard.report.infrastructure

import com.thoughtworks.archguard.report.infrastructure.coupling.DataClumpsRepositoryImpl
import org.junit.jupiter.api.Test
import org.springframework.beans.factory.annotation.Autowired
import org.springframework.boot.test.context.SpringBootTest
import org.springframework.test.context.jdbc.Sql
import org.springframework.test.context.web.WebAppConfiguration
import kotlin.test.assertEquals

@SpringBootTest
@WebAppConfiguration
internal class DataClumpsRepositoryImplTest {

    @Autowired
    lateinit var dataClumpsRepositoryImpl: DataClumpsRepositoryImpl

    @Test
    @Sql("classpath:sqls/insert_jclass_and_lcom4_dit.sql")
    fun should_get_data_clumps_total_count() {
        val lcoM4AboveThresholdCount = dataClumpsRepositoryImpl.getLCOM4AboveThresholdCount(1, 1)
        assertEquals(2, lcoM4AboveThresholdCount)
    }

    @Test
    @Sql("classpath:sqls/insert_jclass_and_lcom4_dit.sql")
    fun should_get_data_clumps_list_by_paging() {

        val page1 = dataClumpsRepositoryImpl
            .getLCOM4AboveThresholdList(1, 1, 1, 0)
        kotlin.test.assertEquals(1, page1.size)
        kotlin.test.assertEquals(3, page1.get(0).lcom4)
        kotlin.test.assertEquals(
            "org.apache.dubbo.demo",
            page1.get(0).packageName
        )
        kotlin.test.assertEquals(
            "DemoService",
            page1.get(0).typeName
        )
        kotlin.test.assertEquals(
            "dubbo-demo-interface",
            page1.get(0).moduleName
        )

        val page2 = dataClumpsRepositoryImpl
            .getLCOM4AboveThresholdList(1, 1, 1, 1)
        kotlin.test.assertEquals(1, page2.size)
        kotlin.test.assertEquals(2, page2.get(0).lcom4)
        kotlin.test.assertEquals(
            "org.apache.dubbo.demo",
            page2.get(0).packageName
        )
        kotlin.test.assertEquals(
            "TestService$1",
            page2.get(0).typeName
        )
        kotlin.test.assertEquals(
            "test-demo-interface",
            page2.get(0).moduleName
        )
    }
}
