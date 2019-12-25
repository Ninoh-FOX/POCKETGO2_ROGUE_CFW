ifeq ($(CONFIG),)
CONFIGS:=$(foreach CFG,$(wildcard config-*.mk),$(CFG:config-%.mk=%))
$(error Please specify CONFIG, possible values: $(CONFIGS))
endif

include config-$(CONFIG).mk

TARGET = pwswd

CC = $(CROSS_COMPILE)gcc

DEFAULT_MIXER ?= "\"PCM\""

LIBS = -lini -lpthread
OBJS = event_listener.o shortcut_handler.o main.o

ifdef BACKEND_VOLUME
	OBJS += backend/volume/volume.o
	CFLAGS += -DBACKEND_VOLUME -DDEFAULT_MIXER=$(DEFAULT_MIXER)
	LIBS += -lasound
endif

ifdef BACKEND_BRIGHTNESS
	OBJS += backend/brightness/brightness.o
	CFLAGS += -DBACKEND_BRIGHTNESS
endif

ifdef BACKEND_SHARPNESS
	OBJS += backend/sharpness/sharpness.o
	CFLAGS += -DBACKEND_SHARPNESS
endif


ifdef BACKEND_POWEROFF
	OBJS += backend/poweroff/poweroff.o
	CFLAGS += -DBACKEND_POWEROFF
endif

ifdef BACKEND_REBOOT
	OBJS += backend/reboot/reboot.o
	CFLAGS += -DBACKEND_REBOOT
endif

ifdef BACKEND_SCREENSHOT
	OBJS += backend/screenshot/screenshot.o
	CFLAGS += -DBACKEND_SCREENSHOT
	LIBS += -lpng
endif

ifdef BACKEND_TVOUT
	OBJS += backend/tvout/tvout.o
	CFLAGS += -DBACKEND_TVOUT
endif

ifdef BACKEND_SUSPEND
	OBJS += backend/suspend/suspend.o
	CFLAGS += -DBACKEND_SUSPEND
endif

ifdef BACKEND_KILL
	OBJS += backend/kill/kill.o
	CFLAGS += -DBACKEND_KILL
endif

ifdef BACKEND_RATIOMODE
	OBJS += backend/ratiomode/ratiomode.o
	CFLAGS += -DBACKEND_RATIOMODE
endif


.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) $^ -o $@

clean:
	rm -f $(TARGET) $(OBJS)
