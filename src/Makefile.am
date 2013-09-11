bin_PROGRAMS = gnome-chat

gnome_chat_built_sources = \
	chat-about-data.c \
	chat-about-data.h \
	chat-enums.c \
	chat-enums.h \
	chat-marshalers.c \
	chat-marshalers.h \
	chat-resources.c \
	chat-resources.h \
	$(NULL)

nodist_gnome_chat_SOURCES = \
	$(gnome_chat_built_sources) \
	$(NULL)

gnome_chat_SOURCES = \
	empathy-images.h \
	empathy-ui-utils.c \
	empathy-ui-utils.h \
	empathy-utils.c \
	empathy-utils.h \
	polari-fixed-size-frame.c \
	polari-fixed-size-frame.h \
	chat-application.c \
	chat-application.h \
	chat-client-factory.c \
	chat-client-factory.h \
	chat-contacts-list-dialog.c \
	chat-contacts-list-dialog.h \
	chat-conversation-view.c \
	chat-conversation-view.h \
	chat-conversations-list.c \
	chat-conversations-list.h \
	chat-embed.c \
	chat-embed.h \
	chat-main-toolbar.c \
	chat-main-toolbar.h \
	chat-main-window.c \
	chat-main-window.h \
	chat-mode-controller.c \
	chat-mode-controller.h \
	chat-settings.c \
	chat-settings.h \
	chat-main.c \
	$(NULL)

BUILT_SOURCES = \
	$(gnome_chat_built_sources) \
	$(NULL)

EXTRA_DIST = \
	chat-enums.c.template \
	chat-enums.h.template \
	chat-generate-about \
	chat.gresource.xml \
	chat-app-menu.ui \
	chat-contacts-list-dialog.ui \
	chat-conversation-view.ui \
	chat-embed.ui \
	chat-main-toolbar.ui \
	chat-main-window.ui \
	chat.css \
	chat-marshalers.list \
	$(NULL)

AM_CPPFLAGS = \
	-DPACKAGE_ICONS_DIR=\""${pkgdatadir}/icons"\" \
	-DPACKAGE_LOCALE_DIR=\""${datadir}/locale"\" \
	$(GIO_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(GTK_CFLAGS) \
	$(TP_GLIB_CFLAGS) \
	$(TP_LOGGER_CFLAGS) \
	$(WARN_CFLAGS) \
	$(NULL)

gnome_chat_LDFLAGS = 

gnome_chat_LDADD = \
	$(GIO_LIBS) \
	$(GLIB_LIBS) \
	$(GTK_LIBS) \
	$(TP_GLIB_LIBS) \
	$(TP_LOGGER_LIBS) \
	$(NULL)

CLEANFILES = \
	$(gnome_chat_built_sources) \
	stamp-chat-about-data.h \
	stamp-chat-enums.h \
	stamp-chat-marshalers.h \
	xgen-ac \
	xgen-ah \
	xgen-etbc \
	xgen-etbh \
	xgen-tmc \
	xgen-tmh \
	$(NULL)

chat-about-data.h: stamp-chat-about-data.h
	@true

stamp-chat-about-data.h: $(top_srcdir)/ARTISTS $(top_srcdir)/AUTHORS
	$(AM_V_GEN) ( \
		( $(srcdir)/chat-generate-about \
			--header $(top_srcdir) \
                ) >> xgen-ah \
		&& ( cmp -s xgen-ah chat-about-data.h || cp xgen-ah chat-about-data.h ) \
		&& rm -f xgen-ah \
		&& echo timestamp > $(@F) \
	)

chat-about-data.c: $(top_srcdir)/ARTISTS $(top_srcdir)/AUTHORS
	$(AM_V_GEN) ( \
		echo "#include \"chat-about-data.h\"" > xgen-ac \
		&& ( $(srcdir)/chat-generate-about \
			--body $(top_srcdir) \
                   ) >> xgen-ac \
		&& ( cmp -s xgen-ac chat-about-data.c || cp xgen-ac chat-about-data.c ) \
		&& rm -f xgen-ac \
	)

chat-enums.h: stamp-chat-enums.h
	@true

stamp-chat-enums.h: chat-enums.h.template
	$(AM_V_GEN) ( \
		( glib-mkenums \
			--template $(srcdir)/chat-enums.h.template \
                        $(srcdir)/chat-mode-controller.h \
                ) >> xgen-etbh \
		&& ( cmp -s xgen-etbh chat-enums.h || cp xgen-etbh chat-enums.h ) \
		&& rm -f xgen-etbh \
		&& echo timestamp > $(@F) \
	)

chat-enums.c: chat-enums.c.template
	$(AM_V_GEN) ( \
		( glib-mkenums \
			--template $(srcdir)/chat-enums.c.template \
                        $(srcdir)/chat-mode-controller.h \
                ) >> xgen-etbc \
		&& ( cmp -s xgen-etbc chat-enums.c || cp xgen-etbc chat-enums.c ) \
		&& rm -f xgen-etbc \
	)

chat-marshalers.h: stamp-chat-marshalers.h
	@true

stamp-chat-marshalers.h: chat-marshalers.list Makefile
	$(AM_V_GEN) ( \
		( glib-genmarshal \
			--prefix=_chat_marshal \
			--header $(srcdir)/chat-marshalers.list \
		) >> xgen-tmh \
		&& ( cmp -s xgen-tmh chat-marshalers.h || cp xgen-tmh chat-marshalers.h ) \
		&& rm -f xgen-tmh \
		&& echo timestamp > $(@F) \
	)

chat-marshalers.c: chat-marshalers.list chat-marshalers.h Makefile
	$(AM_V_GEN) ( \
		echo "#include \"chat-marshalers.h\"" > xgen-tmc \
		&& ( glib-genmarshal \
			--prefix=_chat_marshal \
			--body $(srcdir)/chat-marshalers.list \
		   ) >> xgen-tmc \
		&& cp xgen-tmc chat-marshalers.c \
		&& rm -f xgen-tmc \
	)

chat-resources.c: chat.gresource.xml $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=$(srcdir) --generate-dependencies $(srcdir)/chat.gresource.xml)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --target=$@ \
		--sourcedir=$(srcdir) \
		--generate-source \
		$<

chat-resources.h: chat.gresource.xml $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=$(srcdir) --generate-dependencies $(srcdir)/chat.gresource.xml)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --target=$@ \
		--sourcedir=$(srcdir) \
		--generate-header \
		$<

-include $(top_srcdir)/git.mk
