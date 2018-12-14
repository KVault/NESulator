using System.Runtime.InteropServices;
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
        public static unsafe void DrawBitmap(int[] rawFrame, ref WriteableBitmap bitmap)
        {
            using (var l = bitmap.Lock())
            {
                var ptr = (uint*)l.Address;
                for (var i = 0; i < l.Size.Width * (l.Size.Height - 1); i++)
                {
                    *(ptr + i) = (uint)rawFrame[i];
                }
            }
        }
    }
}
