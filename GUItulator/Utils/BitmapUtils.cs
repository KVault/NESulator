using System;
using Avalonia;
using Avalonia.Media.Imaging;
using Avalonia.Platform;
using InterpolationMode = System.Drawing.Drawing2D.InterpolationMode;

namespace GUItulator.Utils
{
    public static class BitmapUtils
    {
        /// <summary>
        /// Uses the frameInfo to retrieve the raw data. It uses it then to render it into the passed bitmap
        /// </summary>
        /// <param name="rawFrame"></param>
        /// <param name="bitmap"></param>
        /// <param name="size"></param>
        /// <param name="antialiassedScale"></param>
        public static unsafe void DrawBitmap(int[] rawFrame, ref WriteableBitmap bitmap, Size size)
        {

            var bitmapSequentialSize = size.Width * (size.Height - 1);
            using (var l = bitmap.Lock())
            {
                var ptr = (uint*)l.Address;
                for (var i = 0; i < bitmapSequentialSize; i++, ptr++)
                {
                    *ptr = (uint)rawFrame[i];
                }
            }
        }

        /// <summary>
        /// More efficient version (unless the data has been copied to managed memory already) that does essentially
        /// a memcpy from the unmanaged code. All the sizes here have to be in bytes, but the method accepts the
        /// size in `int` size. so a multiplication to translate it to bytes is necessary
        /// </summary>
        /// <param name="rawFrame"></param>
        /// <param name="bitmap"></param>
        /// <param name="size"></param>
        public static unsafe void DrawBitmap(CWrapper.FrameInfo rawFrame, ref WriteableBitmap bitmap, Size size)
        {
            var bitmapSequentialSize = (long)(size.Width * size.Height * 4);//The 4 to get the size in bytes
            using (var ptr = bitmap.Lock())
            {
                Buffer.MemoryCopy((void *)rawFrame.buffer, (void *)ptr.Address,
                                  bitmapSequentialSize, rawFrame.size * 4);
            }
        }

        /// <summary>
        /// Fills in the given bitmap with a solid colour passed in as a parameter.
        /// </summary>
        /// <param name="solidColour"></param>
        /// <param name="bitmap"></param>
        /// <param name="size"></param>
        public static unsafe void DrawBitmap(uint solidColour, ref WriteableBitmap bitmap, Size size)
        {
            var bitmapSequentialSize = size.Width * (size.Height - 1);
            using (var l = bitmap.Lock())
            {
                var ptr = (uint*)l.Address;
                for (var i = 0; i < bitmapSequentialSize; i++, ptr++)
                {
                    *ptr = solidColour;
                }
            }
        }

        public static unsafe Bitmap GetBitmap(CWrapper.FrameInfo rawFrame, Size size)
        {
            var bitmapSequentialSize = (long)(size.Width * size.Height * 4);//The 4 to get the size in bytes
            var bmp = new WriteableBitmap(new PixelSize(128,128),
                                                   new Vector(96,96), PixelFormat.Rgba8888);
            using (var ptr = bmp.Lock())
            {
                Buffer.MemoryCopy((void *)rawFrame.buffer, (void *)ptr.Address, bitmapSequentialSize, rawFrame.size * 4);
            }

            return bmp;
        }
    }
}
