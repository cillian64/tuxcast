# Distributed under the terms of the GNU General Public License v2
# Ebuild made by cypher_ of freenode's #lugradio

inherit eutils flag-o-matic

DESCRIPTION="The Linux Podcast Client"
HOMEPAGE="http://tuxcast.sourceforge.net"
SRC_URI="mirror://sourceforge/tuxcast/tuxcast-0.2.tgz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="x86 sparc"
DEPEND="net-misc/curl
	dev-libs/libxml2"

src_unpack() {
	unpack ${A}
	cd "${S}"
}

src_compile() {
	emake -j1 || die "emake failed" 
}

src_install() {
	dobin tuxcast/tuxcast 
	dobin config/tuxcast-config
	dodoc docs/01_Introduction docs/02_Installation docs/03_Configuration docs/04_Usage
}

