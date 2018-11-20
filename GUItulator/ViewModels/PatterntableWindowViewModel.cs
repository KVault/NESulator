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
        private string catPath = @"C:\Users\jossanta\Desktop\Cat.jpg";

        public void DrawDummyBitmap(object sender, RoutedEventArgs args)
        {
            try
            {
                var size = CWrapper.PatterntableFrame(out var buffer);
                var backBuffer = new byte[size];
                Marshal.Copy(buffer, backBuffer, 0, size);
            }
            catch (Exception e)
            {
                Log.Error(e, e.Message);
            }

        }
    }
}
