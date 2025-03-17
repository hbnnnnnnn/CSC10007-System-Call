#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/sysinfo.h"

void testcall()
{
    struct sysinfo info;
    if (sysinfo(&info) < 0) {
        printf("sysinfotest [TESTCALL()]: sysinfo call test failed\n");
        exit(1);
    }
    printf("sysinfo [TESTCALL()]: freemem = %d, nproc = %d\n", info.freemem, info.nproc);
}

void testmem()
{
    struct sysinfo before, after;
    if (sysinfo(&before) < 0) {
        printf("sysinfotest [TESTMEM()]: sysinfo failed\n");
        exit(1);
    }

    void *mem = sbrk(4096);
    if (mem == (void*)-1) {
        printf("sysinfotest [TESTMEM()]: sbrk failed\n");
        exit(1);
    }
    
    if (sysinfo(&after) < 0) {
        printf("sysinfotest [TESTMEM()]: sysinfo failed\n");
        exit(1);
    }

    printf("sysinfotest [TESTMEM()]: before: %d, after: %d\n", before.freemem, after.freemem);
    if (before.freemem <= after.freemem) {
        printf("sysinfotest [TESTMEM()]: memory allocation test failed\n");
        exit(1);
    }
}

void testproc() {
    struct sysinfo info;
    uint64 initial_nproc;
    int pid, status;

    if (sysinfo(&info) < 0) {
        printf("sysinfotest [TESTPROC()]: sysinfo failed\n");
        exit(1);
    }
    initial_nproc = info.nproc;

    pid = fork();
    if (pid < 0) {
        printf("sysinfotest [TESTPROC()]: fork failed\n");
        exit(1);
    }

    if (pid == 0) { 
        if (sysinfo(&info) < 0) {
            printf("sysinfotest [TESTPROC()]: sysinfo failed in child\n");
            exit(1);
        }
        printf("sysinfotest [TESTPROC()]: (child) nproc = %d, expected = %d\n", info.nproc, initial_nproc + 1);
        if (info.nproc != initial_nproc + 1) {
            printf("sysinfotest [TESTPROC()]: process count test failed\n");
            exit(1);
        }
        exit(0);
    }

    wait(&status);

    sleep(1);
    if (sysinfo(&info) < 0) {
        printf("sysinfotest [TESTPROC()]: sysinfo failed after wait\n");
        exit(1);
    }
    printf("sysinfotest [TESTPROC()]: (parent) nproc = %d, expected = %d\n", info.nproc, initial_nproc);
    if (info.nproc != initial_nproc) {
        printf("sysinfotest [TESTPROC()]: process count test failed\n");
        exit(1);
    }
}

  

int main() {
    printf("sysinfotest: booting\n");
    testcall();
    testmem();
    testproc();
    printf("sysinfotest: OK\n");
    return 0;
}