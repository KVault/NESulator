using System;
using GUItulator.Utils;

namespace GUItulator.ViewModels
{
    /// <summary>
    /// Base class that implement a basic 60FPS loop. All the extending class has to do is to implement the Update
    /// function that will be called each frame
    /// </summary>
    public abstract class FPSWindowBase : ViewModelBase
    {
        private Action invalidate;
        private bool isRunning;

        /// <summary>
        /// This represents how many times it has to tick per second
        /// </summary>
        private int fps;

        /// <summary>
        /// FPSLoopManager ticks 60fps, this variable will hold how many ticks it has to wait before actually doing
        /// an update.
        /// </summary>
        private int fpsWaitInterval;

        /// <summary>
        /// Simply used to keep track of how many frames this window has skipped
        /// </summary>
        private int fpsWaitCounter;

        /// <summary>
        /// </summary>
        /// <param name="onFrameExecuted">Callback every time a frame completes</param>
        /// <param name="fps">Times per second that the window refreshes</param>
        protected FPSWindowBase(Action onFrameExecuted, int fps = 30)
        {
            invalidate = onFrameExecuted;
            this.fps = fps;
            fpsWaitInterval = 60 / this.fps;
        }

        public void Start()
        {
            FPSLoopManager.Instance.OnFrameTick += MainUpdateLoop;
        }

        public void Stop()
        {
            FPSLoopManager.Instance.OnFrameTick += MainUpdateLoop;
        }

        /// <summary>
        /// Actual update loop to be called by each different class
        /// </summary>
        protected abstract void Update();


        private void MainUpdateLoop()
        {
            if (fpsWaitCounter < fpsWaitInterval) //Just keep waiting
            {
                fpsWaitCounter++;
            }
            else //Now we can run the loop
            {
                Update();
                fpsWaitCounter = 0;
                invalidate?.Invoke();
            }
        }
    }
}
