#include <barelib.h>
#include <fs.h>

extern fsystem_t* fsd;
extern filetable_t oft[NUM_FD];

/*  Modify  the state  of the open  file table to  close  *
 *  the 'fd' index and write the inode back to the block  *
    device.  If the  entry is already closed,  return an  *
 *  error.                                                */
int32 fs_close(int32 fd) {
  // If oft entry is FSTATE_CLOSED, function MUST return -1
  if (oft[fd].state == FSTATE_CLOSED) {
    return -1;
  }
  // Function MUST write oft inode to the ramdisk

  oft[fd].state = FSTATE_CLOSED;
  inode_t inode_block = oft[fd].inode;
  bs_write(inode_block.id, oft[fd].head, &inode_block, sizeof(inode_t));
  return 0;
}
