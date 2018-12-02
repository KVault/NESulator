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
        public WriteableBitmap LeftPatterntable {get;}
        public WriteableBitmap RightPatterntable {get;}
        private readonly Action invalidate;
        private Task refreshTask;
        public bool FramesRunning {get; set;}

        public PatterntableWindowViewModel(Action invalidate)
        {
            this.invalidate = invalidate;
            LeftPatterntable = new WriteableBitmap(new PixelSize(128,128), new Vector(96,96), PixelFormat.Rgba8888);
            RightPatterntable = new WriteableBitmap(new PixelSize(128,128), new Vector(96,96), PixelFormat.Rgba8888);
            refreshTask = new Task(Update);
            refreshTask.Start();
        }

        /// <summary>
        /// Main update function for the window. It renders both patterntables side by side
        /// </summary>
        private void Update()
        {
            try
            {
                if (!FramesRunning) return;

                //temp variables used because the ref parameter doesn't like it otherwise
                var leftPatterntable = LeftPatterntable;
                var rightPatterntable = RightPatterntable;
                DrawPatterntable(CWrapper.LeftPatterntable(), ref leftPatterntable);
                DrawPatterntable(CWrapper.RightPatterntable(), ref rightPatterntable);
                ScheduleNextFrame();
            }
            catch (Exception e)
            {
                Log.Error(e, e.Message);
            }
        }

        /// <summary>
        /// Uses the frameInfo to retrieve the raw data. It uses it then to render it into the passed bitmap
        /// </summary>
        /// <param name="frameInfo"></param>
        /// <param name="patterntable"></param>
        private unsafe void DrawPatterntable(CWrapper.FrameInfo frameInfo, ref WriteableBitmap patterntable)
        {
            var rawFrame = new int[frameInfo.size];
            Marshal.Copy(frameInfo.buffer, rawFrame, 0, frameInfo.size);

            using (var l = patterntable.Lock())
            {
                var ptr = (uint*)l.Address;
                for (var i = 0; i < l.Size.Width * (l.Size.Height - 1); i++)
                {
                    *(ptr + i) = (uint)rawFrame[i];
                }
            }
        }


        private void ScheduleNextFrame()
        {
            refreshTask.Wait(15); //15 mills. 60FPS
            refreshTask = new Task(Update);
            refreshTask.Start();
            invalidate?.Invoke();
        }
    }
}
