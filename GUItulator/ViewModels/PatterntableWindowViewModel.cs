using System;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
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
        private Task refreshTask;
        public bool FramesRunning {get; set;}

        public PatterntableWindowViewModel(Action invalidate)
        {
            this.invalidate = invalidate;
            Patterntable = new WriteableBitmap(new PixelSize(512,240), new Vector(96,96), PixelFormat.Rgba8888);
            refreshTask = new Task(DrawPatterntable);
            refreshTask.Start();
        }

        public unsafe void DrawPatterntable()
        {
            try
            {
                ScheduleNextFrame();
                if (!FramesRunning) return;
                var frameInfo = CWrapper.PatterntableFrame();
                var rawFrame = new int[frameInfo.size];
                Marshal.Copy(frameInfo.buffer, rawFrame, 0, frameInfo.size);

                using (var l = Patterntable.Lock())
                {
                    var ptr = (uint*)l.Address;

                    for (var i = 0; i < l.Size.Width * (l.Size.Height - 1); i++)
                    {
                        *(ptr + i) = (uint)rawFrame[i];
                    }
                }
            }
            catch (Exception e)
            {
                Log.Error(e, e.Message);
            }
        }

        private void ScheduleNextFrame()
        {
            refreshTask.Wait(15); //15 mills. 60FPS
            refreshTask = new Task(DrawPatterntable);
            refreshTask.Start();
            invalidate?.Invoke();
        }
    }
}
