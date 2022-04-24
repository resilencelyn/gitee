val ktlint by configurations.creating

plugins {
    // springfox not support spring boot 2.6, see in https://github.com/springfox/springfox/issues/3462
    id("org.springframework.boot") version "2.5.10"

    // flyway 7.0 require spring .boot > 2.4
    id("org.flywaydb.flyway").version("7.15.0")
    id("io.spring.dependency-management") version "1.0.9.RELEASE"
    id("com.avast.gradle.docker-compose") version "0.15.2"

    kotlin("jvm") version "1.6.10"
    kotlin("plugin.spring") version "1.6.10"
    kotlin("plugin.serialization") version "1.6.10"

    jacoco
}

group = "com.thoughtworks.archguard"
version = "1.6.2"
java.sourceCompatibility = JavaVersion.VERSION_1_8

repositories {
    maven {
        url = uri("https://oss.sonatype.org/content/repositories/snapshots")
    }
    mavenCentral()
    mavenLocal()
}

dependencies {
    ktlint("com.pinterest:ktlint:0.44.0") {
        attributes {
            attribute(Bundling.BUNDLING_ATTRIBUTE, objects.named(Bundling.EXTERNAL))
        }
    }

    // kotlin configs
    implementation(kotlin("stdlib"))
    implementation(kotlin("stdlib-jdk8"))
    implementation(kotlin("reflect"))
    // Kotlin reflection.
    implementation(kotlin("test"))
    implementation(kotlin("test-junit"))

    implementation("org.jdbi:jdbi3-core:3.10.1")
    implementation("org.jdbi:jdbi3-spring4:3.10.1")    // provide JdbiFactoryBean
    implementation("org.jdbi:jdbi3-kotlin-sqlobject:3.10.1")
    implementation("org.jdbi:jdbi3-kotlin:3.10.1")
    implementation("org.jdbi:jdbi3-testing:3.10.1")

    implementation("io.pebbletemplates:pebble:3.0.8")

    implementation("io.springfox:springfox-boot-starter:3.0.0")
    implementation("io.springfox:springfox-swagger-ui:3.0.0")

    implementation("io.ktor:ktor-server-core:1.1.4")
    implementation("io.ktor:ktor-server-netty:1.1.4")
    implementation("io.ktor:ktor-gson:1.1.4")

    implementation("org.springframework.boot:spring-boot-starter-jdbc")
    implementation("org.springframework.boot:spring-boot-starter-web")
    implementation("org.springframework.boot:spring-boot-starter-actuator")
    implementation("org.springframework.boot:spring-boot-starter-validation")
    // cache for overview api
    implementation("org.springframework.boot:spring-boot-starter-cache")

    implementation("org.nield:kotlinstatistics:0.3.0")
    implementation("com.alibaba:druid-spring-boot-starter:1.2.8")

    implementation("dom4j:dom4j:1.6.1")

    implementation("com.google.code.gson:gson:2.8.6")
    implementation("com.fasterxml.jackson.module:jackson-module-kotlin")

    runtimeOnly("mysql:mysql-connector-java")

    implementation("org.flywaydb:flyway-core:7.15.0")
    testImplementation("io.mockk:mockk:1.10.0")
    testImplementation("com.github.database-rider:rider-spring:1.16.1")
    testImplementation("com.h2database:h2:1.4.200")
    testImplementation("org.flywaydb:flyway-core:6.5.7")
    testImplementation("org.springframework.boot:spring-boot-starter-test")
}

configure<io.spring.gradle.dependencymanagement.dsl.DependencyManagementExtension> {
    imports {
        mavenBom("org.springframework.cloud:spring-cloud-dependencies:Hoxton.SR6")
    }
}

tasks.withType<org.jetbrains.kotlin.gradle.tasks.KotlinCompile> {
    kotlinOptions {
        freeCompilerArgs = listOf("-Xjsr305=strict")
        jvmTarget = "1.8"
    }
}

tasks.withType<Test> {
    useJUnitPlatform()
}
//
//tasks.test {
//    finalizedBy(tasks.jacocoTestCoverageVerification)
//}

tasks.jacocoTestReport {
    dependsOn(tasks.test)
}

tasks.jacocoTestReport {
    reports {
        xml.required.set(true)
        csv.required.set(false)
        html.outputLocation.set(layout.buildDirectory.dir("jacocoHtml"))
    }
}

tasks.jacocoTestCoverageVerification {
    violationRules {
        rule {
            limit {
                counter = "LINE"
                value = "COVEREDRATIO"
                minimum = "0.25".toBigDecimal()
            }
        }

        rule {
            limit {
                counter = "BRANCH"
                value = "COVEREDRATIO"
                minimum = "0.3".toBigDecimal()
            }
        }
    }
}

tasks.jacocoTestCoverageVerification {
    dependsOn(tasks.jacocoTestReport)
}

val outputDir = "${project.buildDir}/reports/ktlint/"
val inputFiles = project.fileTree(mapOf("dir" to "src", "include" to "**/*.kt"))

val ktlintCheck by tasks.creating(JavaExec::class) {
    inputs.files(inputFiles)
    outputs.dir(outputDir)

    description = "Check Kotlin code style."
    classpath = ktlint
    mainClass.set("com.pinterest.ktlint.Main")
    args = listOf("src/**/*.kt")
}

val ktlintFormat by tasks.creating(JavaExec::class) {
    inputs.files(inputFiles)
    outputs.dir(outputDir)

    description = "Fix Kotlin code style deviations."
    classpath = ktlint
    mainClass.set("com.pinterest.ktlint.Main")
    args = listOf("-F", "src/**/*.kt")
}

val installGitHooks = task<Copy>("installGitHooks") {
    from(file("$projectDir/config/githooks/commit-msg"))
    into(file("$projectDir/.git/hooks"))
    fileMode = 493
}

tasks.check { dependsOn(installGitHooks) }

dockerCompose {
    isRequiredBy(project.tasks.bootRun)
    useComposeFiles.set(listOf("$projectDir/config/infrastructure/docker-compose.local.yml"))
    removeVolumes.set(false)
}
