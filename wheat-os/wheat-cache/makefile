BASE_PATH =  $(shell pwd)
STORAGE_PATH = $(BASE_PATH)/storage
BASE_OUT = $(BASE_PATH)/bin

MAKEFLAGS+= --no-print-directory

dcgen:
	@python3 ./shell/gen_protobuf.py
	@python3 ./shell/proto.py
	@python3 ./shell/make-struct.py

.PHONY: build-storage
build-storage:
	@cd storage && go build -o $(BASE_OUT)/storage

.PHONY: build-gateway
build-gateway:
	@cd gateway && go build -o $(BASE_OUT)/gateway

.PHONY: install
install:
	@make gen-middleware
	@make build-storage
	@make build-gateway
	@sudo python3 ./shell/init_conf.py

.PHONY: storage
storage:
	@./bin/storage storage

.PHONY: gateway
gateway:
	@./bin/gateway gateway

.PHONY: gen-struct
gen-struct:
	@python3 ./shell/make-struct.py

.PHONY: gen-protobuf
gen-protobuf:
	@python3 ./shell/gen_protobuf.py

.PHONY: gen-middleware
gen-middleware:
	@python3 ./shell/gen_middleware.py

.PHONY: init-conf
init-conf:
	@python3 ./shell/init_conf.py


.PHONY: gen-service
gen-service:
	@python3 ./shell/make_service.py

.PHONY: gen-mock
gen-mock:
	@mockgen -source=./pkg/proto/storage.pb.go CommServerClient > ./mock/storage/mock_client.gen.go