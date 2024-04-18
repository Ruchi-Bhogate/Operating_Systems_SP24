#include <barelib.h>
#include <fs.h>
extern filetable_t oft[NUM_FD];
extern fsystem_t* fsd;
/* fs_read - Takes a file descriptor index into the 'oft', a  pointer to a  *
 *           buffer that the function writes data to and a number of bytes  *
 *           to read.                                                       *
 *                                                                          *
 *           'fs_read' reads data starting at the open file's 'head' until  *
 *           it has  copied either 'len' bytes  from the file's  blocks or  *
 *           the 'head' reaches the end of the file.                        *
 *                                                                          *
 * returns - 'fs_read' should return the number of bytes read (either 'len' *
 *           or the  number of bytes  remaining in the file,  whichever is  *
 *           smaller).                                                      */
uint32 fs_read(uint32 fd, char* buff, uint32 len) {
  filetable_t* file = &oft[fd];
  inode_t* inode = &file->inode;
  int head = file->head;
  int bytes_read = 0;
  int block_size = fsd->device.blocksz;
  int start_block_index = head / block_size;
  int start_block_offset = head % block_size;
  int remaining_bytes = inode->size - head;
  int bytes_to_read = (len < remaining_bytes) ? len : remaining_bytes;
  while (bytes_to_read > 0) {
      int block_index = inode->blocks[start_block_index];
      int block_offset = start_block_offset;
      int read_amount = block_size - block_offset;
      if (read_amount > bytes_to_read) {
          read_amount = bytes_to_read;
      }
      bs_read(block_index, block_offset, buff, read_amount);
      bytes_read += read_amount;
      buff += read_amount;
      bytes_to_read -= read_amount;
      head += read_amount;
      start_block_index += 1;
      start_block_offset = 0;
  }
  file->head = head;
  return bytes_read;
}
