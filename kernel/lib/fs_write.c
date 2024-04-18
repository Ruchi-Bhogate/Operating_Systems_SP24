#include <barelib.h>
#include <fs.h>
static uint32 allocate_new_block() {
  for (int block_index = 2; block_index < fsd->device.nblocks; block_index++) {
      if (!fs_getmaskbit(block_index)) {
          fs_setmaskbit(block_index);
          return block_index;
      }
  }
  return -1;
}
/* fs_write - Takes a file descriptor index into the 'oft', a  pointer to a  *
 *            buffer  that the  function reads data  from and the number of  *
 *            bytes to copy from the buffer to the file.                     *
 *                                                                           *
 *            'fs_write' reads data from the 'buff' and copies it into the   *
 *            file  'blocks' starting  at the 'head'.  The  function  will   *
 *            allocate new blocks from the block device as needed to write   *
 *            data to the file and assign them to the file's inode.          *
 *                                                                           *
 *  returns - 'fs_write' should return the number of bytes written to the    *
 *            file.                                                          */
uint32 fs_write(uint32 fd, char* buff, uint32 len) {
  filetable_t* file = &oft[fd];
  inode_t* inode = &file->inode;
  int head = file->head;
  int bytes_written = 0;
  int block_size = fsd->device.blocksz;
  int start_block_index = head / block_size;
  int start_block_offset = head % block_size;
  while (len > 0) {
      int block_index = inode->blocks[start_block_index];
      if (head%block_size ==0) {
          block_index = allocate_new_block();
          if (block_index == -1) {
              return -1;
          }
          inode->blocks[start_block_index] = block_index;
      }
      int bytes_to_write = block_size - start_block_offset;
      if (bytes_to_write > len) {
          bytes_to_write = len;
      }
      bs_write(block_index, start_block_offset, buff, bytes_to_write);
      bytes_written += bytes_to_write;
      buff += bytes_to_write;
      len -= bytes_to_write;
      head += bytes_to_write;
      start_block_index += 1;
      start_block_offset = 0;
  }
  if (head > inode->size) {
      inode->size = head;
  }
  file->head = head;
  bs_write(inode->id, 0, inode, sizeof(inode_t));
  return bytes_written;
}
