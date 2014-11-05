#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "hash.h"
#include "dirent.h"
#include "block_pc.h"
#include "block.h"
#include "embext.h"

int main(int argc __attribute__((__unused__)), char *argv[] __attribute__((__unused__))) {
    int p = 0, r;
    int result;
    char buffer[256];
    struct md_context hash_context;
    uint8_t real_hash[16];
    struct stat st;
    struct ext2context *context;
    FILE *fhash;
  
    printf("Running EXT2 tests...\n\n");
    
    /* First pass, start the block driver layer */
    block_pc_set_image_name("testext.img");
    printf("[%4d] %-60s", p++, "start block device emulation...");
    fflush(stdout);
    result = block_init();
    if(result == 0) {
        printf("    pass\n");
    } else {
        printf("    fail\n");
        printf("    Return code %d\n", result);
        exit(1);
    }  
  
    /* Second pass, mount the filesystem */
    printf("[%4d] %-60s", p++, "mount filesystem");
    fflush(stdout);
  
    result = ext2_mount(0, block_get_volume_size(), 0, &context);

    if(result == 0) {
        printf("    pass\n");
    } else {
        printf("    fail\n");
        printf("    Return code %d\n", result);
        exit(1);
    }
  
    /* Try and open the root folder for reading */
    printf("[%4d] %-60s", p++, "open root folder");
    fflush(stdout);
    void *fe = ext2_open(context, "/", O_RDONLY, 0777, &result);
    if(fe) {
        printf("    pass\n");
    } else {
        printf("    fail\n");
        printf("    errno = %d %s\n", result, strerror(result));
        exit(1);
    }
    void *fe2;
    struct dirent *de;
  
    while((de = ext2_readdir(fe, &result))) {
        snprintf(buffer, sizeof(buffer), "/%s", de->d_name);
        if(!(fe2 = ext2_open(context, buffer, O_RDONLY, 0777, &result))) {
            printf("Opening %s failed. [%d]\n", buffer, result);
            exit(1);
        }
    
        if(ext2_fstat(fe2, &st, &result)) {
            printf("Couldn't stat %s. [%d]\n", buffer, result);
            exit(1);
        }
        printf("/%s [%d] %d\n", de->d_name, de->d_ino, (int)st.st_size);
        if((st.st_mode & S_IFDIR) && (strcmp(de->d_name, ".") != 0) &&
            (strcmp(de->d_name, "..") != 0)) {
            while((de = ext2_readdir(fe2, &result))) {
                printf("  %s [%d]\n", de->d_name, de->d_ino);
            }
        }
        ext2_close(fe2, &result);
    }
    ext2_close(fe, &result);
  
    /* Read a binary file and do an MD5 sum to check it was read correctly */
    printf("[%4d] %-60s", p++, "read binary file");
    fflush(stdout);
    
    FILE *fw = fopen("dump.png", "wb");
    if(!(fe = ext2_open(context, "/static/test_image.png", O_RDONLY, 0777, &result))) {
        printf("    fail [%d] %s\n", result, strerror(result));
        exit(1);
    }
  
    md5_start(&hash_context);
    while((r = ext2_read(fe, &buffer, sizeof(buffer), &result)) == sizeof(buffer)) {
        fwrite(buffer, 1, r, fw);
        md5_update(&hash_context, (uint8_t *)buffer, r);
    }
    if(r > 0) {
        fwrite(buffer, 1, r, fw);
        md5_update(&hash_context, (uint8_t *)buffer, r);
    }
    fclose(fw);
    ext2_close(fe, &result);
  
    md5_finish(&hash_context);
    if(!(fhash = fopen("test_image.md5", "rb"))) {
        printf("\nCouldn't open md5 sum of image for verification\n");
        printf("Have you run the latest version of ext_tests.py?\n");
        exit(1);
    }
    if(fread(real_hash, 1, 16, fhash) != 16) {
        printf("Couldn't read hash from hash file.  Can't verify image\n");
        exit(1);
    }
    fclose(fhash);
  
    if(memcmp(hash_context.digest, real_hash, 16)) {
        printf("    fail\n");
        exit(1);
    } else {
        printf("    pass\n");
    }

    /* test appending to a file */
    printf("[%4d] %-60s", p++, "append test");
    fflush(stdout);
  
    fe = ext2_open(context, "/logs/test.txt", O_WRONLY | O_APPEND, 0777, &result);
    if(fe == NULL) {
        printf("    fail\n");
        printf("    Open for writing failed, errno=%d (%s)\r\n", result, strerror(result));
        exit(-1);
    }
  
    r = ext2_write(fe, "Hello world\r\n", 13, &result);
    if(r != 13) {
        printf("    fail\n");
        printf("    writing failed, tried 13 bytes, actually wrote %d\n", r);
        printf("    errno = %d, %s\n", result, strerror(result));
        exit(1);
    }
    ext2_close(fe, &result);
  
    printf("    pass\n");
  
    /* unmount the volume */
    printf("[%4d] %-60s", p++, "unmount volume");
    fflush(stdout);
    ext2_umount(context);
  
    printf("    pass\n");
    
    
    block_pc_snapshot_all("writenfs.img");
  
    block_halt();
  
    exit(0);
}
