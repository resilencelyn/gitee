package com.thoughtworks.archguard.scanner2.domain.service

import com.thoughtworks.archguard.scanner2.domain.CycleDetector
import com.thoughtworks.archguard.scanner2.domain.model.Dependency
import com.thoughtworks.archguard.scanner2.domain.model.Graph
import com.thoughtworks.archguard.scanner2.domain.model.GraphStore
import com.thoughtworks.archguard.scanner2.domain.model.IdNode
import com.thoughtworks.archguard.scanner2.domain.model.JClassVO
import com.thoughtworks.archguard.scanner2.domain.model.JMethodVO
import com.thoughtworks.archguard.scanner2.domain.model.Node
import com.thoughtworks.archguard.scanner2.domain.repository.JClassRepository
import com.thoughtworks.archguard.scanner2.domain.repository.JMethodRepository
import com.thoughtworks.archguard.scanner2.infrastructure.Toggle
import org.springframework.stereotype.Service

@Service
class ScannerCircularDependencyService(private val jClassRepository: JClassRepository, private val jMethodRepository: JMethodRepository) {
    fun getClassCircularDependency(systemId: Long): List<List<JClassVO>> {
        val allClassDependencies = jClassRepository.getDistinctClassDependenciesAndNotThirdParty(systemId)
        val cycles = findCyclesFromDependencies(allClassDependencies)
        val jClassesHasModules = jClassRepository.getJClassesNotThirdPartyAndNotTest(systemId)
        if (cycles.isEmpty()) {
            return emptyList()
        }

        val cycleList = cycles.map { it.map { jClassesHasModules.first { jClass -> jClass.id == it.getNodeId() }.toVO() } }
        return if (Toggle.EXCLUDE_INTERNAL_CLASS_CYCLE_DEPENDENCY.getStatus()) {
            cycleList.filter { cycle -> !isInternalClassCycle(cycle) }
        } else {
            cycleList
        }
    }

    private fun isInternalClassCycle(jClassList: List<JClassVO>): Boolean {
        return when (jClassList.map { it.getBaseClassName() }.toSet().size) {
            1 -> true
            else -> false
        }
    }

    fun getMethodCircularDependency(systemId: Long): List<List<JMethodVO>> {
        val allMethodDependencies = jMethodRepository.getDistinctMethodDependenciesAndNotThirdParty(systemId)
        val cycles = findCyclesFromDependencies(allMethodDependencies)
        if (cycles.isEmpty()) {
            return emptyList()
        }
        val methodsHasModules = jMethodRepository.getMethodsNotThirdParty(systemId)
        return cycles.map { it.map { methodsHasModules.first { jMethod -> jMethod.id == it.getNodeId() }.toVO() } }
    }

    fun getModuleCircularDependency(systemId: Long): List<List<String>> {
        val moduleDependencies = buildModuleDependencies(systemId)
        val cycles = findCyclesFromDependencies(moduleDependencies)
        if (cycles.isEmpty()) {
            return emptyList()
        }
        return cycles.map { it.map { it.getNodeId() } }
    }

    fun getPackageCircularDependency(systemId: Long): List<List<String>> {
        val packageDependencies = buildPackageDependencies(systemId)
        val cycles = findCyclesFromDependencies(packageDependencies)
        if (cycles.isEmpty()) {
            return emptyList()
        }
        return cycles.map { it.map { it.getNodeId() } }
    }

    private fun buildModuleDependencies(systemId: Long): MutableSet<Dependency<String>> {
        val allClassDependencies = buildAllClassDependencies(systemId)
        val moduleDependencies = mutableSetOf<Dependency<String>>()
        allClassDependencies.forEach {
            if (it.caller.module!! != it.callee.module!!) {
                moduleDependencies.add(Dependency(it.caller.module, it.callee.module))
            }
        }
        return moduleDependencies
    }

    private fun buildPackageDependencies(systemId: Long): MutableSet<Dependency<String>> {
        val allClassDependencies = buildAllClassDependencies(systemId)
        val packageDependencies = mutableSetOf<Dependency<String>>()
        allClassDependencies.forEach {
            val callerPackageName = "${it.caller.module}.${it.caller.getPackageName()}"
            val calleePackageName = "${it.caller.module}.${it.callee.getPackageName()}"
            if (callerPackageName != calleePackageName) {
                packageDependencies.add(Dependency(callerPackageName, calleePackageName))
            }
        }
        return packageDependencies
    }

    private fun buildAllClassDependencies(systemId: Long): List<Dependency<JClassVO>> {
        val allClassIdDependencies = jClassRepository.getDistinctClassDependenciesAndNotThirdParty(systemId)
        val jClassesHasModules = jClassRepository.getJClassesNotThirdPartyAndNotTest(systemId)
        return allClassIdDependencies.map { dependency: Dependency<String> ->
            Dependency(
                jClassesHasModules.first { jClass -> jClass.id == dependency.caller }.toVO(),
                jClassesHasModules.first { jClass -> jClass.id == dependency.callee }.toVO()
            )
        }
    }

    private fun findCyclesFromDependencies(dependencies: Collection<Dependency<String>>): List<List<Node>> {
        val graph = buildGraph(dependencies)
        return CycleDetector(graph).findCycles()
    }

    private fun buildGraph(allClassDependencies: Collection<Dependency<String>>): Graph {
        val graph = GraphStore()
        allClassDependencies.forEach { graph.addEdge(IdNode(it.caller), IdNode(it.callee)) }
        return graph.toDirectedGraph()
    }
}
