#include "stdio.h"

#include "include/tlog.h"


int main() {
    TLogInit();
    TLog("Hello World! %d\n", 1);
    TLogClose();
    return 0;
}