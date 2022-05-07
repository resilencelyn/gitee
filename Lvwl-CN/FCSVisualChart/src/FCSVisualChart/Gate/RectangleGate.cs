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
    /// 矩形门
    /// </summary>
    public class RectangleGate : BaseGate
    {
        #region 数据
        public double X1 { get; private set; }
        public double X2 { get; private set; }
        public double Y1 { get; private set; }
        public double Y2 { get; private set; }
        #endregion
        public RectangleGate() { }
        public RectangleGate(RectangleGateModel model) : base(model)
        {
            this.X1 = model.X1;
            this.X2 = model.X2;
            this.Y1 = model.Y1;
            this.Y2 = model.Y2;
        }
        /// <summary>
        /// 初始化
        /// </summary>
        protected override void Init()
        {
            this.ShortName = "R";
            base.Init();
        }

        #region 门创建
        internal override void Panel_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (IsCreating && sender is Panel panel)
            {
                var point = e.GetPosition(panel);
                this.X1 = OwnerChart.XAxis.GetLocationValue(point.X);
                this.Y1 = OwnerChart.YAxis.GetLocationValue(point.Y);
            }
        }
        internal override void Panel_MouseMove(object sender, MouseEventArgs e)
        {
            if (sender is Panel panel && e.LeftButton == MouseButtonState.Pressed)
            {
                if (IsCreating)
                {
                    var point = e.GetPosition(panel);
                    this.X2 = OwnerChart.XAxis.GetLocationValue(point.X);
                    this.Y2 = OwnerChart.YAxis.GetLocationValue(point.Y);
                    Draw();
                }
                else if (IsEditing && EditingPath != null)
                {
                    var point = e.GetPosition(panel);
                    var tempx = OwnerChart.XAxis.GetLocationValue(point.X);
                    var tempy = OwnerChart.YAxis.GetLocationValue(point.Y);
                    var index = ControlPaths.IndexOf(EditingPath);
                    switch (index)
                    {
                        case 0:
                            this.X1 = tempx;
                            this.Y1 = tempy;
                            break;
                        case 1:
                            this.X2 = tempx;
                            this.Y1 = tempy;
                            break;
                        case 2:
                            this.X2 = tempx;
                            this.Y2 = tempy;
                            break;
                        case 3:
                            this.X1 = tempx;
                            this.Y2 = tempy;
                            break;
                        default:
                            break;
                    }
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
            var px = OwnerChart.XAxis.GetConvertParam();
            var py = OwnerChart.YAxis.GetConvertParam();
            var tempx1 = OwnerChart.XAxis.GetValueLocation(this.X1, px);
            var tempx2 = OwnerChart.XAxis.GetValueLocation(this.X2, px);
            var tempy1 = OwnerChart.YAxis.GetValueLocation(this.Y1, py);
            var tempy2 = OwnerChart.YAxis.GetValueLocation(this.Y2, py);

            GeometryGroup group = new GeometryGroup();
            #region 图形
            var geometry = new StreamGeometry();
            using (var sgc = geometry.Open())
            {
                sgc.BeginFigure(new Point(tempx1, tempy1), true, true);
                sgc.PolyLineTo(new Point[] { new Point(tempx2, tempy1), new Point(tempx2, tempy2), new Point(tempx1, tempy2) }, true, true);
            }
            geometry.Freeze();
            group.Children.Add(geometry);
            #endregion
            #region 区域名称
            group.Children.Add(CreateFormattedTextGeometry(Areas[0].Name, (tempx1 + tempx2) / 2, (tempy1 + tempy2) / 2));
            #endregion
            group.Freeze();
            return group;
        }
        #endregion

        #region 门控制
        protected override void DrawControl()
        {
            var px = OwnerChart.XAxis.GetConvertParam();
            var py = OwnerChart.YAxis.GetConvertParam();
            var tempx1 = OwnerChart.XAxis.GetValueLocation(this.X1, px);
            var tempx2 = OwnerChart.XAxis.GetValueLocation(this.X2, px);
            var tempy1 = OwnerChart.YAxis.GetValueLocation(this.Y1, py);
            var tempy2 = OwnerChart.YAxis.GetValueLocation(this.Y2, py);
            var g1 = CreateControlGeometry(new Point(tempx1, tempy1));
            var g2 = CreateControlGeometry(new Point(tempx2, tempy1));
            var g3 = CreateControlGeometry(new Point(tempx2, tempy2));
            var g4 = CreateControlGeometry(new Point(tempx1, tempy2));
            if (ControlPaths.Count == 0)
            {
                ControlPaths.Add(new Path() { Data = g1, Cursor = Cursors.SizeAll });
                ControlPaths.Add(new Path() { Data = g2, Cursor = Cursors.SizeAll });
                ControlPaths.Add(new Path() { Data = g3, Cursor = Cursors.SizeAll });
                ControlPaths.Add(new Path() { Data = g4, Cursor = Cursors.SizeAll });
            }
            else
            {
                ControlPaths[0].Data = g1;
                ControlPaths[1].Data = g2;
                ControlPaths[2].Data = g3;
                ControlPaths[3].Data = g4;
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
                var px = OwnerChart.XAxis.GetConvertParam();
                var py = OwnerChart.YAxis.GetConvertParam();
                Func<double, ValueLocationConvertParam, double> fx = OwnerChart.XAxis.GetValueLocation;
                Func<double, ValueLocationConvertParam, double> fy = OwnerChart.YAxis.GetValueLocation;
                Func<double, ValueLocationConvertParam, double> fcx = OwnerChart.XAxis.GetLocationValue;
                Func<double, ValueLocationConvertParam, double> fcy = OwnerChart.YAxis.GetLocationValue;
                this.X1 = fcx(fx(this.X1, px) + x, px);
                this.X2 = fcx(fx(this.X2, px) + x, px);
                this.Y1 = fcy(fy(this.Y1, py) + y, py);
                this.Y2 = fcy(fy(this.Y2, py) + y, py);
            }
            if (GatePath != null && GatePath.RenderTransform is MatrixTransform transform)
            {
                var matrix = transform.Matrix;
                transform.Matrix = new Matrix(1, 0, 0, 1, matrix.OffsetX + x, matrix.OffsetY + y);
            }
            if (!IsCreating) DrawControl();
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
            if (param is RectangleAreaIndexsFuncParam p)
            {
                if (x < p.X1 || x > p.X2 || y < p.Y1 || y > p.Y2) return null;
                return p.Areas[0];
            }
            return null;
        }
        /// <summary>
        /// 获取判断点是否在门区域内的方法的参数
        /// </summary>
        /// <returns></returns>
        public override AreaIndexsFuncParam GetAreaIndexsFuncParam()
        {
            if (Areas == null || Areas.Length != 1) return null;
            return new RectangleAreaIndexsFuncParam()
            {
                Areas = this.Areas,
                X1 = Math.Min(this.X1, this.X2),
                X2 = Math.Max(this.X1, this.X2),
                Y1 = Math.Min(this.Y1, this.Y2),
                Y2 = Math.Max(this.Y1, this.Y2)
            };
        }
        #endregion

        /// <summary>
        /// 获取门的数据模型
        /// </summary>
        /// <returns></returns>
        public override BaseGateModel GetModel()
        {
            return new RectangleGateModel()
            {
                AreaNames = this.Areas.Select(p => p.Name).ToArray(),
                X1 = Math.Min(this.X1, this.X2),
                X2 = Math.Max(this.X1, this.X2),
                Y1 = Math.Min(this.Y1, this.Y2),
                Y2 = Math.Max(this.Y1, this.Y2)
            };
        }
    }

    public class RectangleGateModel : BaseGateModel
    {
        public double X1 { get; set; }
        public double X2 { get; set; }
        public double Y1 { get; set; }
        public double Y2 { get; set; }
    }

    public class RectangleAreaIndexsFuncParam : AreaIndexsFuncParam
    {
        internal double X1 { get; set; }
        internal double X2 { get; set; }
        internal double Y1 { get; set; }
        internal double Y2 { get; set; }
    }
}
