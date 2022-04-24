package com.thoughtworks.archguard.code.project

import org.jdbi.v3.core.Jdbi
import org.jdbi.v3.core.mapper.reflect.ConstructorMapper
import org.springframework.beans.factory.annotation.Autowired
import org.springframework.stereotype.Repository

@Repository
class ProjectRepositoryImpl : ProjectRepository {

    @Autowired
    lateinit var jdbi: Jdbi

    override fun getProjectDependencies(systemId: Long): List<CompositionDependencyDTO> {
        return jdbi.withHandle<List<CompositionDependencyDTO>, Nothing> {
            it.registerRowMapper(ConstructorMapper.factory(CompositionDependencyDTO::class.java))
            it.createQuery(
                "select id, name, version, system_id as systemId, parent_id as parentId, " +
                    "package_manager as packageManager, dep_name as depName, dep_artifact as depArtifact, " +
                    "dep_group as depGroup, dep_metadata as depMetadata, dep_scope as depScope, dep_source as depSource, " +
                    "dep_version as depVersion " +
                    "from project_composition_dependencies where system_id=$systemId"
            )
                .mapTo(CompositionDependencyDTO::class.java)
                .list()
        }
    }
}
