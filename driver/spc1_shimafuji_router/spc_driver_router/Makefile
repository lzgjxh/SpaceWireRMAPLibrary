#
# SpaceWire �f�o�C�X�h���C�o
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

# �\�[�X�̈ˑ��֌W�t�@�C�� (��������)
DEPS = Dependencies
DEPENDENCIES_OUTPUT := $(DEPS)

# �f�o�C�X�h���C�o�W�����[��
include $(BD)/driver/etc/makerules

# �쐬�Ώ�
TARGET = spwr

# �\�[�X�t�@�C�������݂���p�X
VPATH = ../src

# �\�[�X�t�@�C��
SRC = spwr.c

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

# �\�[�X�̈ˑ��֌W
ifdef DEPENDENCIES_OUTPUT
  $(DEPS): ; touch $(DEPS)
else
  $(DEPS): $(SRC) ; $(MAKEDEPS) $@ $?
endif
-include $(DEPS)