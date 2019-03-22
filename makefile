CC ?= gcc
CCC ?= g++
LD = ld

CFLAGS += -g \
          -Wall \
          -Wextra \
          -Wno-unused-parameter \
          -Wno-unused-function \
#          -rdynamic \
          -I
#          -I. \
#          -DUSE_LIBUV

CCFLAGS = -g \
          -Wall \
          -Wno-unused-function \
#          -rdynamic \
          -I
#          -I. \
#          -DUSE_LIBUV

LINK_FLAGS =
#LINK_FLAGS = \
#             -lssl -lcrypto

INCLUDES =
#INCLUDES = src/netstructs.h

INCLUDES_PATHS =
#INCLUDES_PATHS = \
#         -I3rd/uv_link_t/include \
#         -I3rd/uv_ssl_t/include

OBJS_C = \
         lib/utils.o \
         lib/i2c.o \
         src/buttons.o \
         src/keyboard.o \
         src/led_monitor.o \
         src/main.o

OBJS_CPP = \
         lib/zini.o

LINKS_LIBS =
#LINK_LIBS = -luv \
#            -L3rd/uv_ssl_t \
#            -luv_ssl_t$(OS_POSTFIX) \
#            -L3rd/uv_link_t \
#            -luv_link_t$(OS_POSTFIX) \
#            -L3rd/ringbuffer \
#            -lringbuffer$(OS_POSTFIX)


all: del_bin build

clean:
	-$(RM) $(OBJS_C) $(OBJS_CPP) midi-controller

rebuild: clean build

build: $(OBJS_C) $(OBJS_CPP)
	$(CCC) -o midi-controller $(OBJS_C) $(OBJS_CPP) $(LINK_LIBS) $(LINK_FLAGS)

$(OBJS_C): %.o : %.c $(INCLUDES)
	$(CC) $(CFLAGS) $(INCLUDES_PATHS) -c -o $@ $<

$(OBJS_CPP): %.o : %.cpp $(INCLUDES)
	$(CCC) $(CCFLAGS) $(INCLUDES_PATHS) -c -o $@ $<

del_bin:
	rm -f midi-controller

run_bin:
	./midi-controller debug
