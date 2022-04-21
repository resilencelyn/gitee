package com.thoughtworks.archguard.scanner2.domain.model

import com.fasterxml.jackson.annotation.JsonIgnore

/**
 * JClassVO is a Value Object, use for LogicModule aggregation
 */
class JClassVO(val name: String, val module: String?) : Node {
    var id: String? = null
    val fullName = "$module.$name"

    companion object {
        fun create(fullName: String): JClassVO {
            val split = fullName.split(".")
            return JClassVO(split.subList(1, split.size).joinToString(".").trim(), split[0].trim())
        }
    }

    override fun toString(): String {
        return "JClassVO(name='$name', module='$module')"
    }

    @JsonIgnore
    override fun getNodeId(): String {
        return id!!
    }

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as JClassVO

        if (name != other.name) return false
        if (module != other.module) return false

        return true
    }

    override fun hashCode(): Int {
        var result = name.hashCode()
        result = 31 * result + module.hashCode()
        return result
    }

    fun getPackageName(): String {
        if (!name.contains('.')) return ""

        val endIndex = name.indexOfLast { it == '.' }
        return name.substring(0, endIndex)
    }

    fun getTypeName(): String {
        return name.substringAfterLast(".")
    }

    fun getBaseClassName(): String {
        return name.substringBefore("$", name)
    }
}
