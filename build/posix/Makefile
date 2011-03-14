SYSTEM_TYPE = posix

##########################
# Options
##########################


##########################
# PATHs
##########################
SPACEWIRELIBRARY_SOURCE_PATH = $(SPACEWIRERMAPLIBRARY_PATH)/source_SpaceWireRMAPLibrary
EXECUTABLE_SOURCE_PATH = $(SPACEWIRERMAPLIBRARY_PATH)/source_Executables
USERPROGRAM_SOURCE_PATH = $(SPACEWIRERMAPLIBRARY_PATH)/source_UserPrograms
THREADLIBRARY_SOURCE_PATH = $(SPACEWIRERMAPLIBRARY_PATH)/source_ThreadLibrary/$(SYSTEM_TYPE)
IPSOCKETLIBRARY_SOURCE_PATH = $(SPACEWIRERMAPLIBRARY_PATH)/source_IPSocketLibrary/$(SYSTEM_TYPE)
#NECSOFT_DRIVER_PATH = $(SPACEWIRERMAPLIBRARY_PATH)/driver/spc1_necsoft
#SHIMAFUJI_DRIVER_PATH = $(SPACEWIRERMAPLIBRARY_PATH)/driver/spc1_shimafuji

VPATH = $(SPACEWIRELIBRARY_SOURCE_PATH):\
$(EXECUTABLE_SOURCE_PATH):\
$(USERPROGRAM_SOURCE_PATH):\
$(THREADLIBRARY_SOURCE_PATH):\
$(IPSOCKETLIBRARY_SOURCE_PATH):\
$(SHIMAFUJI_DRIVER_PATH):\
$(NECSOFT_DRIVER_PATH)

SOURCE_PATH = $(SPACEWIRELIBRARY_SOURCE_PATH) \
$(EXECUTABLE_SOURCE_PATH) \
$(USERPROGRAM_SOURCE_PATH) \
$(THREADLIBRARY_SOURCE_PATH) \
$(IPSOCKETLIBRARY_SOURCE_PATH) \
$(SHIMAFUJI_DRIVER_PATH) \
$(NECSOFT_DRIVER_PATH)


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
SpaceWireIFViaSpW2TCPIPBridge.cc \
SpaceWireLinkStatus.cc \
SpaceWirePacket.cc \
SpaceWireUtilities.cc \
SpaceWireCLI.cc \
SpaceWireSSDTPModule.cc \
SpaceWireIFImplementations.cc

SPACEWIRELIBRARY_HEADER = $(addsuffix .hh, $(basename $(SPACEWIRELIBRARY_SOURCE))) SpaceWire.hh RMAP.hh
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
# Executable Sources
##########################
EXECUTABLE_SOURCE =  \
main_rmaphongo \
main_speedtest \
main_test_RMAPPacket.cc \
main_test_RMAPSocket_Timeout.cc \
main_test_SpaceWireIFCommandLineIF.cc \
main_test_Thread.cc \
main_test_Message.cc \
main_test_Mutex.cc \
main_example01.cc \
main_test_IPClientSokcet.cc \
main_test_IPServerClient.cc \
main_test_SSDTPModule_speedtest.cc \
main_test_SpaceWireIFViaSpW2TCPIPBridge.cc \
main_test_SpaceWireIFDomesticPipe.cc \
main_test_SpaceWireIFOverIPClient.cc \
main_test_SpaceWireIFOverIPServer.cc \
main_test_SpaceWireIFOverIPClient_speedtest.cc \
main_test_SpaceWireIFOverIPServer_speedtest.cc \
main_test_RMAPDestination_clone.cc \
main_test_SpaceWireSSDTPModule_server_send.cc

EXECUTABLE_HEADER =
EXECUTABLE_OBJECT = $(addsuffix .o, $(basename $(EXECUTABLE_SOURCE)))


##########################
# SOURCES, HEADERS, OBJECTS
##########################
SOURCES = $(SPACEWIRELIBRARY_SOURCE) $(THREADLIBRARY_SOURCE) $(IPSOCKETLIBRARY_SOURCE) $(EXECUTABLE_SOURCE)
HEADERS = $(SPACEWIRELIBRARY_HEADER) $(THREADLIBRARY_HEADER) $(IPSOCKETLIBRARY_HEADER) $(EXECUTABLE_HEADER)
OBJECTS = $(SPACEWIRELIBRARY_OBJECT) $(THREADLIBRARY_OBJECT) $(IPSOCKETLIBRARY_OBJECT) $(EXECUTABLE_OBJECT)
EXECUTABLES = $(basename $(EXECUTABLE_SOURCE))


##########################
# Libraries
##########################
SPACEWIRELIBRARY_LIBRARY = libSpaceWireRMAPHongo.a
THREADLIBRARY_LIBRARY = libThreadHongo.a
IPSOCKETLIBRARY_LIBRARY = libIPSocketLibrary.a
#SPC1_NECSOFT = $(NECSOFT_DRIVER_PATH)/libspw.a
#SPC1_SHIMAFUJI = $(SHIMAFUJI_DRIVER_PATH)/libShimafuji.a
LIBRARIES = $(SPACEWIRELIBRARY_LIBRARY) $(THREADLIBRARY_LIBRARY) $(IPSOCKETLIBRARY_LIBRARY) $(SPC1_NECSOFT) $(SPC1_SHIMAFUJI)


##########################
# Flags
##########################
CFLAGS += $(addprefix -I, $(SOURCE_PATH)) -Wall
ifeq ($(SYSTEM_TYPE), posix)
CPPFLAGS += -DSYSTEM_TYPE_POSIX
LDLIBS += -lpthread
endif
ifeq ($(SYSTEM_TYPE), t-kernel)
CPPFLAGS += -DSYSTEM_TYPE_T_KERNEL
endif


##########################
# Installation
##########################
INSTALL_DIRECTORY = $(SPACEWIRERMAPLIBRARY_PATH)/executables/
LIBRARY_INSTALL_DIRECTORY = $(SPACEWIRERMAPLIBRARY_PATH)/libraries/
HEADER_INSTALL_DIRECTORY = $(SPACEWIRERMAPLIBRARY_PATH)/includes/


##########################
# Compile and Link
##########################
.PHONY : all

all: $(LIBRARIES) $(EXECUTABLES)

$(SPACEWIRELIBRARY_LIBRARY) : $(SPACEWIRELIBRARY_HEADER) $(SPACEWIRELIBRARY_OBJECT)
	ar cr $@ $(SPACEWIRELIBRARY_OBJECT)
	ranlib $@

$(THREADLIBRARY_LIBRARY) : $(THREADLIBRARY_HEADER) $(THREADLIBRARY_OBJECT)
	ar cr $@ $(THREADLIBRARY_OBJECT)
	ranlib $@

$(IPSOCKETLIBRARY_LIBRARY) : $(IPSOCKETLIBRARY_HEADER) $(IPSOCKETLIBRARY_OBJECT)
	ar cr $@ $(IPSOCKETLIBRARY_OBJECT)
	ranlib $@

$(EXECUTABLES) : $(EXECUTABLE_HEADER) $(EXECUTABLE_OBJECT) $(LIBRARIES)
	$(CXX) -o $@ $@.o $(LIBRARIES) $(LDOBJS) $(LDFLAGS) $(LOADLIBES) $(LDLIBS)

.cc.o:
	$(CXX) -g -O0 $(CPPFLAGS) $(CXXFLAGS) -c  $(CFLAGS) $<

install:
	#installing executables
	@mkdir -p $(INSTALL_DIRECTORY)
	@/bin/cp -f $(EXECUTABLES) $(INSTALL_DIRECTORY)
	#installing libraries
	@mkdir -p $(LIBRARY_INSTALL_DIRECTORY)
	@/bin/cp -f $(LIBRARIES) $(LIBRARY_INSTALL_DIRECTORY)
	@(cd $(LIBRARY_INSTALL_DIRECTORY); ranlib $(LIBRARIES))
	#installing header files
	@mkdir -p $(HEADER_INSTALL_DIRECTORY)
	@for file in $(SPACEWIRELIBRARY_HEADER); do \
	/bin/cp -f $(SPACEWIRELIBRARY_SOURCE_PATH)/$$file $(HEADER_INSTALL_DIRECTORY); \
	done
	@for file in $(THREADLIBRARY_HEADER); do \
	/bin/cp -f $(THREADLIBRARY_SOURCE_PATH)/*.hh $(HEADER_INSTALL_DIRECTORY); \
	done
	@for file in $(IPSOCKETLIBRARY_HEADER); do \
	/bin/cp -f $(IPSOCKETLIBRARY_SOURCE_PATH)/*.hh $(HEADER_INSTALL_DIRECTORY); \
	done
	#completed!

clean:
	/bin/rm -f $(LIBRARIES) $(EXECUTABLES) *.o
	/bin/rm -rf $(INSTALL_DIRECTORY)
	/bin/rm -rf $(LIBRARY_INSTALL_DIRECTORY)
	/bin/rm -rf $(HEADER_INSTALL_DIRECTORY)

# History
# 2008-05-xx file created (Takayuki Yuasa)
# 2008-09-04 path settings were modified (Takayuki Yuasa)
# 2010-01-18 Snow Leopard support (Takayuki Yuasa)
# 2010-03-11 Header files are also installed into single folder (Takayuki Yuasa)