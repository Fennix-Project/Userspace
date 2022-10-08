// EXPERIMENTAL

enum FexFormatType
{
    FexFormatType_Unknown,
    FexFormatType_Executable,
    FexFormatType_Driver
    /* ... */
};

enum FexOSType
{
    FexOSType_Unknown,
    FexOSType_Fennix,
    FexOSType_Linux
    /* ... */
};

struct Fex
{
    char Magic[4];
    int Type;
    int OS;
    unsigned long Pointer;
};

extern "C" int _start();

__attribute__((section(".header")))
Fex header = {
    .Magic = {'F', 'E', 'X', '\0'},
    .Type = FexFormatType_Executable,
    .OS = FexOSType_Fennix,
    .Pointer = (unsigned long)_start};

extern "C" int _start()
{
    return 0;
}
