#include "include/log.h"

int
main (int argc, char const *argv[])
{
  log.writeLog (LOG_DEBUG, "Hello World");
  return 0;
}
