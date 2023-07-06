using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_GUI_Cognex.User_Controls
{
    public partial class Uct_Mini_View : UserControl
    {
        private Image m_imgMiniMap;
        private RectangleF m_imgRect;
        private RectangleF m_rectSelectedArea;
        private RectangleF m_rectGuideLineArea;
        private double m_dRatio;
        private Uct_main_View m_MainPic;
        PointF m_MainOrgRectSize;
        PointF m_OrgImgSize;
        RectangleF m_MiniMapOrgSize;

        //public void GetMouseMoveEvent(ref DelegateEvent.delMouseMoveChain delMouseChain)
        //{
        //    delMouseChain += Pic_Mini_MouseMove;
        //}
        //public void GetMouseClickEvent(ref DelegateEvent.delMouseClickChain delMouseChain)
        //{
        //    delMouseChain += Pic_Mini_Click;
        //}
        public PictureBox GetPictureBoxObj()
        {
            return Pic_Mini;
        }
        public RectangleF PassvalueMini
        {
            get { return this.m_rectSelectedArea; }
            set { this.m_rectSelectedArea = value; }
        }

        public void SetVisible(bool value)
        {
            this.Visible = value;
        }
        public Uct_Mini_View()
        {
            try
            {
                InitializeComponent();
                this.Pic_Mini.SizeMode = PictureBoxSizeMode.StretchImage;
                m_rectSelectedArea = new RectangleF(0, 0, 0, 0);
                m_rectGuideLineArea = new RectangleF(0, 0, 0, 0);
                m_MainOrgRectSize = new PointF();
                Pic_Mini.BackColor = Color.Red;
            }
            catch (Exception ex)
            {
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
                throw ex;
            }
        }

        public Bitmap GetMinimapImg()
        {
            try
            {
                return (Bitmap)m_imgMiniMap;
            }
            catch (Exception ex)
            {
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
                throw ex;
            }
        }

        public void Pic_Mini_Paint(object sender, PaintEventArgs e)
        {
            try
            {
                if (m_imgMiniMap != null)
                {
                    e.Graphics.DrawImage(m_imgMiniMap, m_imgRect);

                    Pen pRect = new Pen(Color.FromArgb(10, 255, 255));
                    e.Graphics.DrawRectangle(pRect, m_rectSelectedArea.X, m_rectSelectedArea.Y, m_rectSelectedArea.Width, m_rectSelectedArea.Height);
                }
            }
            catch (Exception ex)
            {
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
                throw ex;
            }
        }
        delegate void SetMiniMapImageCallback(Image imgMiniMap);
        public void m_fnSetMiniMapImage(Image imgMiniMap)
        {

            try
            {
                if (this.InvokeRequired)
                {
                    SetMiniMapImageCallback SetMiniMapImg = new SetMiniMapImageCallback(m_fnSetMiniMapImage);
                    this.Invoke(SetMiniMapImg, imgMiniMap);
                }
                this.m_imgMiniMap = imgMiniMap;
            }
            catch (Exception ex)
            {
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
                throw ex;
            }
        }
        public void picMapResize(Point point)
        {
            try
            {
                int nLocX = Math.Abs(Pic_Mini.Width - point.X) / 2;
                int nLocY = Math.Abs(Pic_Mini.Height - point.Y) / 2;

                this.Width = point.X;
                this.Height = point.Y;
                Pic_Mini.BackColor = Color.Black;
                Pic_Mini.Width = point.X;
                Pic_Mini.Height = point.Y;

                point.X = Math.Abs(this.Location.X + nLocX);
                point.Y = Math.Abs(this.Location.Y + nLocY);
                this.Location = point;
            }
            catch (Exception ex)
            {
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
                throw ex;
            }
        }

        public void ucRefMainImg(ref Uct_main_View MainPic)
        {
            try
            {
                m_MainPic = (Uct_main_View)MainPic;
            }
            catch (Exception ex)
            {
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
                throw ex;
            }
        }

        public void m_fnSetOringSize()
        {
            try
            {
                this.Location = new Point(/*(int)m_MiniMapOrgSize.X*/840, (int)m_MiniMapOrgSize.Y);
                this.Width = (int)m_MiniMapOrgSize.Width;
                this.Height = (int)m_MiniMapOrgSize.Height;
                Pic_Mini.Width = (int)m_MiniMapOrgSize.Width;
                Pic_Mini.Height = (int)m_MiniMapOrgSize.Height;
            }
            catch (Exception ex)
            {
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
                throw ex;
            }
        }

        public void miniMapSize(int Width, int Height, int LocX, int LocY)
        {
            try
            {
                this.Width = Width;
                this.Height = Height;
                Pic_Mini.Width = Width;
                Pic_Mini.Height = Height;
                this.Location = new Point(LocX, LocY);

                m_MiniMapOrgSize = new RectangleF(LocX, LocY, Width, Height);
            }
            catch (Exception ex)
            {
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
                throw ex;
            }
        }


        public void Pic_Mini_Paint(Point pntOriginSize, RectangleF RectFov, int nPicWidth, int nPicHeight, double dRoiRatio)
        {
            try
            {
                if (m_imgMiniMap != null)
                {

                    float nRoiX = Math.Abs(RectFov.X);
                    float nRoiY = Math.Abs(RectFov.Y);
                    float nRoiWidth = RectFov.Width;
                    float nRoiHeight = RectFov.Height;

                    m_MainOrgRectSize.X = (int)nRoiWidth;
                    m_MainOrgRectSize.Y = (int)nRoiHeight;
                    m_OrgImgSize = pntOriginSize;


                    m_imgRect = new Rectangle(0, 0, m_imgMiniMap.Width, m_imgMiniMap.Height);
                    m_dRatio = dRoiRatio;

                    RectangleF adjustedArea = new RectangleF();

                    adjustedArea = CalcRectCoord(RectFov);

                    if (adjustedArea.X < 0) adjustedArea.X = 0;
                    if (adjustedArea.X > Pic_Mini.Width) adjustedArea.X = m_imgMiniMap.Width;
                    if (adjustedArea.Y < 0) adjustedArea.Y = 0;
                    if (adjustedArea.Y > Pic_Mini.Height) adjustedArea.Y = m_imgMiniMap.Height;


                    m_rectSelectedArea = adjustedArea;

                    Pic_Mini.Invalidate();
                }
            }
            catch (Exception ex)
            {
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
                throw ex;
            }
        }

        private RectangleF CalcRectCoord(RectangleF Rect)
        {
            try
            {
                RectangleF adjustedArea = new RectangleF();
                PointF OrgSize = new PointF();

                OrgSize = m_OrgImgSize;

                adjustedArea.X = (m_imgMiniMap.Width * Rect.X / OrgSize.X);
                adjustedArea.Y = (m_imgMiniMap.Height * Rect.Y / OrgSize.Y);
                adjustedArea.Width = (m_imgMiniMap.Width * Rect.Width / OrgSize.X) - 1;
                adjustedArea.Height = (m_imgMiniMap.Height * Rect.Height / OrgSize.Y) - 1;
                return adjustedArea;
            }
            catch (Exception ex)
            {
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
                throw ex;
            }
        }

        private void Pic_Mini_Click(object sender, EventArgs e)
        {
            try
            {
                //Program.Painter.ClearDrawingPolygon();
                PointF mouseLoc = Pic_Mini.PointToClient(Cursor.Position);
                MoveRec(mouseLoc.X, mouseLoc.Y);
            }
            catch (Exception ex)
            {
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
                throw ex;
            }
        }

        private void MoveRec(float X, float Y)
        {
            try
            {
                m_rectSelectedArea.X = (X - (m_rectSelectedArea.Width / 2));
                m_rectSelectedArea.Y = (Y - (m_rectSelectedArea.Height / 2));

                if (m_rectSelectedArea.Left < 0)
                    m_rectSelectedArea.X = 0;
                else if (m_rectSelectedArea.Right > Pic_Mini.Width)
                    m_rectSelectedArea.X = Pic_Mini.Width - m_rectSelectedArea.Width - 1;

                if (m_rectSelectedArea.Top < 0)
                    m_rectSelectedArea.Y = 0;
                else if (m_rectSelectedArea.Bottom > Pic_Mini.Height)
                    m_rectSelectedArea.Y = Pic_Mini.Height - m_rectSelectedArea.Height - 2;
                m_MainPic.m_fnMiniMapMove(m_imgMiniMap, m_rectSelectedArea);
            }
            catch (Exception ex)
            {
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
                throw ex;
            }
        }

        private void Pic_Mini_MouseMove(object sender, MouseEventArgs e)
        {
            try
            {
                if (m_imgMiniMap != null && e.Button == MouseButtons.Left)
                {
                   // Program.frm_Layers.UnSelectedObject();
                    Point mouseLoc = Pic_Mini.PointToClient(Cursor.Position);
                    MoveRec(mouseLoc.X, mouseLoc.Y);
                   // Program.frm_Main.LayerBoard.GetPictureBox().Invalidate();
                }
            }
            catch (Exception ex)
            {
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
                throw ex;
            }
        }
        public RectangleF GetImageRect()
        {
            return m_imgRect;
        }
    }
}
