#include "ps4gl-display.h"
#include "ps4gl-frame.h"
#include "ps4gl-sdl.h"
#include "ps4libc-threading.h"
#include "vk_ps4_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <orbis/libkernel.h>
#include "data-path.h"
#include "bootlog.h"

static int boot_fd=-1;
void avp_boot_log(const char *message) {
    if (boot_fd<0) return;
    /* Kernel writes bypass the SDK's unreliable stdout/stderr redirection. */
    sceKernelWrite(boot_fd,message,strlen(message));
    sceKernelWrite(boot_fd,"\n",1);
}
static int readable_data(const char *path) {
    int fd=sceKernelOpen(path,O_RDONLY,0);
    if (fd<0) return 0;
    unsigned char byte;
    int readable=sceKernelRead(fd,&byte,1)==1;
    sceKernelClose(fd);
    return readable;
}
#include "directories.inc"

static PS4GLDisplayOwner display;
static PS4GLFrameOwner frames;
static int begin(void *p) {
    if (frames.frames<3) avp_boot_log("FRAME begin");
    int rc=ps4glFrameBegin(p);
    if (rc) avp_boot_log("FAIL frame begin");
    return rc;
}
static int present(void *p) {
    int rc=ps4glFramePresent(p);
    if (rc) avp_boot_log("FAIL frame present");
    else if (frames.frames<=3) avp_boot_log("PASS frame presented");
    return rc;
}
static int quiesce(void *p) {
    if (frames.state && !frames.failed && ps4glFramePresent(p)) return -1;
    return ps4glFrameQuiesce(p);
}
extern int __real_main(int, char **);
int __wrap_main(int argc, char **argv) {
    (void)argc; (void)argv;
    mkdir("/data/avp", 0777);
    boot_fd=sceKernelOpen("/data/avp/bootstrap-v04.log",O_WRONLY|O_CREAT|O_TRUNC,0666);
    avp_boot_log("AVP 0.4 bootstrap: entry");
    ps4EnableLibcLocks();
    avp_boot_log("PASS allocator locks");
    freopen("/data/avp/stdout.log", "w", stdout);
    freopen("/data/avp/stderr.log", "w", stderr);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    puts("AVP PS4 experimental - original game copyright Rebellion 1999-2000");
    setenv("HOME", "/data/avp", 1);
    const char *data=avp_select_data(readable_data);
    if (!data) {
        avp_boot_log("STOP: no complete data root. Use /data/avp/gamedata or /mnt/usbN/avp/gamedata");
        return 5;
    }
    avp_boot_log(data);
    selected_data_root=data;
    setenv("AVP_DATA", data, 1);
    printf("AVP game data: %s\n", data);
    vk_ps4_log_open("/data/avp/driver.log");
    avp_boot_log("START display init");
    if (ps4glDisplayInit(&display)) { avp_boot_log("FAIL display init"); return 2; }
    avp_boot_log("PASS display init; START frame init");
    if (ps4glFrameInit(&frames, display.device, display.queue, display.swap,
            display.pass, display.images, display.targets, display.width, display.height)) {
        avp_boot_log("FAIL frame init"); return 3;
    }
    avp_boot_log("PASS frame init");
    memcpy(frames.depth_images, display.depth_images, sizeof(frames.depth_images));
    PS4GLSDLOwner owner = {.physical=display.physical, .device=display.device,
        .pass=display.pass, .width=display.width, .height=display.height,
        .depth_stencil=1, .begin=begin, .present=present, .quiesce=quiesce, .data=&frames};
    avp_boot_log("START SDL owner");
    if (ps4glSDLSetOwner(&owner)) { avp_boot_log("FAIL SDL owner"); return 4; }
    avp_boot_log("PASS SDL owner; START engine main");
    char *args[] = {"avp", "-w", "-c", NULL};
    int result=__real_main(3, args);
    avp_boot_log("RETURN engine main");
    if (!ps4glSDLSetOwner(NULL)) {
        if (!ps4glFrameDestroy(&frames)) ps4glDisplayDestroy(&display);
    }
    return result;
}
