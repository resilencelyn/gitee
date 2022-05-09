using System.Windows.Media;

namespace FCSVisualChart
{
    /// <summary>
    /// 门划分出来的区域
    /// </summary>
    public class GateArea : NotifyPropertyChanged
    {
        private string name;
        /// <summary>
        /// 名称
        /// </summary>
        public string Name
        {
            get { return name; }
            set { name = value; OnPropertyChanged(); }
        }

        private Color displayColor;
        /// <summary>
        /// 显示的颜色，含透明度
        /// </summary>
        public Color DisplayColor { get { return displayColor; } set { displayColor = value; OnPropertyChanged(); } }

        /// <summary>
        /// 所属门
        /// </summary>
        public BaseGate OwnerGate { get; internal set; }

        public GateArea()
        {
            this.DisplayColor = ChartDataSource.RandomColor();
        }

    }
}
