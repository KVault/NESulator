using System;
using System.Runtime.InteropServices;

namespace GUItulator
{
    public class CWrapper
    {
        [DllImport("libNESulator.dll", EntryPoint = "last_cpu_speed")]
        public static extern int CPUSpeed();

        [DllImport("libNESulator.dll", EntryPoint = "gui_start_emulation")]
        public static extern void StartEmulation(string o);

        [DllImport("libNESulator.dll", EntryPoint = "gui_stop_emulation")]
        public static extern int StopEmulation();

        [DllImport("libNESulator.dll", EntryPoint = "gui_patterntable_frame")]
        public static extern FrameInfo PatterntableFrame();

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public unsafe struct FrameInfo
        {
            public int size;
            public IntPtr buffer;
        }
    }
}
