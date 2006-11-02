confdir= $(sysconfdir)
themedir= $(datadir)/@PACKAGE@

dist_conf_DATA = enter.conf
nobase_dist_theme_DATA = \
	themes/default/theme \
	themes/default/background.png

CLEANFILES = $(dist_conf_DATA)
EXTRA_DIST = enter.conf.in

edit = sed \
       -e 's|@sbindir[@]|$(sbindir)|g' \
       -e 's|@PACKAGE[@]|$(PACKAGE)|g' \
       -e 's|@datarootdir[@]|$(datarootdir)|g' \
       -e 's|@prefix[@]|$(prefix)|g'

enter.conf: Makefile
	@$(edit) $(srcdir)/enter.conf.in > $@

enter.conf: $(srcdir)/enter.conf.in

