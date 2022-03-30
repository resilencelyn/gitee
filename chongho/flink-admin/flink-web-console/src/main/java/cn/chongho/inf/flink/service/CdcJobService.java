package cn.chongho.inf.flink.service;


import cn.chongho.inf.flink.model.CdcJob;
import org.springframework.ui.ModelMap;

import java.util.List;

/**
 * @author ming
 */
public interface CdcJobService {

    List<CdcJob> selectAll(CdcJob cdcJob);

    List<CdcJob> selectEnabledAndRunJob();

    int selectCount(CdcJob cdcJob);


    List<CdcJob> selectByPage(int page, int pageSize ,CdcJob cdcJob, Integer loginUserId);

    CdcJob selectById(Integer id);

    boolean delete(Integer id, Integer loginUserId);

    boolean update(CdcJob job);

    ModelMap editShow(Integer id );

    boolean runJob(Integer id, Integer loginUserId);

    boolean savepoint(Integer id, Integer loginUserId);

    boolean stopJob(Integer id, Integer loginUserId);
}
