#include "test.h"

#include "../src/file_type.h"

void file_type_tests(void) {
  ASSERT_EQ_INT(LIBMTP_FILETYPE_JPEG, filetype_from_name("photo.JPG"));
  ASSERT_EQ_INT(LIBMTP_FILETYPE_PNG, filetype_from_name("photo.png"));
  ASSERT_EQ_INT(LIBMTP_FILETYPE_TEXT, filetype_from_name("debug.log"));
  ASSERT_EQ_INT(LIBMTP_FILETYPE_XML, filetype_from_name("manifest.xml"));
  ASSERT_EQ_INT(LIBMTP_FILETYPE_MP3, filetype_from_name("song.mp3"));
  ASSERT_EQ_INT(LIBMTP_FILETYPE_UNKNOWN, filetype_from_name("activity.fit"));
  ASSERT_EQ_INT(LIBMTP_FILETYPE_UNKNOWN, filetype_from_name("APP.PRG"));
  ASSERT_EQ_INT(LIBMTP_FILETYPE_UNKNOWN, filetype_from_name("filename-without-extension"));
}
