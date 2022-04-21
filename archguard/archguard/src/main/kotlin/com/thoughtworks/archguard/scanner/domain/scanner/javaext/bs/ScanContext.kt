package com.thoughtworks.archguard.scanner.domain.scanner.javaext.bs

import com.thoughtworks.archguard.scanner.domain.config.model.ToolConfigure
import com.thoughtworks.archguard.scanner.domain.system.BuildTool
import com.thoughtworks.archguard.scanner.infrastructure.command.StreamConsumer
import java.io.File

data class ScanContext(
    val systemId: Long,
    var repo: String,
    var buildTool: BuildTool,
    val workspace: File,
    val dbUrl: String,
    val config: List<ToolConfigure>,
    val language: String,
    val codePath: String,
    val branch: String,
    val logStream: StreamConsumer,
    val additionArguments: List<String>
)
