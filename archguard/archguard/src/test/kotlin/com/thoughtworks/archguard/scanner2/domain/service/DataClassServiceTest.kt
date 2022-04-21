package com.thoughtworks.archguard.scanner2.domain.service

import com.thoughtworks.archguard.scanner2.domain.model.JClass
import com.thoughtworks.archguard.scanner2.domain.model.JField
import com.thoughtworks.archguard.scanner2.domain.model.JMethod
import org.assertj.core.api.Assertions.assertThat
import org.junit.jupiter.api.Test

internal class DataClassServiceTest {

    @Test
    internal fun should_return_true_when_class_is_data_class() {
        val jClass = JClass("c1", "class1", "module1")
        val jField1 = JField("j1", "hello", "boolean")
        val jField2 = JField("j2", "goodField", "java.lang.String")
        jClass.fields = listOf(jField1, jField2)
        val jMethod1 = JMethod("m1", "isHello", "class1", "module1", "boolean", emptyList())
        val jMethod2 = JMethod("m2", "setHello", "class1", "module1", "void", listOf("boolean"))
        val jMethod3 = JMethod("m3", "getGoodField", "class1", "module1", "java.lang.String", emptyList())
        val jMethod4 = JMethod("m4", "setGoodField", "class1", "module1", "void", listOf("java.lang.String"))
        jClass.methods = listOf(jMethod1, jMethod2, jMethod3, jMethod4)

        assertThat(checkIsDataClass(jClass)).isTrue()
    }

    @Test
    internal fun should_return_false_when_check_class_is_data_class_when_wrong_method_count() {
        val jClass = JClass("c1", "class1", "module1")
        val jField1 = JField("j1", "hello", "boolean")
        val jField2 = JField("j2", "goodField", "java.lang.String")
        jClass.fields = listOf(jField1, jField2)
        val jMethod1 = JMethod("m1", "isHello", "class1", "module1", "boolean", emptyList())
        val jMethod2 = JMethod("m2", "setHello", "class1", "module1", "void", listOf("boolean"))
        val jMethod3 = JMethod("m3", "getGoodField", "class1", "module1", "java.lang.String", emptyList())
        val jMethod4 = JMethod("m4", "setGoodField", "class1", "module1", "void", listOf("java.lang.String"))
        val jMethod5 = JMethod("m5", "check", "class1", "module1", "boolean", listOf("java.lang.String"))

        jClass.methods = listOf(jMethod1, jMethod2, jMethod3, jMethod4, jMethod5)

        assertThat(checkIsDataClass(jClass)).isFalse()
    }

    @Test
    internal fun should_return_false_when_check_class_is_not_data_class_when_wrong_name() {
        val jClass = JClass("c1", "class1", "module1")
        val jField1 = JField("j1", "hello", "boolean")
        val jField2 = JField("j2", "goodField", "java.lang.String")
        jClass.fields = listOf(jField1, jField2)
        val jMethod1 = JMethod("m1", "isHello", "class1", "module1", "boolean", emptyList())
        val jMethod2 = JMethod("m2", "setHello", "class1", "module1", "void", listOf("boolean"))
        val jMethod3 = JMethod("m3", "retrieveGoodField", "class1", "module1", "java.lang.String", emptyList())
        val jMethod4 = JMethod("m4", "setGoodField", "class1", "module1", "void", listOf("java.lang.String"))

        jClass.methods = listOf(jMethod1, jMethod2, jMethod3, jMethod4)

        assertThat(checkIsDataClass(jClass)).isFalse()
    }
}
