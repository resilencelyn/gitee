using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;

namespace FCSVisualChart
{
    /// <summary>
    /// 散点图
    /// </summary>
    public class ScatterSeries : ISeries
    {
        /// <summary>
        /// 点的宽度
        /// </summary>
        public int PointLength
        {
            get { return (int)GetValue(PointLengthProperty); }
            set { SetValue(PointLengthProperty, value); }
        }
        public static readonly DependencyProperty PointLengthProperty = DependencyProperty.Register("PointLength", typeof(int), typeof(ScatterSeries), new PropertyMetadata(2, (d, e) => { if (d is ISeries series) series.Draw(); }));

        public ScatterSeries() : base() { }
        public ScatterSeries(ScatterSeriesModel model) : base(model)
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
            var fillbrush = new SolidColorBrush(Color.FromArgb(Convert.ToByte(FillColorA * pa), color.R, color.G, color.B));
            fillbrush.Freeze();
            var strokebrush = new SolidColorBrush(Color.FromArgb(Convert.ToByte(StrokeColorA * pa), color.R, color.G, color.B));
            strokebrush.Freeze();
            Brush fill = ShowFill ? fillbrush : null;
            Pen pen = ShowStroke ? new Pen(strokebrush, StrokeThickness) : null;
            foreach (var stream in list)
            {
                if (stream.IsEmpty()) continue;
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
                StreamGeometry stream = new StreamGeometry();
                using (StreamGeometryContext sgc = stream.Open())
                {
                    foreach (var w in points)
                    {
                        foreach (var h in w.Value)
                        {
                            if (h.Value > 0)
                            {
                                var xtemp = w.Key;
                                var ytemp = h.Key;
                                var point = new Point(xtemp, ytemp);
                                sgc.BeginFigure(point, true, true);
                                sgc.PolyLineTo(new Point[] { new Point(xtemp + pointLength, ytemp), new Point(xtemp + pointLength, ytemp + pointLength), new Point(xtemp, ytemp + pointLength) }, true, true);
                            }
                        }
                    }
                }
                stream.Freeze();
                return new List<StreamGeometry>() { stream };
            }, token);
        }

        public override BaseSeriesModel GetModel()
        {
            return new ScatterSeriesModel()
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
    public class ScatterSeriesModel : BaseSeriesModel
    {
        public int PointLength { get; set; }
    }
}
