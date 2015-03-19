TESTDIR := $(dir $(lastword $(MAKEFILE_LIST)))

TEST := $(TESTDIR)test
TESTSRC := $(TEST).cpp
TESTOBJ := $(TEST).o

# Altough $(TESTSRC) does not contains the text `int main`,
# it nerverthless can be compiled to an executable
# because the CATCH framework provides a main.
# So it makes sense to include it as a "main" file.
MAIN += $(TESTSRC)

NOMAIN += $(shell find $(TESTDIR) -wholename $(TESTSRC) -prune -o -name "*.cpp" -print)

$(TEST): $(NOMAIN:.cpp=.o)

.PHONY: test
test: $(TEST)
	$(TEST)


.PHONY: test-clean test-mostlyclean

mostlyclean: test-mostlyclean

# We will not remove test/test.o because it is very costly to rebuild.
test-mostlyclean:
	find $(TESTDIR) -name "*.o" -! -wholename $(TESTOBJ) -exec rm {} +

clean: test-clean
test-clean: test-mostlyclean
	rm $(TESTOBJ)
