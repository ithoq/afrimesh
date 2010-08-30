include $(TOPDIR)/rules.mk

PKG_NAME:=tinyscheme
PKG_VERSION:=1.39
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)$(PKG_VERSION).zip
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=55523524236ba546fdaf5f6b96a49a2b

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/tinyscheme
  SECTION:=lang
  CATEGORY:=Languages
	TITLE:=A minimal implementation of scheme
  URL:=http://tinyscheme.sourceforge.net
endef

define Package/tinyscheme/description
 TinyScheme is a lightweight Scheme interpreter that implements
 as large a subset of R5RS as was possible without getting very
 large and complicated. It is meant to be used as an embedded
 scripting interpreter for other programs. As such, it does not
 offer IDEs or extensive toolkits although it does sport a small
 top-level loop, included conditionally. As an embedded interpreter,
 it allows multiple interpreter states to coexist in the same
 program, without any interference between them. Programmatically,
 foreign functions in C can be added and values can be defined in
 the Scheme environment. Being a quite small program, it is easy
 to comprehend, get to grips with, and use.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC) $(TARGET_CFLAGS)" \
		FEATURES="-DUSE_DL=1 -DUSE_PLIST=1 \
							-DInitFile=\\\"/usr/share/tinyscheme/init.scm\\\" \
							-DExtDir=\\\"/usr/lib/tinyscheme\\\"" \
		SYS_LIBS="-ldl -lm" \
		scheme
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include/tinyscheme
	$(CP) $(PKG_BUILD_DIR)/*.h $(1)/usr/include/tinyscheme
endef

define Package/tinyscheme/install
	$(INSTALL_DIR)  $(1)/usr/bin
	$(INSTALL_DIR)  $(1)/usr/share/tinyscheme
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/scheme   $(1)/usr/bin
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/init.scm $(1)/usr/share/tinyscheme
endef

$(eval $(call BuildPackage,tinyscheme))
