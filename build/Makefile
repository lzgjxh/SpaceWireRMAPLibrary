#Makefile for SpaceWire RMAP Library

##########################
# Options
##########################

# Library Root Path Setting
#
# set absolute path to the root directory
# of SpaceWire/RMAP Library.

#export SPACEWIRERMAPLIBRARY_PATH = /usr/local/SpaceWireRMAPLibrary
export SPACEWIRERMAPLIBRARY_PATH = $(HOME)/workspace/SpaceWireRMAPLibrary_20110308/SpaceWireRMAPLibrary

##########################

.PHONY : all posix t-kernel

all: posix


posix:
	$(MAKE) -C posix all
	$(MAKE) -C posix install

t-kernel:
	$(MAKE) -C t-kernel all

clean:
	$(MAKE) -C posix clean
#	$(MAKE) -C t-kernel clean
