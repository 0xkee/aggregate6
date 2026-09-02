include $(TOPDIR)/rules.mk

PKG_NAME:=aggregate6
PKG_VERSION:=0.2.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/0xkee/aggregate6/releases/download/v$(PKG_VERSION)
PKG_HASH:=1faaa6f4124cb5ec6f252cbddf9df996a5957ad4e8f8beb614098db1fbeb685f

PKG_LICENSE:=GPL-3.0-or-later
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/aggregate6
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Fast CIDR prefix aggregation tool
  URL:=https://github.com/0xkee/aggregate6
  DEPENDS:=+libc
endef

define Package/aggregate6/description
  Reads IPv4/IPv6 prefixes from stdin, aggregates them,
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
