using System;
using Avalonia;
using Avalonia.Direct2D1.Media;
using Avalonia.Media.Imaging;
using Avalonia.Platform;
using GUItulator.Utils;
using ReactiveUI;

namespace GUItulator.ViewModels
{
    public class PatterntableWindowViewModel : FPSWindowBase
    {
        private WriteableBitmap LeftPatterntable {get;}
        private Size LeftSize {get;}
        private WriteableBitmap RightPatterntable {get;}
        private Size RightSize {get;}

        private WriteableBitmap[] palettes;
        public WriteableBitmap[] Palettes
        {
            get {return palettes;}
            private set {this.RaiseAndSetIfChanged(ref palettes, value);}
        }
        private Size PaletteSize {get;}

        public PatterntableWindowViewModel(Action invalidate, int fps) : base(invalidate, fps)
        {
            LeftPatterntable = new WriteableBitmap(new PixelSize(256,256),
                                                   new Vector(96,96), PixelFormat.Rgba8888);
            LeftSize = LeftPatterntable.Size;
            RightPatterntable = new WriteableBitmap(new PixelSize(256,256),
                                                   new Vector(96,96), PixelFormat.Rgba8888);
            RightSize = RightPatterntable.Size;

            Palettes = new WriteableBitmap[32];
            for (var i = 0; i < Palettes.Length; i++)
            {
                Palettes[i] = new WriteableBitmap(new PixelSize(32, 32),
                                                  new Vector(96,96), PixelFormat.Rgba8888);
            }

            PaletteSize = Palettes[0].Size;//Just take one for the size, it doesn't matter
        }

        /// <summary>
        /// Main update function for the window. It renders both patterntables side by side
        /// </summary>
        protected override void Update()
        {
            //temp variables used because the ref parameter doesn't like it otherwise
            var leftPatterntable = LeftPatterntable;
            var rightPatterntable = RightPatterntable;
            BitmapUtils.DrawBitmap(CWrapper.LeftPatterntable().ToResolution(LeftSize.Width, LeftSize.Height),
                                   ref leftPatterntable, LeftSize);
            BitmapUtils.DrawBitmap(CWrapper.RightPatterntable().ToResolution(RightSize.Width, RightSize.Height),
                                   ref rightPatterntable, RightSize);
            var colours = CWrapper.PaletteDump().ToIntArray();

            for (var i = 0; i < Palettes.Length; i++)
            {
                var bmp = Palettes[i];
                BitmapUtils.DrawBitmap((uint)colours[i], ref bmp, PaletteSize);
            }
        }
    }
}
