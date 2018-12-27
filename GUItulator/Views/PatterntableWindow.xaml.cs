using System.Linq;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using Avalonia.Media.Imaging;
using Avalonia.Threading;
using Avalonia.VisualTree;
using GUItulator.ViewModels;

namespace GUItulator.Views
{
    public class PatterntableWindow : Window
    {
        private PatterntableWindowViewModel viewModel;
        private Image leftPatterntableControl;
        private Image rightPatterntableControl;


        public PatterntableWindow()
        {
            InitializeComponent();
            Closed += (e, args) => viewModel.Stop();
#if DEBUG
            this.AttachDevTools();
#endif
        }

        private void InitializeComponent()
        {
            AvaloniaXamlLoader.Load(this);
            var rootPanel = (StackPanel)Content;

            leftPatterntableControl = rootPanel.FindControl<Image>("LeftPatterntable");
            rightPatterntableControl = rootPanel.FindControl<Image>("RightPatterntable");

            viewModel = new PatterntableWindowViewModel(() =>
                        Dispatcher.UIThread.InvokeAsync(InvalidateControls).Wait(), 5);
            viewModel.Start();
            DataContext = viewModel;
        }

        /// <summary>
        /// Invalidates all the images in the window. The two patterntables are very straightforward, the palette is
        /// a bit trickier. We have to access the actual control through the Visual children of the list to be able
        /// to retrieve an Image instead of a WriteableBitmap
        /// </summary>
        private void InvalidateControls()
        {
            leftPatterntableControl.InvalidateVisual();
            rightPatterntableControl.InvalidateVisual();

            var paletteControl= ((StackPanel)Content).FindControl<ItemsControl>("PaletteItems");
            if (paletteControl.ItemCount > 0)
            {
                foreach (var itemContainerInfo in paletteControl.ItemContainerGenerator.Containers)
                {
                    var image = itemContainerInfo.ContainerControl.VisualChildren.FirstOrDefault() as Image;
                    image?.InvalidateVisual();
                }
            }
        }

    }
}
