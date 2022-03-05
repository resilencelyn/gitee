#!/bin/sh

SPRING_PROFILE_CONFIG=""


################################################## active config start #################################################
if [ "${SPRING_PROFILES_ACTIVE}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dspring.profiles.active='$SPRING_PROFILES_ACTIVE'"
else
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dspring.profiles.active='docker'"
fi
################################################## active config end ###################################################


################################################## swagger config start ################################################
if [ "${VISUAL_SWAGGER_ENABLE}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.swagger.enable='$VISUAL_SWAGGER_ENABLE'"
fi
################################################## swagger config end ##################################################


################################################## datasource config start #############################################
if [ "${SPRING_DATASOURCE_URL}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dspring.datasource.druid.master.url='$SPRING_DATASOURCE_URL'"
fi
if [ "${SPRING_DATASOURCE_USERNAME}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dspring.datasource.druid.master.username='$SPRING_DATASOURCE_USERNAME'"
fi
if [ "${SPRING_DATASOURCE_PASSWORD}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dspring.datasource.druid.master.password='$SPRING_DATASOURCE_PASSWORD'"
fi
################################################## datasource config end ###############################################


###################################################### engine config start #############################################
if [ "${VISUAL_ENGINE_SELECTED}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.engine.selected='$VISUAL_ENGINE_SELECTED'"
fi

if [ "${VISUAL_ENGINE_PROXIMA_HOST}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.engine.proxima.host='$VISUAL_ENGINE_PROXIMA_HOST'"
fi
if [ "${VISUAL_ENGINE_PROXIMA_PORT}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.engine.proxima.port='$VISUAL_ENGINE_PROXIMA_PORT'"
fi

if [ "${VISUAL_ENGINE_MILVUS_HOST}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.engine.milvus.host='$VISUAL_ENGINE_MILVUS_HOST'"
fi
if [ "${VISUAL_ENGINE_MILVUS_PORT}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.engine.milvus.port='$VISUAL_ENGINE_MILVUS_PORT'"
fi
###################################################### engine config end ###############################################


###################################################### model config start ##############################################

if [ "${VISUAL_MODEL_FACEDETECTION_NAME}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.model.faceDetection.name='$VISUAL_MODEL_FACEDETECTION_NAME'"
fi
if [ "${VISUAL_MODEL_FACEDETECTION_PATH}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.model.faceDetection.modelPath='$VISUAL_MODEL_FACEDETECTION_PATH'"
fi
if [ "${VISUAL_MODEL_FACEDETECTION_THREAD}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.model.faceDetection.thread='$VISUAL_MODEL_FACEDETECTION_THREAD'"
fi

if [ "${VISUAL_MODEL_FACEDETECTION_BACKUP_NAME}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.model.faceDetection.backup.name='$VISUAL_MODEL_FACEDETECTION_BACKUP_NAME'"
fi
if [ "${VISUAL_MODEL_FACEDETECTION_BACKUP_PATH}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.model.faceDetection.backup.modelPath='$VISUAL_MODEL_FACEDETECTION_BACKUP_PATH'"
fi
if [ "${VISUAL_MODEL_FACEDETECTION_BACKUP_THREAD}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.model.faceDetection.backup.thread='$VISUAL_MODEL_FACEDETECTION_BACKUP_THREAD'"
fi

if [ "${VISUAL_MODEL_FACEKEYPOINT_NAME}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.model.faceKeyPoint.name='$VISUAL_MODEL_FACEKEYPOINT_NAME'"
fi
if [ "${VISUAL_MODEL_FACEKEYPOINT_PATH}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.model.faceKeyPoint.modelPath='$VISUAL_MODEL_FACEKEYPOINT_PATH'"
fi
if [ "${VISUAL_MODEL_FACEKEYPOINT_THREAD}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.model.faceKeyPoint.thread='$VISUAL_MODEL_FACEKEYPOINT_THREAD'"
fi

if [ "${VISUAL_MODEL_FACEALIGNMENT_NAME}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.model.faceAlignment.name='$VISUAL_MODEL_FACEALIGNMENT_NAME'"
fi
if [ "${VISUAL_MODEL_FACEALIGNMENT_PATH}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.model.faceAlignment.modelPath='$VISUAL_MODEL_FACEALIGNMENT_PATH'"
fi
if [ "${VISUAL_MODEL_FACEALIGNMENT_THREAD}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.model.faceAlignment.thread='$VISUAL_MODEL_FACEALIGNMENT_THREAD'"
fi

if [ "${VISUAL_MODEL_FACERECOGNITION_NAME}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.model.faceRecognition.name='$VISUAL_MODEL_FACERECOGNITION_NAME'"
fi
if [ "${VISUAL_MODEL_FACERECOGNITION_PATH}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.model.faceRecognition.modelPath='$VISUAL_MODEL_FACERECOGNITION_PATH'"
fi
if [ "${VISUAL_MODEL_FACERECOGNITION_THREAD}" ];then
    SPRING_PROFILE_CONFIG="${SPRING_PROFILE_CONFIG} -Dvisual.model.faceRecognition.thread='$VISUAL_MODEL_FACERECOGNITION_THREAD'"
fi
###################################################### model config end ###############################################


sh -c "java -server ${SPRING_PROFILE_CONFIG} ${SPRING_OPTS} ${JAVA_OPTS} -jar /app/face-search/face-search-server.jar"
