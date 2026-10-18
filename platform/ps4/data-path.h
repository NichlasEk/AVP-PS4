#ifndef AVP_DATA_PATH_H
#define AVP_DATA_PATH_H
#include <stdio.h>
/* A root must contain core files, not merely an empty directory. Never merge
 * partially copied HDD data with USB files from a different game installation. */
static const char *avp_select_data(int (*readable)(const char *))
{
    static const char *roots[] = {
        "/data/avp/gamedata",
        "/mnt/usb0/avp/gamedata", "/mnt/usb1/avp/gamedata",
        "/mnt/usb2/avp/gamedata", "/mnt/usb3/avp/gamedata",
        "/mnt/usb4/avp/gamedata", "/mnt/usb5/avp/gamedata",
        "/mnt/usb6/avp/gamedata", "/mnt/usb7/avp/gamedata"
    };
    static const char *required[] = {
        "avp_huds/alien.rif", "avp_huds/alien_hud.rif",
        "avp_rifs/temple.rif", "fastfile/ffinfo.txt", "language.txt"
    };
    char path[256];
    for (unsigned r=0; r<sizeof(roots)/sizeof(*roots); ++r) {
        unsigned i;
        for (i=0; i<sizeof(required)/sizeof(*required); ++i) {
            snprintf(path,sizeof(path),"%s/%s",roots[r],required[i]);
            if (!readable(path)) break;
        }
        if (i==sizeof(required)/sizeof(*required)) return roots[r];
    }
    return NULL;
}
#endif
