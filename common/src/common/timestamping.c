#include <common/timestamping.h>

#include <time.h>
#include <sys/time.h>

MicroSecondsTimeStamp
getMicroSecondsTimeStamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * (MicroSecondsTimeStamp) MICROSECONDS_MULTIPLIER) + tv.tv_usec);
}