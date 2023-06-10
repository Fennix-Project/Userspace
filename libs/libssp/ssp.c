#ifndef STACK_CHK_GUARD_VALUE
#if UINTPTR_MAX == UINT32_MAX
#define STACK_CHK_GUARD_VALUE 0xDEAD57AC
#else
#define STACK_CHK_GUARD_VALUE 0xDEAD57AC00000000
#endif
#endif


#ifndef PUBLIC
#define PUBLIC __attribute__((visibility("default")))
#endif // !PUBLIC

#ifndef PRIVATE
#define PRIVATE __attribute__((visibility("hidden")))
#endif // !PRIVATE

PUBLIC __UINTPTR_TYPE__ __stack_chk_guard = 0;

static void __attribute__((constructor, no_stack_protector)) __guard_setup(void)
{
	if (__stack_chk_guard == 0)
		__stack_chk_guard = STACK_CHK_GUARD_VALUE;
}

PUBLIC __attribute__((weak, noreturn, no_stack_protector)) void __stack_chk_fail(void)
{
	const char *msg = "Stack smashing detected";
	__asm__ __volatile__("syscall"
						 :
						 : "a"(0), "D"(0x57AC)
						 : "rcx", "r11", "memory");
	__builtin_unreachable();
}

PUBLIC __attribute__((weak, noreturn, no_stack_protector)) void __chk_fail(void)
{
	const char *msg = "Buffer overflow detected";
	__asm__ __volatile__("syscall"
						 :
						 : "a"(0), "D"(0xF700)
						 : "rcx", "r11", "memory");
	__builtin_unreachable();
}
