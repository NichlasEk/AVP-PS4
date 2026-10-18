#include <assert.h>
#include <string.h>
#include "data-path.h"
static unsigned available;
static int partial_hdd;
static int readable(const char *path) {
    if (!strncmp(path,"/data/avp/gamedata/",18))
        return (available&1) && !(partial_hdd && strstr(path,"language.txt"));
    for (unsigned i=0;i<8;i++) {
        char root[64]; snprintf(root,sizeof(root),"/mnt/usb%u/avp/gamedata/",i);
        if (!strncmp(path,root,strlen(root))) return (available & (2u<<i))!=0;
    }
    return 0;
}
int main(void) {
    assert(!avp_select_data(readable));
    available=1; assert(!strcmp(avp_select_data(readable),"/data/avp/gamedata"));
    available=511; assert(!strcmp(avp_select_data(readable),"/data/avp/gamedata"));
    partial_hdd=1; assert(!strcmp(avp_select_data(readable),"/mnt/usb0/avp/gamedata"));
    for (unsigned i=0;i<8;i++) {
        char expected[64]; snprintf(expected,sizeof(expected),"/mnt/usb%u/avp/gamedata",i);
        available=2u<<i; assert(!strcmp(avp_select_data(readable),expected));
    }
    available=1; assert(!avp_select_data(readable));
    puts("PASS data selection: HDD priority, USB0..7, incomplete HDD fallback, missing data");
}
