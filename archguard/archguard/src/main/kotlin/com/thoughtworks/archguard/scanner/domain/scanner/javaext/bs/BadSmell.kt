package com.thoughtworks.archguard.scanner.domain.scanner.javaext.bs

data class BadSmell(
    val id: String,
    val systemId: Long,
    val entityName: String,
    val line: Int,
    val description: String?,
    val size: Int,
    val type: String
)
