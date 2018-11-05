using System.Runtime.InteropServices;

namespace GUItulator
{
    public class CWrapper
    {
        [DllImport("libNESulator.dll", EntryPoint = "last_cpu_speed")]
        public static extern int CPUSpeed();

        [DllImport("libNESulator.dll", EntryPoint = "gui_start_emulation")]
        public static extern int StartEmulation(string romPath);

    }
}
