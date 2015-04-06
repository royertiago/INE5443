TESTDIR := $(dir $(lastword $(MAKEFILE_LIST)))

TEST := $(TESTDIR)test
TESTSRC := $(shell find $(TESTDIR) -name "*.cpp")
TESTOBJ := $(TESTSRC:.cpp=.o)

DEP += $(TESTSRC:.cpp=.dep.mk)

$(TESTOBJ): %.o : %.cpp
	$(compile_obj)

# Admittedly complex dependency management.
$(TEST): $(TEST).o $(TESTOBJ) $(NOMAIN:.cpp=.o)
	$(link_main)

all : $(TEST)

.PHONY: test
test: $(TEST)
	$(TEST)


.PHONY: test-clean test-mostlyclean

mostlyclean: test-mostlyclean

# We will not remove test/test.o because it is very costly to rebuild.
test-mostlyclean:
	find $(TESTDIR) -name "*.o" -! -wholename $(TEST).o -exec rm {} +

clean: test-clean
test-clean: test-mostlyclean
	rm -f $(TEST) $(TEST).o
