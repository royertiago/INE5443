# Utility defines
define compile_obj
$(CXX) $(CXXFLAGS) -c $< -o $@
endef

define link_main
$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
endef


# This makefile handles multiple programs in the same directory
# that include several files.
#
# However, some strictness must be followed in the code;
# see "naming_conventions.md" for more information.
CXXFLAGS += -std=c++1y -g -iquote./ \
			-isystem Catch/single_include $$(pkg-config opencv --cflags) -iquote./
LDFLAGS += $$(pkg-config opencv --libs)

# Directories whose makefiles need to be included
INCLUDE := $(wildcard */makefile.mk)

# Subdirecories thad need to be ignored by find.
# For instance, we won't scan directories with their own makefile.
ignore_dirs := $(dir $(INCLUDE))
ignore_dirs := .git Catch $(ignore_dirs:%/=%)
FINDIGNORE := $(patsubst %, -path "./%" -prune -o, $(ignore_dirs))


# cpp files that contains a "main" function
MAIN := $(shell find . $(FINDIGNORE) -name "*.cpp" -exec grep -l "^int main" {} +)
# cpp files without "main" function
NOMAIN := $(shell find . $(FINDIGNORE) -name "*.cpp" -exec grep -L "^int main" {} +)

DEP :=


# Intermediary file includion - see submakefiles.md for details.
include $(INCLUDE)


# Dependency files
DEP += $(MAIN:.cpp=.dep.mk) $(NOMAIN:.cpp=.dep.mk)

# Object files
MAINOBJ := $(MAIN:.cpp=.o)
NOMAINOBJ := $(NOMAIN:.cpp=.o)

# Programs generated by this project
PROGRAMS := $(MAIN:.cpp=)


-include $(DEP)


.DEFAULT_GOAL := all

.PHONY: all
all: $(PROGRAMS)

# Suboptimal dependency management.
# I have made every program depend on every non-main object file;
# this way I can guarantee that the object file will be remade
# whenever any module used by an artifficial inteligence is updated.
#
# Resolving this without such intrusion would need to somehow
# inspect the object files and tell make which symbols are defined
# in each object. This inspection could be done by using by somehow parsing
# 'nm -u' (to get the dependecies) and 'nm | grep " *[TtRr]"' (to get the
# provided symbols), but I have no idea of how to dynamically update
# the make's DAG without invoking make repeatedly
# --- which should be make's work.
$(PROGRAMS): %: %.o $(NOMAINOBJ)
	$(link_main)

$(MAINOBJ) $(NOMAINOBJ): %.o : %.cpp
	$(compile_obj)

$(DEP): %.dep.mk: %.cpp
	$(CXX) $(CXXFLAGS) $*.cpp -MM -MP -MF $*.dep.mk -MT '$*.o $*.dep.mk'

.PHONY: mostlyclean clean
mostlyclean:
	-find $(FINDIGNORE) -name "*.o" -exec rm '{}' \;
	-find -name "*.dep.mk" -exec rm '{}' \;

clean: mostlyclean
	-rm -f $(PROGRAMS)
