SRCS := $(shell find . -name "*.c" -not -name "*_template.c")
TARGET=video-viewer
CC = clang -arch x86_64

CFLAGS := -O2 -w -std=c99 -Wno-implicit-function-declaration
OBJS := $(SRCS:%.c=%.o)

SRCS += .
INC = -I./src
INC += -I/usr/local/include
#INC += -I/Users/yaofu/voip/ffmpeg/inbin/include
INC += -I/usr/local/Cellar/ffmpeg/6.0_1/include
#INC += -I/usr/local/Cellar/sdl2/2.28.2/include

LIB  = -Wl
LIB += -lm -L/usr/local/lib
#LIB += -L/Users/yaofu/voip/ffmpeg/inbin/lib -lavcodec -lavfilter -lavutil
LIB += -L/usr/local/Cellar/ffmpeg/6.0_1/lib -lavcodec -lavfilter -lavutil
LIB += -lSDL2

CPP_SRCS = $(foreach d,$(SRCS),$(wildcard $(d)/*.c))
OBJS = $(addprefix $(OBJDIR), $(patsubst %.c, %.o, $(CPP_SRCS)))

h264dec: $(OBJS)
	$(CC) -o $(TARGET) $(INC) $(CFLAGS) $(OBJS) $(LIB)
	@echo ""
	@echo "+--------------------------------------------+"
	@echo "|      Finish compilation video-$(TARGET)          |"
	@echo "+--------------------------------------------+"
	@echo "|        Copyright(c) voipman@qq.com         |"
	@echo "+--------------------------------------------+"

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)

$(OBJDIR)%.o : %.cpp
	$(CC) $(INC) -c $< -o $@
$(OBJDIR)%.o : %.c
	$(CC) $(INC) $(CFLAGS) -c $< -o $@
$(OBJDIR)%.o : %.cc
	$(CC) $(INC) -c $< -o $@
