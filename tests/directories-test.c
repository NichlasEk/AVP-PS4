#define _GNU_SOURCE
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
static int mkdir_result, open_result, mkdir_calls, open_calls, close_calls;
static const char *last_message;
static void avp_boot_log(const char *message) {
    /* Only retain constant strings; the error-format buffer is transient. */
    if (strncmp(message,"FAIL DIR save path:",19)) last_message=message;
}
static int sceKernelMkdir(const char *path, unsigned mode) {
    assert(!strcmp(path,"/data/avp/.avp") && mode==0777);
    ++mkdir_calls; return mkdir_result;
}
static int sceKernelOpen(const char *path, int flags, unsigned mode) {
    assert(!strcmp(path,"/data/avp/.avp"));
    assert(flags==(O_RDONLY|O_DIRECTORY) && !mode);
    ++open_calls; return open_result;
}
static int sceKernelClose(int fd) { assert(fd==open_result); ++close_calls; return 0; }
#include "directories.inc"
int main(void) {
    const char *local=NULL, *global=NULL;
    assert(avp_ps4_directories(&local,&global)==-1);
    assert(!mkdir_calls && !open_calls);
    selected_data_root="/data/avp/gamedata";
    assert(avp_ps4_directories(NULL,&global)==-1);
    assert(avp_ps4_directories(&local,NULL)==-1);
    open_result=4;
    assert(!avp_ps4_directories(&local,&global));
    assert(!strcmp(local,"/data/avp/.avp") && global==selected_data_root);
    assert(!strcmp(last_message,"PASS DIR native PS4 initialization"));
    assert(mkdir_calls==1 && open_calls==1 && close_calls==1);
    /* Existing saves: failed mkdir is fine when the existing directory opens. */
    mkdir_result=-17;
    selected_data_root="/mnt/usb3/avp/gamedata";
    assert(!avp_ps4_directories(&local,&global));
    assert(global==selected_data_root && !strcmp(local,"/data/avp/.avp"));
    const char *previous=global;
    open_result=-13;
    assert(avp_ps4_directories(&local,&global)==-1);
    assert(global==previous && close_calls==2);
    puts("PASS native directories: HDD/USB roots, fixed HDD saves, existing saves, failed open, invalid arguments");
}
