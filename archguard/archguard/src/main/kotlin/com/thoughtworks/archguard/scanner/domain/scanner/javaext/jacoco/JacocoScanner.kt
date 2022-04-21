package com.thoughtworks.archguard.scanner.domain.scanner.javaext.jacoco

import com.thoughtworks.archguard.scanner.domain.scanner.Scanner
import com.thoughtworks.archguard.scanner.domain.scanner.javaext.bs.ScanContext
import com.thoughtworks.archguard.scanner.infrastructure.db.SqlScriptRunner
import org.slf4j.LoggerFactory
import org.springframework.beans.factory.annotation.Autowired
import org.springframework.stereotype.Component
import java.io.File

@Component
class JacocoScanner(@Autowired val sqlScriptRunner: SqlScriptRunner) : Scanner {
    private val DELETE_BUNDLE = "delete from test_coverage_bundle where 1=1"
    private val DELETE_ITEM = "delete from test_coverage_item where 1=1"

    private val log = LoggerFactory.getLogger(JacocoScanner::class.java)
    override fun getScannerName(): String {
        return "Jacoco"
    }

    override fun canScan(context: ScanContext): Boolean {
        return context.language.lowercase() == "jvm"
    }

    override fun scan(context: ScanContext) {
        log.info("start scan jacoco exec file")
        sqlScriptRunner.run(DELETE_BUNDLE)
        sqlScriptRunner.run(DELETE_ITEM)
        getTargetProjects(context.workspace)
            .forEach { w -> runSql(JacocoTool(context.workspace, w, context.buildTool, context.logStream).execToSql()) }
        log.info("finished scan jacoco exec file")
    }

    private fun runSql(sqlFile: File?) {
        if (sqlFile != null) {
            sqlScriptRunner.run(sqlFile)
        }
    }

    private fun getTargetProjects(workspace: File): List<File> {
        return workspace.walkTopDown()
            .filter { f -> f.absolutePath.endsWith("jacoco.exec") }
            .map { f -> f.parentFile.parentFile }
            .toList()
    }
}
