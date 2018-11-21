using System;
using System.IO;
using System.Runtime.InteropServices;
using Avalonia.Interactivity;
using ReactiveUI;
using Serilog;
using Bitmap = Avalonia.Media.Imaging.Bitmap;

namespace GUItulator.ViewModels
{
    public class PatterntableWindowViewModel : ViewModelBase
    {
        private Bitmap patterntable;
        public Bitmap Patterntable
        {
            get {return patterntable;}
            set {this.RaiseAndSetIfChanged(ref patterntable, value);}
        }

        public void DrawDummyBitmap(object sender, RoutedEventArgs args)
        {
            try
            {
                var frameInfo = CWrapper.PatterntableFrame();
                var backBuffer = new byte[frameInfo.size];
                Marshal.Copy(frameInfo.buffer, backBuffer, 0, frameInfo.size);
            }
            catch (Exception e)
            {
                Log.Error(e, e.Message);
            }

        }
    }
}
