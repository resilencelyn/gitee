FROM ubuntu
ARG DEBIAN_FRONTEND=noninteractive
ENV LANG=zh_CN.UTF-8 \
    TZ=Asia/Shanghai \
    PS1="\u@\h:\w \$ "
RUN sed -i s@/archive.ubuntu.com/@/mirrors.huaweicloud.com/@g /etc/apt/sources.list \
	&& apt update \
	&& apt install -y --no-install-recommends \
	   language-pack-zh-han* \
       subversion \
       apache2 \
       libapache2-mod-svn \
       openjdk-8-jre \
       ttf-dejavu \
       fontconfig \
       tzdata \
       tini \
    && fc-cache -f -v \
    && ln -sf /usr/share/zoneinfo/${TZ} /etc/localtime \
    && echo "${TZ}" > /etc/timezone \
    && rm -rf /var/cache/apk/* /tmp/*
COPY target/svnWebUI-*.jar /home/svnWebUI.jar
COPY entrypoint.sh /usr/local/bin/entrypoint.sh
ENTRYPOINT ["tini","/usr/local/bin/entrypoint.sh"]