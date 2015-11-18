
#-----------------------

PHPC_EMBED = pcs://internal/tools/embed.php
PHPC_CMD = $(PHP_EXECUTABLE) -r "require '$(PHPC_EMBED)';" --

.PHONY: phpc code

phpc: code

code:
	$(PHPC_CMD) -s $(srcdir)/php/src $@ $(srcdir)/php/phpc/$@.phpc

cleanup: clean
	phpize --clean
	@if [ -d include ] ; then rmdir include ; fi
	@if [ -d modules ] ; then rmdir modules ; fi
