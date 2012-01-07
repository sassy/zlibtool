#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 4096

int main(int argc, char **argv) {
  int status = Z_OK;
  FILE *out, *in;
  char *in_buf;
  char *out_buf;
  int flush, read_size, out_size;
  z_stream strm;

  if (argc != 3) {
    printf("argument error!\n");
    return -1;
  }

  if (!(in = fopen(argv[1], "rb"))) {
    printf("file error!\n");
    return -1;
  }

  if (!(out = fopen(argv[2], "wb"))) {
    printf("file error!\n");
    fclose(in);
    return -1;
  }

  if ((in_buf = (char *)malloc(BUF_SIZE)) == NULL) {
    fclose(out);
    fclose(in);
    return -1;
  }

  if ((out_buf = (char *)malloc(BUF_SIZE)) == NULL) {
    free(out_buf);
    fclose(out);
    fclose(in);
    return -1;
  }

  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  if (deflateInit(&strm, Z_DEFAULT_COMPRESSION) != Z_OK) {
    free(out_buf);
    free(in_buf);
    fclose(out);
    fclose(in);
    return -1;
  }

  strm.avail_in = 0;
  strm.next_out = out_buf;
  strm.avail_out = BUF_SIZE;
  strm.next_in == in_buf;
  read_size = fread(in_buf, sizeof(char), BUF_SIZE, in);
  if (read_size < BUF_SIZE) {
    flush = Z_FINISH;
  } else  {
    flush = Z_NO_FLUSH;
  }

  /* compress process */
  for (;;) {
    status = deflate(&strm, flush);
    if (status == Z_STREAM_END) {
      out_size = BUF_SIZE - strm.avail_out;
      if (out_size > 0) {
	fwrite(out_buf, sizeof(char), out_size, out);
      }
      break;
    } else if (status != Z_OK) {
      free(out_buf);
      free(in_buf);
      fclose(out);
      fclose(in);
      return -1;
    }
    if (strm.avail_out == 0) {
      fwrite(out_buf, sizeof(char), BUF_SIZE, out);
      strm.next_out = out_buf;
      strm.avail_out = BUF_SIZE;
    }
    if (strm.avail_in == 0) {
      strm.next_in = in_buf;
       read_size = fread(in_buf, sizeof(char), BUF_SIZE, in);
       if (read_size < BUF_SIZE) {
	 flush = Z_FINISH;
       }
    }
  }

  deflateEnd(&strm);

  free(out_buf);
  free(in_buf);
  fclose(out);
  fclose(in);

  return 0;
}
