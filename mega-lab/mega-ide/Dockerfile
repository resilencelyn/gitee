FROM python:3.7.7
WORKDIR /opt
ADD ./alembic ./alembic
ADD ./asserts ./asserts
ADD ./init_data ./init_data
ADD ./public ./public
ADD ./server ./server
ADD ./templates ./templates
ADD ./alembic.ini .
ADD ./initial_data.py .
ADD ./job_scan_task.py .
ADD ./LICENSE .
ADD ./main.py .
ADD ./requirements.txt .
ADD ./start-server.sh .
ADD ./start-worker.sh .
RUN pip install -i https://mirrors.aliyun.com/pypi/simple/ -r ./requirements.txt