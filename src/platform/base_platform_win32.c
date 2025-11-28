#define WIN32_LEAN_AND_MEAN
#include <windows.h>

Seconds platform_get_performance_time() {
	LARGE_INTEGER li_counter = {};
	QueryPerformanceCounter(&li_counter);
	LARGE_INTEGER li_freq = {};
	QueryPerformanceFrequency(&li_freq);
	Seconds seconds = { li_counter.QuadPart / (f64) li_freq.QuadPart };
	return seconds;
}

Seconds platform_get_process_times() {
	FILETIME ft_dummy;
	FILETIME ft_kernel;
	FILETIME ft_user;
	GetProcessTimes(GetCurrentProcess(), &ft_dummy, &ft_dummy, &ft_kernel, &ft_user);
	u64 t_kernel = U64FromU32Packed(ft_kernel.dwHighDateTime, ft_kernel.dwLowDateTime);
	u64 t_user   = U64FromU32Packed(ft_user.dwHighDateTime, ft_user.dwLowDateTime);
	Seconds seconds = { (t_kernel + t_user) * 100 * NANOSECONDS_TO_SECONDS };
	return seconds;
}

void platform_sleep_seconds(Seconds seconds) {
	Sleep((u32) (1000 * seconds.seconds));
}
