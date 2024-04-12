#include <barelib.h>
#include <fs.h>

extern fsystem_t* fsd;
extern filetable_t oft[NUM_FD];

/*  Search for a filename  in the directory, if the file doesn't exist  *
 *  or it is  already  open, return  an error.   Otherwise find a free  *
 *  slot in the open file table and initialize it to the corresponding  *
 *  inode in the root directory.                                        *
 *  'head' is initialized to the start of the file.                     */
int32 fs_open(char* filename) {
  //  If directory entry with filename does not exist, function MUST return -1
    int dir_entry_index = -1;
    for (int i = 0; i < fsd->root_dir.numentries; i++) {
        int match = 1;
        for (int j = 0; j < FILENAME_LEN; j++) {
            if ((fsd->root_dir.entry[i].name[j] != filename[j]) ||
                (fsd->root_dir.entry[i].name[j] == '\0' && filename[j] != '\0') ||
                (fsd->root_dir.entry[i].name[j] != '\0' && filename[j] == '\0') ){
                match = 0;
                break;
            }
            if (fsd->root_dir.entry[i].name[j] == '\0' && filename[j] == '\0') {
                break;
            }
        }
        if (match) {
            dir_entry_index = i;
            break;
        }
    }
    if (dir_entry_index == -1) {
        return -1;
    }
    //  If file exists in the oft, function MUST return -1
    for (int i = 0; i < NUM_FD; i++) {
        if (oft[i].state == FSTATE_OPEN && oft[i].direntry == dir_entry_index) {
            return -1;
        }
    }
    int fd_slot = -1;
    for (int i = 0; i < NUM_FD; i++) {
        if (oft[i].state == FSTATE_CLOSED) {
            fd_slot = i;
            break;
        }
    }
    if (fd_slot == -1) {
        return -1; 
    }

    oft[fd_slot].state = FSTATE_OPEN;
    oft[fd_slot].head = 0;
    oft[fd_slot].direntry = dir_entry_index;

    return fd_slot;
}
