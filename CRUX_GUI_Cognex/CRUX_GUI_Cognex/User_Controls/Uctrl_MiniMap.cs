using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.Drawing.Imaging;
using CRUX_Renewal.Class;

namespace CRUX_Renewal.User_Controls
{
    public partial class Uctrl_MiniMap : UserControl
    {

        //LIVE 
        ////////////////////////////////////////           
        Thread m_thread;
        private int m_nVisionNo = 0;
        private bool m_bIsDoWork;
        private bool _threadFlg;
        private Bitmap m_imgLive;        
        //test
        private Point m_PixelSize = new Point(6576, 4384);
        ////////////////////////////////////////
        Point m_OrgImgSize;
        Point m_MainOrgRectSize;
        Rectangle m_MiniMapOrgSize;
        private Image m_imgMiniMap;
        private double m_dRatio;
        private Dictionary<int, struPoint> m_DicPnt;
        private Dictionary<int, struRect> m_DicRoi;        
        private Rectangle m_imgRect; 
        private Rectangle m_rectSelectedArea;
        private Rectangle m_rectGuideLineArea;
        private Color m_GuideLineColor;
        private Color m_selectionColor;
        private Uctrl_MainPic m_MainPic;
        private double m_dGuideLineOffsetT = 0.0F;
        private int  m_nImageLoadFlag;
        private bool m_bIsLive;
        private bool m_bIsGrayScale;
        private int  m_nPxlFormatNo;
        PixelFormat m_PxlFormat;
        private int m_nCamIndex;
        public Uctrl_MiniMap(int nVisionNo)
        {
           // m_MainPic = (Uctrl_MainPic)MainPic;  
            m_nVisionNo = nVisionNo;
            InitializeComponent();
            this.picMinImg.SizeMode = PictureBoxSizeMode.StretchImage;
            m_rectSelectedArea = new Rectangle(0, 0, 0, 0);
            m_rectGuideLineArea = new Rectangle(0, 0, 0, 0);
            m_MainOrgRectSize = new Point();
            m_selectionColor = Color.FromArgb(128, 255, 255);
            m_GuideLineColor = Color.FromArgb(128, 255, 255);
        }

        public void ucRefMainImg(ref Uctrl_MainPic MainPic)
        {
            m_MainPic = (Uctrl_MainPic)MainPic;          
        }
        public void miniMapSize(int Width, int Height, int LocX, int LocY)
        {
            this.Width = Width;
            this.Height = Height;
            picMinImg.Width = Width;
            picMinImg.Height = Height;
            this.Location = new Point(LocX, LocY);
            //picMinImg.Location =  new Point(LocX, LocY);
            //pnlLoading.Width = Width;
           // pnlLoading.Height = Height;
          
            m_MiniMapOrgSize = new Rectangle(LocX, LocY, Width, Height);
            //lbLoading.Location = new Point((this.Width - lbLoading.Width) / 2, (this.Height - lbLoading.Height) / 2);
        }

        delegate void SetMiniMapImageCallback(Image imgMiniMap);
        public void m_fnSetMiniMapImage(Image imgMiniMap)
        {
            //if (m_imgMiniMap != null)
            //{
            if (this.InvokeRequired)
            {
                SetMiniMapImageCallback SetMiniMapImg = new SetMiniMapImageCallback(m_fnSetMiniMapImage);
                this.Invoke(SetMiniMapImg, imgMiniMap);               
            }
               this.m_imgMiniMap = imgMiniMap;
            //}
        }

        delegate void LiveStartCallback(int nImageLoadFlag, bool bIsLive, bool bIsGrayScale, int nPxlFormatNo, PixelFormat PxlFormat, Point PixelSize, int  nCamIndex);
        public void LiveStart(int nImageLoadFlag, bool bIsLive, bool bIsGrayScale, int nPxlFormatNo, PixelFormat PxlFormat, Point PixelSize, int nCamIndex)
        {
            try
            {
               
                if (this.InvokeRequired)
                {

                    LiveStartCallback delOpenImg = new LiveStartCallback(LiveStart);
                    this.Invoke(delOpenImg, nImageLoadFlag, bIsLive, bIsGrayScale, nPxlFormatNo, PxlFormat, PixelSize, nCamIndex);
                    return;
                }
                m_nCamIndex = nCamIndex;
                m_nImageLoadFlag = nImageLoadFlag;
                m_bIsLive = bIsLive;
                m_bIsGrayScale = bIsGrayScale;
                m_nPxlFormatNo = nPxlFormatNo;
                m_PxlFormat = PxlFormat;
                m_PixelSize = PixelSize;
                m_imgRect = new Rectangle(0, 0, 0, 0);
            
                m_imgRect.Width = m_PixelSize.X;
                m_imgRect.Height = m_PixelSize.Y;
                _threadFlg = true;
                m_bIsDoWork = true;

                m_imgLive = new Bitmap(picMinImg.Width, picMinImg.Height);           

                Thread thread = new Thread(new ThreadStart(delegate() { ThreadImagLoad(); }));
                thread.IsBackground = true;
                m_thread = thread;
                thread.Start();

                picMinImg.Invalidate();
            }
            catch (Exception)
            {
                //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E091", Globals.nLanguageFlg), true);
            }
        }
//         public bool ThumbnailCallback()
//         {
//             return false;
//         }
        delegate void ThreadImagLoadCallback();
        public void ThreadImagLoad()
        {
            try
            {
                int nRet = Consts.APP_NG;

                while (_threadFlg)
                {
                    if (m_bIsDoWork)
                    {
                        if (this.InvokeRequired)
                        {
                            int nWidth = picMinImg.Width / 4 * 4;
                            int nHeight = picMinImg.Height / 4 * 4;
                            CmdMsgParam Param = new CmdMsgParam();
                            Byte[] dat = new Byte[nWidth * nHeight * m_nPxlFormatNo];
                               
                            Param.SetParam(dat);
                            Param.SetInteger(m_nImageLoadFlag);
                            Param.SetInteger(Convert.ToInt32(m_bIsLive));                                     //int   : 카메라 영상 1 , BMP 이미지 호출 0
                            //Param.SetInteger(Convert.ToInt32(m_bIsGrayScale));                                   //int   : RGB 0 , RAW 1
                            Param.SetUInteger(Convert.ToUInt32(Math.Abs(m_imgRect.X)));
                            Param.SetUInteger(Convert.ToUInt32(Math.Abs(m_imgRect.Y)));
                            Param.SetUInteger(Convert.ToUInt32(m_PixelSize.X));
                            Param.SetUInteger(Convert.ToUInt32(m_PixelSize.Y));
                            Param.SetUInteger(Convert.ToUInt32(nWidth));
                            Param.SetUInteger(Convert.ToUInt32(nHeight));
                            if (m_imgRect.Width <= 0 || m_imgRect.Height <= 0 || nWidth <= 0 || nHeight <= 0)
                            {
                                System.Diagnostics.Debug.WriteLine("{0} // {1} // {2} // {3}", m_imgRect.Width, m_imgRect.Height, nWidth, nHeight);
                            }
                            nRet = Systems.g_Ipc.SendCommand((ushort)((m_nVisionNo + 1) * 100 + IpcConst.CAMERA_TASK + m_nCamIndex), IpcConst.CAMERA_FUNC, IpcConst.LIVE_REQ,
                                                                IpcInterface.CMD_TYPE_RES, 500 /*IpcInterface.CMD_TIMEOUT*/, Param.GetByteSize(), Param.GetParam(), (int)Enums.CompressLevel.NONE, 1000);
                            if (nRet == Consts.APP_OK)
                            {

                                this.m_imgMiniMap = (Bitmap)Utils.ImageProc.ConvertBitmap(Param.GetParam(), nWidth, nHeight, m_bIsGrayScale, m_PxlFormat);
                                                                 
                                picMinImg.BeginInvoke(new Action(() => picMinImg.Invalidate()));

                                System.Diagnostics.Debug.WriteLine(DateTime.Now);
                            }                            
                            Param.ClearOffset();
                        }
                    }
                    Thread.Sleep(500);
                }
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
                //Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
            }
        }

        public void m_fnSetOringSize()
        {
         
            this.Location = new Point(m_MiniMapOrgSize.X, m_MiniMapOrgSize.Y);
            this.Width = m_MiniMapOrgSize.Width;
            this.Height = m_MiniMapOrgSize.Height;
            picMinImg.Width = m_MiniMapOrgSize.Width;
            picMinImg.Height = m_MiniMapOrgSize.Height;


            //pnlLoading.Visible = true;
            //lbLoading.Visible = true;
//             pnlLoading.Width = picMinImg.Width;
//             pnlLoading.Height = picMinImg.Height;
//             pnlLoading.Location = picMinImg.Location;
            //lbLoading.Location = new Point((this.Width - lbLoading.Width) / 2, (this.Height - lbLoading.Height) / 2);
           // picMinImg.Location = new Point(m_MiniMapOrgSize.X, m_MiniMapOrgSize.Y);
            
        }
        public void GetOriginImage()
        {
            picMinImg.Image = m_imgMiniMap;            
        }

        public void picMapResize(Point point)
        {

            int nLocX = Math.Abs(picMinImg.Width - point.X) / 2;
            int nLocY = Math.Abs(picMinImg.Height - point.Y) / 2;

            this.Width = point.X;
            this.Height = point.Y;
            picMinImg.Width = point.X;
            picMinImg.Height = point.Y;

            point.X = Math.Abs(this.Location.X + nLocX);
            point.Y = Math.Abs(this.Location.Y + nLocY);
            this.Location = point;
        }

        public void m_fnDrawGuideLine(/*Image imgMiniMap,*/ Point pntOriginSize, Rectangle RectFov, double dGuideLineOffsetT,
                                   bool bIsLive)
        {
            if (m_imgMiniMap != null)
            {
                //pnlLoading.Visible = false;
                // lbLoading.Visible = false;
                int nRoiX = Math.Abs(RectFov.X), nRoiY = Math.Abs(RectFov.Y), nRoiWidth = RectFov.Width, nRoiHeight = RectFov.Height;
  
                m_MainOrgRectSize.X = nRoiWidth;
                m_MainOrgRectSize.Y = nRoiHeight;
                m_OrgImgSize = pntOriginSize;
                //m_bLiveFlg = bIsLive;
                //this.m_imgMiniMap = imgMiniMap;
                m_dGuideLineOffsetT = dGuideLineOffsetT;
                m_imgRect = new Rectangle(0, 0, m_imgMiniMap.Width, m_imgMiniMap.Height);
             
                Rectangle adjustedArea = new Rectangle();
                //dRoiRatio = 0.125F;
                if (bIsLive)
                    adjustedArea = CalcRectCoord(RectFov);


                if (adjustedArea.X < 0) adjustedArea.X = 0;
                if (adjustedArea.X > picMinImg.Width) adjustedArea.X = m_imgMiniMap.Width;
                if (adjustedArea.Y < 0) adjustedArea.Y = 0;
                if (adjustedArea.Y > picMinImg.Height) adjustedArea.Y = m_imgMiniMap.Height;
                m_rectGuideLineArea = adjustedArea;

                picMinImg.Invalidate();
            }
        }


        public void picMinImg_Paint(/*Image imgMiniMap,*/Point pntOriginSize, Rectangle RectFov, Dictionary<int, struPoint> dicPnt,
                                    Dictionary<int, struRect> dicRoi, int nPicWidth, int nPicHeight, double dRoiRatio, bool bIsLive)     
        {
            if (m_imgMiniMap != null)
            {
                //pnlLoading.Visible = false;
               // lbLoading.Visible = false;
                int nRoiX = Math.Abs(RectFov.X), nRoiY = Math.Abs(RectFov.Y), nRoiWidth = RectFov.Width, nRoiHeight = RectFov.Height;
                m_DicPnt = dicPnt;
                m_DicRoi = dicRoi; //m_lstRoiRect = dicRoi;
                m_MainOrgRectSize.X = nRoiWidth;
                m_MainOrgRectSize.Y = nRoiHeight;
                m_OrgImgSize = pntOriginSize;
                m_bIsDoWork = bIsLive;
                //this.m_imgMiniMap = imgMiniMap;
                                
                m_imgRect = new Rectangle(0, 0, m_imgMiniMap.Width, m_imgMiniMap.Height);
                m_dRatio = dRoiRatio;
          
                Rectangle adjustedArea = new Rectangle();
                //dRoiRatio = 0.125F;

                if (bIsLive)
                {
                    adjustedArea = CalcRectCoord(RectFov);
                    
//                     adjustedArea.X = (imgMiniMap.Width * nRoiX / pntOriginSize.X);
//                     adjustedArea.Y = (imgMiniMap.Height * nRoiY / pntOriginSize.Y);
// 
//                     adjustedArea.Width = (imgMiniMap.Width * nRoiWidth / pntOriginSize.X);
//                     adjustedArea.Height = (imgMiniMap.Height * nRoiHeight / pntOriginSize.Y);

                }
                else
                {
                    adjustedArea.X = (int)Math.Round(m_imgMiniMap.Width * nRoiX / dRoiRatio / nRoiWidth);
                    adjustedArea.Y = (int)Math.Round(m_imgMiniMap.Height * nRoiY / dRoiRatio / nRoiHeight);

                    adjustedArea.Width = (int)Math.Round(m_imgMiniMap.Width * nPicWidth / nRoiWidth / dRoiRatio);
                    adjustedArea.Height = (int)Math.Round(m_imgMiniMap.Height * nPicHeight / nRoiHeight / dRoiRatio);

                }
             
                if (adjustedArea.X < 0) adjustedArea.X = 0;
                if (adjustedArea.X > picMinImg.Width) adjustedArea.X = m_imgMiniMap.Width;
                if (adjustedArea.Y < 0) adjustedArea.Y = 0;
                if (adjustedArea.Y > picMinImg.Height) adjustedArea.Y = m_imgMiniMap.Height;
                m_rectSelectedArea = adjustedArea;
             
                picMinImg.Invalidate();             
            }
        }
        
   
        private void picMinImg_MouseMove(object sender, MouseEventArgs e)
        {
            if (m_imgMiniMap != null && e.Button == MouseButtons.Left) //이미지 이동
            {
                Point mouseLoc = picMinImg.PointToClient(Cursor.Position);
                MoveRec(mouseLoc.X, mouseLoc.Y);
            }
            
        }

        private void picMinImg_Click(object sender, EventArgs e)
        {
            Point mouseLoc = picMinImg.PointToClient(Cursor.Position);
            MoveRec(mouseLoc.X, mouseLoc.Y);
        }

        private void MoveRec(int X, int Y)
        {
            m_rectSelectedArea.X = (X - (m_rectSelectedArea.Width / 2 ));
            m_rectSelectedArea.Y = (Y - (m_rectSelectedArea.Height / 2 ));
            
            if (m_rectSelectedArea.Left < 0)
                m_rectSelectedArea.X = 0;
            else if (m_rectSelectedArea.Right > picMinImg.Width)
                m_rectSelectedArea.X = picMinImg.Width - m_rectSelectedArea.Width - 1;

            if (m_rectSelectedArea.Top < 0)
                m_rectSelectedArea.Y = 0;
            else if (m_rectSelectedArea.Bottom > picMinImg.Height)
                m_rectSelectedArea.Y = picMinImg.Height - m_rectSelectedArea.Height - 1;
            m_MainPic.m_fnMiniMapMove(m_imgMiniMap, m_rectSelectedArea.X, m_rectSelectedArea.Y); 
           // m_MainPic.m_fnMiniMapMove(m_imgMiniMap, m_rectSelectedArea.X, m_rectSelectedArea.Y);           
        }

        public delegate List<Point> DelDrawPoint();
        public delegate List<Rectangle> DelDrawRect();
        private void picMinImg_Paint(object sender, PaintEventArgs e)
        {

            if (m_imgMiniMap != null)
            {
                e.Graphics.DrawImage(m_imgMiniMap, m_imgRect);
             
                using (Pen p = new Pen(Color.Red, 2))
                {
                    e.Graphics.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
                    e.Graphics.TranslateTransform((float)(m_rectGuideLineArea.Width / 2) + m_rectGuideLineArea.X, (float)(m_rectGuideLineArea.Height / 2) + m_rectGuideLineArea.Y);
                    e.Graphics.RotateTransform((float)m_dGuideLineOffsetT);
                    e.Graphics.TranslateTransform(-(float)((m_rectGuideLineArea.Width / 2) + m_rectGuideLineArea.X), -(float)((m_rectGuideLineArea.Height / 2) + m_rectGuideLineArea.Y));
                    e.Graphics.DrawRectangle(p, m_rectGuideLineArea);
                }
                if (m_DicRoi != null)
                // ROI 표시 유무 플래그 필요
                for (int nRoiCnt = 0; nRoiCnt < m_DicRoi.Count; nRoiCnt++)
                {
                    m_fnDrawRec(e.Graphics, m_DicRoi[nRoiCnt].GetPenVal(), m_DicRoi[nRoiCnt].GetRectListVal());
                }

                if (m_DicPnt != null)
                    for (int nPntCnt = 0; nPntCnt < m_DicPnt.Count; nPntCnt++)
                {
                    m_fnDrawLine(e.Graphics, m_DicPnt[nPntCnt].GetPenVal(), m_DicPnt[nPntCnt].GetViewKind(), m_DicPnt[nPntCnt].GetPointListVal());
                }

                 Pen pRect = new Pen(Color.WhiteSmoke); // using (Pen p = new Pen(m_selectionColor))
                 e.Graphics.DrawRectangle(pRect, m_rectSelectedArea);

                //using (Pen penRoi = new Pen(Color.Red, 1))
                  //  m_fnDrawRec(e.Graphics, penRoi, new DelDrawRect(getRoiRectList));
//                 using (Pen penOrigin = new Pen(Color.Yellow, 2))
//                     m_fnDrawLine(e.Graphics, penOrigin, new DelDrawPoint(getOringPntList));
//                 using (Pen penAlign = new Pen(Color.LightGreen, 2))
//                     m_fnDrawLine(e.Graphics, penAlign, new DelDrawPoint(getAlignPntList));
            }

        }
        public void m_fnDrawLine(Graphics grp, Pen penPnt, int nKey,  List<Point> listPnt/*DelDrawPoint listPnt*/)
        {

            foreach (Point Pnt in listPnt)
            {
                switch (nKey)
                {
                    case (int)Enums.E_PNT_VIEW_KIND.CROSS:
                        {
                            grp.DrawLine(penPnt, CalcLineCoord(Pnt).X,
                                                 CalcLineCoord(Pnt).Y - 5,
                                                 CalcLineCoord(Pnt).X,
                                                 CalcLineCoord(Pnt).Y + 5
                                         );

                            grp.DrawLine(penPnt, CalcLineCoord(Pnt).X - 5,
                                                 CalcLineCoord(Pnt).Y,
                                                 CalcLineCoord(Pnt).X + 5,
                                                 CalcLineCoord(Pnt).Y
                                        );
                            break;
                        }

                    case (int)Enums.E_PNT_VIEW_KIND.POINT:
                        {
                            Rectangle rectTmp = new Rectangle(Pnt.X - 5, Pnt.Y - 5, 5, 5);
                            Rectangle rect = CalcRectCoord(rectTmp);
                            grp.FillRectangle(penPnt.Brush, rect);
                            break;
                        }
                }              

            }
        }
        private Point CalcLineCoord(Point Rect)
        {
            Point drawRect = new Point();
            drawRect.X = (m_imgMiniMap.Width * Rect.X / m_OrgImgSize.X);
            drawRect.Y = (m_imgMiniMap.Height * Rect.Y / m_OrgImgSize.Y);
            return drawRect;
        }
        public void m_fnDrawRec(Graphics grp, Pen penRect, /*DelDrawRect*/List<Rectangle> listRect/*, Rectangle RoiRect*/)
        {
            // grp.DrawRectangle(penRect, CalcRoiCoord(RoiRect));
            Pen tmpPen = new Pen(penRect.Color, 1);           
            tmpPen.DashStyle = System.Drawing.Drawing2D.DashStyle.Solid;
            foreach (Rectangle rect in listRect)
            {
                grp.DrawRectangle(tmpPen, CalcRectCoord(rect));             
            }
        }
        private Rectangle CalcRectCoord(Rectangle Rect)
        {
        
            Rectangle adjustedArea = new Rectangle();
            Point OrgSize = new Point();

            if (m_bIsDoWork)
                OrgSize = m_OrgImgSize;
            else
                OrgSize = m_MainOrgRectSize;

            adjustedArea.X = (m_imgMiniMap.Width * Rect.X / OrgSize.X);
            adjustedArea.Y = (m_imgMiniMap.Height * Rect.Y / OrgSize.Y);
            adjustedArea.Width = (m_imgMiniMap.Width * Rect.Width / OrgSize.X);
            adjustedArea.Height = (m_imgMiniMap.Height * Rect.Height / OrgSize.Y);
            return adjustedArea;
        }


      
        private void ClearRoi()
        {
            m_DicRoi.Clear();
        }        
     

        public void SetMinimapThreadWork(bool bThreakWork)
        {
            m_bIsDoWork = bThreakWork;
        }
        public bool GetMinimapThreadWork()
        {
           return  m_bIsDoWork;
        }
        public void SetimgThreadEnd(bool bThreakEnd)
        {
            _threadFlg = bThreakEnd;
        }
        public Bitmap GetMinimapImg()
        {
            return (Bitmap)m_imgMiniMap;
        }
        
        /// <summary>
        /// 기  능  : 이미지 수신 스레드가 종료 될때 까지 대기한다.
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void m_fnWaitImgThreadEnd()
        {
            try
            {
                int nWaitTime = 300;
                int nSleepCnt = 0;
                if (m_thread != null) while (m_thread.IsAlive && nWaitTime > nSleepCnt++) Thread.Sleep(10);
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
            }
        }
    }
}
