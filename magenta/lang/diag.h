
#ifndef _magenta_lang_diag_
#define _magenta_lang_diag_

#include "github.scottt.debugbreak/debugbreak.h"

///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _WIN32
inline int IsDebuggerPresent(void)
{
    int fd = open("/proc/self/status", O_RDONLY);
    if (fd == -1)
        return 0;

    char buf[1024];
    ssize_t num_read = read(fd, buf, sizeof(buf)-1);
    if (!num_read)
		return 0;

	static const char tag[] = "TracerPid:";
	buf[num_read] = '\0';
	char *pid = strstr(buf, tag);
	if (pid)
		return !!atoi(pid + sizeof(tag) - 1);

    return 0;
}
#endif

#define BB if (IsDebuggerPresent()) debug_break();

///////////////////////////////////////////////////////////////////////////////////////////////

#endif // _magenta_lang_diag_
