using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;

namespace FCSVisualChart
{
    /// <summary>
    /// 密度图,单色带透明效果
    /// </summary>
    public class DensitySeries : ISeries
    {
        /// <summary>
        /// 点的宽度
        /// </summary>
        public int PointLength
        {
            get { return (int)GetValue(PointLengthProperty); }
            set { SetValue(PointLengthProperty, value); }
        }
        public static readonly DependencyProperty PointLengthProperty = DependencyProperty.Register("PointLength", typeof(int), typeof(DensitySeries), new PropertyMetadata(2, (d, e) => { if (d is ISeries series) series.Draw(); }));

        private const byte DefaultMinOpacity = 0x10;
        /// <summary>
        /// 最小不透明度,最小为0,密度分级从该值到255
        /// </summary>
        public byte MinOpacity
        {
            get { return (byte)GetValue(MinOpacityProperty); }
            set { SetValue(MinOpacityProperty, value); }
        }
        public static readonly DependencyProperty MinOpacityProperty = DependencyProperty.Register("MinOpacity", typeof(byte), typeof(DensitySeries), new PropertyMetadata(DefaultMinOpacity, (d, e) => { if (d is ISeries series) series.Draw(); }));

        public DensitySeries() : base() { }
        public DensitySeries(DensitySeriesModel model) : base(model)
        {
            this.PointLength = model.PointLength;
            this.MinOpacity = model.MinOpacity;
        }

        /// <summary>
        /// 绘制内容
        /// </summary>
        /// <param name="color">基础颜色</param>
        /// <param name="list">矢量数据流</param>
        /// <param name="dc">画布</param>
        internal override void DCDraw(Color color, List<StreamGeometry> list, DrawingContext dc)
        {
            var pa = color.A / 255d;
            var count = list.Count;
            byte i = MinOpacity;
            foreach (var stream in list)
            {
                if (stream.IsEmpty()) continue;
                var brush = new SolidColorBrush(Color.FromArgb(Convert.ToByte(i * pa), color.R, color.G, color.B));
                brush.Freeze();
                Brush fill = ShowFill ? brush : null;
                Pen pen = ShowStroke ? new Pen(brush, StrokeThickness) : null;
                dc.DrawGeometry(fill, pen, stream);
                i++;
            }
        }

        /// <summary>
        /// 获取数据源的矢量数据流
        /// </summary>
        /// <param name="dataSource">数据源</param>
        /// <returns></returns>
        protected override Task<List<StreamGeometry>> CreateStreamGeometryByChartDataSource(ChartDataSource dataSource, IList<int> indexs, GateArea gateArea = null)
        {
            if (!this.IsLoaded || this.OwnerChart == null || this.OwnerChart.XAxis == null || this.OwnerChart.YAxis == null || (gateArea == null && (dataSource.CancelTokenSource == null || dataSource.CancelTokenSource.IsCancellationRequested)) || (gateArea != null && (!dataSource.GateAreaCancelTokenSource.ContainsKey(gateArea) || dataSource.GateAreaCancelTokenSource[gateArea] == null || dataSource.GateAreaCancelTokenSource[gateArea].IsCancellationRequested))) return Task.FromResult<List<StreamGeometry>>(null);
            var token = gateArea == null ? dataSource.CancelTokenSource.Token : dataSource.GateAreaCancelTokenSource[gateArea].Token;
            var pointLength = PointLength;
            var maxDegreeOfParallelism = this.MaxDegreeOfParallelism;
            var xParam = this.OwnerChart.XAxis.GetConvertParam();
            var yParam = this.OwnerChart.YAxis.GetConvertParam();
            Func<double, ValueLocationConvertParam, double> xGetLocation = this.OwnerChart.XAxis.GetValueLocation;
            Func<double, ValueLocationConvertParam, double> yGetLocation = this.OwnerChart.YAxis.GetValueLocation;
            var excessDisplayAtEdge = this.ExcessDisplayAtEdge;
            var minopacity = this.MinOpacity;
            return Task.Factory.StartNew<List<StreamGeometry>>(() =>
            {
                var points = PointCountStatistics(token, dataSource.XSource, dataSource.YSource, indexs, dataSource.XSourceConverter, dataSource.YSourceConverter, pointLength, maxDegreeOfParallelism, xParam, yParam, xGetLocation, yGetLocation, excessDisplayAtEdge);
                if (points == null) return null;
                var maxcount = points.Max(p => p.Value.Max(k => k.Value));
                if (maxcount <= 0) return null;
                var count = 256 - minopacity;
                StreamGeometry[] streams = new StreamGeometry[count];
                StreamGeometryContext[] contexts = new StreamGeometryContext[count];
                for (int i = 0; i < count; i++)
                {
                    StreamGeometry stream = new StreamGeometry();
                    streams[i] = stream;
                    contexts[i] = stream.Open();
                }
                var arrayindexmax = count - 1d;
                foreach (var w in points)
                {
                    foreach (var h in w.Value)
                    {
                        if (h.Value > 0)
                        {
                            var i = Convert.ToInt32(Math.Round(h.Value * arrayindexmax / maxcount, 0));
                            var sgc = contexts[i];
                            var xtemp = w.Key;
                            var ytemp = h.Key;
                            var point = new Point(xtemp, ytemp);
                            sgc.BeginFigure(point, true, true);
                            sgc.PolyLineTo(new Point[] { new Point(xtemp + pointLength, ytemp), new Point(xtemp + pointLength, ytemp + pointLength), new Point(xtemp, ytemp + pointLength) }, true, true);
                        }
                    }
                }
                for (int i = 0; i < count; i++)
                {
                    contexts[i].Close();
                    streams[i].Freeze();
                }
                return streams.ToList();
            }, token);
        }

        public override BaseSeriesModel GetModel()
        {
            return new DensitySeriesModel()
            {
                ShowFill = this.ShowFill,
                ShowStroke = this.ShowStroke,
                StrokeThickness = this.StrokeThickness,
                FillColorA = this.FillColorA,
                StrokeColorA = this.StrokeColorA,
                ExcessDisplayAtEdge = this.ExcessDisplayAtEdge,
                PointLength = this.PointLength,
                MinOpacity = this.MinOpacity
            };
        }
    }

    public class DensitySeriesModel : BaseSeriesModel
    {
        public int PointLength { get; set; }
        public byte MinOpacity { get; set; }
    }
}
