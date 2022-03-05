debug:
	python ./main.py

run:
	uvicorn main:app --reload

migrate:
	alembic revision --autogenerate -m "up"&&\
	alembic upgrade head

init-data:
	python ./initial_data.py

release:
	mkdir -p ./public&&\
	docker run --rm -v `pwd`:/codes -i node:16.13.0 sh /codes/build-web.sh &&\
	docker build -t mega-ide .
