# Makefile for tinydtls
#
#
# Copyright (c) 2011, 2012, 2013, 2014, 2015, 2016 Olaf Bergmann (TZI) and others.
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# and Eclipse Distribution License v. 1.0 which accompanies this distribution.
#
# The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html
# and the Eclipse Distribution License is available at 
# http://www.eclipse.org/org/documents/edl-v10.php.
#
# Contributors:
#    Olaf Bergmann  - initial API and implementation
#

# the library's version
VERSION:=0.8.6

# tools

SHELL = /bin/sh
MKDIR = mkdir
ETAGS = /bin/false

prefix = /home/htandat/WorkSpace/WorkSpace1/leaf-data/wp77dev/wp77-legato/build/wp77xx/3rdParty/tinydtls
exec_prefix = ${prefix}
abs_builddir = /home/htandat/WorkSpace/WorkSpace1/_build_mySystem/wp77xx/component/8aa7f2c886d591915ffb58906ba22528
top_builddir = .
libdir = ${exec_prefix}/lib
includedir = ${prefix}/include/tinydtls
package = tinydtls-0.8.6

install := cp

# files and flags
SOURCES:= dtls.c crypto.c ccm.c hmac.c netq.c peer.c dtls_time.c session.c dtls_debug.c
SUB_OBJECTS:=aes/rijndael.o  ecc/ecc.o sha2/sha2.o
OBJECTS:= $(patsubst %.c, %.o, $(SOURCES)) $(SUB_OBJECTS)
HEADERS:=dtls.h hmac.h dtls_debug.h dtls_config.h uthash.h numeric.h crypto.h global.h ccm.h \
 netq.h alert.h utlist.h prng.h peer.h state.h dtls_time.h session.h \
 tinydtls.h
CFLAGS:=-Wall -pedantic -std=c99 -w -std=gnu99 -DSIERRA -DWITH_SHA256 -I/home/htandat/WorkSpace/WorkSpace1/leaf-data/wp77dev/wp77-legato/3rdParty/Lwm2mCore/include/platform-specific/linux -fPIC --sysroot=/home/htandat/WorkSpace/WorkSpace1/leaf-data/wp77dev/wp77-toolchain/sysroots/armv7a-neon-poky-linux-gnueabi -DSIERRA_MDM9X28 -DSIERRA_WP76XX -DPCM_IF=SEC_AUX -DI2S_IF=SEC_MI2S -DUSB_IF=AFE_PCM -DCODEC_IF=PRI_MI2S -DMM_1_ALSA_MIXER_CTRL=MultiMedia1 -DMM_1_ALSA_PCM_DEVICE_ID=0 -DMM_2_ALSA_MIXER_CTRL=MultiMedia2 -DMM_2_ALSA_PCM_DEVICE_ID=13 -DADC_BEGIN_NUM=0 -DADC_COUNT=4 -DPDP_MAX_PROFILE=16 -DPDP_MAX_MULTIPDP=8 -DEUICC -DLWM2M_OBJECT_3_BATTERY -DLWM2M_OBJECT_9 -I /home/htandat/WorkSpace/WorkSpace1/_build_mySystem/wp77xx -I/home/htandat/WorkSpace/WorkSpace1/leaf-data/wp77dev/wp77-legato/interfaces -I/home/htandat/WorkSpace/WorkSpace1/leaf-data/wp77dev/wp77-legato/framework/include -I/home/htandat/WorkSpace/WorkSpace1/leaf-data/wp77dev/wp77-legato/build/wp77xx/framework/include -I/home/htandat/WorkSpace/WorkSpace1 -I/home/htandat/WorkSpace/WorkSpace1/leaf-data/wp77dev/wp77-legato/interfaces/modemServices -I/home/htandat/WorkSpace/WorkSpace1/leaf-data/wp77dev/wp77-legato/interfaces/positioning -I/home/htandat/WorkSpace/WorkSpace1/leaf-data/wp77dev/wp77-legato/interfaces/atServices
CPPFLAGS:=--sysroot=/home/htandat/WorkSpace/WorkSpace1/leaf-data/wp77dev/wp77-toolchain/sysroots/armv7a-neon-poky-linux-gnueabi  -DDTLSv12 -DWITH_SHA256 -DDTLS_CHECK_CONTENTTYPE
SUBDIRS:=doc platform-specific sha2 aes
DISTSUBDIRS:=$(SUBDIRS)
DISTDIR=$(top_builddir)/$(package)
FILES:=Makefile.in configure configure.in dtls_config.h.in tinydtls.h.in \
  Makefile.tinydtls $(SOURCES) $(HEADERS)
LIB:=libtinydtls.a
LDFLAGS:=
ARFLAGS:=cru
doc:=doc

.PHONY: all dirs clean install dist distclean .gitignore doc TAGS

ifneq ("", "1")
.SUFFIXES:
.SUFFIXES:      .c .o

all:	$(LIB) dirs

check:	
	echo DISTDIR: $(DISTDIR)
	echo top_builddir: $(top_builddir)
	$(MAKE) -C tests check

dirs:	$(SUBDIRS)
	for dir in $^; do \
		$(MAKE) -C $$dir ; \
	done

$(SUB_OBJECTS)::
	$(MAKE) -C $(@D) $(@F)

$(LIB):	$(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^ 
	ranlib $@

clean:
	@rm -f $(PROGRAM) main.o $(LIB) $(OBJECTS)
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean ; \
	done
endif # WITH_CONTIKI

doc:	
	$(MAKE) -C doc

distclean:	clean
	@rm -rf $(DISTDIR)
	@rm -f *~ $(DISTDIR).tar.gz

dist:	$(FILES) $(DISTSUBDIRS)
	test -d $(DISTDIR) || mkdir $(DISTDIR)
	cp $(FILES) $(DISTDIR)
	for dir in $(DISTSUBDIRS); do \
		$(MAKE) -C $$dir dist; \
	done
	tar czf $(package).tar.gz $(DISTDIR)

install:	$(LIB) $(HEADERS) $(SUBDIRS)
	test -d $(libdir) || mkdir -p $(libdir)
	test -d $(includedir) || mkdir -p $(includedir)
	$(install) $(LIB) $(libdir)/
	$(install) $(HEADERS) $(includedir)/
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir install="$(install)" includedir=$(includedir) install; \
	done

TAGS:	
	$(ETAGS) -o $@.new $(SOURCES) 
	$(ETAGS) -a -o $@.new $(HEADERS) 
	mv $@.new $@

# files that should be ignored by git
GITIGNOREDS:= core \*~ \*.[oa] \*.gz \*.cap \*.pcap Makefile \
 autom4te.cache/ config.h config.log config.status configure \
 doc/Doxyfile doc/doxygen.out doc/html/ $(LIB) tests/ccm-test \
 tests/dtls-client tests/dtls-server tests/prf-test $(package) \
 $(DISTDIR)/ TAGS \*.patch .gitignore ecc/testecc ecc/testfield \
 \*.d \*.hex \*.elf \*.map obj_\* tinydtls.h dtls_config.h \
 $(addprefix \*., $(notdir $(wildcard ../../platform/*))) \
 .project

.gitignore:
	echo $(GITIGNOREDS) | sed 's/ /\n/g' > $@

