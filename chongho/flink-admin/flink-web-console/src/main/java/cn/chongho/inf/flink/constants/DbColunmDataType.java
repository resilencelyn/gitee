package cn.chongho.inf.flink.constants;

import java.util.ArrayList;
import java.util.List;

/**
 * @author ming
 */

public class DbColunmDataType {


    public static final String UNSIGNED = "unsigned";

    public enum MysqlDataType {
        TINYINT("TINYINT",false),
        SMALLINT("SMALLINT",false),
        TINYINTUNSIGNED("SMALLINT",false),
        INT("INT",false),
        MEDIUMINT("INT",false),
        SMALLINTUNSIGNED ("INT",false),
        BIGINT("BIGINT",false),
        INTUNSIGNED("BIGINT",false),
        BIGINTUNSIGNED("DECIMAL(20, 0)",false),
        REAL("FLOAT",false),
        FLOAT("FLOAT",false),
        DOUBLE("DOUBLE",false),
        DECIMAL("DECIMAL",true),
        NUMERIC("DECIMAL",true),
        DATE("DATE",false),
        CHAR("CHAR",true),
        VARCHAR("VARCHAR",true),
        YEAR("INT",false),
        DATETIME("TIMESTAMP",false),
        ENUM("STRING",false),
        BOOLEAN("BOOLEAN",false),
        ;


        public String getFlinkDataType() {
            return flinkDataType;
        }

        public boolean isNeeddDigit() {
            return needdDigit;
        }

        MysqlDataType(String flinkDataType , boolean needdDigit) {
            this.flinkDataType = flinkDataType;
            this.needdDigit = needdDigit;
        }

        private String flinkDataType;

        private boolean needdDigit;
    }


    public enum ElasticsearchDataType {
        TEXT("STRING","text"),
        KEYWORD("STRING","keyword"),
        INTEGER("INT","integer"),
        FLOAT("FLOAT","float"),
        DOUBLE("DOUBLE","double"),
        BOOLEAN("BOOLEAN","boolean"),
        DATE("DATE","date"),
        TIME("TIME","time"),
        TIMESTAMP("DATE-TIME","timestamp"),
        ARRAY("ARRAY","array"),
        OBJECT("ROW","object"),
        ;


        public String getFlinkDataType() {
            return flinkDataType;
        }

        public String getType() {
            return type;
        }

        ElasticsearchDataType(String flinkDataType , String type) {
            this.flinkDataType = flinkDataType;
            this.type = type;
        }

        private String flinkDataType;

        private String type;

        public static List<String> getTypes(){
            List<String> types = new ArrayList<>(ElasticsearchDataType.values().length);
            for(ElasticsearchDataType dataType : ElasticsearchDataType.values()){
                types.add(dataType.getType());
            }
            return types;
        }
    }

}
