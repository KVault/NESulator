using Avalonia;
using Avalonia.Controls;
using Avalonia.Media;
using Avalonia.Media.Imaging;
using Avalonia.Platform;
using Avalonia.Visuals.Media.Imaging;

namespace GUItulator.Controls
{
    public class FrameControl : Control
    {
        private WriteableBitmap bitmap =
            new WriteableBitmap(new PixelSize(255, 255), new Vector(96, 96), PixelFormat.Bgra8888);

        public override unsafe void Render(DrawingContext context)
        {
            var color = 0xff000000u;
            using (var l = bitmap.Lock())
            {
                var ptr = (uint*)l.Address;
                for(var x=0; x<l.Size.Width; x++)
                    for (var y = 0; y < l.Size.Height; y++)
                    {
                        color++;
                        ptr[y * l.RowBytes / 4 + x] = (uint)color;
                    }
            }
            context.DrawImage(bitmap, 1, new Rect(bitmap.Size), Bounds, BitmapInterpolationMode.HighQuality);
        }
    }
}
