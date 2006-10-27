CLEANFILES = enter.conf

confdir= $(sysconfdir)
themedir= $(datadir)/@PACKAGE@

dist_conf_DATA = enter.conf
dist_theme_DATA = \
	themes/default/theme \
	themes/default/background.png

edit = sed \
       -e 's|@sbindir[@]|$(sbindir)|g' \
       -e 's|@PACKAGE[@]|$(PACKAGE)|g' \
       -e 's|@datarootdir[@]|$(datarootdir)|g' \
       -e 's|@prefix[@]|$(prefix)|g'

enter.conf: enter.conf.in
	$(edit) $< > $@

