package com.thoughtworks.archguard.scanner.domain.hubexecutor

import com.thoughtworks.archguard.scanner.domain.scanner.javaext.bs.ScanContext
import com.thoughtworks.archguard.scanner.infrastructure.FileOperator

class HubExecutor(private val context: ScanContext, private val manager: ScannerManager) : HubLifecycle {

    override fun execute() {
        manager.execute(context)
    }

    override fun clean() {
        FileOperator.deleteDirectory(context.workspace)
    }
}
