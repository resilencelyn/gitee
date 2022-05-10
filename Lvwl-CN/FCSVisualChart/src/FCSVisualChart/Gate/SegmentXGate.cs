using System;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;

namespace FCSVisualChart
{
    /// <summary>
    /// X轴段选门
    /// </summary>
    public class SegmentXGate : BaseGate
    {
        #region 数据
        public double X1 { get; private set; }
        public double X2 { get; private set; }
        public double Y { get; private set; }
        #endregion
        public SegmentXGate() : base() { }
        public SegmentXGate(SegmentXGateModel model) : base(model)
        {
            this.X1 = model.X1;
            this.X2 = model.X2;
            this.Y = model.Y;
        }
        /// <summary>
        /// 初始化
        /// </summary>
        protected override void Init()
        {
            ShortName = "S";
            base.Init();
        }

        #region 门创建
        internal override void Panel_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (IsCreating && sender is Panel panel)
            {
                var point = e.GetPosition(panel);
                var x = OwnerChart.XAxis.GetLocationValue(point.X);
                this.X1 = this.X2 = x;
                this.Y = point.Y;
                Draw();
            }
        }
        internal override void Panel_MouseMove(object sender, MouseEventArgs e)
        {
            if (sender is Panel panel && e.LeftButton == MouseButtonState.Pressed)
            {
                if (IsCreating)
                {
                    var point = e.GetPosition(panel);
                    var x = OwnerChart.XAxis.GetLocationValue(point.X);
                    X2 = x;
                    Y = point.Y;
                    Draw();
                }
                else if (IsEditing && EditingPath != null)
                {
                    var point = e.GetPosition(panel);
                    var x = OwnerChart.XAxis.GetLocationValue(point.X);
                    var index = ControlPaths.IndexOf(EditingPath);
                    this.Y = point.Y;
                    if (index == 0) this.X1 = x;
                    else this.X2 = x;
                    Draw();
                }
                else if (IsMoving)
                {
                    GatePath_MouseMove(GatePath, e);
                }
            }
        }
        internal override void Panel_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (IsCreating)
            {
                if (X1 == X2) return;
                IsCreating = false;
            }
            else if (IsEditing)
            {
                IsEditing = false;
                EditingPath = null;
                RefreshAreaIndexs();
            }
            else if (IsMoving)
            {
                IsMoving = false;
                GateMoveStartPoint = default;
                Draw();
            }
        }
        #endregion

        #region 门绘制
        /// <summary>
        /// 创建门的矢量数据
        /// </summary>
        /// <returns></returns>
        protected override Geometry CreateGatePathData()
        {
            if (Y <= 0 || Y >= OwnerChart.ViewPanel.ActualHeight) Y = OwnerChart.ViewPanel.ActualHeight / 2;
            var p = OwnerChart.XAxis.GetConvertParam();
            var tempx1 = OwnerChart.XAxis.GetValueLocation(X1, p);
            var tempx2 = OwnerChart.XAxis.GetValueLocation(X2, p);
            var height = 10;
            GeometryGroup group = new GeometryGroup();
            #region 图形
            var geometry = new StreamGeometry();
            using (var sgc = geometry.Open())
            {
                sgc.BeginFigure(new Point(tempx1, Y - height), false, false);
                sgc.LineTo(new Point(tempx1, Y + height), true, true);
                sgc.BeginFigure(new Point(tempx1, Y), false, false);
                sgc.LineTo(new Point(tempx2, Y), true, true);
                sgc.BeginFigure(new Point(tempx2, Y - height), false, false);
                sgc.LineTo(new Point(tempx2, Y + height), true, true);
            }
            geometry.Freeze();
            group.Children.Add(geometry);
            #endregion
            #region 区域名称
            group.Children.Add(CreateFormattedTextGeometry(Areas[0].Name, (tempx1 + tempx2) / 2, Y - 5, 0.5d, 1d));
            #endregion
            group.Freeze();
            return group;
        }
        #endregion

        #region 门控制
        /// <summary>
        /// 绘制控制点
        /// </summary>
        protected override void DrawControl()
        {
            var p = OwnerChart.XAxis.GetConvertParam();
            var tempx1 = OwnerChart.XAxis.GetValueLocation(X1, p);
            var tempx2 = OwnerChart.XAxis.GetValueLocation(X2, p);
            var g1 = CreateControlGeometry(new Point(tempx1, Y));
            var g2 = CreateControlGeometry(new Point(tempx2, Y));
            if (ControlPaths.Count == 0)
            {
                ControlPaths.Add(new Path() { Data = g1, Cursor = Cursors.SizeWE });
                ControlPaths.Add(new Path() { Data = g2, Cursor = Cursors.SizeWE });
            }
            else
            {
                ControlPaths[0].Data = g1;
                ControlPaths[1].Data = g2;
            }
        }
        #endregion

        #region 门移动
        /// <summary>
        /// 移动门位置
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="changeAxisValue">是否变更了坐标值</param>
        internal override void Move(double x, double y, bool changeAxisValue = false)
        {
            if (changeAxisValue)
            {
                var p = OwnerChart.XAxis.GetConvertParam();
                this.X1 = OwnerChart.XAxis.GetLocationValue(OwnerChart.XAxis.GetValueLocation(X1, p) + x, p);
                this.X2 = OwnerChart.XAxis.GetLocationValue(OwnerChart.XAxis.GetValueLocation(X2, p) + x, p);
            }
            this.Y = this.Y + y;
            if (GatePath != null && GatePath.RenderTransform is MatrixTransform transform)
            {
                var matrix = transform.Matrix;
                transform.Matrix = new Matrix(1, 0, 0, 1, matrix.OffsetX + x, matrix.OffsetY + y);
            }
            DrawControl();
        }
        #endregion

        #region 判断数据是否在区域内
        /// <summary>
        /// 数据在门的哪个区域内
        /// </summary>
        /// <param name="x">x轴真实值</param>
        /// <param name="y">y轴真实值</param>
        /// <param name="param">数据在区域内判断方法的参数</param>
        /// <returns>null表示不在任何区域内</returns>
        public override GateArea DataInArea(double x, double y, AreaIndexsFuncParam param)
        {
            return (param is SegmentXAreaIndexsFuncParam p && x >= p.MinX && x <= p.MaxX) ? param.Areas[0] : null;
        }
        /// <summary>
        /// 获取判断点是否在门区域内的方法的参数
        /// </summary>
        /// <returns></returns>
        public override AreaIndexsFuncParam GetAreaIndexsFuncParam()
        {
            if (Areas == null || Areas.Length != 1) return null;
            return new SegmentXAreaIndexsFuncParam()
            {
                MinX = Math.Min(X1, X2),
                MaxX = Math.Max(X1, X2),
                Areas = this.Areas
            };
        }
        #endregion

        /// <summary>
        /// 获取门的数据模型
        /// </summary>
        /// <returns></returns>
        public override BaseGateModel GetModel()
        {
            return new SegmentXGateModel()
            {
                AreaNames = this.Areas.Select(p => p.Name).ToArray(),
                AreaColors = this.Areas.Select(p => string.Concat(p.DisplayColor.A.ToString("X2"), p.DisplayColor.R.ToString("X2"), p.DisplayColor.G.ToString("X2"), p.DisplayColor.B.ToString("X2"))).ToArray(),
                X1 = this.X1,
                X2 = this.X2,
                Y = this.Y
            };
        }
    }

    public class SegmentXGateModel : BaseGateModel
    {
        public double X1 { get; set; }
        public double X2 { get; set; }
        public double Y { get; set; }
    }

    public class SegmentXAreaIndexsFuncParam : AreaIndexsFuncParam
    {
        internal double MinX { get; set; }
        internal double MaxX { get; set; }
    }
}
