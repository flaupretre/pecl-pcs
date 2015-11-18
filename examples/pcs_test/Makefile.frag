
#-----------------------

PHPC_EMBED = pcs://internal/tools/embed.php
PHPC_CMD = $(PHP_EXECUTABLE) -r "require '$(PHPC_EMBED)';" --

.PHONY: phpc code1 code2 code3 empty

phpc: code1 code2 code3 empty

code1:
	$(PHPC_CMD) -s $(srcdir)/php/src/$@ $@ $(srcdir)/php/phpc/$@.phpc

code2:
	$(PHPC_CMD) -s $(srcdir)/php/src/$@ $@ $(srcdir)/php/phpc/$@.phpc

code3:
	$(PHPC_CMD) -s $(srcdir)/php/src/$@ $@ $(srcdir)/php/phpc/$@.phpc

empty:
	[ -d $(srcdir)/php/src/$@ ] || mkdir $(srcdir)/php/src/$@
	$(PHPC_CMD) -s $(srcdir)/php/src/$@ $@ $(srcdir)/php/phpc/$@.phpc

cleanup: clean
	phpize --clean
	@if [ -d include ] ; then rmdir include ; fi
	@if [ -d modules ] ; then rmdir modules ; fi
