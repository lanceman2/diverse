#!gmake
# App Name
NAME := quad
DEFINES := 

# Objects
OBJECTS := quad.o 

# Libraries Required
LIBS := 

# Libraries Locations
LIBDIRS := $(shell dgl-config --libs) $(shell dgl-config --vtk-libs)

# Includes Locations
INCDIRS := $(shell dgl-config --include) $(shell dgl-config --vtk-include)

# Compiler and Compiler Options
CXX := $(shell dgl-config --compiler)
CXXFLAGS := $(shell dgl-config --cflags)
LINK := $(CXX) 

CLINE := $(strip ${CC} ${CFLAGS} ${INCDIRS} ${DEFINES})
CXXLINE := $(strip ${CXX} ${CXXFLAGS} ${INCDIRS} ${DEFINES})

SHELL := /bin/sh
OBJDIR := .

##### No Need To Modify Below Here #####
#insert directories into variables
OBJECTS := $(patsubst %.o,$(OBJDIR)/%.o,$(OBJECTS))

#Pattern Rules
$(OBJDIR)/%.o: %.cpp
	$(CXXLINE) -c -o $@ $<

install: all
# Make Target
all: ${NAME}

${NAME}: ${OBJECTS} 
	${LINK} -o $@ $^ $(CXXFLAGS) $(LIBDIRS) $(LIBS)  

# Clean Objects and Target
clean:
	rm -f $(OBJDIR)/*.o tags .depend $(NAME)

distclean: clean
	echo rm -f tags *~

tags: *[cp]
	ctags *[cp]

depend: .depend

.depend: Makefile *cpp
	${CXX} $(DEFINES) ${INCDIRS} -M *.cpp  > .depend
	sed -e '/^[A-Za-z1-9]/s/^/$(OBJDIR)\//' .depend > .depend-temp
	mv -f .depend-temp .depend

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
ifneq ($(MAKECMDGOALS),tags)
-include .depend
endif
endif
endif

.PHONY: all install clean distclean depend .depend

