using System;
using Avalonia;
using Avalonia.Media.Imaging;

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
        public static unsafe void DrawBitmap(uint[] rawFrame, ref WriteableBitmap bitmap, Size size)
        {
            var bitmapSequentialSize = size.Width * (size.Height - 1);
            using (var l = bitmap.Lock())
            {
                var ptr = (uint*)l.Address;
                for (var i = 0; i < bitmapSequentialSize; i++, ptr++)
                {
                    *ptr = rawFrame[i];
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
            using (var l = bitmap.Lock())
            {
                Buffer.MemoryCopy((void *)rawFrame.buffer, (void *)l.Address, bitmapSequentialSize, rawFrame.size * 4);
            }
        }
    }
}
