package cn.chongho.inf.flink.constants;

/**
 * @author ming
 */
public class Constant {

    public static final long DELAY_TIME = 3 * 1000L;

    public static final String ADY_START_TIME = " 00:00:00";

    public static final String ADY_END_TIME = " 23:59:59";

    public enum  JobState {
        /**
         * 取消
         */
        CANCELED,
        /**
         * 运行
         */
        RUNNING,
        /**
         * 重启
         */
        RESTARTING,

        FINISHED,
        ;
    }

    public enum EnableFlag {
        /**
         * 不可用
         */
        NOT,
        /**
         * 可用
         */
        ENABLE;
    }

    public enum DbType {
        MYSQL,
        ORACLE,
        ELASTICSEARCH,
        ;
    }

    public enum CdcJobType {
        /**
         * 单表同步
         */
        POINT_TO_POINT_SQL(1),
        /**
         * 多表合并同步
         */
        CONFLUENCE_SQL(2),
        ;

        private Integer value;

        CdcJobType(Integer value) {
            this.value = value;
        }

        public Integer getValue() {
            return value;
        }

        public static CdcJobType getTypeByValue(Integer value){
            for(CdcJobType cdcJobType : CdcJobType.values()){
                if(cdcJobType.getValue().equals(value)){
                    return cdcJobType;
                }
            }
            return null;
        }
    }


    public enum CheckJobForm {
        /**
         * 普通任务
         */
        JOB(1),
        /**
         * cdc任务
         */
        CDCJOB(2),
        ;

        private Integer value;

        CheckJobForm(Integer value) {
            this.value = value;
        }

        public Integer getValue() {
            return value;
        }

        public static CheckJobForm getTypeByValue(Integer value){
            for(CheckJobForm jobForm : CheckJobForm.values()){
                if(jobForm.getValue().equals(value)){
                    return jobForm;
                }
            }
            return null;
        }
    }

    public enum CheckPointType {
        /**
         * 系统自动
         */
        CHECK(1),
        /**
         * 手动触发
         */
        SAVE(2),
        ;

        private Integer value;

        CheckPointType(Integer value) {
            this.value = value;
        }

        public Integer getValue() {
            return value;
        }

        public static CheckPointType getTypeByValue(Integer value){
            for(CheckPointType pointType : CheckPointType.values()){
                if(pointType.getValue().equals(value)){
                    return pointType;
                }
            }
            return null;
        }
    }

    /**
     * 数据权限，数据类型
     */
    public enum DataType {
        JOB(1),
        CDCJOB(2);

        private Integer value;

        public Integer getValue() {
            return value;
        }

        DataType(Integer value) {
            this.value = value;
        }
    }
}
