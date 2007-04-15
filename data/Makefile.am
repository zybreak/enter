confdir= $(sysconfdir)
themedir= $(datadir)/@PACKAGE@

dist_conf_DATA = enter.conf
nobase_dist_theme_DATA = \
	themes/default/theme \
	themes/default/background.png \
	themes/default/input.png \
	themes/xmas/theme \
	themes/xmas/background.png \
	themes/xmas/input.png

CLEANFILES = $(dist_conf_DATA)
EXTRA_DIST = enter.conf.in

edit = $(SED) \
       -e 's|@X_PATH[@]|$(X_PATH)|g'

enter.conf: Makefile
	@$(edit) $(srcdir)/$@.in > $@

enter.conf: $(srcdir)/enter.conf.in

