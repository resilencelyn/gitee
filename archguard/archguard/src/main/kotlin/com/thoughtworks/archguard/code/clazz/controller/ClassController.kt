package com.thoughtworks.archguard.code.clazz.controller

import com.thoughtworks.archguard.code.clazz.domain.CodeTree
import com.thoughtworks.archguard.code.clazz.domain.JClass
import com.thoughtworks.archguard.code.clazz.domain.service.ClassService
import org.springframework.http.ResponseEntity
import org.springframework.web.bind.annotation.GetMapping
import org.springframework.web.bind.annotation.PathVariable
import org.springframework.web.bind.annotation.RequestMapping
import org.springframework.web.bind.annotation.RequestParam
import org.springframework.web.bind.annotation.RestController

@RestController
@RequestMapping("/api/systems/{systemId}/classes")
class ClassController(val service: ClassService) {

    @GetMapping("/{name}/dependencies")
    fun getDependencies(
        @PathVariable("systemId") systemId: Long,
        @PathVariable("name") name: String,
        @RequestParam(value = "module", required = false, defaultValue = "") module: String,
        @RequestParam("deep", required = false, defaultValue = "3") deep: Int
    ): JClass {
        return service.getDependencies(systemId, module, name, deep)
    }

    @GetMapping("/{name}/invokes")
    fun getInvokes(
        @PathVariable("systemId") systemId: Long,
        @PathVariable("name") name: String,
        @RequestParam(value = "module", required = false, defaultValue = "") module: String,
        @RequestParam(value = "deep", required = false, defaultValue = "3") deep: Int,
        @RequestParam(value = "callerDeep", required = false) callerDeep: Int?,
        @RequestParam(value = "calleeDeep", required = false) calleeDeep: Int?,
        @RequestParam(value = "needIncludeImpl", required = false, defaultValue = "true") needIncludeImpl: Boolean?
    ): JClass {
        return service.findInvokes(
            systemId, module, name, callerDeep ?: deep, calleeDeep ?: deep,
            needIncludeImpl
                ?: true
        )
    }

    @GetMapping("/{name}/methods_callees")
    fun getMethodsCallees(
        @PathVariable("systemId") systemId: Long,
        @PathVariable("name") name: String,
        @RequestParam(value = "module", required = false, defaultValue = "") module: String,
        @RequestParam(value = "deep", required = false, defaultValue = "3") deep: Int,
        @RequestParam(value = "needParents", required = false, defaultValue = "true") needParents: Boolean,
        @RequestParam(value = "needIncludeImpl", required = false, defaultValue = "true") needIncludeImpl: Boolean
    ): JClass {
        return service.findMethodsCallees(systemId, module, name, deep, needIncludeImpl, needParents)
    }

    @GetMapping("/code-tree")
    fun getCodeTree(@PathVariable("systemId") systemId: Long): ResponseEntity<CodeTree> {
        val codeTree = service.initCodeTree(systemId)
        return ResponseEntity.ok(codeTree)
    }
}
