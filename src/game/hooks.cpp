// Hooks for default.xbe. A strong definition replaces the recompiled function at that address, for example:
//
// #include "xdk/hle.h"
//
// X86_EXTERN_FUNC(__imp__sub_00012345);
// X86_FUNC(sub_00012345) {
//     __imp__sub_00012345(ctx, base);  // the original
// }

#include "xdk/hle.h"

// The XDK USB stack maps the host controller at 0xFED00000, which the runtime does not emulate. Input comes
// through the runtime's own path, so skip host controller enumeration. HCD_EnumHardware is cdecl with no
// arguments, so the stub pops only the return address.
X86_FUNC(GUEST(HCD_EnumHardware)) {
    ctx.esp += 4;
}

