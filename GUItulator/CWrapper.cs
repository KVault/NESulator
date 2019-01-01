using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using Avalonia;
using GUItulator.Utils;
using Bitmap = System.Drawing.Bitmap;

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

        [DllImport("libNESulator.dll", EntryPoint = "gui_palette_dump")]
        public static extern FrameInfo PaletteDump();

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public unsafe struct FrameInfo
        {
            public int size;
            public IntPtr buffer;
            public int width;
            public int height;

            public byte[] ToByteArray()
            {
                var array = new byte[size];
                Marshal.Copy(buffer, array, 0, size);
                return array;
            }

            public int[] ToIntArray()
            {
                var array = new int[size];
                Marshal.Copy(buffer, array, 0, size);
                return array;
            }

            public short[] ToShortArray()
            {
                var array = new short[size];
                Marshal.Copy(buffer, array, 0, size);
                return array;
            }

            public int[] ToResolution(double destWidth, double destHeight)
            {
                return ToResolution((int)destWidth, (int)destHeight);
            }
            public int[] ToResolution(int destWidth, int destHeight)
            {
                var oldImage = ToIntArray();
                var newImage = new int[destWidth * destHeight];
                var xRatio = ((width << 16) / destWidth) + 1;
                var yRatio = ((height << 16)/ destHeight) + 1;

                for (var i = 0; i < destHeight; i++)
                {
                    for (var j = 0; j < destWidth; j++)
                    {
                        var xVal = (j * xRatio) >> 16;
                        var yVal = (i * yRatio) >> 16;
                        newImage[(i * destWidth) + j] = oldImage[((yVal * width) + xVal)];
                    }
                }

                return newImage;
            }
        }
    }
}
