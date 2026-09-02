PORTNAME=	aggregate6
DISTVERSION=	0.1.0
CATEGORIES=	net

MAINTAINER=	0xkee@users.noreply.github.com
COMMENT=	Fast CIDR prefix aggregation tool
WWW=		https://github.com/0xkee/aggregate6

LICENSE=	GPLv3+
LICENSE_FILE=	${WRKSRC}/LICENSE

MASTER_SITES=	https://github.com/0xkee/aggregate6/releases/download/v${DISTVERSION}/
DISTFILES=	aggregate6-${DISTVERSION}${EXTRACT_SUFX}

USES=		gmake
ALL_TARGET=	all
INSTALL_TARGET=	install

MAKE_ARGS=	PREFIX=${PREFIX} CC=${CC}

PLIST_FILES=	bin/aggregate6

.include <bsd.port.mk>
