#######################################################
# Project 2, for EECS 22L, Winter 2018
#
# Authors: Team4 (ZOTCOMM)
# Date: 3/4/2018
#
# Makefile: top level makefile for project 2
#
#######################################################


# Default target
all:
	cd src_server; make
	cd src_client; make

# Targets to run the chat program
test:
	@echo "To run the program, first start the server in one terminal,"
	@echo "then start one (or multiple) client(s) in another."
	@echo
	@echo "For example:"
	@echo "crystalcove% ./Server 10000"
	@echo "crystalcove% ./Client cyrstalcove 10000"
	@echo "Note: If you typed make all, the executables will be in the bin folder"

test-gui:
	@echo "To run the program, first start the server in one terminal,"
	@echo "then start one (or multiple) client(s) in another."
	@echo
	@echo "For example:"
	@echo "crystalcove% ./Server 10000"
	@echo "crystalcove% ./Client cyrstalcove 10000"
	@echo "Note: If you typed make all, the executables will be in the bin folder"

test-comm:
	cd src_client; make ClientModuleTest	
	@echo "Client comm module test that excludes gui has been created in bin folder."
	@echo "To run the test module, first start the server in one terminalm"
	@echo "then start one (or multiple) client(s) in another."
	@echo
	@echo "For example:"
	@echo "crystalcove% ./Server 10000"
	@echo "crystalcove% ./ClientModuleTest crystalcove 10000"
	@echo "Note: The executables will be in the bin folder"

# Others

# Target to tar the source code package
tar:
	make clean;
	gtar cvzf ../Chat_Alpha_src.tar.gz ../SourcePackage

# Target for clean-up
clean:
	cd src_server; make clean
	cd src_client; make clean
