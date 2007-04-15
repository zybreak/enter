man_MANS = enter.1 enter.conf.5

CLEANFILES = $(man_MANS)
EXTRA_DIST = enter.1.in enter.conf.5.in

edit = $(SED) \
       -e 's|@VERSION[@]|$(VERSION)|g' \
       -e 's|@sysconfdir[@]|$(sysconfdir)|g' \
       -e 's|@PACKAGE[@]|$(PACKAGE)|g' \
       -e 's|@datadir[@]|$(datadir)|g'

enter.1 enter.conf.5: Makefile
	@$(edit) $(srcdir)/$@.in > $@

enter.1: $(srcdir)/enter.1.in
enter.conf.5: $(srcdir)/enter.conf.5.in

