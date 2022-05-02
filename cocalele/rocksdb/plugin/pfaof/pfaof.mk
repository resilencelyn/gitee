pfaof_SOURCES = pfaof.cc
pfaof_HEADERS = pfaof.h
pfaof_LDFLAGS = -u pfaof_reg -L$(PF_LIB) -ls5common
pfaof_CXXFLAGS = -I$(PF_INC)
