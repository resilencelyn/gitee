using System;
using System.Collections;
using System.Collections.Generic;
using System.Threading;
using System.Windows.Media;

namespace FCSVisualChart
{
    public class ChartDataSource : NotifyPropertyChanged
    {
        #region 数据
        private IList xSource;
        /// <summary>
        /// x轴数据源
        /// </summary>
        public IList XSource { get { return xSource; } set { xSource = value; OnPropertyChanged("XSource"); } }

        private IList ySource;
        /// <summary>
        /// y轴数据源
        /// </summary>
        public IList YSource { get { return ySource; } set { ySource = value; OnPropertyChanged("YSource"); } }

        private IList<int> indexs;
        /// <summary>
        /// 有效数据的索引
        /// </summary>
        public IList<int> Indexs { get { return indexs; } set { indexs = value; OnPropertyChanged("Indexs"); } }

        private IList<int> displayIndexs;
        /// <summary>
        /// 显示数据的索引，整体数据不一定全部显示
        /// </summary>
        public IList<int> DisplayIndexs { get { return displayIndexs == null ? indexs : displayIndexs; } set { displayIndexs = value; OnPropertyChanged("DisplayIndexs"); } }

        private ObservableDictionary<GateArea, IList<int>> areaIndexs = new ObservableDictionary<GateArea, IList<int>>();
        /// <summary>
        /// 门划分的区域集合
        /// </summary>
        public ObservableDictionary<GateArea, IList<int>> AreaIndexs { get { return areaIndexs; } set { areaIndexs = value; OnPropertyChanged("AreaIndexs"); } }

        private Func<object, double> xSourceConverter = DefaultConverter;
        /// <summary>
        /// x值转换方法
        /// </summary>
        public Func<object, double> XSourceConverter { get { return xSourceConverter; } set { xSourceConverter = value; OnPropertyChanged("XSourceConverter"); } }

        private Func<object, double> ySourceConverter = DefaultConverter;
        /// <summary>
        /// y值转换方法
        /// </summary>
        public Func<object, double> YSourceConverter { get { return ySourceConverter; } set { ySourceConverter = value; OnPropertyChanged("YSourceConverter"); } }

        private static double DefaultConverter(object o)
        {
            return Convert.ToDouble(o);
        }
        #endregion

        #region 外观
        private Color displayColor;
        /// <summary>
        /// 显示的颜色，含透明度
        /// </summary>
        public Color DisplayColor { get { return displayColor; } set { displayColor = value; OnPropertyChanged("DisplayColor"); } }

        static readonly Random random = new Random();
        /// <summary>
        /// 随机一个新颜色值
        /// </summary>
        /// <returns></returns>
        public static Color RandomColor(byte a = 0xff)
        {
            byte r = Convert.ToByte(random.Next(255));
            byte g = Convert.ToByte(random.Next(255));
            byte b = Convert.ToByte(random.Next(255));
            return new Color() { A = a, R = r, G = g, B = b };
        }
        #endregion

        #region 性能
        internal CancellationTokenSource CancelTokenSource { get; set; }
        internal Dictionary<GateArea, CancellationTokenSource> GateAreaCancelTokenSource { get; } = new Dictionary<GateArea, CancellationTokenSource>();
        /// <summary>
        /// 是否正在绘制图形
        /// </summary>
        public bool IsDrawing { get { return CancelTokenSource != null && !CancelTokenSource.IsCancellationRequested; } }
        #endregion

        public ChartDataSource()
        {
            DisplayColor = RandomColor();
        }
    }
}
