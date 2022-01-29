#include <libpad.h>
#include <tamtypes.h>

/// cosmito
static char padBuf[256] __attribute__((aligned(64)));
static char actAlign[6];
static int actuators;

int initializePad(int port, int slot);
int padUtils_ReadButton(int port, int slot, u32 old_pad, u32 new_pad);
int padUtils_ReadButtonWait(int port, int slot, u32 old_pad, u32 new_pad);
int waitPadReady(int port, int slot);


