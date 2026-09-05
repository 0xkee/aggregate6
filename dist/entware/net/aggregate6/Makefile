include $(TOPDIR)/rules.mk

PKG_NAME:=aggregate6
PKG_VERSION:=0.3.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/0xkee/aggregate6/releases/download/v$(PKG_VERSION)
PKG_HASH:=c6b3b172891a734ba7d4236c00938c7f08ae0e65488085cce233cbfdc416af34

PKG_LICENSE:=GPL-3.0-only
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/aggregate6
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Fast multi-threaded IPv4/IPv6 CIDR prefix aggregation tool
  URL:=https://github.com/0xkee/aggregate6
  DEPENDS:=+libc
endef

define Package/aggregate6/description
  Reads bulk IPv4/IPv6 prefix lists from stdin or files,
  aggregates them using a multi-threaded Patricia trie,
  outputs minimal covering set. Written in C99 with zero
  external dependencies.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/aggregate6/install
	$(INSTALL_DIR) $(1)/opt/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/aggregate6 $(1)/opt/bin/
endef

$(eval $(call BuildPackage,aggregate6))
