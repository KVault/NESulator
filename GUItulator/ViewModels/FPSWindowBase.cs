using System;
using System.Threading.Tasks;
using Serilog;

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
        private int waitBetweenFrames;
        private Task refreshTask;

        /// <summary>
        /// </summary>
        /// <param name="onFrameExecuted">Callback every time a frame completes</param>
        /// <param name="fps">Times per second that the window refreshes</param>
        protected FPSWindowBase(Action onFrameExecuted, int fps = 30)
        {
            invalidate = onFrameExecuted;
            waitBetweenFrames = 1000 / fps;
        }

        public void StartLoop()
        {
            isRunning = true;
            refreshTask = new Task(MainUpdateLoop);
            refreshTask.Start();
        }

        public void StopLoop()
        {
            isRunning = false;
        }

        /// <summary>
        /// Actual update loop to be called by each different class
        /// </summary>
        protected abstract void Update();


        private void MainUpdateLoop()
        {
            try
            {
                if (isRunning)
                {
                    ScheduleNextFrame();
                    Update();
                }
            }
            catch (Exception e)
            {
                Log.Error(e, e.Message);
            }
        }

        private void ScheduleNextFrame()
        {
            invalidate?.Invoke();
            refreshTask.Wait(waitBetweenFrames);
            refreshTask = new Task(MainUpdateLoop);
            refreshTask.Start();
        }
    }
}
