#include "file_type.h"

#include <string.h>
#include <strings.h>

LIBMTP_filetype_t filetype_from_name(const char *name) {
  const char *dot = strrchr(name, '.');
  if (dot == NULL || dot[1] == '\0') {
    return LIBMTP_FILETYPE_UNKNOWN;
  }

  const char *ext = dot + 1;
  if (strcasecmp(ext, "jpg") == 0 || strcasecmp(ext, "jpeg") == 0) {
    return LIBMTP_FILETYPE_JPEG;
  }
  if (strcasecmp(ext, "png") == 0) {
    return LIBMTP_FILETYPE_PNG;
  }
  if (strcasecmp(ext, "gif") == 0) {
    return LIBMTP_FILETYPE_GIF;
  }
  if (strcasecmp(ext, "bmp") == 0) {
    return LIBMTP_FILETYPE_BMP;
  }
  if (strcasecmp(ext, "txt") == 0 || strcasecmp(ext, "log") == 0 || strcasecmp(ext, "json") == 0) {
    return LIBMTP_FILETYPE_TEXT;
  }
  if (strcasecmp(ext, "xml") == 0) {
    return LIBMTP_FILETYPE_XML;
  }
  if (strcasecmp(ext, "mp3") == 0) {
    return LIBMTP_FILETYPE_MP3;
  }
  if (strcasecmp(ext, "mp4") == 0) {
    return LIBMTP_FILETYPE_MP4;
  }
  return LIBMTP_FILETYPE_UNKNOWN;
}
