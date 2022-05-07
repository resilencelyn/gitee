using System.Windows;
using System.Windows.Media;
using System.Xml.Serialization;

namespace FCSVisualChart
{
    public abstract class IAxis : FrameworkElement
    {
        #region 绘制内容
        protected DrawingVisual DV { get; } = new DrawingVisual();
        protected DrawingVisual GainDV { get; } = new DrawingVisual();
        protected override int VisualChildrenCount => 2;
        protected override Visual GetVisualChild(int index)
        {
            if (index == 0) return DV;
            else return GainDV;
        }
        #endregion

        public IAxis()
        {
            this.AddVisualChild(DV);
            this.AddVisualChild(GainDV);
            this.Loaded += (sender, e) =>
            {
                Draw();
                DrawGain();
                if (this.OwnerChart != null)
                {
                    if (this.OwnerChart.Series != null) this.OwnerChart.Series.Draw();
                    if (this.OwnerChart.Gates != null) foreach (var gate in this.OwnerChart.Gates) gate.AxisChanged();
                }
            };
            this.MouseLeftButtonDown += (sender, e) =>
            {
                if (this.OwnerChart == null || (AxisType == AxisType.X && !this.OwnerChart.GainXCanChange) || (AxisType == AxisType.Y && !this.OwnerChart.GainYCanChange) || this.OwnerChart.GainMaxValue <= this.OwnerChart.GainMinValue) return;
                ResetValue(e.GetPosition(this));
            };
            this.MouseMove += (sender, e) =>
            {
                if (e.LeftButton != System.Windows.Input.MouseButtonState.Pressed || this.OwnerChart == null || (AxisType == AxisType.X && !this.OwnerChart.GainXCanChange) || (AxisType == AxisType.Y && !this.OwnerChart.GainYCanChange) || this.OwnerChart.GainMaxValue <= this.OwnerChart.GainMinValue) return;
                ResetValue(e.GetPosition(this));
            };
            this.MouseLeftButtonUp += (sender, e) =>
            {
                if (this.OwnerChart == null || (AxisType == AxisType.X && !this.OwnerChart.GainXCanChange) || (AxisType == AxisType.Y && !this.OwnerChart.GainYCanChange) || this.OwnerChart.GainMaxValue <= this.OwnerChart.GainMinValue) return;
                ResetValue(e.GetPosition(this), true);
            };
            this.MouseLeave += (sender, e) =>
            {
                if (e.LeftButton != System.Windows.Input.MouseButtonState.Pressed || this.OwnerChart == null || (AxisType == AxisType.X && !this.OwnerChart.GainXCanChange) || (AxisType == AxisType.Y && !this.OwnerChart.GainYCanChange) || this.OwnerChart.GainMaxValue <= this.OwnerChart.GainMinValue) return;
                ResetValue(e.GetPosition(this), true);
            };
        }
        public IAxis(BaseAxisModel model) : this()
        {
            if (model != null)
            {
                this.Max = model.Max;
                this.Min = model.Min;
            }
        }

        /// <summary>
        /// 所属chart控件
        /// </summary>
        public Chart OwnerChart { get; internal set; }
        /// <summary>
        /// 轴类型/X、Y
        /// </summary>
        public AxisType AxisType { get; internal set; }

        #region 样式
        /// <summary>
        /// 前景色
        /// </summary>
        public Brush Foreground
        {
            get { return (Brush)GetValue(ForegroundProperty); }
            set { SetValue(ForegroundProperty, value); }
        }
        public static readonly DependencyProperty ForegroundProperty = DependencyProperty.Register("Foreground", typeof(Brush), typeof(IAxis), new PropertyMetadata(Brushes.Black, (d, e) => { if (d is IAxis axis) axis.Draw(); }));

        /// <summary>
        /// 字体大小
        /// </summary>
        public double FontSize
        {
            get { return (double)GetValue(FontSizeProperty); }
            set { SetValue(FontSizeProperty, value); }
        }
        public static readonly DependencyProperty FontSizeProperty = DependencyProperty.Register("FontSize", typeof(double), typeof(IAxis), new PropertyMetadata(12d, (d, e) => { if (d is IAxis axis) axis.Draw(); }));

        /// <summary>
        /// 指数字体大小
        /// </summary>
        public double PowerFontSize
        {
            get { return (double)GetValue(PowerFontSizeProperty); }
            set { SetValue(PowerFontSizeProperty, value); }
        }
        public static readonly DependencyProperty PowerFontSizeProperty = DependencyProperty.Register("PowerFontSize", typeof(double), typeof(IAxis), new PropertyMetadata(8d, (d, e) => { if (d is IAxis axis) axis.Draw(); }));

        #endregion

        #region 数据计算相关

        /// <summary>
        /// 最大值
        /// </summary>
        public double Max
        {
            get { return (double)GetValue(MaxProperty); }
            internal set { SetValue(MaxProperty, value); }
        }
        public static readonly DependencyProperty MaxProperty = DependencyProperty.Register("Max", typeof(double), typeof(IAxis), new PropertyMetadata(100000d));
        /// <summary>
        /// 最小值
        /// </summary>
        public double Min
        {
            get { return (double)GetValue(MinProperty); }
            internal set { SetValue(MinProperty, value); }
        }
        public static readonly DependencyProperty MinProperty = DependencyProperty.Register("Min", typeof(double), typeof(IAxis), new PropertyMetadata(0d));
        /// <summary>
        /// 设置最大最小值
        /// </summary>
        /// <param name="max"></param>
        /// <param name="min"></param>
        public void SetMaxMin(double max, double min)
        {
            this.Max = max;
            this.Min = min;
            this.Draw();
            if (this.OwnerChart != null)
            {
                if (this.OwnerChart.Series != null) this.OwnerChart.Series.Draw();
                if (this.OwnerChart.Gates != null) foreach (var gate in this.OwnerChart.Gates) gate.Draw();
            }
        }
        #endregion

        #region 点位图形值，实际数值，实际坐标值相互转换

        /// <summary>
        /// 获取实际值和坐标值转换参数
        /// </summary>
        /// <returns></returns>
        public virtual ValueAxisConvertParamBase GetValueAxisConvertParam()
        {
            return null;
        }

        /// <summary>
        /// 实际值转换成坐标值
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public abstract double ValueToAxisValue(double value, ValueAxisConvertParamBase param);
        public virtual double ValueToAxisValue(double value)
        {
            return ValueToAxisValue(value, GetValueAxisConvertParam());
        }
        /// <summary>
        /// 坐标值转换成实际值
        /// </summary>
        /// <param name="axisvalue"></param>
        /// <returns></returns>
        public abstract double AxisValueToValue(double axisvalue, ValueAxisConvertParamBase param);
        public virtual double AxisValueToValue(double value)
        {
            return AxisValueToValue(value, GetValueAxisConvertParam());
        }

        /// <summary>
        /// 获取实际值转化成坐标值的参数，用于GetLocation()
        /// </summary>
        /// <returns></returns>
        public virtual ValueLocationConvertParam GetConvertParam()
        {
            var p = GetValueAxisConvertParam();
            return new ValueLocationConvertParam()
            {
                MaxAxisValue = ValueToAxisValue(Max, p),
                MinAxisValue = ValueToAxisValue(Min, p),
                Length = AxisType == AxisType.X ? this.ActualWidth : this.ActualHeight,
                AxisType = AxisType,
                ValueAxisParam = p
            };
        }
        /// <summary>
        /// 获取真实数字对应的位置
        /// </summary>
        /// <param name="value">真实数据值</param>
        /// <returns></returns>
        public virtual double GetValueLocation(double value)
        {
            if (!IsLoaded) return default;
            return GetValueLocation(value, this.GetConvertParam());
        }
        /// <summary>
        /// 获取真实数据对应的坐标位置
        /// </summary>
        /// <param name="value">真实数据值</param>
        /// <param name="convert">转化参数，GetConvertParam()获取</param>
        /// <returns></returns>
        public virtual double GetValueLocation(double value, ValueLocationConvertParam convert)
        {
            value = ValueToAxisValue(value, convert.ValueAxisParam);
            return GetAxisValueLocation(value, convert);
        }
        /// <summary>
        /// 获取坐标数字对应的位置
        /// </summary>
        /// <param name="value">坐标数据</param>
        /// <returns></returns>
        public virtual double GetAxisValueLocation(double value)
        {
            if (!IsLoaded) return default;
            return GetAxisValueLocation(value, this.GetConvertParam());
        }
        /// <summary>
        /// 获取坐标数据对应的坐标位置
        /// </summary>
        /// <param name="value">数据值--xy轴坐标上的值</param>
        /// <param name="convert">转化参数，GetConvertParam()获取</param>
        /// <returns></returns>
        public virtual double GetAxisValueLocation(double value, ValueLocationConvertParam convert)
        {
            var temp = (value - convert.MinAxisValue) * convert.Length / (convert.MaxAxisValue - convert.MinAxisValue);
            return convert.AxisType == AxisType.X ? temp : convert.Length - temp;
        }
        /// <summary>
        /// 获取点位对应的坐标值
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public virtual double GetLocationAxisValue(double value)
        {
            return GetLocationAxisValue(value, GetConvertParam());
        }
        /// <summary>
        /// 获取点位对应的坐标值
        /// </summary>
        /// <param name="value"></param>
        /// <param name="convert"></param>
        /// <returns></returns>
        public virtual double GetLocationAxisValue(double value, ValueLocationConvertParam convert)
        {
            return (convert.AxisType == AxisType.X ? value : convert.Length - value) * (convert.MaxAxisValue - convert.MinAxisValue) / convert.Length + convert.MinAxisValue;
        }
        /// <summary>
        /// 获取点位对应的实际值
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public virtual double GetLocationValue(double value)
        {
            return GetLocationValue(value, GetConvertParam());
        }
        /// <summary>
        /// 获取点位对应的实际值
        /// </summary>
        /// <param name="value"></param>
        /// <param name="convert"></param>
        /// <returns></returns>
        public virtual double GetLocationValue(double value, ValueLocationConvertParam convert)
        {
            return AxisValueToValue(GetLocationAxisValue(value, convert), convert.ValueAxisParam);
        }
        #endregion

        /// <summary>
        /// 绘制轴的显示
        /// </summary>
        /// <returns></returns>
        internal abstract void Draw();

        /// <summary>
        /// 鼠标移动
        /// </summary>
        /// <param name="x">x轴移动距离</param>
        /// <param name="y">y轴移动距离</param>
        internal virtual void Move(double x, double y)
        {
            if (!IsLoaded) return;
            double controlActualLength = 1;
            double movelength = 0d;
            switch (AxisType)
            {
                case AxisType.X:
                    controlActualLength = this.ActualWidth;
                    movelength = x;
                    break;
                case AxisType.Y:
                    controlActualLength = this.ActualHeight;
                    movelength = -y;//纵坐标的方向在图形显示和轴上是反向的
                    break;
                default:
                    break;
            }
            var p = GetValueAxisConvertParam();
            var MaxAxis = ValueToAxisValue(Max, p);
            var MinAxis = ValueToAxisValue(Min, p);
            var temp = (MaxAxis - MinAxis) * movelength / controlActualLength;
            Max = AxisValueToValue(MaxAxis - temp, p);
            Min = AxisValueToValue(MinAxis - temp, p);
            Draw();
        }

        /// <summary>
        /// 鼠标滚轮放大缩小
        /// </summary>
        /// <param name="value">放大缩小比例</param>
        /// <param name="point">中心点位</param>
        internal virtual void Zoom(double percent, Point point)
        {
            if (!IsLoaded) return;
            double controlLength = 1;
            double pv = 0d;
            switch (AxisType)
            {
                case AxisType.X:
                    controlLength = this.ActualWidth;
                    pv = point.X;
                    break;
                case AxisType.Y:
                    controlLength = this.ActualHeight;
                    pv = this.ActualHeight - point.Y;
                    break;
                default:
                    break;
            }
            var p = GetValueAxisConvertParam();
            var MaxAxis = ValueToAxisValue(Max, p);
            var MinAxis = ValueToAxisValue(Min, p);
            percent = 1 / percent;
            var v = (MaxAxis - MinAxis) * (pv / controlLength) + MinAxis;
            Min = AxisValueToValue(v - (v - MinAxis) * percent, p);
            Max = AxisValueToValue(v + (MaxAxis - v) * percent, p);
            Draw();
        }

        #region 增益
        /// <summary>
        /// 重置增益值
        /// </summary>
        /// <param name="point"></param>
        internal virtual void ResetValue(Point point, bool executeCommand = false)
        {
            if (AxisType == AxisType.X)
            {
                var temp = (this.OwnerChart.GainMaxValue - this.OwnerChart.GainMinValue) * point.X / this.ActualWidth + this.OwnerChart.GainMinValue;
                if (temp < this.OwnerChart.GainMinValue) this.OwnerChart.GainXValue = this.OwnerChart.GainMinValue;
                else if (temp > this.OwnerChart.GainMaxValue) this.OwnerChart.GainXValue = this.OwnerChart.GainMaxValue;
                else this.OwnerChart.GainXValue = temp;
                if (executeCommand && this.OwnerChart.GainXValueChangedCommand != null && this.OwnerChart.GainXValueChangedCommand.CanExecute(this.OwnerChart.GainXValue))
                    this.OwnerChart.GainXValueChangedCommand.Execute(this.OwnerChart.GainXValue);
            }
            else
            {
                var temp = this.OwnerChart.GainMaxValue - (this.OwnerChart.GainMaxValue - this.OwnerChart.GainMinValue) * point.Y / this.ActualHeight;
                if (temp < this.OwnerChart.GainMinValue) this.OwnerChart.GainYValue = this.OwnerChart.GainMinValue;
                else if (temp > this.OwnerChart.GainMaxValue) this.OwnerChart.GainYValue = this.OwnerChart.GainMaxValue;
                else this.OwnerChart.GainYValue = temp;
                if (executeCommand && this.OwnerChart.GainYValueChangedCommand != null && this.OwnerChart.GainYValueChangedCommand.CanExecute(this.OwnerChart.GainYValue))
                    this.OwnerChart.GainYValueChangedCommand.Execute(this.OwnerChart.GainYValue);
            }
        }

        /// <summary>
        /// 绘制增益
        /// </summary>
        internal virtual void DrawGain()
        {
            if (OwnerChart == null || !this.IsLoaded) return;
            if (AxisType == AxisType.X) DrawXGain();
            else DrawYGain();
        }

        protected virtual void DrawXGain()
        {
            using (var dc = GainDV.RenderOpen())
            {
                dc.DrawGeometry(Brushes.Transparent, null, new RectangleGeometry(new Rect(0, 0, this.ActualWidth, this.ActualHeight)));
                if (this.OwnerChart.GainXCanChange)
                {
                    var center = (this.OwnerChart.GainXValue - this.OwnerChart.GainMinValue) * this.ActualWidth / (this.OwnerChart.GainMaxValue - this.OwnerChart.GainMinValue);
                    var ft = new FormattedText(this.OwnerChart.GainXValue.ToString("n0"), System.Globalization.CultureInfo.CurrentUICulture, FlowDirection.LeftToRight, new Typeface("Segoe UI"), FontSize, Foreground, VisualTreeHelper.GetDpi(this).PixelsPerDip);
                    var ftg = ft.BuildGeometry(default);
                    ftg.Transform = new MatrixTransform(1, 0, 0, 1, center - ft.Width / 2, this.ActualHeight - ft.Height);
                    dc.DrawGeometry(OwnerChart.GainStroke, new Pen(OwnerChart.GainStroke, 0.1d), ftg);
                    StreamGeometry geometry = new StreamGeometry();
                    using (var gc = geometry.Open())
                    {
                        gc.BeginFigure(new Point(center, 0), true, true);
                        var points = new Point[] { new Point(center + 6, 6), new Point(center - 6, 6) };
                        gc.PolyLineTo(points, true, true);
                    }
                    geometry.Freeze();
                    dc.DrawGeometry(OwnerChart.GainFill, new Pen(OwnerChart.GainStroke, OwnerChart.GainStrokeThickness), geometry);
                }
            }
        }

        protected virtual void DrawYGain()
        {
            using (var dc = GainDV.RenderOpen())
            {
                dc.DrawGeometry(Brushes.Transparent, null, new RectangleGeometry(new Rect(0, 0, this.ActualWidth, this.ActualHeight)));
                if (this.OwnerChart.GainYCanChange)
                {
                    var center = (this.OwnerChart.GainMaxValue - this.OwnerChart.GainYValue) * this.ActualHeight / (this.OwnerChart.GainMaxValue - this.OwnerChart.GainMinValue);
                    var ft = new FormattedText(this.OwnerChart.GainYValue.ToString("n0"), System.Globalization.CultureInfo.CurrentUICulture, FlowDirection.LeftToRight, new Typeface("Segoe UI"), FontSize, Foreground, VisualTreeHelper.GetDpi(this).PixelsPerDip);
                    var ftg = ft.BuildGeometry(default);
                    ftg.Transform = new MatrixTransform(0, -1, 1, 0, 0, center + ft.Width / 2);
                    dc.DrawGeometry(OwnerChart.GainStroke, new Pen(OwnerChart.GainStroke, 0.1d), ftg);
                    StreamGeometry geometry = new StreamGeometry();
                    using (var gc = geometry.Open())
                    {
                        gc.BeginFigure(new Point(this.ActualWidth, center), true, true);
                        var points = new Point[] { new Point(this.ActualWidth - 6, center + 6), new Point(this.ActualWidth - 6, center - 6) };
                        gc.PolyLineTo(points, true, true);
                    }
                    geometry.Freeze();
                    dc.DrawGeometry(OwnerChart.GainFill, new Pen(OwnerChart.GainStroke, OwnerChart.GainStrokeThickness), geometry);
                }
            }
        }
        #endregion

        #region 数据模型
        /// <summary>
        /// 获取轴模型对象
        /// </summary>
        /// <returns></returns>
        public abstract BaseAxisModel GetModel();
        /// <summary>
        /// 创建新的轴对象
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        public static IAxis CreateByModel(BaseAxisModel model)
        {
            if (model is LinearAxisModel lm) return new LinearAxis(lm);
            else if (model is NegativeLogAxisModel nm) return new NegativeLogAxis(nm);
            return new LinearAxis();
        }
        #endregion
    }

    /// <summary>
    /// 轴类型
    /// </summary>
    public enum AxisType
    {
        X,
        Y
    }
    /// <summary>
    /// 轴转换参数，跨线程使用
    /// </summary>
    public class ValueLocationConvertParam
    {
        /// <summary>
        /// 最大坐标值
        /// </summary>
        public double MaxAxisValue { get; set; }
        /// <summary>
        /// 最小坐标值
        /// </summary>
        public double MinAxisValue { get; set; }
        /// <summary>
        /// 轴图形长度
        /// </summary>
        public double Length { get; set; }
        /// <summary>
        /// 轴类型
        /// </summary>
        public AxisType AxisType { get; set; }
        /// <summary>
        /// 实际值和坐标值转换参数
        /// </summary>
        public ValueAxisConvertParamBase ValueAxisParam { get; set; }
    }

    /// <summary>
    /// 实际值和坐标值转换参数
    /// </summary>
    public class ValueAxisConvertParamBase { }

    /// <summary>
    /// 轴数据模型
    /// </summary>
    [XmlInclude(typeof(LinearAxisModel))]
    [XmlInclude(typeof(NegativeLogAxisModel))]
    public abstract class BaseAxisModel
    {
        public double Min { get; set; }
        public double Max { get; set; }
    }
}
