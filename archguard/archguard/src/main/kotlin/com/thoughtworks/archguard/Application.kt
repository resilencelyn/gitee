package com.thoughtworks.archguard

import com.thoughtworks.archguard.change.infrastructure.DiffChangeDao
import com.thoughtworks.archguard.change.infrastructure.GitChangeDao
import com.thoughtworks.archguard.report.infrastructure.ContainerServiceDao
import com.thoughtworks.archguard.scanner.infrastructure.db.BadSmellDao
import com.thoughtworks.archguard.scanner.infrastructure.db.ConfigDao
import com.thoughtworks.archguard.scanner.infrastructure.db.OverviewDao
import com.thoughtworks.archguard.scanner.infrastructure.db.ScannerGitHotFileDao
import com.thoughtworks.archguard.scanner.infrastructure.db.StyleDao
import com.thoughtworks.archguard.scanner.infrastructure.db.TestBadSmellDao
import com.thoughtworks.archguard.scanner2.infrastructure.mysql.ClassMetricsDao
import com.thoughtworks.archguard.scanner2.infrastructure.mysql.MethodMetricsDao
import com.thoughtworks.archguard.scanner2.infrastructure.mysql.ModuleMetricsDao
import com.thoughtworks.archguard.scanner2.infrastructure.mysql.PackageMetricsDao
import org.jdbi.v3.core.Jdbi
import org.jdbi.v3.core.kotlin.KotlinPlugin
import org.jdbi.v3.core.spi.JdbiPlugin
import org.jdbi.v3.spring4.JdbiFactoryBean
import org.jdbi.v3.sqlobject.SqlObjectPlugin
import org.jdbi.v3.sqlobject.kotlin.KotlinSqlObjectPlugin
import org.springframework.beans.factory.annotation.Autowired
import org.springframework.boot.autoconfigure.SpringBootApplication
import org.springframework.boot.runApplication
import org.springframework.cache.annotation.EnableCaching
import org.springframework.context.annotation.Bean
import org.springframework.jdbc.datasource.TransactionAwareDataSourceProxy
import springfox.documentation.oas.annotations.EnableOpenApi
import java.util.TimeZone
import java.util.function.Consumer
import javax.sql.DataSource

@SpringBootApplication
@EnableOpenApi
@EnableCaching
class Application {
    @Bean
    fun jdbiFactory(@Autowired ds: DataSource): JdbiFactoryBean {
        val factoryBean = JdbiFactoryBean(ds)
        factoryBean.setAutoInstallPlugins(true)
        return factoryBean
    }

    @Bean
    fun gitHotFileDao(jdbi: Jdbi): GitChangeDao {
        return jdbi.onDemand(GitChangeDao::class.java)
    }

    @Bean
    fun diffChangeDao(jdbi: Jdbi): DiffChangeDao {
        return jdbi.onDemand(DiffChangeDao::class.java)
    }

    @Bean
    fun containerDemandDao(jdbi: Jdbi): ContainerServiceDao {
        return jdbi.onDemand(ContainerServiceDao::class.java)
    }

    @Bean
    fun scannerGitHotFileDao(jdbi: Jdbi): ScannerGitHotFileDao {
        return jdbi.onDemand(ScannerGitHotFileDao::class.java)
    }

    @Bean
    fun jdbi(ds: DataSource, jdbiPlugins: List<JdbiPlugin>): Jdbi {
        val proxy = TransactionAwareDataSourceProxy(ds)
        val jdbi = Jdbi.create(proxy)
        jdbiPlugins.forEach(Consumer { plugin: JdbiPlugin? -> jdbi.installPlugin(plugin) })
        return jdbi
    }

    @Bean
    fun jdbiPlugins(): List<JdbiPlugin> {
        return listOf(SqlObjectPlugin(), KotlinPlugin(), KotlinSqlObjectPlugin())
    }

    @Bean
    fun badSmellModelDao(jdbi: Jdbi): BadSmellDao {
        return jdbi.onDemand(BadSmellDao::class.java)
    }

    @Bean
    fun testBadSmellModelDao(jdbi: Jdbi): TestBadSmellDao {
        return jdbi.onDemand(TestBadSmellDao::class.java)
    }

    @Bean
    fun checkStylesDao(jdbi: Jdbi): StyleDao {
        return jdbi.onDemand(StyleDao::class.java)
    }

    @Bean
    fun overviewDao(jdbi: Jdbi): OverviewDao {
        return jdbi.onDemand(OverviewDao::class.java)
    }

    @Bean
    fun configDao(jdbi: Jdbi): ConfigDao {
        return jdbi.onDemand(ConfigDao::class.java)
    }

    @Bean
    fun classMetricsDao(jdbi: Jdbi): ClassMetricsDao {
        return jdbi.onDemand(ClassMetricsDao::class.java)
    }

    @Bean
    fun methodMetricsDao(jdbi: Jdbi): MethodMetricsDao {
        return jdbi.onDemand(MethodMetricsDao::class.java)
    }

    @Bean
    fun packageMetricsDao(jdbi: Jdbi): PackageMetricsDao {
        return jdbi.onDemand(PackageMetricsDao::class.java)
    }

    @Bean
    fun moduleMetricsDao(jdbi: Jdbi): ModuleMetricsDao {
        return jdbi.onDemand(ModuleMetricsDao::class.java)
    }
}

fun main(args: Array<String>) {
    TimeZone.setDefault(TimeZone.getTimeZone("Asia/Shanghai"))
    runApplication<Application>(*args)
}
