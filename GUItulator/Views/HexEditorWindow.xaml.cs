using Avalonia.Controls;
using Avalonia.Markup.Xaml;
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

            var rootPanel = (StackPanel)Content;
            var labelsList = rootPanel.FindControl<ListBox>("Labels");
            var byteValues = rootPanel.FindControl<ListBox>("ByteValues");
            labelsList. += (sender, args) => byteValues.Scroll.Offset = labelsList.Scroll.Offset;
            byteValues.PointerWheelChanged += (sender, args) => labelsList.Scroll.Offset = byteValues.Scroll.Offset;

            viewModel = new HexEditorWindowViewModel(() => {}, 30);
            DataContext = viewModel;
            viewModel.Start();
        }
    }
}
