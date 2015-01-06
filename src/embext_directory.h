#ifndef EMBEXT_DIRECTORY_H
#define EMBEXT_DIRECTORY_H 1

struct ext2_dir_header {
    uint32_t inode;
    uint16_t rec_len;
    uint8_t name_len;
    uint8_t file_type;
};

int ext2_append_to_directory(struct ext2context *context, char *directory, uint32_t inode,
                             char *filename, int *rerrno);
int ext2_delete_from_directory(struct ext2context *context, char *filename, int *rerrno);

#endif /* ifndef EMBEXT_DIRECTORY_H */
