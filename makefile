PREFIX?=/usr/local
MANPREFIX?=${PREFIX}/share/man
STRIP?=strip

default: mailcat

mailcat: mailcat.c
	$(CC) -Wall -Os -o mailcat mailcat.c

clean:
	@echo cleaning
	@rm -f mailcat

install: mailcat
	@echo stripping executable
	@${STRIP} mailcat
	@echo installing executable to ${PREFIX}/bin
	@mkdir -p ${PREFIX}/bin
	@cp -f mailcat ${PREFIX}/bin/mailcat
	@chmod 755 ${PREFIX}/bin/mailcat

	@echo installing manual pages to ${MANPREFIX}/man1
	@mkdir -p ${MANPREFIX}/man1
	@cp -f mailcat.1 ${MANPREFIX}/man1/mailcat.1
	@chmod 644 ${MANPREFIX}/man1/mailcat.1

uninstall:
	@echo removing executable from ${PREFIX}/bin
	@rm ${PREFIX}/bin/mailcat
	@echo removing manual pages from ${MANPREFIX}/man1
	@rm ${MANPREFIX}/man1/mailcat.1

test:
	@sh test/tests.sh

.PHONY: clean install uninstall test
