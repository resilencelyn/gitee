FROM openjdk:8u332-oracle

ENV TZ Asia/Shanghai
ENV LANG C.UTF-8
ENV LC_ALL C.UTF-8
ENV LANGUAGE C.UTF-8

RUN mkdir -p /srv/grule

ADD bin /srv/grule/bin
ADD conf /srv/grule/conf
ADD src /srv/grule/src
ADD start.sh /srv/grule/start.sh
ADD build.gradle /srv/grule/build.gradle
ADD gradle /srv/grule/gradle
ADD gradlew /srv/grule/gradlew

WORKDIR /srv/grule/

RUN ./gradlew --no-daemon deps

ENTRYPOINT exec sh start.sh $JAVA_OPTS