#ifndef __FENNIX_LIBS_SYS_PROC_H__
#define __FENNIX_LIBS_SYS_PROC_H__

#include <stddef.h>

enum ProcessState
{
    PROCESS_STATE_READY,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_SLEEPING,
    PROCESS_STATE_WAITING,
    PROCESS_STATE_STOPPED,
    PROCESS_STATE_TERMINATED
};

typedef struct
{
    char Name[256];
    unsigned long ID;
    enum ProcessState State;
    void *KernelPrivate;
} Process;

typedef struct
{
    char Name[256];
    unsigned long ID;
    enum ProcessState State;
    void *KernelPrivate;
} Thread;

/**
 * @brief Create a new process from a path
 *
 * @param Path Path to the executable
 * @return Process* Pointer to the process structure
 */
Process *Spawn(const char *Path);

/**
 * @brief Create a new thread
 * 
 * @param EntryPoint Entry point of the thread
 * @return Thread* Pointer to the thread structure
 */
Thread *SpawnThread(uintptr_t EntryPoint);

/**
 * @brief Get list of threads
 * 
 * @param Process Process to get the threads from
 * @return Thread** Pointer to the thread list (NULL terminated)
 */
Thread **GetThreadList(Process *Process);

/**
 * @brief Get process by ID
 * 
 * @param ID Process ID
 * @return Process* Pointer to the process structure
 */
Process *GetProcessByID(unsigned long ID);

/**
 * @brief Get thread by ID
 * 
 * @param ID Thread ID
 * @return Thread* Pointer to the thread structure
 */
Thread *GetThreadByID(unsigned long ID);

/**
 * @brief Get current process
 * 
 * @return Process* Pointer to the process structure
 */
Process *GetCurrentProcess();

/**
 * @brief Get current thread
 * 
 * @return Thread* Pointer to the thread structure
 */
Thread *GetCurrentThread();

/**
 * @brief [SYSTEM] Create a new empty process
 *
 * @param KernelPrivate Process parent
 * @param Name Process name
 * @param TrustLevel Process trust level [RESERVED FOR TRUSTED PROCESSES]
 * @param Image Process file image already loaded in memory
 * @return Process* Pointer to the process structure
 */
Process *KrnlCreateProcess(void *KernelPrivate,
                           const char *Name,
                           long TrustLevel,
                           void *Image);

/**
 * @brief [SYSTEM] Create a new thread
 * 
 * @param KernelPrivate Process parent
 * @param EntryPoint Entry point of the thread
 * @param argv Arguments
 * @param envp Environment variables
 * @param auxv Auxiliary variables
 * @param Offset Offset of the entry point
 * @param Architecture Architecture of the thread
 * @param Compatibility Compatibility of the thread
 * @return Thread* Pointer to the thread structure
 */
Thread *KrnlCreateThread(void *KernelPrivate,
                         unsigned long EntryPoint,
                         const char **argv,
                         const char **envp,
                         void *auxv,
                         unsigned long Offset,
                         long Architecture,
                         long Compatibility);

#endif // !__FENNIX_LIBS_SYS_PROC_H__
