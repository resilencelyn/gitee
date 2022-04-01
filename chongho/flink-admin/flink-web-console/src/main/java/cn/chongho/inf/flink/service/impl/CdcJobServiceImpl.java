package cn.chongho.inf.flink.service.impl;

import cn.chongho.inf.flink.constants.Constant;
import cn.chongho.inf.flink.mapper.CdcJobMapper;
import cn.chongho.inf.flink.restapi.JarApi;
import cn.chongho.inf.flink.restapi.JobApi;
import cn.chongho.inf.flink.service.tasks.SyncSavePointTask;
import cn.chongho.inf.flink.utils.StringUtils;
import com.alibaba.fastjson.JSON;
import cn.chongho.inf.flink.model.*;
import cn.chongho.inf.flink.service.*;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.ui.ModelMap;

import javax.annotation.Resource;
import java.util.*;
import java.util.stream.Collectors;

/**
 * @author ming
 */
@Service
@Slf4j
public class CdcJobServiceImpl implements CdcJobService {

    @Resource
    private CdcJobMapper cdcJobMapper;

    @Resource
    private JobRunConfigService jobRunConfigService;

    @Resource
    private DbTableColumnService dbTableColumnService;

    @Resource
    private JarService jarService;

    @Resource
    private CheckPointInfoService checkPointInfoService;

    @Autowired
    private CdcSqlServiceImpl cdcSqlService;

    @Autowired
    private DbTableService dbTableService;

    @Autowired
    private DataAuthorityService dataAuthorityService;

    @Autowired
    private JarApi jarApi;

    @Autowired
    private JobApi jobApi;

    @Override
    public List<CdcJob> selectAll(CdcJob cdcJob) {
        return cdcJobMapper.select(cdcJob);
    }

    @Override
    public List<CdcJob> selectEnabledAndRunJob() {
        return cdcJobMapper.selectAllRunningJob();
    }

    @Override
    public int selectCount(CdcJob cdcJob) {
        return cdcJobMapper.countByPage(cdcJob);
    }

    @Override
    public List<CdcJob> selectByPage(int page, int pageSize,CdcJob cdcJob, Integer loginUserId) {
        int offset = (page-1) * pageSize;
        return cdcJobMapper.selectByPage(offset, pageSize ,cdcJob, loginUserId);
    }

    @Override
    public CdcJob selectById(Integer id) {
        return cdcJobMapper.selectByPrimaryKey(id);
    }

    @Override
    public boolean delete(Integer id , Integer loginUserId) {

        CdcJob dbCdcJob = cdcJobMapper.selectById(id);

        dataAuthorityService.checkDataAuthority(dbCdcJob, Constant.DataType.CDCJOB, loginUserId);

        if(dbCdcJob.getStatus() != Constant.JobState.CANCELED.ordinal()){
            return false;
        }

        CdcJob updateJob = new CdcJob();
        updateJob.setId(id);
        updateJob.setEnableFlag(Constant.EnableFlag.NOT.ordinal());
        updateJob.setUpdateTime(new Date());
        return cdcJobMapper.updateByPrimaryKeySelective(updateJob) > 0;
    }

    @Override
    public boolean update(CdcJob cdcJob) {
        int ret = 0;
        cdcJob.setUpdateTime(new Date());
        cdcJob.setParallelism(cdcJob.getParallelism() == null ? 1 : cdcJob.getParallelism());
        if (cdcJob.getId() != null) {
            CdcJob dbCdcJob = cdcJobMapper.selectById(cdcJob.getId());

            dataAuthorityService.checkDataAuthority(dbCdcJob, Constant.DataType.CDCJOB, cdcJob.getUpdateUserId());
            return cdcJobMapper.updateCdcJobById(cdcJob);
        } else  {
            cdcJob.setEnableFlag(Constant.EnableFlag.ENABLE.ordinal());
            cdcJob.setCreateTime(new Date());
            cdcJob.setCreateUserId(cdcJob.getUpdateUserId());
            cdcJob.setStatus(Constant.JobState.CANCELED.ordinal());
            ret = cdcJobMapper.insertSelective(cdcJob);
        }
        return ret > 0;
    }

    @Override
    public ModelMap editShow(Integer id ) {
        ModelMap map = new ModelMap();
        CdcJob cdcJob = selectById(id);
        map.put("cdcJob", cdcJob);
        map.put("jobType",cdcJob.getJobType());

        map.put("checkPointList",checkPointInfoService.getCheckPointInfoByJobId(id, Constant.CheckJobForm.CDCJOB.getValue()));

        //源库信息
        List<SourceInfo> sourceInfoList = JSON.parseArray(cdcJob.getSourceInfo() , SourceInfo.class);

        List<DbTable> sourcedbTableList = dbTableService.findDbTableBydbId(sourceInfoList.stream().map(SourceInfo::getDbId).collect(Collectors.toList()));

        Map<Integer, List<DbTable>> collect = sourcedbTableList.stream().collect(Collectors.groupingBy(DbTable::getDbId));

        for(SourceInfo sourceInfo : sourceInfoList){
            sourceInfo.setDbTableList(collect.get(sourceInfo.getDbId()));
        }
        //源库表信息
        map.put("sourceDbList", sourceInfoList);

        //目标库表
        DbTable dbTable = new DbTable();
        dbTable.setDbId(cdcJob.getTargetDbId());
        map.put("targetTableList", dbTableService.getAllDbTable(dbTable));

        Constant.CdcJobType jobType = Constant.CdcJobType.getTypeByValue(cdcJob.getJobType());

        switch (jobType){
            case POINT_TO_POINT_SQL:
                map.putAll(showType1(cdcJob));
                break;
            default:
        }
        return map;
    }

    /**
     * 单表同步编辑页面信息展示
     * @param cdcJob
     * @return
     */
    private ModelMap showType1(CdcJob cdcJob){
        ModelMap map = new ModelMap();
        SourceInfo sourceInfo = JSON.parseArray(cdcJob.getSourceInfo() , SourceInfo.class).get(0);
        //源表列
        List<ColumnAssociation> columnAssociationList = JSON.parseArray(cdcJob.getColumnAssociation() ,ColumnAssociation.class);
        Map<String, String> columnAssociationMap = columnAssociationList.stream().collect(Collectors.toMap(ColumnAssociation::getSourceColumnName, ColumnAssociation::getTargetColumnName));
        //列对应信息
        DbTableColumn dbTableColumn = new DbTableColumn();
        dbTableColumn.setTableId(sourceInfo.getTableId());
        List<DbTableColumn> sourceDbTableColumnList = dbTableColumnService.getAllDbTableColumn(dbTableColumn);
        sourceDbTableColumnList.forEach(column->{
            if(columnAssociationMap.containsKey(column.getColumnName())){
                column.setTargetColumnName(columnAssociationMap.get(column.getColumnName()));
            }
        });
        map.put("columnAssociationList" , sourceDbTableColumnList);

        //目标表列
        dbTableColumn = new DbTableColumn();
        dbTableColumn.setTableId(cdcJob.getTargetTableId());
        List<DbTableColumn> targetDbTableColumnList = dbTableColumnService.getAllDbTableColumn(dbTableColumn);
        map.put("targetDbTableColumnList" , targetDbTableColumnList);
        return map;
    }

    /**
     * 运行任务
     * @param id
     * @return
     */
    @Override
    public boolean runJob(Integer id, Integer loginUserId) {
        CdcJob cdcJob = cdcJobMapper.selectById(id);

        dataAuthorityService.checkDataAuthority(cdcJob, Constant.DataType.CDCJOB, loginUserId);

        JobRunConfig jobRunConfig = jobRunConfigService.selectByJobType(Constant.CdcJobType.getTypeByValue(cdcJob.getJobType()).name());
        log.info("JobRunConfig{}", JSON.toJSONString(jobRunConfig));
        if(jobRunConfig == null){
            return false;
        }

        Jar jar = jarService.getJarOne(new Jar(jobRunConfig.getJarId() ,Constant.EnableFlag.ENABLE.ordinal()));
        log.info("jar{}", JSON.toJSONString(jar));
        if(jar == null){
            return false;
        }

        Map<String, Object> params = new HashMap<>(4);
        params.put("entryClass" , jobRunConfig.getEntryClass());
        params.put("programArgsList" , runJobArgsArray(cdcJob));
        params.put("parallelism" ,cdcJob.getParallelism() == null ? 1 : cdcJob.getParallelism());
        if(!StringUtils.isEmpty(cdcJob.getSavepointPath())){
            params.put("savepointPath" ,cdcJob.getSavepointPath());
        }
        log.info("job args:{}", JSON.toJSONString(params));

        /**
         * 提交到所选集群地址
         */
        String jobid = jarApi.runJars(cdcJob.getFlinkColonyUrl(), params, jar.getJobKey());
        CdcJob updateJob = new CdcJob();
        updateJob.setId(id);
        updateJob.setJobId(jobid);
        updateJob.setStatus(Constant.JobState.RUNNING.ordinal());
        updateJob.setUpdateTime(new Date());
        updateJob.setUpdateUserId(loginUserId);
        cdcJobMapper.updateByPrimaryKeySelective(updateJob);
        return true;
    }

    private  List<String> runJobArgsArray(CdcJob cdcJob){

        List<SourceInfo> sourceInfoList = JSON.parseArray(cdcJob.getSourceInfo(), SourceInfo.class);

        /**
         * 源表语句
         * 可能会有多个表，多源合并输出的情况
         */
        StringBuilder sourceSqlBuilder = new StringBuilder();

        for(SourceInfo sourceInfo : sourceInfoList){
            sourceSqlBuilder.append(cdcSqlService.buildSourceSql(cdcJob, sourceInfo)).append(";");
        }
        sourceSqlBuilder.deleteCharAt(sourceSqlBuilder.length() - 1);

        /**
         * 目标表语句
         */
        String targetSql = cdcSqlService.buildTargetSql(cdcJob);

        /**
         * 插入语句
         */
        String insertSql = cdcSqlService.createInsertSql(cdcJob );

        List<String>  argsArray = new ArrayList<>(8);

        argsArray.add("--sourceTableSql");
        argsArray.add(sourceSqlBuilder.toString());

        argsArray.add("--targetTableSql");
        argsArray.add(targetSql);

        argsArray.add("--insertSql");
        argsArray.add(insertSql);

        argsArray.add("--jobName");
        argsArray.add(cdcJob.getJobName());

        return argsArray;
    }

    @Override
    public boolean savepoint(Integer id, Integer loginUserId) {

        CdcJob cdcJob = cdcJobMapper.selectById(id);

        dataAuthorityService.checkDataAuthority(cdcJob, Constant.DataType.CDCJOB, loginUserId);

        String triggerId = jobApi.savepoint(cdcJob.getFlinkColonyUrl(), cdcJob.getJobId());

        CheckPointInfo checkPointInfo = new CheckPointInfo(id ,triggerId);
        checkPointInfo.setJobForm(Constant.CheckJobForm.CDCJOB.getValue());
        checkPointInfo.setPointType(Constant.CheckPointType.SAVE.getValue());
        checkPointInfo.setCreateUserId(loginUserId);
        checkPointInfo.setLocation("");
        checkPointInfoService.addCheckPointInfo(checkPointInfo);

        SyncSavePointTask syncSavePointTask = new SyncSavePointTask(checkPointInfo.getId() ,cdcJob.getJobId(), triggerId, cdcJob.getFlinkColonyUrl());
        new Timer().schedule(syncSavePointTask ,Constant.DELAY_TIME);
        return true;
    }

    @Override
    public boolean stopJob(Integer id, Integer loginUserId){
        CdcJob cdcJob = cdcJobMapper.selectById(id);

        dataAuthorityService.checkDataAuthority(cdcJob, Constant.DataType.CDCJOB, loginUserId);

        String triggerId = jobApi.stopJob(cdcJob.getFlinkColonyUrl(), cdcJob.getJobId());

        CheckPointInfo checkPointInfo = new CheckPointInfo(id ,triggerId);
        checkPointInfo.setJobForm(Constant.CheckJobForm.CDCJOB.getValue());
        checkPointInfo.setPointType(Constant.CheckPointType.SAVE.getValue());
        checkPointInfo.setCreateUserId(loginUserId);
        checkPointInfo.setLocation("");
        checkPointInfoService.addCheckPointInfo(checkPointInfo);

        cdcJobMapper.updateJobStatusByJobId(cdcJob.getJobId(), Constant.JobState.CANCELED.ordinal());

        SyncSavePointTask syncSavePointTask = new SyncSavePointTask(checkPointInfo.getId(), cdcJob.getJobId(), triggerId, cdcJob.getFlinkColonyUrl());
        new Timer().schedule(syncSavePointTask ,Constant.DELAY_TIME);
        return true;
    }

    public static void main(String[] args) {
        StringBuilder targetPrimaryColumn = new StringBuilder();
        targetPrimaryColumn.append("1,2,3,");
        System.out.println(targetPrimaryColumn.deleteCharAt(targetPrimaryColumn.length()-1));
    }
}
