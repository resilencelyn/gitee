using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;

namespace FCSVisualChart
{
    /// <summary>
    /// 横线门
    /// </summary>
    public class LineHorizontalGate : BaseGate
    {
        #region 数据
        /// <summary>
        /// 横线的y真实值
        /// </summary>
        public double Y { get; private set; }
        #endregion
        public LineHorizontalGate() : base() { }
        public LineHorizontalGate(LineHorizontalGateModel model) : base(model)
        {
            this.Y = model.Y;
        }
        /// <summary>
        /// 初始化
        /// </summary>
        protected override void Init()
        {
            this.ShortName = "H";
            this.AreaCount = 2;
            base.Init();
        }

        #region 门创建
        internal override void Panel_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {

        }
        internal override void Panel_MouseMove(object sender, MouseEventArgs e)
        {
            if (sender is Panel panel)
            {
                if (IsCreating)
                {
                    var point = e.GetPosition(panel);
                    this.Y = OwnerChart.YAxis.GetLocationValue(point.Y);
                    Draw();
                }
                else if (IsEditing && EditingPath != null && e.LeftButton == MouseButtonState.Pressed)
                {
                    var point = e.GetPosition(panel);
                    this.Y = OwnerChart.YAxis.GetLocationValue(point.Y);
                    Draw();
                }
                else if (IsMoving && e.LeftButton == MouseButtonState.Pressed)
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
            var tempy = OwnerChart.YAxis.GetValueLocation(Y);
            var tempx1 = 0;
            var tempx2 = OwnerChart.ViewPanel.ActualWidth;
            GeometryGroup group = new GeometryGroup();
            #region 图形
            var geometry = new StreamGeometry();
            using (var sgc = geometry.Open())
            {
                sgc.BeginFigure(new Point(tempx1, tempy), false, false);
                sgc.LineTo(new Point(tempx2, tempy), true, true);
            }
            geometry.Freeze();
            group.Children.Add(geometry);
            #endregion
            #region 区域名称
            group.Children.Add(CreateFormattedTextGeometry(Areas[0].Name, OwnerChart.ViewPanel.ActualWidth / 2, tempy - 15, 0.5d, 1d));
            group.Children.Add(CreateFormattedTextGeometry(Areas[1].Name, OwnerChart.ViewPanel.ActualWidth / 2, tempy + 15, 0.5d, 0d));
            #endregion
            group.Freeze();
            return group;
        }

        #endregion

        #region 门控制
        /// <summary>
        /// 创建门的控制按钮
        /// </summary>
        protected override void DrawControl()
        {
            var tempx = OwnerChart.XAxis.ActualWidth / 2;
            var tempy = OwnerChart.YAxis.GetValueLocation(this.Y);
            var g = CreateControlGeometry(new Point(tempx, tempy));
            if (ControlPaths.Count == 0)
            {
                ControlPaths.Add(new Path() { Data = g, Cursor = Cursors.SizeNS });
            }
            else
            {
                ControlPaths[0].Data = g;
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
                this.Y = OwnerChart.YAxis.GetLocationValue(OwnerChart.YAxis.GetValueLocation(this.Y) + y);
            }
            Draw();
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
            return (param is LineHorizontalAreaIndexsFuncParam p && y >= p.Y) ? param.Areas[0] : param.Areas[1];
        }
        /// <summary>
        /// 获取判断点是否在门区域内的方法的参数
        /// </summary>
        /// <returns></returns>
        public override AreaIndexsFuncParam GetAreaIndexsFuncParam()
        {
            if (Areas == null || Areas.Length != 2) return null;
            return new LineHorizontalAreaIndexsFuncParam()
            {
                Areas = this.Areas,
                Y = this.Y
            };
        }
        #endregion

        /// <summary>
        /// 获取门的数据模型
        /// </summary>
        /// <returns></returns>
        public override BaseGateModel GetModel()
        {
            return new LineHorizontalGateModel()
            {
                AreaNames = this.Areas.Select(p => p.Name).ToArray(),
                AreaColors = this.Areas.Select(p => string.Concat(p.DisplayColor.A.ToString("X2"), p.DisplayColor.R.ToString("X2"), p.DisplayColor.G.ToString("X2"), p.DisplayColor.B.ToString("X2"))).ToArray(),
                Y = this.Y
            };
        }
    }

    public class LineHorizontalGateModel : BaseGateModel
    {
        public double Y { get; set; }
    }

    public class LineHorizontalAreaIndexsFuncParam : AreaIndexsFuncParam
    {
        internal double Y { get; set; }
    }
}
