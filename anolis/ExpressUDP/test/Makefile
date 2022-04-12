
ALL := $(filter test%.c, $(shell ls case))
ALL := $(ALL:%.c=bin/%)
ALL += bin/lib.o

all: clean $(ALL)

run: $(ALL)
	sh netns.sh
	#python2 -m pytest --durations=0 --fulltrace auto/ $(pytest)
	python3 -m pytest --durations=0  auto/ $(pytest)

bin/lib.o: case/lib.c
	@echo CC lib.o
	@mkdir bin 2>/dev/null || true
	@gcc  $^ -I ../objs/ -l elf -l pthread -fPIC -c -o $@ -g

bin/test_%: case/test_%.c bin/lib.o
	@echo CC $@
	@mkdir bin 2>/dev/null || true
	@gcc $^ -o $@ -I ../objs/  ../objs//libxudp.a -fPIC -l elf -l pthread -g


clean:
	rm -rf bin



