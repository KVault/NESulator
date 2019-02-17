using System;
using ReactiveUI;

namespace GUItulator.ViewModels
{
    public class HexEditorWindowViewModel : FPSWindowBase
    {
        private HexEditorRowModel[] tenBytesHeaderCount;
        public HexEditorRowModel[] TenBytesHeaderCount
        {
            get {return tenBytesHeaderCount;}
            private set {this.RaiseAndSetIfChanged(ref tenBytesHeaderCount, value);}
        }

        /// <summary>
        /// This defines the row size of the editor. That is how many `word`s will fit into each row.
        /// </summary>
        public static readonly int RowSize = 0x10;

        public HexEditorWindowViewModel(Action onFrameExecuted, int fps = 1) : base(onFrameExecuted, fps){}

        protected override void Update()
        {
            var dump = CWrapper.VRAMDump().ToShortArray();
            PopulateByteRows(ref dump);
        }

        /// <summary>
        /// Creates a new labels list for each 10 bytes
        /// </summary>
        /// <param name="headersCount"></param>
        private void PopulateByteRows(ref short[] buffer)
        {
            var counter = 0;
            var rowsCount = (buffer.Length) / RowSize;
            TenBytesHeaderCount = new HexEditorRowModel[rowsCount];
            var numberOfDigits = buffer.Length.ToString().Length;
            for (var i = 0; i < rowsCount; i++)
            {
                var row = new HexEditorRowModel(counter.ToString("X"));
                for (var j = 0; j < RowSize; j++)
                {
                    row.ByteValues[j] = buffer[(i * RowSize) + j].ToString("X");
                    row.CharValues[j] = buffer[(i * RowSize) + j].ToString("");
                }

                TenBytesHeaderCount[i] = row;
                counter += RowSize;
            }
        }
    }

    public class HexEditorRowModel
    {
        public string Label {get; set;}
        public string[] ByteValues {get; set;}
        public string[] CharValues {get; set;}

        public HexEditorRowModel(string label)
        {
            Label = label;
            ByteValues = new string[HexEditorWindowViewModel.RowSize];
            CharValues = new string[HexEditorWindowViewModel.RowSize];
        }
    }
}
