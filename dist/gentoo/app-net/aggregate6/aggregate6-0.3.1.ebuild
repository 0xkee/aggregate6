# Copyright 2026 Gentoo Authors
# Distributed under the terms of the GNU General Public License v3

EAPI=8

DESCRIPTION="Fast multi-threaded IPv4/IPv6 CIDR prefix aggregation tool"
HOMEPAGE="https://github.com/0xkee/aggregate6"
SRC_URI="https://github.com/0xkee/${PN}/archive/refs/tags/v${PV}.tar.gz -> ${P}.tar.gz"

LICENSE="GPL-3"
SLOT="0"
KEYWORDS="amd64 arm arm64 x86"

src_compile() {
	emake CFLAGS="${CFLAGS}"
}

src_test() {
	emake test
}

src_install() {
	emake install PREFIX=/usr DESTDIR="${D}"
	einstalldocs
}
