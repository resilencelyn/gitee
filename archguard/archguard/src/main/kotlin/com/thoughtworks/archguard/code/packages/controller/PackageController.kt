package com.thoughtworks.archguard.code.packages.controller

import com.thoughtworks.archguard.code.packages.domain.ModulePackage
import com.thoughtworks.archguard.code.packages.domain.PackageService
import org.springframework.web.bind.annotation.GetMapping
import org.springframework.web.bind.annotation.PathVariable
import org.springframework.web.bind.annotation.RequestMapping
import org.springframework.web.bind.annotation.RequestParam
import org.springframework.web.bind.annotation.RestController

@RestController
@RequestMapping("/api/systems/{systemId}/package")
class PackageController(val packageService: PackageService) {

    @GetMapping("/dependencies")
    fun getPackageDependencies(
        @PathVariable("systemId") systemId: Long,
        @RequestParam(value = "language", required = false, defaultValue = "jvm") language: String,
    ): List<ModulePackage> {
        return packageService.getPackageDependencies(systemId, language)
    }
}
