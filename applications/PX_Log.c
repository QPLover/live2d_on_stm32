#include "PX_Typedef.h"
#include "rtthread.h"

#define DBG_TAG "PainterEngine"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

void PX_ASSERT(void)
{
#ifdef PX_DEBUG_MODE
	while(1) *(char *)(0)=1;
#endif
}

void PX_ERROR(px_char fmt[])
{
	LOG_D(fmt);
}


px_char * PX_GETLOG(void)
{
	return "";
}

void PX_LOG(px_char fmt[])
{
    LOG_D(fmt);
}
