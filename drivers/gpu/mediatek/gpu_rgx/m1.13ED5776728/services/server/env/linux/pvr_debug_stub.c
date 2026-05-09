/* SPDX-License-Identifier: GPL-2.0 */
#include <linux/kernel.h>
#include "img_types.h"
#include "pvrsrv_error.h"

typedef void (DUMPDEBUG_PRINTF_FUNC)(void *, const char *, ...);
typedef struct _PVRSRV_RGXDEV_INFO_ PVRSRV_RGXDEV_INFO;

void PVRSRVDebugPrintf(IMG_UINT32, const IMG_CHAR *, IMG_UINT32, const IMG_CHAR *, ...);
void PVRSRVReleasePrintf(const IMG_CHAR *, ...);
int PVRDebugCreateDIEntries(void);
void PVRDebugRemoveDIEntries(void);
PVRSRV_ERROR RGXDebugInit(PVRSRV_RGXDEV_INFO *);
PVRSRV_ERROR RGXDebugDeinit(PVRSRV_RGXDEV_INFO *);
void RGXDumpRGXDebugSummary(DUMPDEBUG_PRINTF_FUNC *, void *, PVRSRV_RGXDEV_INFO *, IMG_BOOL);
PVRSRV_ERROR RGXDumpRGXRegisters(DUMPDEBUG_PRINTF_FUNC *, void *, PVRSRV_RGXDEV_INFO *);
PVRSRV_ERROR HTB_CreateDIEntry(void);
void HTB_DestroyDIEntry(void);

void PVRSRVDebugPrintf(IMG_UINT32 ui32DebugLevel,
                       const IMG_CHAR *pszFileName,
                       IMG_UINT32 ui32Line,
                       const IMG_CHAR *pszFormat, ...) {}

void PVRSRVReleasePrintf(const IMG_CHAR *pszFormat, ...) {}

int PVRDebugCreateDIEntries(void) { return 0; }
void PVRDebugRemoveDIEntries(void) {}

PVRSRV_ERROR RGXDebugInit(PVRSRV_RGXDEV_INFO *psDevInfo)
{ return PVRSRV_OK; }

PVRSRV_ERROR RGXDebugDeinit(PVRSRV_RGXDEV_INFO *psDevInfo)
{ return PVRSRV_OK; }

void RGXDumpRGXDebugSummary(DUMPDEBUG_PRINTF_FUNC *pfnDumpDebugPrintf,
                             void *pvDumpDebugFile,
                             PVRSRV_RGXDEV_INFO *psDevInfo,
                             IMG_BOOL bRGXPoweredON) {}

PVRSRV_ERROR RGXDumpRGXRegisters(DUMPDEBUG_PRINTF_FUNC *pfnDumpDebugPrintf,
                                  void *pvDumpDebugFile,
                                  PVRSRV_RGXDEV_INFO *psDevInfo)
{ return PVRSRV_OK; }

PVRSRV_ERROR HTB_CreateDIEntry(void) { return PVRSRV_OK; }
void HTB_DestroyDIEntry(void) {}
