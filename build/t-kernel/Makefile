SYSTEM_TYPE = t-kernel

##########################
# Options
##########################

# NEC IP Core Option
#
# Delete comment out below if you need to use
# NEC Soft's SpaceWire IP core and related classes
# such as SpaceWireIFSpaceCube1NECSoftIPcore.cc.
# When you use NEC IP core, related files listed
# belowshould be copied to drivers/spc1_necsoft/
#
# libspw.h (header file)
# libspw.a (library archive)
# common.h (header file)
#

#USE_SPC1_NEC_IP_CORE = YES


##########################
# T-Kernel options
##########################
BD = /usr/local/te

MACHINE = vr5701
opt1 = spc

USE_CPP = 1
USE_MT = 1
UNIXEMU = 1

DEPS = Dependencies
DEPENDENCIES_OUTPUT := $(DEPS)

PATH_ORIGINAL := $(PATH)
include $(BD)/util/etc/makerules
PATH := $(PATH_ORIGINAL)

LOADLIBES += -lstlport
CPPFLAGS += -D__USE_IOSTREAMS -D__USE_THREADS
LDLIBS += -ltm


##########################
# PATHs
##########################
SPACEWIRELIBRARY_SOURCE_PATH = $(SPACEWIRERMAPLIBRARY_PATH)/source_SpaceWireRMAPLibrary
EXECUTABLE_SOURCE_PATH = $(SPACEWIRERMAPLIBRARY_PATH)/source_Executables
USERPROGRAM_SOURCE_PATH = $(SPACEWIRERMAPLIBRARY_PATH)/source_UserPrograms
THREADLIBRARY_SOURCE_PATH = $(SPACEWIRERMAPLIBRARY_PATH)/source_ThreadLibrary/$(SYSTEM_TYPE)
IPSOCKETLIBRARY_SOURCE_PATH = $(SPACEWIRERMAPLIBRARY_PATH)/source_IPSocketLibrary/$(SYSTEM_TYPE)
SHIMAFUJI_DRIVER_PATH = $(SPACEWIRERMAPLIBRARY_PATH)/driver/spc1_shimafuji3
SHIMAFUJI_ROUTER_DRIVER_PATH = $(SPACEWIRERMAPLIBRARY_PATH)/driver/spc1_shimafuji_router

VPATH = $(SPACEWIRELIBRARY_SOURCE_PATH):\
$(EXECUTABLE_SOURCE_PATH):\
$(USERPROGRAM_SOURCE_PATH):\
$(THREADLIBRARY_SOURCE_PATH):\
$(IPSOCKETLIBRARY_SOURCE_PATH):\
$(SHIMAFUJI_DRIVER_PATH):\
$(SHIMAFUJI_ROUTER_DRIVER_PATH)

SOURCE_PATH = $(SPACEWIRELIBRARY_SOURCE_PATH) \
$(EXECUTABLE_SOURCE_PATH) \
$(USERPROGRAM_SOURCE_PATH) \
$(THREADLIBRARY_SOURCE_PATH) \
$(IPSOCKETLIBRARY_SOURCE_PATH) \
$(SHIMAFUJI_DRIVER_PATH) \
$(SHIMAFUJI_ROUTER_DRIVER_PATH)


##########################
# SpaceWire Library Sources
##########################
SPACEWIRELIBRARY_SOURCE = \
RMAPDestination.cc \
RMAPEngine.cc \
RMAPException.cc \
RMAPExceptionHandler.cc \
RMAPDestinationKnownDestinations.cc \
RMAPPacket.cc \
RMAPSocket.cc \
RMAPSocketConnectionTester.cc \
RMAPUtilities.cc \
SpaceWireAsynchronousStatus.cc \
SpaceWireDestination.cc \
SpaceWireException.cc \
SpaceWireExceptionHandler.cc \
SpaceWireIF.cc \
SpaceWireIFCommandLineIF.cc \
SpaceWireIFConnectedToDummyRMAPTarget.cc \
SpaceWireIFDomesticPipe.cc \
SpaceWireIFDomesticPipeCore.cc \
SpaceWireIFOverIPClient.cc \
SpaceWireIFOverIPServer.cc \
SpaceWireIFSpaceCube1ShimafujiIPcore.cc \
SpaceWireIFSpaceCube1ShimafujiRouterIPcore.cc \
SpaceWireIFSpaceCube1NECSoftIPcore.cc \
SpaceWireIFViaSpW2TCPIPBridge.cc \
SpaceWireIFOverIPClient.cc \
SpaceWireIFOverIPServer.cc \
SpaceWireLinkStatus.cc \
SpaceWirePacket.cc \
SpaceWireUtilities.cc \
SpaceWireCLI.cc \
SpaceWireSSDTPModule.cc \
SpaceWireIFImplementations.cc

ifeq ($(USE_SPC1_NEC_IP_CORE), YES)
SPACEWIRELIBRARY_SOURCE += SpaceWireIFSpaceCube1NECSoftIPcore.cc
endif

SPACEWIRELIBRARY_HEADER = $(addsuffix .hh, $(basename $(SPACEWIRELIBRARY_SOURCE)))
SPACEWIRELIBRARY_OBJECT = $(addsuffix .o, $(basename $(SPACEWIRELIBRARY_SOURCE)))


##########################
# Thread Library Sources
##########################
THREADLIBRARY_SOURCE = \
Condition.cc \
Message.cc \
Mutex.cc \
Thread.cc \
SynchronousException.cc

THREADLIBRARY_HEADER = $(addsuffix .hh, $(basename $(THREADLIBRARY_SOURCE)))
THREADLIBRARY_OBJECT = $(addsuffix .o, $(basename $(THREADLIBRARY_SOURCE)))


##########################
# IP Socket Library Sources
##########################
IPSOCKETLIBRARY_SOURCE = \
IPClientSocket.cc \
IPServerSocket.cc \
IPSocket.cc \
IPSocketException.cc

IPSOCKETLIBRARY_HEADER = $(addsuffix .hh, $(basename $(IPSOCKETLIBRARY_SOURCE)))
IPSOCKETLIBRARY_OBJECT = $(addsuffix .o, $(basename $(IPSOCKETLIBRARY_SOURCE)))


##########################
# NEC IP Core Option
##########################
ifeq ($(USE_SPC1_NEC_IP_CORE), YES)
NECSOFT_DRIVER_PATH = $(SPACEWIRERMAPLIBRARY_PATH)/driver/spc1_necsoft
VPATH += :$(NECSOFT_DRIVER_PATH)
SOURCE_PATH += $(NECSOFT_DRIVER_PATH)
CPPFLAGS += -DUSE_SPC1_NEC_IP_CORE
endif


##########################
# Executable Sources
##########################
EXECUTABLE_SOURCE =  \
main_rmaphongo.cc \
main_speedtest.cc \
main_sthongo.cc \
main_test_SpaceWireIFOverIPClient_speedtest.cc \
main_test_SpaceWireIFOverIPServer_speedtest.cc


EXECUTABLE_HEADER =
EXECUTABLE_OBJECT = $(addsuffix .o, $(basename $(EXECUTABLE_SOURCE)))


##########################
# SOURCES, HEADERS, OBJECTS
##########################
SOURCES = $(SPACEWIRELIBRARY_SOURCE) $(THREADLIBRARY_SOURCE) $(IPSOCKETLIBRARY_SOURCE) $(EXECUTABLE_SOURCE)
HEADERS = $(SPACEWIRELIBRARY_HEADER) $(THREADLIBRARY_HEADER) $(IPSOCKETLIBRARY_HEADER) $(EXECUTABLE_HEADER)
OBJECTS = $(SPACEWIRELIBRARY_OBJECT) $(THREADLIBRARY_OBJECT) $(IPSOCKETLIBRARY_OBJECT) $(EXECUTABLE_OBJECT)
EXECUTABLE = $(basename $(EXECUTABLE_SOURCE))


##########################
# Libraries
##########################
SPACEWIRELIBRARY_LIBRARY = libSpaceWireRMAPHongo.a
THREADLIBRARY_LIBRARY = libThreadHongo.a
IPSOCKETLIBRARY_LIBRARY = libIPSocketLibrary.a
SPC1_SHIMAFUJI_LIBRARY = $(SHIMAFUJI_DRIVER_PATH)/libShimafuji.a
SPC1_SHIMAFUJI_ROUTER_LIBRARY = $(SHIMAFUJI_ROUTER_DRIVER_PATH)/libShimafujiRouter.a
LIBRARIES = $(SPACEWIRELIBRARY_LIBRARY) $(THREADLIBRARY_LIBRARY) $(IPSOCKETLIBRARY_LIBRARY) $(SPC1_SHIMAFUJI_LIBRARY) $(SPC1_SHIMAFUJI_ROUTER_LIBRARY)

ifeq ($(USE_SPC1_NEC_IP_CORE), YES)
SPC1_NECSOFT_LIBRARY = $(NECSOFT_DRIVER_PATH)/libspw.a
LIBRARIES += $(SPC1_NECSOFT_LIBRARY)
endif


##########################
# Flags
##########################
CFLAGS += $(addprefix -I, $(SOURCE_PATH))
ifeq ($(SYSTEM_TYPE), posix)
CPPFLAGS += -DSYSTEM_TYPE_POSIX
endif
ifeq ($(SYSTEM_TYPE), t-kernel)
CPPFLAGS += -DSYSTEM_TYPE_T_KERNEL
endif


##########################
# Installation
##########################
INSTALL_DIRECTORY = $(SPACEWIRERMAPLIBRARY_PATH)/executables/$(SYSTEM_TYPE)
LIBRARY_INSTALL_DIRECTORY = $(SPACEWIRERMAPLIBRARY_PATH)/libraries/$(SYSTEM_TYPE)


##########################
# Compile and Link
##########################
.PHONY : all install dump

all : $(LIBRARIES) $(EXECUTABLE)

dump :
	echo $(VPATH)

install :
	/bin/cp $(EXECUTABLE) $(INSTALL_DIRECTORY)
	/bin/cp $(LIBRARIES) $(LIBRARY_INSTALL_DIRECTORY)

$(SPACEWIRELIBRARY_LIBRARY) : $(SPACEWIRELIBRARY_HEADER) $(SPACEWIRELIBRARY_OBJECT)
	$(AR) cr $@ $(SPACEWIRELIBRARY_OBJECT)
	$(RANLIB) $@

$(THREADLIBRARY_LIBRARY) : $(THREADLIBRARY_HEADER) $(THREADLIBRARY_OBJECT)
	$(AR) cr $@ $(THREADLIBRARY_OBJECT)
	$(RANLIB) $@

$(IPSOCKETLIBRARY_LIBRARY) : $(IPSOCKETLIBRARY_HEADER) $(IPSOCKETLIBRARY_OBJECT)
	$(AR) cr $@ $(IPSOCKETLIBRARY_OBJECT)
	$(RANLIB) $@

$(SPC1_NECSOFT_LIBRARY) :
ifeq ($(USE_SPC1_NEC_IP_CORE), YES)
	$(MAKE) -C $(NECSOFT_DRIVER_PATH)
endif

$(SPC1_SHIMAFUJI_LIBRARY) :
	$(MAKE) -C $(SHIMAFUJI_DRIVER_PATH)
	cp $(SHIMAFUJI_DRIVER_PATH)/lib* .

$(SPC1_SHIMAFUJI_ROUTER_LIBRARY) :
	$(MAKE) -C $(SHIMAFUJI_ROUTER_DRIVER_PATH)
	cp $(SHIMAFUJI_ROUTER_DRIVER_PATH)/lib* .

$(EXECUTABLE) : $(EXECUTABLE_HEADER) $(EXECUTABLE_OBJECT) $(LIBRARIES)
	$(LINK.o) $@.o $(LIBRARIES) $(LDOBJS) $(LOADLIBES) $(LDLIBS) $(OUTPUT_OPTION)

.cc.o :
	$(CXX) -g -O3 $(CPPFLAGS) $(CXXFLAGS) -c  $(CFLAGS) $<

clean :
	/bin/rm -f $(LIBRARIES) $(EXECUTABLE) *.o lib*
	/bin/rm -f $(INSTALL_DIRECTORY)/*
	/bin/rm -f $(LIBRARY_INSTALL_DIRECTORY)/*
	/bin/rm -f Dependencies
	$(MAKE) -C $(SHIMAFUJI_ROUTER_DRIVER_PATH) clean
	$(MAKE) -C $(SHIMAFUJI_DRIVER_PATH) clean
ifeq ($(USE_SPC1_NEC_IP_CORE), YES)
	$(MAKE) -C $(NECSOFT_DRIVER_PATH) clean
endif

# History
# 2008-05-xx file created (Takayuki Yuasa)
# 2008-09-04 NEC IP functionalities were separated (Takayuki Yuasa)
# 2008-09-04 path settings were modified (Takayuki Yuasa)
