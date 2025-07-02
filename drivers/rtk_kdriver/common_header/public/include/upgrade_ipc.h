#ifndef __UPGRADE_IPC_H__
#define __UPGRADE_IPC_H__
#include <rbus/stb_reg.h>

#define UPGRADE_STATUS_reg      (STB_SW_DATA12_reg)

enum upgrade_shm_magic {
    UPGRADE_QUIESCENT_MAGIC = 0x901e2ce7,
    UPGRADE_KEYPAD_MAGIC,
    UPGRADE_AUTO_MAGIC,
};

#endif  /* __UPGRADE_IPC_H__ */
