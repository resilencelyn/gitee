using System;
using System.Windows;
using System.Windows.Media;

namespace FCSVisualChart
{
    public class LinearAxis : IAxis
    {
        /// <summary>
        /// 10的次方数
        /// </summary>
        public int Power
        {
            get { return (int)GetValue(PowerProperty); }
            private set { SetValue(PowerProperty, value); }
        }
        public static readonly DependencyProperty PowerProperty = DependencyProperty.Register("Power", typeof(int), typeof(LinearAxis), new PropertyMetadata(0));

        public LinearAxis() : base() { }
        public LinearAxis(LinearAxisModel model) : base(model) { }

        /// <summary>
        /// 实际值转坐标值
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public override double ValueToAxisValue(double value, ValueAxisConvertParamBase param)
        {
            return value;
        }
        /// <summary>
        /// 坐标值转实际值
        /// </summary>
        /// <param name="axisvalue"></param>
        /// <returns></returns>
        public override double AxisValueToValue(double axisvalue, ValueAxisConvertParamBase param)
        {
            return axisvalue;
        }

        internal override void Draw()
        {
            if (!this.IsLoaded) return;
            var valuelocationparam = GetConvertParam();
            var powermax = Math.Floor(Max == 0 ? 0 : Math.Log10(Math.Abs(Max)));
            var powermin = Math.Floor(Min == 0 ? powermax < 0 ? powermax * 2 : powermax / 2 : Math.Log10(Math.Abs(Min)));
            Power = Convert.ToInt32((powermax + powermin) / 2);
            var powervalue = Convert.ToDecimal(Math.Pow(10, Power));
            var range = Max - Min;
            int count;
            if (range < 1)
            {
                var temp = range;
                while (temp < 1) temp *= 10;
                count = Convert.ToInt32(valuelocationparam.Length / (Math.Max(temp.ToString("N0").Length, 3) * FontSize));
            }
            else count = Convert.ToInt32(valuelocationparam.Length / (Math.Max(Max.ToString("N0").Length, 3) * FontSize));

            var subrange = Math.Pow(10, Math.Floor(Math.Log(range / count) / Math.Log(10)));
            var residual = range / count / subrange;
            if (residual > 5) subrange = 10 * subrange;
            else if (residual > 2) subrange = 5 * subrange;
            else if (residual > 1) subrange = 2 * subrange;

            var start = Min % subrange != 0d ? (Min - Min % subrange) : Min;
            var minsubrange = subrange / 10;
            var typeFace = new Typeface("Segoe UI");
            var numberSubstitution = new NumberSubstitution();
            var pen = new Pen(Foreground, 1d);
            var stringpen = new Pen(Foreground, 0.1d);
            var pointzero = new Point(0, 0);
            using (var dc = DV.RenderOpen())
            {
                var ft = new FormattedText(Power.ToString(), System.Globalization.CultureInfo.CurrentUICulture, FlowDirection.LeftToRight, typeFace, PowerFontSize, Foreground, numberSubstitution, VisualTreeHelper.GetDpi(this).PixelsPerDip);
                var fst = new FormattedText("x10", System.Globalization.CultureInfo.CurrentUICulture, FlowDirection.LeftToRight, typeFace, FontSize, Foreground, numberSubstitution, VisualTreeHelper.GetDpi(this).PixelsPerDip);
                var ftg = ft.BuildGeometry(pointzero);
                var fstg = fst.BuildGeometry(pointzero);
                if (AxisType == AxisType.X)
                {
                    dc.DrawLine(pen, new Point(0, 0), new Point(this.ActualWidth, 0));
                    ftg.Transform = new MatrixTransform(1, 0, 0, 1, this.ActualWidth - ft.Width - 5, FontSize + 5);
                    fstg.Transform = new MatrixTransform(1, 0, 0, 1, this.ActualWidth - ft.Width - fst.Width - 5, FontSize + 5);
                }
                else
                {
                    dc.DrawLine(pen, new Point(this.ActualWidth, 0), new Point(this.ActualWidth, this.ActualHeight));
                    ftg.Transform = new MatrixTransform(0, -1, 1, 0, this.ActualWidth - 10 - FontSize * 2, ft.Width + 5);
                    fstg.Transform = new MatrixTransform(0, -1, 1, 0, this.ActualWidth - 10 - FontSize * 2, ft.Width + fst.Width + 5);
                }
                dc.DrawGeometry(Foreground, stringpen, ftg);
                dc.DrawGeometry(Foreground, stringpen, fstg);
                for (int i = 0; i < range / subrange; i++)
                {
                    var value = Convert.ToDecimal(start) + Convert.ToDecimal(i * subrange);
                    var valuedouble = Convert.ToDouble(value);
                    var valuestr = (value / powervalue).ToString();
                    var valuelocation = GetValueLocation(valuedouble);
                    var vft = new FormattedText(valuestr, System.Globalization.CultureInfo.CurrentUICulture, FlowDirection.LeftToRight, typeFace, FontSize, Foreground, numberSubstitution, VisualTreeHelper.GetDpi(this).PixelsPerDip);
                    var vftg = vft.BuildGeometry(pointzero);
                    if (valuelocation >= 0 && valuelocation <= valuelocationparam.Length)
                    {
                        if (AxisType == AxisType.X)
                        {
                            dc.DrawLine(pen, new Point(valuelocation, 0), new Point(valuelocation, 5));
                            vftg.Transform = new MatrixTransform(1, 0, 0, 1, valuelocation - vft.Width / 2, 5);
                        }
                        else
                        {
                            dc.DrawLine(pen, new Point(this.ActualWidth, valuelocation), new Point(this.ActualWidth - 5, valuelocation));
                            vftg.Transform = new MatrixTransform(0, -1, 1, 0, this.ActualWidth - 10 - FontSize, valuelocation + vft.Width / 2);
                        }
                        dc.DrawGeometry(Foreground, stringpen, vftg);
                    }
                    for (int j = 1; j < 10; j++)
                    {
                        var subv = valuedouble + j * minsubrange;
                        if (subv < Min || subv > Max) continue;
                        var subvaluelocation = GetValueLocation(subv);
                        if (AxisType == AxisType.X) dc.DrawLine(pen, new Point(subvaluelocation, 0), new Point(subvaluelocation, 3));
                        else dc.DrawLine(pen, new Point(this.ActualWidth, subvaluelocation), new Point(this.ActualWidth - 3, subvaluelocation));
                    }
                    if (i == 0)
                    {
                        for (int j = 1; j < 10; j++)
                        {
                            var subv = valuedouble - j * minsubrange;
                            if (subv < Min || subv > Max) continue;
                            var subvaluelocation = GetValueLocation(subv);
                            if (AxisType == AxisType.X) dc.DrawLine(pen, new Point(subvaluelocation, 0), new Point(subvaluelocation, 3));
                            else dc.DrawLine(pen, new Point(this.ActualWidth, subvaluelocation), new Point(this.ActualWidth - 3, subvaluelocation));
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
            return new LinearAxisModel() { Max = this.Max, Min = this.Min };
        }
    }

    public class LinearAxisModel : BaseAxisModel { }
}
