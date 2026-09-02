# Copyright 2025 Gentoo Authors
# Distributed under the terms of the GNU General Public License v3+

EAPI=8

inherit git-r3

DESCRIPTION="Fast CIDR prefix aggregation tool"
HOMEPAGE="https://github.com/0xkee/aggregate6"
EGIT_REPO_URI="https://github.com/0xkee/aggregate6.git"

LICENSE="GPL-3+"
SLOT="0"
KEYWORDS="~amd64 ~arm ~arm64 ~x86"

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
