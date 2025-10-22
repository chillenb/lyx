include $(top_srcdir)/config/common.am

noinst_LIBRARIES = liblyxzlib.a

ZLIB_VERSION=1.3.1

EXTRA_DIST = \
	CMakeLists.txt \
	$(ZLIB_VERSION)/LICENSE \
	$(ZLIB_VERSION)/README \
	$(ZLIB_VERSION)/treebuild.xml \
	$(ZLIB_VERSION)/zconf.h.in \
	$(ZLIB_VERSION)/zconf.h.cmakein \
	$(ZLIB_VERSION)/zlib.3 \
	$(ZLIB_VERSION)/zlib.map

liblyxzlib_a_SOURCES = \
	$(ZLIB_VERSION)/adler32.c \
	$(ZLIB_VERSION)/compress.c \
	$(ZLIB_VERSION)/crc32.c \
	$(ZLIB_VERSION)/crc32.h \
	$(ZLIB_VERSION)/deflate.c \
	$(ZLIB_VERSION)/deflate.h \
	$(ZLIB_VERSION)/gzclose.c \
	$(ZLIB_VERSION)/gzguts.h \
	$(ZLIB_VERSION)/gzlib.c \
	$(ZLIB_VERSION)/gzread.c \
	$(ZLIB_VERSION)/gzwrite.c \
	$(ZLIB_VERSION)/infback.c \
	$(ZLIB_VERSION)/inffast.c \
	$(ZLIB_VERSION)/inffast.h \
	$(ZLIB_VERSION)/inffixed.h \
	$(ZLIB_VERSION)/inflate.c \
	$(ZLIB_VERSION)/inflate.h \
	$(ZLIB_VERSION)/inftrees.c \
	$(ZLIB_VERSION)/inftrees.h \
	$(ZLIB_VERSION)/trees.c \
	$(ZLIB_VERSION)/trees.h \
	$(ZLIB_VERSION)/uncompr.c \
	$(ZLIB_VERSION)/zconf.h \
	$(ZLIB_VERSION)/zlib.h \
	$(ZLIB_VERSION)/zutil.c \
	$(ZLIB_VERSION)/zutil.h

