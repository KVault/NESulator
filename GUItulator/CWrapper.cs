using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using Avalonia;
using GUItulator.Utils;
using Bitmap = System.Drawing.Bitmap;
using Size = Avalonia.Size;

namespace GUItulator
{
    public class CWrapper
    {
        [DllImport("libNESulator.dylib", EntryPoint = "last_cpu_speed")]
        public static extern int CPUSpeed();

        [DllImport("libNESulator.dylib", EntryPoint = "gui_start_emulation")]
        public static extern void StartEmulation(string o);

        [DllImport("libNESulator.dylib", EntryPoint = "gui_stop_emulation")]
        public static extern int StopEmulation();

        [DllImport("libNESulator.dylib", EntryPoint = "gui_frame")]
        public static extern FrameInfo BackBuffer();

        [DllImport("libNESulator.dylib", EntryPoint = "gui_left_patterntable")]
        public static extern FrameInfo LeftPatterntable();

        [DllImport("libNESulator.dylib", EntryPoint = "gui_right_patterntable")]
        public static extern FrameInfo RightPatterntable();

        [DllImport("libNESulator.dylib", EntryPoint = "gui_nametable")]
        public static extern FrameInfo Nametable(int nametableIndex);

        [DllImport("libNESulator.dylib", EntryPoint = "gui_ram_dump")]
        public static extern FrameInfo RAMDump();

        [DllImport("libNESulator.dylib", EntryPoint = "gui_vram_dump")]
        public static extern FrameInfo VRAMDump();

        [DllImport("libNESulator.dylib", EntryPoint = "gui_cpu_speed")]
        public static extern void CPUSpeed(int speedHertz);

        [DllImport("libNESulator.dylib", EntryPoint = "gui_palette_dump")]
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

            public int[] Resize(Size size)
            {
                return Resize(size.Width, size.Height);
            }

            public int[] Resize(double destWidth, double destHeight)
            {
                return Resize((int)destWidth, (int)destHeight);
            }
            public int[] Resize(int destWidth, int destHeight)
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
