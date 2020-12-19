#ifndef UNISTD_WIN_H_
#define UNISTD_WIN_H_

// based on https://stackoverflow.com/a/826027/1202830

#include <windows.h>

#define usleep(x) Sleep(x / 1000)

#endif // UNISTD_WIN_H_