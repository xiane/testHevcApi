CROSS_COMPILE ?=
CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++

TARGET=testHevcApi
AMLENC_LIB=test.o
LDFLAGS += -lm -lrt -Lhevc_enc -lvphevcodec
#LDFLAGS += -lge2d
CFLAGS+=-O2 -g

LIB=hevc_enc

$(TARGET):$(AMLENC_LIB)
	$(MAKE) -C $(LIB)
	$(CXX) $(CFLAGS) $< -o $@ $(LinkIn) $(LDFLAGS)

$(AMLENC_LIB):%.o:%.cpp
	$(CXX) -c $(CFLAGS) $< -o $@

install: $(TARGET)
	$(MAKE) -C $(LIB) install
	cp $^ /usr/local/bin/

clean:
	-rm -f *.o
	-rm -f $(TARGET)
	$(MAKE) -C $(LIB) clean
