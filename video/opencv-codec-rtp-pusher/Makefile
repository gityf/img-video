PROJECT = bin/video-pusher

#CC=gcc -std=c++11 -g -O3 $(CFLAGS)
#CC=gcc -std=c++11 -g
CC=clang -std=c++11 -g

SHARED_FLAG = -fPIC -shared

OBJDIR = ./obj/
SRCS += src
SRCS += src/common
SRCS += src/rtp-x264
SRCS += src/codec
SRCS += src/stream-push

LIB  = -Wl,-E
LIB = -L/usr/local/lib -lopencv_video -lopencv_highgui -lopencv_videoio -lopencv_core -lopencv_imgcodecs -lopencv_imgproc
LIB += -lx264 -lx265 -lvpx -laom
LIB += -lvvenc
LIB += -lstdc++ -lpthread -lm -ldl
#LIB += /Users/yaofu/voip/vvenc-1.9.0/lib/release-static/libvvenc.a
LIB += /Users/yaofu/voip/xavs2/xavs2/build/linux/inbin/lib/libxavs2.a

INC  = -I./src
INC += -I./src/common
INC += -I./src/rtp-x264
INC += -I./src/stream-push
INC += -I./src/codec
INC += -I/usr/local/include
INC += -I/Users/yaofu/voip/xavs2/xavs2/build/linux/inbin/include

CPP_SRCS = $(foreach d,$(SRCS),$(wildcard $(d)/*.cpp))
OBJS = $(addprefix $(OBJDIR), $(patsubst %.cpp, %.o, $(CPP_SRCS)))

all : obj $(PROJECT)
obj :
	mkdir -p $(addprefix $(OBJDIR), $(SRCS))

deps :

$(PROJECT) : $(OBJS) $(MODULE)
	test -d ./bin || mkdir -p ./bin
	$(CC) -o $@ $(OBJS) $(LIB)
	@echo ""
	@echo "+--------------------------------------------+"
	@echo "|      Finish compilation video-pusher       |"
	@echo "+--------------------------------------------+"
	@echo "|        Copyright(c) voipman@qq.com         |"
	@echo "+--------------------------------------------+"

clean:
	rm -rf $(OBJS) obj $(PROJECT)

install:
	test -d ./output || mkdir -p ./output
	test -e $(PROJECT) || cp $(PROJECT) output/

$(OBJDIR)%.o : %.cpp
	$(CC) $(INC) -c $< -o $@
$(OBJDIR)%.o : %.c
	$(CC) $(INC) -c $< -o $@
$(OBJDIR)%.o : %.cc
	$(CC) $(INC) -c $< -o $@
