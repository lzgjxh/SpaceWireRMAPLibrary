#
# SpaceWire デバイスドライバ
# Copyright (C) 2007
#

MACHINE = vr5701
opt1 = spc
BD = /usr/local/te

USE_CPP = 1
USE_MT = 1
UNIXEMU = 1

DEPS = Dependencies
DEPENDENCIES_OUTPUT := $(DEPS)

include $(BD)/util/etc/makerules

CPPFLAGS += -D__USE_IOSTREAMS
LOADLIBES += -lstlport -L../driver -lspw
CPPFLAGS += -D__USE_THREADS
LDLIBS += -ltm

# ソースの依存関係ファイル (自動生成)
DEPS = Dependencies
DEPENDENCIES_OUTPUT := $(DEPS)

# デバイスドライバ標準ルール
include $(BD)/driver/etc/makerules

# 作成対象
TARGET = spw

# ソースファイルが存在するパス
VPATH = ../src

# ソースファイル
SRC = spw.c

OBJ = $(addsuffix .o, $(basename $(SRC)))

CFLAGS += $(CFLAGS_WARNING)
LDLIBS += -ltm

PATH += :/usr/bin:/bin

# ----------------------------------------------------------------------------
.PHONY: all clean

all: $(TARGET) $(TARGET:%=%.map)

$(TARGET): $(OBJ)
	$(LINK.o) $(LDOBJS) $^ $(LOADLIBES) $(LDLIBS) $(OUTPUT_OPTION)
	$(STRIP) $@

clean:
	/bin/rm -f $(OBJ) $(TARGET) $(DEPS)
	/bin/rm -f *.map

# ソースの依存関係
ifdef DEPENDENCIES_OUTPUT
  $(DEPS): ; touch $(DEPS)
else
  $(DEPS): $(SRC) ; $(MAKEDEPS) $@ $?
endif
-include $(DEPS)
