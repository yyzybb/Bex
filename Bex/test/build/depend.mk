# Bex test depend.mk

include config.mk

.PHONY: depend_reference
depend_reference : ${objects:.o=.d}

${objects:.o=.d} : %.d : %.cpp
	${GCC} ${CXXFLAGS} ${INCLUDEPATH} -MM $< | grep -v boost > $@.$$$$; \
	basename $< | sed s/\.cpp/\.d/g | paste - $@.$$$$ -d\  > $@; \
	rm $@.$$$$

