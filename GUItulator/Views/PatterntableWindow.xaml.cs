using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using GUItulator.ViewModels;

namespace GUItulator.Views
{
    public class PatterntableWindow : Window
    {
        private PatterntableWindowViewModel viewModel;
        public PatterntableWindow()
        {
            InitializeComponent();
#if DEBUG
            this.AttachDevTools();
#endif
        }

        private void InitializeComponent()
        {
            AvaloniaXamlLoader.Load(this);
            viewModel = new PatterntableWindowViewModel();
            DataContext = viewModel;
            GotFocus += viewModel.DrawDummyBitmap;
        }
    }
}
