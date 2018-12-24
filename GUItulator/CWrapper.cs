using System;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading;

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

        [DllImport("libNESulator.dll", EntryPoint = "gui_left_patterntable")]
        public static extern FrameInfo LeftPatterntable();

        [DllImport("libNESulator.dll", EntryPoint = "gui_right_patterntable")]
        public static extern FrameInfo RightPatterntable();

        [DllImport("libNESulator.dll", EntryPoint = "gui_nametable")]
        public static extern FrameInfo Nametable(int nametableIndex);

        [DllImport("libNESulator.dll", EntryPoint = "gui_ram_dump")]
        public static extern FrameInfo RAMDump();

        [DllImport("libNESulator.dll", EntryPoint = "gui_cpu_speed")]
        public static extern void CPUSpeed(int speedHertz);

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public unsafe struct FrameInfo
        {
            public int size;
            public IntPtr buffer;

            public byte[] ToByteArray()
            {
                var array = new byte[size];
                Marshal.Copy(buffer, array, 0, size);
                return array;
            }

            public uint[] ToUIntArray()
            {
                var array = new int[size];
                Marshal.Copy(buffer, array, 0, size);
                return Array.ConvertAll(array, i => (uint)i);
            }

            public short[] ToShortArray()
            {
                var array = new short[size];
                Marshal.Copy(buffer, array, 0, size);
                return array;
            }
        }
    }
}
