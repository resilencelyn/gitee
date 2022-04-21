package com.thoughtworks.archguard.code.method.infrastructure

import com.thoughtworks.archguard.code.clazz.domain.JField
import com.thoughtworks.archguard.code.clazz.infrastructure.JClassRepositoryImpl
import com.thoughtworks.archguard.code.method.domain.JMethod
import com.thoughtworks.archguard.code.method.domain.JMethodRepository
import org.jdbi.v3.core.Jdbi
import org.jdbi.v3.core.mapper.reflect.ConstructorMapper
import org.slf4j.LoggerFactory
import org.springframework.stereotype.Repository

@Repository
class JMethodRepositoryImpl(val jdbi: Jdbi) : JMethodRepository {
    private val log = LoggerFactory.getLogger(JClassRepositoryImpl::class.java)

    override fun findMethodFields(id: String): List<JField> {
        val sql = "SELECT id, name, type FROM code_field WHERE id in (select b from code_ref_method_fields where a='$id')"
        return jdbi.withHandle<List<JField>, Nothing> {
            it.registerRowMapper(ConstructorMapper.factory(JField::class.java))
            it.createQuery(sql)
                .mapTo(JField::class.java)
                .list()
        }
    }

    override fun findMethodsByModuleAndClass(systemId: Long, module: String?, name: String): List<JMethod> {
        val sql = "SELECT id, name, clzname as clazz, module, returntype, argumenttypes, access FROM code_method WHERE clzname='$name' AND system_id='$systemId' AND module <=>'$module'"
        return jdbi.withHandle<List<JMethod>, Nothing> {
            it.registerRowMapper(ConstructorMapper.factory(JMethod::class.java))
            it.createQuery(sql)
                .mapTo(JMethodDto::class.java)
                .map { it.toJMethod() }
                .list()
        }
    }

    override fun findMethodCallers(id: String): List<JMethod> {
        val sql = "SELECT id, name, clzname as clazz, module, returntype, argumenttypes, access  FROM code_method WHERE id IN (SELECT a FROM code_ref_method_callees WHERE b='$id') "
        return jdbi.withHandle<List<JMethod>, Nothing> {
            it.registerRowMapper(ConstructorMapper.factory(JMethod::class.java))
            it.createQuery(sql)
                .mapTo(JMethodDto::class.java)
                .map { it.toJMethod() }
                .list()
        }
    }

    override fun findMethodCallees(id: String): List<JMethod> {
        val sql = "SELECT id, name, clzname as clazz, module, returntype, argumenttypes, access  FROM code_method WHERE id IN (SELECT b FROM code_ref_method_callees WHERE a='$id') "
        return jdbi.withHandle<List<JMethod>, Nothing> {
            it.registerRowMapper(ConstructorMapper.factory(JMethod::class.java))
            it.createQuery(sql)
                .mapTo(JMethodDto::class.java)
                .map { it.toJMethod() }
                .list()
        }
    }

    override fun findMethodImplements(id: String, name: String): List<JMethod> {
        val sql = "SELECT id, name, clzname as clazz, module, returntype, argumenttypes, access  " +
            "FROM code_method " +
            "WHERE id IN (SELECT DISTINCT cm.b " +
            "             FROM code_class c, " +
            "                  code_ref_class_methods cm, " +
            "                  code_class p, " +
            "                  code_ref_class_methods pm, " +
            "                  code_ref_class_parent cp " +
            "             WHERE pm.b = '$id' " +
            "               AND p.id = pm.a " +
            "               AND cp.b = p.id " +
            "               AND c.id = cp.a " +
            "               AND cm.a = c.id) " +
            "  AND name = '$name'"
        return jdbi.withHandle<List<JMethod>, Nothing> {
            it.registerRowMapper(ConstructorMapper.factory(JMethod::class.java))
            it.createQuery(sql)
                .mapTo(JMethodDto::class.java)
                .map { it.toJMethod() }
                .list()
        }
    }

    override fun findMethodByModuleAndClazzAndName(systemId: Long, moduleName: String?, clazzName: String, methodName: String): List<JMethod> {
        val sql = "SELECT id, name, clzname as clazz, module, returntype, argumenttypes, access FROM code_method WHERE " +
            "system_id=:systemId AND name=:methodName AND clzname=:clazzName AND module <=> :moduleName"
        return jdbi.withHandle<List<JMethod>, Nothing> {
            it.registerRowMapper(ConstructorMapper.factory(JMethod::class.java))
            it.createQuery(sql)
                .bind("systemId", systemId)
                .bind("methodName", methodName)
                .bind("clazzName", clazzName)
                .bind("moduleName", moduleName)
                .mapTo(JMethodDto::class.java)
                .map { it.toJMethod() }
                .list()
        }
    }
}

class JMethodDto(val id: String, val name: String, val clazz: String, val module: String?, val returnType: String, val argumentTypes: String?, val access: String) {
    fun toJMethod(): JMethod {
        val argumentTypeList = if (argumentTypes.isNullOrBlank()) emptyList() else argumentTypes.split(",")
        val jMethod = JMethod(id, name, clazz, module, returnType, argumentTypeList)
//        TypeMap.getMethodType(access.toInt()).forEach { jMethod.addType(it) }
        return jMethod
    }
}
