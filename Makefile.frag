

PHPC_EMBED = $(srcdir)/php/src/internal/tools/embed.php

.PHONY: phpc parser_code tools_code

phpc: parser_code tools_code

parser_code:
	$(PHP_EXECUTABLE) $(PHPC_EMBED) -s $(srcdir)/php/src/internal/parser \
		parser_code $(srcdir)/php/phpc/$@.phpc

tools_code:
	$(PHP_EXECUTABLE) $(PHPC_EMBED) -s $(srcdir)/php/src/internal/tools \
		tools_code $(srcdir)/php/phpc/$@.phpc

cleanup: clean
	phpize --clean
	@if [ -d include ] ; then rmdir include ; fi
	@if [ -d modules ] ; then rmdir modules ; fi
	@for i in examples/* ; do if [ -f $$i/Makefile ] ; then $(MAKE) -C $$i $@ ; fi ; done

fulltest:
	sh travis/run-test.sh

