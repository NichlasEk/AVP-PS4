#ifndef AVP_BOOTLOG_H
#define AVP_BOOTLOG_H
#ifdef __ORBIS__
void avp_boot_log(const char *message);
#define AVP_BOOT(message) avp_boot_log(message)
#else
#define AVP_BOOT(message) ((void)0)
#endif
#endif
