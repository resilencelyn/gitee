package com.thoughtworks.archguard.change.application

import com.thoughtworks.archguard.change.domain.GitChangeRepo
import com.thoughtworks.archguard.change.domain.GitHotFile
import com.thoughtworks.archguard.change.domain.GitPathChangeCount
import org.springframework.beans.factory.annotation.Value
import org.springframework.stereotype.Service

@Service
class GitChangeService(val gitChangeRepo: GitChangeRepo, @Value("\${scm_git_hot_file.modified_count_baseline}") val modifiedCountBaseline: Int) {
    fun getGitHotFilesBySystemId(systemId: Long): List<GitHotFile> {
        return gitChangeRepo.findBySystemId(systemId)
            .filter { (it.jclassId != null) && (it.modifiedCount >= modifiedCountBaseline) }
    }

    fun getGitFileChanges(systemId: Long): List<GitHotFile> {
        return gitChangeRepo.findBySystemId(systemId)
    }

    fun getPathChangeCount(systemId: Long): List<GitPathChangeCount> {
        return gitChangeRepo.findCountBySystemId(systemId)
    }

    fun getUnstableFile(systemId: Long): List<GitPathChangeCount> {
        return gitChangeRepo.findUnstableFile(systemId)
    }

    fun getChangesByRange(systemId: Long, startTime: String, endTime: String): List<String> {
        return gitChangeRepo.findChangesByRange(systemId, startTime, endTime)
    }
}
