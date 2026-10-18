/* Included by main.c; deliberately independent of musl getenv/access/stat. */
static const char *selected_data_root;
int avp_ps4_directories(const char **local, const char **global)
{
    avp_boot_log("DIR enter native PS4 initialization");
    if (!selected_data_root || !local || !global) {
        avp_boot_log("FAIL DIR: no validated data root");
        return -1;
    }
    const char *save_root="/data/avp/.avp";
    avp_boot_log("DIR create/check save directory");
    /* mkdir may report EEXIST; independently open the directory to verify it.
     * This never deletes or overwrites existing profiles and saves. */
    int made=sceKernelMkdir(save_root,0777);
    int fd=sceKernelOpen(save_root,O_RDONLY|O_DIRECTORY,0);
    if (fd<0) {
        char message[128];
        snprintf(message,sizeof(message),"FAIL DIR save path: mkdir=0x%x open=0x%x",made,fd);
        avp_boot_log(message);
        return -1;
    }
    sceKernelClose(fd);
    *local=save_root;
    *global=selected_data_root;
    avp_boot_log("DIR save root: /data/avp/.avp");
    avp_boot_log(selected_data_root);
    avp_boot_log("PASS DIR native PS4 initialization");
    return 0;
}
