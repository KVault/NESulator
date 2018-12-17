using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using Avalonia.Threading;
using GUItulator.ViewModels;

namespace GUItulator.Views
{
    public class HexEditorWindow : Window
    {
        private HexEditorWindowViewModel viewModel;

        public HexEditorWindow()
        {
            InitializeComponent();
            Closed += (e, args) => viewModel.Stop();
        }

        private void InitializeComponent()
        {
            AvaloniaXamlLoader.Load(this);

            viewModel = new HexEditorWindowViewModel(() => {}, 30);
            DataContext = viewModel;
            viewModel.Start();
        }
    }
}
