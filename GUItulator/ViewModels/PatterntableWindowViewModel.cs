using System;
using System.ComponentModel;
using System.Runtime.InteropServices;
using Avalonia;
using Avalonia.Interactivity;
using Avalonia.Media.Imaging;
using Avalonia.Platform;
using ReactiveUI;
using Serilog;

namespace GUItulator.ViewModels
{
    public class PatterntableWindowViewModel
    {
        public WriteableBitmap Patterntable { get;}
        private readonly Action invalidate;

        public PatterntableWindowViewModel(Action invalidate)
        {
            this.invalidate = invalidate;
            Patterntable = new WriteableBitmap(new PixelSize(512,240), new Vector(96,96), PixelFormat.Rgba8888);
        }

        public unsafe void DrawDummyBitmap(object sender, RoutedEventArgs args)
        {
            try
            {
                var frameInfo = CWrapper.PatterntableFrame();
                byte[] rawFrame = new byte[frameInfo.size];
                Marshal.Copy(frameInfo.buffer, rawFrame, 0, frameInfo.size);

                var pixelCounter = 0;
                uint color = 0xff000000u;

                using (var l = Patterntable.Lock())
                {
                    var ptr = (uint*)l.Address;
                    for(var x=0; x<l.Size.Width; x++)
                        for (var y = 0; y < l.Size.Height; y++)
                        {
                            ptr[y * l.RowBytes / 4 + x] = (uint)GetPixel(ref rawFrame, pixelCounter);
                            pixelCounter += 4;
                        }
                }
            }
            catch (Exception e)
            {
                Log.Error(e, e.Message);
            }

        }

        private uint GetPixel(ref byte[] backBuffer, int pos)
        {
            var pixelColor = (uint)((0xff << 23)
                                     + (backBuffer[pos]<< 16)
                                     + (backBuffer[pos + 1]<< 8)
                                     + backBuffer[pos + 2]);
            return pixelColor;
        }
    }
}
