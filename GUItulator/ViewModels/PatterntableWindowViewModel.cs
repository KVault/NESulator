using System;
using Avalonia;
using Avalonia.Media.Imaging;
using Avalonia.Platform;
using GUItulator.Utils;

namespace GUItulator.ViewModels
{
    public class PatterntableWindowViewModel : FPSWindowBase
    {
        private WriteableBitmap LeftPatterntable {get;}
        private WriteableBitmap RightPatterntable {get;}

        public PatterntableWindowViewModel(Action invalidate, int fps) : base(invalidate, fps)
        {
            LeftPatterntable = new WriteableBitmap(new PixelSize(128,128), new Vector(96,96), PixelFormat.Rgba8888);
            RightPatterntable = new WriteableBitmap(new PixelSize(128,128), new Vector(96,96), PixelFormat.Rgba8888);
        }

        /// <summary>
        /// Main update function for the window. It renders both patterntables side by side
        /// </summary>
        protected override void Update()
        {
            //temp variables used because the ref parameter doesn't like it otherwise
            var leftPatterntable = LeftPatterntable;
            var rightPatterntable = RightPatterntable;
            BitmapUtils.DrawBitmap(CWrapper.LeftPatterntable().ToIntArray(), ref leftPatterntable);
            BitmapUtils.DrawBitmap(CWrapper.RightPatterntable().ToIntArray(), ref rightPatterntable);
        }
    }
}
