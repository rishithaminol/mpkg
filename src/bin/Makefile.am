AM_CFLAGS = -I$(top_srcdir)/src/bin

AM_CFLAGS += $(SQLITE3_CFLAGS)

bin_PROGRAMS = mpkg
mpkg_SOURCES = mpkg.c ar.c copy.c in_list.c info.c mpkg_err.c \
				sqlfrms.c utils.c v_compare.c version.c

mpkg_LDADD = $(AM_LDFLAGS)
mpkg_LDADD += $(SQLITE3_LIBS)
