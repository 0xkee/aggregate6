# COPR SCM builder — must live at .copr/Makefile in repo root.
# Downloads source tarball from GitHub Releases and builds SRPM.
#
# Usage by COPR: make -f .copr/Makefile srpm outdir=<result_dir>

.PHONY: srpm

SPEC = dist/rpm/aggregate6.spec

srpm:
	curl -L -o aggregate6-$(shell rpmspec -q --queryformat='%{version}' $(SPEC)).tar.gz \
		$(shell rpmspec -q --queryformat='%{source}' $(SPEC) | head -1)
	rpmbuild -bs \
		--define "_sourcedir $(CURDIR)" \
		--define "_srcrpmdir $(outdir)" \
		$(SPEC)
