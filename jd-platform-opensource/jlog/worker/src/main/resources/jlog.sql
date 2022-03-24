
CREATE TABLE tracer_log (
  tracerId Int64,
  className String,
  threadName String,
  methodName String,
  logLevel String,
  content String,
  node String,
  normal String,
  tag1 String,
 createTime DateTime
) ENGINE = MergeTree()
    PARTITION BY toYYYYMMDD ( createTime )
    ORDER BY createTime
    PRIMARY key createTime
    TTL createTime + toIntervalDay ( 15 )



  CREATE TABLE tracer_model (
    tracerId Int64,
    requestContent String,
    responseContent String,
    costTime String,
    uid String,
    errno String,
    errmsg String,
    app String,
    uri String,
   createTime DateTime
  ) ENGINE = MergeTree()
   PARTITION BY toYYYYMMDD ( createTime )
   ORDER BY ( uid, createTime )
    PRIMARY key uid
   TTL createTime + toIntervalDay ( 15 )
