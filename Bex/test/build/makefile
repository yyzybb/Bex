# Bex test makefile
#
#

include config.mk

test : ${objects}
	${GCC} ${CXXFLAGS} ${objects} ${STATIC_LINK} ${DYNAMIC_LINK} -o test

.PHONY: clean
clean:
	-rm *.d *.o test

include *.d
