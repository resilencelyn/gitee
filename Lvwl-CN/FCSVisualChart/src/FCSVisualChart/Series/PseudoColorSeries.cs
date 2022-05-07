using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;

namespace FCSVisualChart
{
    /// <summary>
    /// 伪彩图
    /// </summary>
    public class PseudoColorSeries : ISeries
    {
        /// <summary>
        /// 点的宽度
        /// </summary>
        public int PointLength
        {
            get { return (int)GetValue(PointLengthProperty); }
            set { SetValue(PointLengthProperty, value); }
        }
        public static readonly DependencyProperty PointLengthProperty = DependencyProperty.Register("PointLength", typeof(int), typeof(PseudoColorSeries), new PropertyMetadata(2, (d, e) => { if (d is ISeries series) series.Draw(); }));

        public PseudoColorSeries() : base() { }
        public PseudoColorSeries(PseudoColorSeriesModel model) : base(model)
        {
            this.PointLength = model.PointLength;
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
            for (int i = 0; i < count; i++)
            {
                var stream = list[i];
                if (stream.IsEmpty()) continue;
                var c = GetGradientColor(i, count, color);
                var fillbrush = new SolidColorBrush(Color.FromArgb(Convert.ToByte(FillColorA * pa), c.R, c.G, c.B));
                fillbrush.Freeze();
                var strokebrush = new SolidColorBrush(Color.FromArgb(Convert.ToByte(StrokeColorA * pa), c.R, c.G, c.B));
                strokebrush.Freeze();
                Brush fill = ShowFill ? fillbrush : null;
                Pen pen = ShowStroke ? new Pen(strokebrush, StrokeThickness) : null;
                dc.DrawGeometry(fill, pen, stream);
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
            return Task.Factory.StartNew<List<StreamGeometry>>(() =>
            {
                var points = PointCountStatistics(token, dataSource.XSource, dataSource.YSource, indexs, dataSource.XSourceConverter, dataSource.YSourceConverter, pointLength, maxDegreeOfParallelism, xParam, yParam, xGetLocation, yGetLocation, excessDisplayAtEdge);
                if (points == null) return null;
                var maxcount = points.Max(p => p.Value.Max(k => k.Value));
                if (maxcount <= 0) return null;
                List<StreamGeometry> streams = new List<StreamGeometry>(maxcount);
                Dictionary<int, StreamGeometryContext> contexts = new Dictionary<int, StreamGeometryContext>();
                for (int i = 1; i <= maxcount; i++)
                {
                    StreamGeometry stream = new StreamGeometry();
                    streams.Add(stream);
                    contexts[i] = stream.Open();
                }
                foreach (var w in points)
                {
                    foreach (var h in w.Value)
                    {
                        if (h.Value > 0)
                        {
                            var sgc = contexts[h.Value];
                            var xtemp = w.Key;
                            var ytemp = h.Key;
                            var point = new Point(xtemp, ytemp);
                            sgc.BeginFigure(point, true, true);
                            sgc.PolyLineTo(new Point[] { new Point(xtemp + pointLength, ytemp), new Point(xtemp + pointLength, ytemp + pointLength), new Point(xtemp, ytemp + pointLength) }, true, true);
                        }
                    }
                }
                foreach (var item in contexts)
                {
                    item.Value.Close();
                    streams[item.Key - 1].Freeze();
                }
                return streams;
            }, token);
        }

        public override BaseSeriesModel GetModel()
        {
            return new PseudoColorSeriesModel()
            {
                ShowFill = this.ShowFill,
                ShowStroke = this.ShowStroke,
                StrokeThickness = this.StrokeThickness,
                FillColorA = this.FillColorA,
                StrokeColorA = this.StrokeColorA,
                ExcessDisplayAtEdge = this.ExcessDisplayAtEdge,
                PointLength = this.PointLength
            };
        }
    }

    public class PseudoColorSeriesModel : BaseSeriesModel
    {
        public int PointLength { get; set; }
    }
}
