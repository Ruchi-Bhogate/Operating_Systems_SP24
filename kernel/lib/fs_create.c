#include <barelib.h>
#include <fs.h>

extern fsystem_t* fsd;

/*  Search for 'filename' in the root directory.  If the  *
 *  file exists,  returns an error.  Otherwise, create a  *
 *  new file  entry in the  root directory, a llocate an  *
 *  unused  block in the block  device and  assign it to  *
 *  the new file.                                         */
int32 fs_create(char* filename) {
    //Function MUST find an unused directory entry
    //If no entry is free, function MUST return -1
    //If entry exists with the same filename, function MUST return -1
    for (int i = 0; i < fsd->root_dir.numentries; i++) {
        const char* temp = fsd->root_dir.entry[i].name;
        const char* file_name = filename;
        while (*temp != '\0' && *file_name != '\0' && *temp == *file_name) {
            temp++;
            file_name++;
        }
        if (*temp == '\0' && *file_name == '\0') {
            return -1;
        }
    }
    if (fsd->root_dir.numentries >= DIR_SIZE) {
        return -1;
    }

    //Function MUST find a free block for file inode
    //Function MUST fill directory entry with new file metadata

    int free_block = -1;
    for (int i = 0; i < fsd->device.nblocks; i++) {
        if (fs_getmaskbit(i) == 0) { 
            free_block = i;
            break;
        }
    }
    if (free_block == -1) {
        return -1; 
    }

    fsd->root_dir.entry[fsd->root_dir.numentries].inode_block = free_block;
    for (int i = 0; i < FILENAME_LEN; i++) {
        if (filename[i] == '\0') {
            fsd->root_dir.entry[fsd->root_dir.numentries].name[i] = '\0';
            break;
        }
        fsd->root_dir.entry[fsd->root_dir.numentries].name[i] = filename[i];
    }
    if (filename[FILENAME_LEN - 1] != '\0') {
        fsd->root_dir.entry[fsd->root_dir.numentries].name[FILENAME_LEN - 1] = '\0';
    }
    fsd->root_dir.numentries++;
    fs_setmaskbit(free_block);

    // Function MUST write inode to ramdisk
    //Function MUST write modified freemask to ramdisk

    inode_t new_inode;
    new_inode.size = 0;
    new_inode.id = free_block;
    bs_write(free_block, 0, &new_inode, sizeof(inode_t));
    // Write the modified freemask to the ramdisk (block device)
    bs_write(SB_BIT, 0, fsd, sizeof(fsystem_t)); 
    bs_write(BM_BIT, 0, fsd->freemask, fsd->freemasksz);
    return 0; // Successfully created the new file
}
