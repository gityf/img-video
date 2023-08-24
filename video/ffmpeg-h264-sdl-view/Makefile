SRCS := $(shell find . -name "*.c" -not -name "*_template.c")
TARGET=h264-viewer
CFLAGS := -I./ffmpeg-src -O2 -w -std=c99 -Wno-implicit-function-declaration
LDFLAGS := -lm -L/usr/local/lib -L/usr/local/Cellar/sdl2/2.28.2/lib -lSDL2

OBJS := $(SRCS:%.c=%.o)

h264dec: $(OBJS)
	$(CC) -o $(TARGET) -I/usr/local/include -I/usr/local/Cellar/sdl2/2.28.2/include  $(OBJS) $(LDFLAGS)
	@echo ""
	@echo "+--------------------------------------------+"
	@echo "|      Finish compilation video-$(TARGET)          |"
	@echo "+--------------------------------------------+"
	@echo "|        Copyright(c) voipman@qq.com         |"
	@echo "+--------------------------------------------+"

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)
