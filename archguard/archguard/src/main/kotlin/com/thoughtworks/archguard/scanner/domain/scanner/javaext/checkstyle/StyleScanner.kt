package com.thoughtworks.archguard.scanner.domain.scanner.javaext.checkstyle

import com.thoughtworks.archguard.scanner.domain.analyser.ArchitectureDependencyAnalysis
import com.thoughtworks.archguard.scanner.domain.config.model.ToolConfigure
import com.thoughtworks.archguard.scanner.domain.scanner.Scanner
import com.thoughtworks.archguard.scanner.domain.scanner.javaext.bs.ScanContext
import org.dom4j.Element
import org.dom4j.io.SAXReader
import org.slf4j.LoggerFactory
import org.springframework.beans.factory.annotation.Autowired
import org.springframework.stereotype.Service
import java.io.File
import java.util.UUID
import kotlin.collections.ArrayList
import kotlin.collections.HashMap

@Service
class StyleScanner : Scanner {

    private val log = LoggerFactory.getLogger(ArchitectureDependencyAnalysis::class.java)

    @Autowired
    private lateinit var styleRepo: StyleRepo
    override fun getScannerName(): String {
        return "CheckStyle"
    }

    override fun toolListGenerator(): List<ToolConfigure> {
        val result = ArrayList<ToolConfigure>()
        val config = HashMap<String, String>()
        config["available"] = "false"
        config["filePath"] = ""
        result.add(ToolConfigure(getScannerName(), config))
        return result
    }

    override fun canScan(context: ScanContext): Boolean {
        return context.language.lowercase() == "jvm"
    }

    override fun scan(context: ScanContext) {
        if (context.config.find { it.type == "CheckStyle" }?.configs?.get("available") == "true") {
            val styleReport = CheckStyleTool(context).getStyleReport()
            val checkStyles = styleReport.mapNotNull { mapTo(it, context.systemId) }.flatten()
            save(checkStyles)
        }
    }

    private fun mapTo(file: File, systemId: Long): List<Style>? {
        val saxReader = SAXReader()
        try {
            val rootElement = saxReader.read(file).rootElement
            if (rootElement.name == "checkstyle") {
                val list = rootElement.elements().map { e ->
                    e as Element
                    val name = e.attributeValue("name")
                    e.elements().map { elementToStyle(it as Element, name, systemId) }
                }
                return list.flatten()
            }
        } catch (e: Exception) {
            log.error("failed to parse checkstyle ", e)
        }
        return null
    }

    private fun elementToStyle(element: Element, name: String, systemId: Long): Style {
        return Style(
            UUID.randomUUID().toString(), systemId, name,
            element.attributeValue("source"),
            element.attributeValue("message"),
            element.attributeValue("line").toInt(),
            element.attributeValue("column").orEmpty().toIntOrNull() ?: 0,
            element.attributeValue("severity")
        )
    }

    private fun save(reports: List<Style>) {
        styleRepo.deleteAll()
        styleRepo.save(reports)
    }
}
