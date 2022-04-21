package com.thoughtworks.archguard.scanner.domain.scanner.javaext.pmd

import com.thoughtworks.archguard.scanner.domain.scanner.Scanner
import com.thoughtworks.archguard.scanner.domain.scanner.javaext.bs.ScanContext
import com.thoughtworks.archguard.scanner.infrastructure.db.PmdRepository
import com.thoughtworks.archguard.scanner.infrastructure.db.Violation
import org.dom4j.Element
import org.dom4j.io.SAXReader
import org.slf4j.Logger
import org.slf4j.LoggerFactory
import org.springframework.beans.factory.annotation.Autowired
import org.springframework.stereotype.Service
import java.io.File

@Service
class PmdScanner(@Autowired val pmdRepository: PmdRepository) : Scanner {
    val log: Logger = LoggerFactory.getLogger(PmdScanner::class.java)
    override fun getScannerName(): String {
        return "pmd"
    }

    override fun scan(context: ScanContext) {
        val reportFiles = PmdTool(context).getReportFiles()
        val voilations = reportFiles.mapNotNull { mapTo(it, context.systemId) }.flatten()
        save(voilations)
    }

    private fun mapTo(file: File, systemId: Long): List<Violation>? {
        val saxReader = SAXReader()
        try {
            val rootElement = saxReader.read(file).rootElement
            if (rootElement.name == "pmd") {
                val list = rootElement.elements().map { fileE ->
                    fileE as Element
                    val fileName = fileE.attributeValue("name")
                    fileE.elements().map { elementToViolation(it as Element, fileName, systemId) }
                }
                return list.flatten()
            }
        } catch (e: Exception) {
            log.error("failed to parse PMD ", e)
        }
        return null
    }

    private fun elementToViolation(violationE: Element, file: String, systemId: Long): Violation {
        return Violation(
            systemId = systemId, file = file,
            beginline = violationE.attributeValue("beginline").toInt(),
            endline = violationE.attributeValue("endline").toInt(),
            priority = violationE.attributeValue("priority").toInt(),
            text = violationE.text
        )
    }

    private fun save(vioList: List<Violation>) {
        pmdRepository.clean()
        pmdRepository.save(vioList)
    }
}
