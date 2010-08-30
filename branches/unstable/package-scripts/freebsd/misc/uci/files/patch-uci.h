--- uci.h.orig  2009-01-05 15:38:07.000000000 +0100
+++ uci.h       2009-01-05 15:38:36.000000000 +0100
@@ -30,8 +30,8 @@
 #include <setjmp.h>
 #include <stdio.h>

-#define UCI_CONFDIR "/etc/config"
-#define UCI_SAVEDIR "/tmp/.uci"
+#define UCI_CONFDIR "/usr/local/etc/config"
+#define UCI_SAVEDIR "/var/tmp/.uci"
 #define UCI_DIRMODE 0700
 #define UCI_FILEMODE 0600
