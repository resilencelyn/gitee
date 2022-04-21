package com.thoughtworks.archguard.report.domain.sizing

import com.thoughtworks.archguard.report.infrastructure.FilterSizingPO

interface SizingRepository {
    fun getModuleSizingListAbovePackageCountThresholdCount(systemId: Long, threshold: Int): Long

    fun getModuleSizingListAbovePackageCountThreshold(systemId: Long, threshold: Int, limit: Long, offset: Long): List<ModuleSizing>

    fun getModuleSizingListAbovePackageCountThresholdCount(systemId: Long, threshold: Int, module: String): Long
    fun getModuleSizingListAbovePackageCountThresholdByFilterSizing(systemId: Long, threshold: Int, limit: Long, offset: Long, module: String): List<ModuleSizing>

    fun getModuleSizingAboveLineThresholdCount(systemId: Long, threshold: Int): Long

    fun getModuleSizingAboveLineThreshold(systemId: Long, threshold: Int, limit: Long, offset: Long): List<ModuleSizing>

    fun getModuleSizingAboveLineThresholdCount(systemId: Long, threshold: Int, module: String): Long
    fun getModuleSizingAboveLineThresholdByFilterSizing(systemId: Long, threshold: Int, limit: Long, offset: Long, s: String): List<ModuleSizing>

    fun getPackageSizingListAboveClassCountThresholdCount(systemId: Long, threshold: Int): Long
    fun getPackageSizingListAboveClassCountThreshold(systemId: Long, threshold: Int, limit: Long, offset: Long): List<PackageSizing>

    fun getPackageSizingListAboveClassCountThresholdCount(systemId: Long, threshold: Int, filter: FilterSizingPO): Long
    fun getPackageSizingListAboveClassCountThresholdByFilterSizing(systemId: Long, threshold: Int, filter: FilterSizingPO): List<PackageSizing>

    fun getPackageSizingAboveLineThresholdCount(systemId: Long, threshold: Int): Long

    fun getPackageSizingAboveLineThreshold(systemId: Long, threshold: Int, limit: Long, offset: Long): List<PackageSizing>

    fun getPackageSizingAboveLineThresholdCount(systemId: Long, threshold: Int, filter: FilterSizingPO): Long
    fun getPackageSizingAboveLineThresholdByFilterSizing(systemId: Long, threshold: Int, filter: FilterSizingPO): List<PackageSizing>

    fun getMethodSizingAboveLineThreshold(systemId: Long, threshold: Int, limit: Long, offset: Long): List<MethodSizing>

    fun getMethodSizingAboveLineThresholdCount(systemId: Long, threshold: Int): Long

    fun getMethodSizingAboveLineThresholdCount(systemId: Long, threshold: Int, filter: FilterSizingPO): Long
    fun getMethodSizingAboveLineThresholdByFilterSizing(systemId: Long, threshold: Int, filter: FilterSizingPO): List<MethodSizing>

    fun getClassSizingAboveLineThresholdCount(systemId: Long, threshold: Int): Long

    fun getClassSizingAboveLineThreshold(systemId: Long, threshold: Int, limit: Long, offset: Long): List<ClassSizingWithLine>

    fun getClassSizingListAboveMethodCountThresholdCount(systemId: Long, threshold: Int): Long

    fun getClassSizingListAboveMethodCountThreshold(systemId: Long, threshold: Int, limit: Long, offset: Long): List<ClassSizingWithMethodCount>

    fun getClassSizingAboveLineThresholdCount(systemId: Long, threshold: Int, filter: FilterSizingPO): Long
    fun getClassSizingAboveLineThresholdByRequestSizing(systemId: Long, threshold: Int, filter: FilterSizingPO): List<ClassSizingWithLine>

    fun getClassSizingListAboveMethodCountThresholdCount(systemId: Long, threshold: Int, filter: FilterSizingPO): Long
    fun getClassSizingListAboveMethodCountThresholdByRequestSizing(systemId: Long, threshold: Int, filter: FilterSizingPO): List<ClassSizingWithMethodCount>
}
