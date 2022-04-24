package com.thoughtworks.archguard.code.method.controller

import com.thoughtworks.archguard.code.method.domain.JMethod
import com.thoughtworks.archguard.code.method.domain.MethodService
import org.springframework.http.ResponseEntity
import org.springframework.web.bind.annotation.GetMapping
import org.springframework.web.bind.annotation.PathVariable
import org.springframework.web.bind.annotation.RequestMapping
import org.springframework.web.bind.annotation.RequestParam
import org.springframework.web.bind.annotation.RestController

@RestController
@RequestMapping("/api/systems/{systemId}/methods")
class MethodController(val methodService: MethodService) {

    @GetMapping("/callees")
    fun getMethodCallees(
        @PathVariable("systemId") systemId: Long,
        @RequestParam("name") methodName: String,
        @RequestParam(value = "clazz") clazzName: String,
        @RequestParam(value = "deep", required = false, defaultValue = "3") deep: Int,
        @RequestParam(value = "needIncludeImpl", required = false, defaultValue = "true") needIncludeImpl: Boolean,
        @RequestParam(value = "module") moduleName: String
    ): ResponseEntity<List<JMethod>> {
        val jMethod = methodService.findMethodCallees(systemId, moduleName, clazzName, methodName, deep, needIncludeImpl)
        return ResponseEntity.ok(jMethod)
    }

    @GetMapping("/callers")
    fun getMethodCallees(
        @PathVariable("systemId") systemId: Long,
        @RequestParam("name") methodName: String,
        @RequestParam(value = "clazz") clazzName: String,
        @RequestParam(value = "deep", required = false, defaultValue = "3") deep: Int,
        @RequestParam(value = "module") moduleName: String
    ): ResponseEntity<List<JMethod>> {
        val jMethod = methodService.findMethodCallers(systemId, moduleName, clazzName, methodName, deep)
        return ResponseEntity.ok(jMethod)
    }

    @GetMapping("/invokes")
    fun getMethodCallees(
        @PathVariable("systemId") systemId: Long,
        @RequestParam("name") methodName: String,
        @RequestParam(value = "clazz") clazzName: String,
        @RequestParam(value = "deep", required = false, defaultValue = "3") deep: Int,
        @RequestParam(value = "callerDeep", required = false) callerDeep: Int?,
        @RequestParam(value = "calleeDeep", required = false) calleeDeep: Int?,
        @RequestParam(value = "needIncludeImpl", required = false, defaultValue = "true") needIncludeImpl: Boolean,
        @RequestParam(value = "module") moduleName: String
    ): ResponseEntity<List<JMethod>> {
        val jMethod = methodService.findMethodInvokes(
            systemId, moduleName, clazzName, methodName,
            callerDeep
                ?: deep,
            calleeDeep
                ?: deep,
            needIncludeImpl
        )
        return ResponseEntity.ok(jMethod)
    }

    @GetMapping
    fun getMethodsBelongToClass(
        @PathVariable("systemId") systemId: Long,
        @RequestParam(value = "clazz") clazzName: String,
        @RequestParam(value = "submodule") submoduleName: String
    ): ResponseEntity<List<JMethod>> {
        val jMethod = methodService.findMethodByModuleAndClazz(systemId, clazzName, submoduleName)
        return ResponseEntity.ok(jMethod)
    }
}
