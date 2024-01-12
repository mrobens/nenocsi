#####################################################################################
# Noxim - the NoC Simulator
#
# Copyright (C) 2005-2018 by the University of Catania
# For the complete list of authors refer to file ../doc/AUTHORS.txt
# For the license applied to these sources refer to file ../doc/LICENSE.txt
#
#####################################################################################
# Downloaded March 23, 2022 from
# https://github.com/davidepatti/noxim/tree/c52ebce2217e57bcd4ff11a97b400323bd00acd5
#************************************************************************************
#
# McAERsim - NoC simulator with tree-based multicast support for AER packets
# Modifications Copyright (C) 2022-2023 Forschungszentrum Juelich GmbH, ZEA-2
# Author: Markus Robens <https://www.fz-juelich.de/profile/robens_m>
# For the license applied to these modifications and McAERsim as a whole
# refer to file ../doc/LICENSE_MCAERSIM.txt
#  
# 2023-02-17: According to the installation instructions in ../doc/INSTALL.txt,
#             system libraries of SystemC and YAML-CPP are used. Thus, there is no
#             need to configure library paths and according entries have been removed
#             compared to the original Makefile. Also some command line switches have
#             been simplified, accordingly. McAERsim requires an additional tool,
#             rtparser, to convert NEST's connectivity information to a global
#             routing table. Respective build instructions have been included in this
#             makefile. Also, there is the possibility to select the number of
#             processing elements per tile. Respective modules have been (re)named
#             appropriately.
#
##### DIRECTORIES ###################################################################

SRCDIR        := ../src
SUBDIRS       := $(filter %/,$(wildcard $(SRCDIR)/*/))
OBJDIR        := ./build
SRCS          := $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(addsuffix *.cpp,$(SUBDIRS)))
OBJS          := $(subst $(SRCDIR),$(OBJDIR),$(SRCS:.cpp=.o))
MODULE        := mcaersim

SRCDIR_PARSER := ../src_parser
OBJDIR_PARSER := ./build_parser
SRCS_PARSER   := $(wildcard $(SRCDIR_PARSER)/*.cpp)
OBJS_PARSER   := $(subst $(SRCDIR_PARSER),$(OBJDIR_PARSER),$(SRCS_PARSER:.cpp=.o))
PARSER        := rtparser

##### COMPILER OPTIONS ##############################################################

# Note: To enable logs with LOG macro uncomment "-g -DDEBUG" below
#       To modify the number of processing elements uncomment -DNO_PES=2
#       and replace 2 by the desired number of PEs
#       Then, recompile everything (call 'make clean' before)

CXX           := g++
OPT           := -O3
#DEBUG         := -g -DDEBUG
#SPECIAL       := -DNO_PES=2
OTHER         := -Wall -DSC_NO_WRITE_CHECK
CXXFLAGS      := $(OPT) $(OTHER) $(DEBUG) $(SPECIAL)
INCDIR        := -I$(SRCDIR)
LIBS          := -lboost_filesystem -lsystemc -lm -lyaml-cpp 
SPACE         := $(subst ,, )
VPATH         := $(SRCDIR):$(subst $(SPACE),:,$(SUBDIRS)):$(SRCDIR_PARSER)

INCDIR_PARSER := -I$(SRCDIR_PARSER)
LIBS_PARSER   := -lboost_filesystem -lyaml-cpp

##### RULES #########################################################################

$(MODULE): $(OBJS)
	$(CXX) $^ $(LIBS) -o $@ 2>&1 | c++filt
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCDIR) -c $< -o $@
$(PARSER): $(OBJS_PARSER)
	$(CXX) $^ $(LIBS_PARSER) -o $@ 2>&1 | c++filt
$(OBJDIR_PARSER)/%.o: %.cpp
	@mkdir -p $(OBJDIR_PARSER)
	$(CXX) $(OPT) -Wall $(INCDIR_PARSER) -c $< -o $@
clean:
	rm -f $(OBJS) $(MODULE) $(OBJS_PARSER) $(PARSER) *~
depend:
	makedepend $(SRCS) -Y -f- 2>/dev/null | sed 's_$(SRCDIR)_$(OBJDIR)_' > Makefile.deps
	makedepend $(SRCS_PARSER) -Y -f- 2>/dev/null | \
	sed '\?$(SRCDIR_PARSER)? !d; s?$(SRCDIR_PARSER)?$(OBJDIR_PARSER)?' >> Makefile.deps

-include Makefile.deps
