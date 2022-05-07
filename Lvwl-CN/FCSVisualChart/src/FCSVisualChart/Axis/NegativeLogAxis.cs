using System;
using System.Windows;
using System.Windows.Media;

namespace FCSVisualChart
{
    /// <summary>
    /// 带负数log
    /// </summary>
    public class NegativeLogAxis : IAxis
    {
        /// <summary>
        /// log的底,尽可能用正整数或e
        /// </summary>
        public double LogBase
        {
            get { return (double)GetValue(LogBaseProperty); }
            set { SetValue(LogBaseProperty, value); }
        }
        public static readonly DependencyProperty LogBaseProperty = DependencyProperty.Register("LogBase", typeof(double), typeof(NegativeLogAxis), new PropertyMetadata(10d, (d, e) =>
        {
            if (d is IAxis axis)
            {
                axis.Draw();
                if (axis.OwnerChart != null)
                {
                    if (axis.OwnerChart.Series != null) axis.OwnerChart.Series.Draw();
                    if (axis.OwnerChart.Gates != null) foreach (var gate in axis.OwnerChart.Gates) gate.Draw();
                }
            }
        }));

        /// <summary>
        /// 线性显示的次方数,0附近次方数为线性显示
        /// </summary>
        public uint LinePowerCount
        {
            get { return (uint)GetValue(LinePowerCountProperty); }
            set { SetValue(LinePowerCountProperty, value); }
        }
        public static readonly DependencyProperty LinePowerCountProperty = DependencyProperty.Register("LinePowerCount", typeof(uint), typeof(NegativeLogAxis), new PropertyMetadata(1u, (d, e) =>
        {
            if (d is IAxis axis)
            {
                axis.Draw();
                if (axis.OwnerChart != null)
                {
                    if (axis.OwnerChart.Series != null) axis.OwnerChart.Series.Draw();
                    if (axis.OwnerChart.Gates != null) foreach (var gate in axis.OwnerChart.Gates) gate.Draw();
                }
            }
        }));

        public NegativeLogAxis() : base() { }
        public NegativeLogAxis(NegativeLogAxisModel model) : base(model)
        {
            this.LogBase = model.LogBase;
            this.LinePowerCount = model.LinePowerCount;
        }

        /// <summary>
        /// 实际值转坐标值
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public override double ValueToAxisValue(double value, ValueAxisConvertParamBase param)
        {
            uint linePowerCount = 1;
            double logBase = 10d;
            if (param is NegativeLogValueAxisConvertParam p)
            {
                linePowerCount = p.LinePowerCount;
                logBase = p.LogBase;
            }
            var lineMaxValue = Math.Pow(logBase, linePowerCount);
            if (double.IsNaN(value)) return value;
            else if (double.IsPositiveInfinity(value)) return double.MaxValue;
            else if (double.IsNegativeInfinity(value)) return double.MinValue;
            else if (value >= lineMaxValue) return Math.Log(value, logBase);
            else if (value <= -lineMaxValue) return -Math.Log(-value, logBase);
            else return value * linePowerCount / lineMaxValue;

        }

        /// <summary>
        /// 坐标值转实际值
        /// </summary>
        /// <param name="axisvalue"></param>
        /// <returns></returns>
        public override double AxisValueToValue(double axisvalue, ValueAxisConvertParamBase param)
        {
            uint linePowerCount = 1;
            double logBase = 10d;
            if (param is NegativeLogValueAxisConvertParam p)
            {
                linePowerCount = p.LinePowerCount;
                logBase = p.LogBase;
            }
            var lineMaxValue = Math.Pow(logBase, linePowerCount);
            if (double.IsNaN(axisvalue)) return axisvalue;
            else if (double.IsPositiveInfinity(axisvalue)) return double.MaxValue;
            else if (double.IsNegativeInfinity(axisvalue)) return double.MinValue;
            else if (axisvalue >= linePowerCount) return Math.Pow(logBase, axisvalue);
            else if (axisvalue <= -linePowerCount) return -Math.Pow(logBase, -axisvalue);
            else return axisvalue * lineMaxValue / linePowerCount;
        }

        /// <summary>
        /// 获取实际值和坐标值转换参数
        /// </summary>
        /// <returns></returns>
        public override ValueAxisConvertParamBase GetValueAxisConvertParam()
        {
            return new NegativeLogValueAxisConvertParam()
            {
                LogBase = this.LogBase,
                LinePowerCount = this.LinePowerCount
            };
        }

        internal override void Draw()
        {
            if (!this.IsLoaded) return;
            var valuelocationparam = GetConvertParam();
            var valueaxisconvertparam = valuelocationparam.ValueAxisParam as NegativeLogValueAxisConvertParam;
            var MaxAxis = ValueToAxisValue(Max, valueaxisconvertparam);
            var MinAxis = ValueToAxisValue(Min, valueaxisconvertparam);

            var range = MaxAxis - MinAxis;
            var count = Convert.ToInt32(valuelocationparam.Length / (Math.Max(MaxAxis.ToString("N0").Length, 4) * FontSize));
            var subrange = Math.Pow(10, Math.Floor(Math.Log(range / count) / Math.Log(10)));
            var residual = range / count / subrange;
            if (residual > 5) subrange = 10 * subrange;
            else if (residual > 2) subrange = 5 * subrange;
            else if (residual > 1) subrange = 2 * subrange;
            var subpowercount = Convert.ToInt32(subrange);
            if (subpowercount < 1) subpowercount = 1;

            var showedTempMax = Convert.ToInt32(Math.Ceiling(MaxAxis));
            var showedTempMin = Convert.ToInt32(Math.Floor(MinAxis));
            double actualLength = AxisType == AxisType.X ? this.ActualWidth : this.ActualHeight;

            var typeFace = new Typeface("Segoe UI");
            var numberSubstitution = new NumberSubstitution();
            var pen = new Pen(Foreground, 1d);
            var stringpen = new Pen(Foreground, 0.1d);
            var pointzero = new Point(0, 0);
            string logBaseString = valueaxisconvertparam.LogBase.ToString();
            if (valueaxisconvertparam.LogBase == Math.E) logBaseString = "e";
            else if (valueaxisconvertparam.LogBase == Math.PI) logBaseString = "π";
#if NET5_0_OR_GREATER
            else if (valueaxisconvertparam.LogBase == Math.Tau) logBaseString = "τ";
#endif
            var subcount = Convert.ToInt32(Math.Floor(valueaxisconvertparam.LogBase));
            var linerange = Math.Pow(valueaxisconvertparam.LogBase, valueaxisconvertparam.LinePowerCount) / valueaxisconvertparam.LinePowerCount / subcount;
            subcount = subcount - 1;
            using (var dc = DV.RenderOpen())
            {
                if (AxisType == AxisType.X) dc.DrawLine(pen, new Point(0, 0), new Point(this.ActualWidth, 0));
                else dc.DrawLine(pen, new Point(this.ActualWidth, 0), new Point(this.ActualWidth, this.ActualHeight));
                for (int i = showedTempMin; i <= showedTempMax; i++)
                {
                    if (i % subpowercount != 0) continue;
                    var value = AxisValueToValue(i, valueaxisconvertparam);
                    var valuelocation = GetValueLocation(value, valuelocationparam);
                    if (i >= valueaxisconvertparam.LinePowerCount || i <= -valueaxisconvertparam.LinePowerCount)
                    {
                        var bString = i > 0 ? logBaseString : string.Concat("-", logBaseString);
                        var pString = i > 0 ? i.ToString() : (-i).ToString();
                        var vft = new FormattedText(bString, System.Globalization.CultureInfo.CurrentUICulture, FlowDirection.LeftToRight, typeFace, FontSize, Foreground, numberSubstitution, VisualTreeHelper.GetDpi(this).PixelsPerDip);
                        var pft = new FormattedText(pString, System.Globalization.CultureInfo.CurrentUICulture, FlowDirection.LeftToRight, typeFace, PowerFontSize, Foreground, numberSubstitution, VisualTreeHelper.GetDpi(this).PixelsPerDip);
                        var vftg = vft.BuildGeometry(pointzero);
                        var pftg = pft.BuildGeometry(pointzero);
                        if (AxisType == AxisType.X)
                        {
                            if (valuelocation >= 0 && valuelocation <= valuelocationparam.Length)
                            {
                                dc.DrawLine(pen, new Point(valuelocation, 0), new Point(valuelocation, 5));
                                vftg.Transform = new MatrixTransform(1, 0, 0, 1, valuelocation - (vft.Width + pft.Width) / 2, 5);
                                dc.DrawGeometry(Foreground, stringpen, vftg);
                                pftg.Transform = new MatrixTransform(1, 0, 0, 1, valuelocation + vft.Width / 2 - pft.Width / 2, 5);
                                dc.DrawGeometry(Foreground, stringpen, pftg);
                            }
                            for (int j = 1; j < (subpowercount == 1 ? subcount : subpowercount); j++)
                            {
                                var sublocation = GetValueLocation(value * (subpowercount == 1 ? (1 + j) : Math.Pow(valueaxisconvertparam.LogBase, j)), valuelocationparam);
                                if (sublocation < 0 || sublocation > valuelocationparam.Length) continue;
                                dc.DrawLine(pen, new Point(sublocation, 0), new Point(sublocation, 3));
                            }
                        }
                        else
                        {
                            if (valuelocation >= 0 && valuelocation <= valuelocationparam.Length)
                            {
                                dc.DrawLine(pen, new Point(this.ActualWidth, valuelocation), new Point(this.ActualWidth - 5, valuelocation));
                                vftg.Transform = new MatrixTransform(0, -1, 1, 0, this.ActualWidth - 10 - FontSize, valuelocation + (vft.Width + pft.Width) / 2);
                                dc.DrawGeometry(Foreground, stringpen, vftg);
                                pftg.Transform = new MatrixTransform(0, -1, 1, 0, this.ActualWidth - 10 - FontSize, valuelocation - vft.Width / 2 + pft.Width / 2);
                                dc.DrawGeometry(Foreground, stringpen, pftg);
                            }
                            for (int j = 1; j < (subpowercount == 1 ? subcount : subpowercount); j++)
                            {
                                var sublocation = GetValueLocation(value * (subpowercount == 1 ? (1 + j) : Math.Pow(valueaxisconvertparam.LogBase, j)), valuelocationparam);
                                if (sublocation < 0 || sublocation > valuelocationparam.Length) continue;
                                dc.DrawLine(pen, new Point(this.ActualWidth, sublocation), new Point(this.ActualWidth - 3, sublocation));
                            }
                        }
                    }
                    else
                    {
                        var vString = value.ToString();
                        var vft = new FormattedText(vString, System.Globalization.CultureInfo.CurrentUICulture, FlowDirection.LeftToRight, typeFace, FontSize, Foreground, numberSubstitution, VisualTreeHelper.GetDpi(this).PixelsPerDip).BuildGeometry(pointzero);
                        if (AxisType == AxisType.X)
                        {
                            if (valuelocation >= 0 && valuelocation <= valuelocationparam.Length)
                            {
                                dc.DrawLine(pen, new Point(valuelocation, 0), new Point(valuelocation, 5));
                                vft.Transform = new MatrixTransform(1, 0, 0, 1, valuelocation - vString.Length * FontSize / 3, 5);
                                dc.DrawGeometry(Foreground, stringpen, vft);
                            }
                            for (int j = 1; j <= (subpowercount == 1 ? subcount : subpowercount); j++)
                            {
                                var subv = i >= 0 ? value + (subpowercount == 1 ? j * linerange : Math.Pow(valueaxisconvertparam.LogBase, j)) : value - (subpowercount == 1 ? j * linerange : Math.Pow(valueaxisconvertparam.LogBase, j));
                                var sublocation = GetValueLocation(subv, valuelocationparam);
                                if (sublocation < 0 || sublocation > valuelocationparam.Length) continue;
                                dc.DrawLine(pen, new Point(sublocation, 0), new Point(sublocation, 3));
                            }
                            if (i == 0)
                            {
                                for (int j = 1; j <= (subpowercount == 1 ? subcount : subpowercount); j++)
                                {
                                    var subv = value - (subpowercount == 1 ? j * linerange : Math.Pow(valueaxisconvertparam.LogBase, j));
                                    var sublocation = GetValueLocation(subv, valuelocationparam);
                                    if (sublocation < 0 || sublocation > valuelocationparam.Length) continue;
                                    dc.DrawLine(pen, new Point(sublocation, 0), new Point(sublocation, 3));
                                }
                            }
                        }
                        else
                        {
                            if (valuelocation >= 0 && valuelocation <= valuelocationparam.Length)
                            {
                                dc.DrawLine(pen, new Point(this.ActualWidth, valuelocation), new Point(this.ActualWidth - 5, valuelocation));
                                vft.Transform = new MatrixTransform(0, -1, 1, 0, this.ActualWidth - 10 - FontSize, valuelocation + vString.Length * FontSize / 3);
                                dc.DrawGeometry(Foreground, stringpen, vft);
                            }
                            for (int j = 1; j <= (subpowercount == 1 ? subcount : subpowercount); j++)
                            {
                                var subv = i >= 0 ? value + (subpowercount == 1 ? j * linerange : Math.Pow(valueaxisconvertparam.LogBase, j)) : value - (subpowercount == 1 ? j * linerange : Math.Pow(valueaxisconvertparam.LogBase, j));
                                var sublocation = GetValueLocation(subv, valuelocationparam);
                                if (sublocation < 0 || sublocation > valuelocationparam.Length) continue;
                                dc.DrawLine(pen, new Point(this.ActualWidth, sublocation), new Point(this.ActualWidth - 3, sublocation));
                            }
                            if (i == 0)
                            {
                                for (int j = 1; j <= (subpowercount == 1 ? subcount : subpowercount); j++)
                                {
                                    var subv = value - (subpowercount == 1 ? j * linerange : Math.Pow(valueaxisconvertparam.LogBase, j));
                                    var sublocation = GetValueLocation(subv, valuelocationparam);
                                    if (sublocation < 0 || sublocation > valuelocationparam.Length) continue;
                                    dc.DrawLine(pen, new Point(this.ActualWidth, sublocation), new Point(this.ActualWidth - 3, sublocation));
                                }
                            }
                        }
                    }
                }
            }
        }

        /// <summary>
        /// 获取轴模型对象
        /// </summary>
        /// <returns></returns>
        public override BaseAxisModel GetModel()
        {
            return new NegativeLogAxisModel() { Max = this.Max, Min = this.Min, LogBase = this.LogBase, LinePowerCount = this.LinePowerCount };
        }
    }

    /// <summary>
    /// 实际值和坐标值转换参数
    /// </summary>
    public class NegativeLogValueAxisConvertParam : ValueAxisConvertParamBase
    {
        /// <summary>
        /// log的底,尽可能用正整数或e
        /// </summary>
        public double LogBase { get; set; }
        /// <summary>
        /// 线性显示的次方数
        /// </summary>
        public uint LinePowerCount { get; set; }
    }

    public class NegativeLogAxisModel : BaseAxisModel
    {
        public double LogBase { get; set; }
        public uint LinePowerCount { get; set; }
    }
}
