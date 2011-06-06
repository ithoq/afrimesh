# New ports collection makefile for:    uci
# Date created:                         February 25 2009
# Whom:                                 antoine@7degrees.co.za
#
# $FreeBSD$
#

PORTNAME=       uci
PORTVERSION=    0.7.5
CATEGORIES=     misc
MASTER_SITES=   http://downloads.openwrt.org/sources/

MAINTAINER=     antoine@7degrees.co.za
COMMENT=        UCI is an Unified Configuration Interface

USE_LDCONFIG=   yes
USE_GMAKE=      yes

do-install:
	${INSTALL_PROGRAM} ${WRKSRC}/uci ${PREFIX}/bin
	${INSTALL} ${WRKSRC}/uci.h ${PREFIX}/include
	${INSTALL} ${WRKSRC}/uci_config.h ${PREFIX}/include
	${INSTALL} ${WRKSRC}/libuci.a ${PREFIX}/lib
	${INSTALL} ${WRKSRC}/libuci.so ${PREFIX}/lib
	${INSTALL} ${WRKSRC}/libuci.so.0.7 ${PREFIX}/lib
	${MKDIR}   ${PREFIX}/etc/config

.include <bsd.port.mk>
