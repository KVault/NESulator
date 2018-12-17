using System.Threading;

namespace GUItulator.Utils
{
    /// <summary>
    /// Yes, it's a Singleton :/
    /// </summary>
    public class FPSLoopManager
    {
        private static FPSLoopManager instance;
        public static FPSLoopManager Instance
        {
            get
            {
                return instance ?? (instance = new FPSLoopManager());
            }
        }

        public delegate void FrameTickEvent();

        /// <summary>
        /// Called every frame
        /// </summary>
        public FrameTickEvent OnFrameTick;

        /// <summary>
        /// Keeps the time and ticks the event every frame
        /// </summary>
        private Thread fpsManager;

        /// <summary>
        /// Make it private so that nobody can create an instance of it
        /// </summary>
        private FPSLoopManager()
        {
            fpsManager = new Thread(Loop);
            fpsManager.Start();
        }

        /// <summary>
        /// Loops on forever, sleeping for about 15ms every time and calling OnFrameTick
        /// </summary>
        private void Loop()
        {
            while (instance != null)
            {
                OnFrameTick?.Invoke();
                Thread.Sleep(15);
            }
        }

    }
}
