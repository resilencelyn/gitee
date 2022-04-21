package com.thoughtworks.archguard.report.domain.container

class ContainerServiceDO(
    val id: Long,
    val systemName: String,
    val language: String
)

class ContainerServiceResponse(
    val id: Long = 0,
    val name: String = "",
    val language: String = "",
    val demands: List<ContainerDemand>,
    val resources: List<ContainerResource>,
)

class ContainerDemand(
    val systemId: String,
    val sourcePackage: String,
    val sourceClass: String,
    val sourceMethod: String,
    var targetUrl: String,
    var originUrl: String = "",
    val targetHttpMethod: String,
)

class ContainerResource(
    val systemId: String,
    var sourceUrl: String,
    var originUrl: String = "",
    val sourceHttpMethod: String,
    val packageName: String,
    val className: String,
    val methodName: String
)
