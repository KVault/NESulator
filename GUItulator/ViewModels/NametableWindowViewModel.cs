using System;
using Avalonia;
using Avalonia.Media.Imaging;
using Avalonia.Platform;
using GUItulator.Utils;

namespace GUItulator.ViewModels
{
    public class NametableWindowViewModel : FPSWindowBase
    {
        private WriteableBitmap[] Nametables {get;}

        public NametableWindowViewModel(Action invalidate, int fps) : base(invalidate, fps)
        {
            Nametables = new WriteableBitmap[4];
            for (var i = 0; i < Nametables.Length; i++)
            {
                Nametables[i] = new WriteableBitmap(new PixelSize(256,240), new Vector(96,96), PixelFormat.Rgba8888);
            }
        }

        protected override void Update()
        {
            for (var i = 0; i < Nametables.Length; i++)
            {
                BitmapUtils.DrawBitmap(CWrapper.Nametable(i).ToIntArray() , ref Nametables[i]);
            }
        }
    }
}
