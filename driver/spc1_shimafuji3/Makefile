##########################
#Shimafuji SpW Driver/Library Files
##########################
SHIMAFUJI_LIBRARY_SOURCE = spw_if.c

SHIMAFUJI_LIBRARY_OBJECT = $(addsuffix .o, $(basename $(SHIMAFUJI_LIBRARY_SOURCE)))

##########################
# T-Kernel options
##########################
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
PATH += :/bin:/usr/bin
##########################

SHIMAFUJI_LIBRARY = libShimafuji.a

LIBRARIES = $(SHIMAFUJI_LIBRARY)

#Compile and Link
.PHONY : all

all : $(LIBRARIES) 
	$(MAKE) -C spc_driver
	
$(SHIMAFUJI_LIBRARY) : $(SHIMAFUJI_LIBRARY_OBJECT)
	$(AR) cr $@ $(SHIMAFUJI_LIBRARY_OBJECT)
	$(RANLIB) $@
	
.cc.o :
	$(CXX) -g -O3 $(CPPFLAGS) $(CXXFLAGS) -c  $(CFLAGS) $<

.c.o :
	$(CXX) -g -O3 $(CPPFLAGS) $(CXXFLAGS) -c  $(CFLAGS) $<
	
clean : 
	/bin/rm -f $(SHIMAFUJI_LIBRARY_OBJECT) $(LIBRARIES)
	$(MAKE) -C spc_driver clean