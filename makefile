# LIBS is the list of libraries used by this repository.
# ILIBS are the include flags for GCC.
LIBS := Catch
ILIBS := -isystem Catch/single_include

# This makefile handles multiple programs in the same directory
# that include several files.
#
# However, some strictness must be followed in the code;
# see "naming_conventions.md" for more information.
CXXFLAGS := -std=c++1y -g $(ILIBS) -I./

# Directories whose makefiles need to be included
INCLUDE := test

# Subdirecories thad are to be ignored by find.
FINDIGNORE := $(patsubst %, -path "./%" -prune -o, .git $(LIBS) $(INCLUDE))


# cpp files that contains a "main" function
MAIN := $(shell find . $(FINDIGNORE) -name "*.cpp" -exec grep -l "^int main" {} +)

# cpp files without "main" function
NOMAIN := $(shell find $(FINDIGNORE) -name "*.cpp" -exec grep -L "^int main" {} +)

# Intermediary file includion - see submakefiles.md for details.
include $(patsubst %, %/makefile.mk, $(INCLUDE))

# Dependency files
MAINDEP := $(MAIN:.cpp=.dep.mk)
NOMAINDEP := $(NOMAIN:.cpp=.dep.mk)

# Object files
OBJ := $(MAIN:.cpp=.o) $(NOMAIN:.cpp=.o)

# Programs generated by this project
PROGRAMS := $(MAIN:.cpp=)


-include $(MAINDEP) $(NOMAINDEP)


.DEFAULT_GOAL := all

.PHONY: all
all: $(PROGRAMS)

$(PROGRAMS): %: %.o
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ): %.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NOMAINDEP): %.dep.mk: %.cpp
	$(CXX) $(CXXFLAGS) $*.cpp -MM -MP -MF $*.dep.mk -MT '$*.o $*.dep.mk'

# The cpp files that contains 'main' functions receive special treatment.
# The recipe for the targets generated by variable PROGRAMS
# link together all the .o files that depend on the program.
# The second command of this recipe updates this dependencies.
$(MAINDEP): %.dep.mk: %.cpp
	$(CXX) $(CXXFLAGS) $*.cpp -MM -MP -MF $*.dep.mk -MT '$*.o $*.dep.mk'
	sed 's|^\([^:]*\).h:$$|&\n$*: \1.o|' -i $*.dep.mk

.PHONY: mostlyclean clean
mostlyclean:
	-find $(FINDIGNORE) -name "*.o" -exec rm '{}' \;
	-find -name "*.dep.mk" -exec rm '{}' \;

clean: mostlyclean
	-rm -f $(PROGRAMS)
