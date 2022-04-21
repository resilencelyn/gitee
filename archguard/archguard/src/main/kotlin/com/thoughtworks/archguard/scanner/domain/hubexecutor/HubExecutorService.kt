package com.thoughtworks.archguard.scanner.domain.hubexecutor

import com.thoughtworks.archguard.scanner.domain.analyser.AnalysisService
import com.thoughtworks.archguard.scanner.domain.config.repository.ScannerConfigureRepository
import com.thoughtworks.archguard.scanner.domain.scanner.javaext.bs.ScanContext
import com.thoughtworks.archguard.scanner.infrastructure.client.EvaluationReportClient
import com.thoughtworks.archguard.scanner.infrastructure.command.InMemoryConsumer
import com.thoughtworks.archguard.scanner.infrastructure.command.StreamConsumer
import org.slf4j.LoggerFactory
import org.springframework.beans.factory.DisposableBean
import org.springframework.beans.factory.annotation.Autowired
import org.springframework.stereotype.Service
import java.util.concurrent.CopyOnWriteArraySet
import kotlin.concurrent.thread

@Service
class HubExecutorService : DisposableBean {
    private val log = LoggerFactory.getLogger(HubExecutorService::class.java)
    private var systemRoot: String = ""

    @Autowired
    private lateinit var manager: ScannerManager

    @Volatile
    private var isRunning: Boolean = false

    private val concurrentSet: CopyOnWriteArraySet<Long> = CopyOnWriteArraySet()

    @Autowired
    private lateinit var analysisService: AnalysisService

    @Autowired
    private lateinit var evaluationReportClient: EvaluationReportClient

    @Autowired
    private lateinit var configureRepository: ScannerConfigureRepository

    fun doScanIfNotRunning(id: Long, dbUrl: String, memoryConsumer: StreamConsumer): Boolean {
        if (!concurrentSet.contains(id)) {
            concurrentSet.add(id)
            try {
                doScan(id, dbUrl, memoryConsumer, listOf())
            } catch (e: Exception) {
                log.error(e.message)
                throw e
            } finally {
                concurrentSet.remove(id)
            }
        }
        return concurrentSet.contains(id)
    }

    fun evaluate(type: String, id: Long, dbUrl: String, arguments: List<String>): Boolean {
        if (!concurrentSet.contains(id)) {
            concurrentSet.add(id)
            thread {
                doScan(id, dbUrl, InMemoryConsumer(), arguments)
                concurrentSet.remove(id)
                evaluationReportClient.generate(type)
            }
        }

        return concurrentSet.contains(id)
    }

    private fun doScan(id: Long, dbUrl: String, memoryConsumer: StreamConsumer, additionArguments: List<String>) {
        val config = configureRepository.getToolConfigures()
        // todo: check workspace dir
        val systemOperator = analysisService.getSystemOperator(id, memoryConsumer)
        systemOperator.cloneAndBuildAllRepo()
        systemOperator.scanProjectMap.forEach { (repo, compiledProject) ->
            val context = ScanContext(
                id,
                repo,
                compiledProject.buildTool,
                compiledProject.workspace,
                dbUrl,
                config,
                compiledProject.language,
                compiledProject.codePath,
                compiledProject.branch,
                memoryConsumer,
                additionArguments
            )
            val hubExecutor = HubExecutor(context, manager)
            hubExecutor.execute()
            systemRoot = context.workspace.parent
        }
    }

    fun getEvaluationStatus(type: String, id: Long): Boolean {
        return concurrentSet.contains(id)
    }

    /**
     * 关闭服务清空临时目录
     */
    override fun destroy() {
//        if(systemRoot.isEmpty()){
//            return
//        }
//        var file = File(systemRoot)
//        if(file.exists()){
//            var del = file.deleteRecursively()
//            log.info("tempFile {}, clean tempFile result：{}", systemRoot, del);
//        }
    }
}
