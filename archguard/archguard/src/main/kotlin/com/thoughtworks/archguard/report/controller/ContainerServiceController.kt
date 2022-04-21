package com.thoughtworks.archguard.report.controller

import com.thoughtworks.archguard.report.application.ServicesMapService
import com.thoughtworks.archguard.report.domain.container.ContainerServiceResponse
import org.springframework.web.bind.annotation.GetMapping
import org.springframework.web.bind.annotation.PathVariable
import org.springframework.web.bind.annotation.RequestMapping
import org.springframework.web.bind.annotation.RequestParam
import org.springframework.web.bind.annotation.RestController

@RestController
@RequestMapping("/api/container-service")
class ContainerServiceController(val service: ServicesMapService) {
    @GetMapping("/{systemId}")
    fun getServicesMap(@PathVariable("systemId") systemId: Long): ContainerServiceResponse {
        return service.findBySystemId(systemId)
    }

    @GetMapping("/systems")
    fun getSystemByIds(@RequestParam(name = "system") ids: List<String>): List<ContainerServiceResponse> {
        return service.findAllServiceByIds(ids)
    }

    @GetMapping("/flare")
    fun getAllContainerServices(): List<ContainerServiceResponse> {
        return service.allContainerServices()
    }
}
