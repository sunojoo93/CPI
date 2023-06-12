using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using OpenCvSharp;
using OpenCvSharp.Extensions;
using Point = System.Drawing.Point;
using Size = System.Drawing.Size;
using System.Threading;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;

namespace CRUX_Renewal.User_Controls
{
    public partial class Uct_main_View : UserControl
    {
        public RectangleF m_imgRect;
        public RectangleF m_imgRect_Temp;
        public Point m_OriginRect = new Point();
        public PointF m_PrevSize = new PointF();
        public PointF m_PointOffset = new PointF();
        private Bitmap m_imgLoaded = null;
        private STRU_LABEL_COORD m_stCurCoord;
        Label[] m_stRGBColor = new Label[3];
        public Label m_lbNowRatio = new Label();
        private Image m_imgMiniMap;
        public Uct_Mini_View m_ucPicMiniView;
        public Point m_PixelSize = new Point();
        private float _m_dRatio = 1F;
        public float m_dRratio {
            get
            {
                return _m_dRatio;
            }
            set
            {
                _m_dRatio = value;
            }
        }
        private bool m_bImgFocus = false;
        private bool m_bUseZoomInOut = true;
        private PointF _m_pointClick;
        public PointF m_pointClick
        {
            get { return _m_pointClick; }
            set
            { _m_pointClick = value;
            }
        }
        private Point m_LastPoint;
        private bool m_bUseMiniMap = false;
        private string m_strImgPath = "";
        private Mat m_srcImage = new Mat();
        private PointF m_imgPoint;
        private Dictionary<int, struRect> m_DicRect;
        private int m_nUseRctNum;
        private bool m_bUseRect = false;
        private bool m_bRectBtnDown = false;
        private bool bAssign = false;
        private PointF m_pntRectRightClick;
        private PointF m_pntRectClick = new PointF();
        private RectangleF m_RectCom = new RectangleF();
        private Rectangle Croprect = new Rectangle();
        private Point[] m_pntsPolygon = new Point[1];
        bool bMinimapStartFlg = true;
        Thread thread;
        double Sacle = 1;
        public bool MouseMoveEvent { get; set; } = false;

        string ImageName = string.Empty;
        public RectangleF Passvalue
        {
            get { return this.m_imgRect; }
            set { this.m_imgRect = value; }
        }
        public struct STRU_LABEL_COORD
        {
            public Label X1;
            public Label Y1;
            public Label X2;
            public Label Y2;
        }
        [Serializable]
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct struRect
        {
            public Pen pen;
            public List<RectangleF> RectList;
            public bool bUseRectView;
            public RectangleF stRectCoord;

            public Pen GetPenVal()
            {
                return this.pen;
            }
            public List<RectangleF> GetRectListVal()
            {
                return this.RectList;
            }
            public bool GetUseRectView()
            {
                return this.bUseRectView;
            }

            public RectangleF GetRectCoord()
            {
                return this.stRectCoord;
            }
        }

        public Bitmap GetMinimapImg()
        {
            return (Bitmap)m_imgMiniMap;
        }
        public Mat GetScrMat()
        {
            return m_srcImage;
        }
        public PointF GetPixelSize()
        {
            return m_PixelSize;
        }
        public void SetRatio(float dRatio)
        {
            m_dRratio = dRatio;
            m_lbNowRatio.Text = string.Format("{0:00.00}", m_dRratio);
        }
        public Uct_main_View(Form Parent, ref Uct_Mini_View ucPicMiniMap, bool bUseMiniMap = false, int nWidth = 870, int nHeight = 700, int nLocX = 9, int nLocY = 47)
        {
            try
            {
                InitializeComponent();
                InitRectData();
                System.Drawing.Size sizeImg = new Size(nWidth / 4 * 4, nHeight / 4 * 4);  
                m_bUseMiniMap = bUseMiniMap;
                m_ucPicMiniView = ucPicMiniMap;

                this.MouseWheel += Pic_Main_MouseWheel;
                this.MouseMove += Pic_Main_MouseMove;
                this.MouseClick += Pic_Main_MouseClick;
                this.MouseDown += Pic_Main_MouseDown;

                if (m_bUseMiniMap) m_ucPicMiniView.Visible = true;
                else m_ucPicMiniView.Visible = false;

                this.Location = new Point(nLocX, nLocY);
                this.Size = sizeImg;
                Pic_Main.Size = sizeImg;
                Parent.Controls.Add(this);
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }
        public Thread GetThread()
        {
            return thread;
        }
        private void InitRectData()
        {
            m_DicRect = new Dictionary<int, struRect>();
        }

        public Bitmap GetBitmap()
        {
            return m_imgLoaded;
        }
        delegate void ThreadIPaintCallback(object sender, PaintEventArgs e);
        private void Pic_Main_Paint(object sender, PaintEventArgs e)
        {
            try
            {
                PictureBox ainview = sender as PictureBox;
                if (m_imgLoaded != null)
                {
                    m_imgMiniMap = m_ucPicMiniView.GetMinimapImg();

                    if (this.InvokeRequired)
                    {
                        ThreadIPaintCallback delOpenImg = new ThreadIPaintCallback(Pic_Main_Paint);
                        this.Invoke(delOpenImg, sender, e);
                        return;
                    }

                    e.Graphics.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
                    e.Graphics.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.None;

                    RectangleF tmp = new Rectangle(0, 0, Pic_Main.Width, Pic_Main.Height);

                    
                    e.Graphics.DrawImage(m_imgLoaded, tmp);

                    Passvalue = m_imgRect;

                    RectangleF recFovRec = new RectangleF();
                    recFovRec = m_imgRect;

                    Pen penTemp = null;

                    if (m_DicRect[m_fnGetUseRctNum()].bUseRectView)
                        penTemp = m_DicRect[m_fnGetUseRctNum()].GetPenVal();


                    if (m_bUseRect && penTemp != null)
                    {
                        RectangleF rectTmp = m_RectCom;

                        if (m_RectCom.Width < 0)
                        {
                            rectTmp.X = m_pntRectClick.X + m_RectCom.Width;
                            rectTmp.Width = m_pntRectClick.X - rectTmp.X;
                        }

                        if (m_RectCom.Height < 0)
                        {
                            rectTmp.Y = m_pntRectClick.Y + m_RectCom.Height;
                            rectTmp.Height = m_pntRectClick.Y - rectTmp.Y;

                        }
                    }
                    m_ucPicMiniView.Pic_Mini_Paint(m_OriginRect, recFovRec, Pic_Main.Width, Pic_Main.Height, m_dRratio);
                }
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }
        public PointF GetImagePoint()
        {
            return m_imgPoint;
        }

        public void InitRectColor(int nKey, Color color)
        {
            try
            {
                Pen penRoi = new Pen(color, 2);
                penRoi.DashStyle = System.Drawing.Drawing2D.DashStyle.Dot;
                SetRectPenVal(nKey, penRoi);
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        private void Pic_Main_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            try
            {
                if (e.KeyCode == Keys.Escape)
                {
                    if (m_bUseRect)
                    {
                        if (m_RectCom.Width != 0 || m_RectCom.Height != 0)
                        {
                            m_bRectBtnDown = false;
                            RectangleF EmptRect = new RectangleF();
                            m_pntRectClick.X = 0;
                            m_pntRectClick.Y = 0;
                            m_RectCom = EmptRect;
                            this.Cursor = Cursors.Default;
                        }
                    }
                }
                Pic_Main.Invalidate();
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        private void SetRectPenVal(int nKey, Pen pen)
        {
            try
            {
                struRect struRoi;
                if (m_DicRect.TryGetValue(nKey, out struRoi))
                {
                    struRoi.pen = pen;
                    m_DicRect[nKey] = struRoi;
                }
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }
        delegate void OpenImageCallback(string strImgPath, bool bAlign = false);
        public void OpenImage(string strImgPath, bool bAlign = false)
        {
            try
            {
                m_strImgPath = strImgPath;

                if (this.InvokeRequired)
                {
                    OpenImageCallback delOpenImg = new OpenImageCallback(OpenImage);
                    this.Invoke(delOpenImg, strImgPath);
                }
                else
                {
                    LoadImage(strImgPath);
                }

            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        public void SetImageName(string name)
        {
            ImageName = name;
        }

        public string GetImageName()
        {
            return ImageName;
        }
        delegate void OpenImageCallback2(string strImgPath, bool bAlign = false);
        public void OpenImage(Bitmap strImgPath, bool bAlign = false)
        {
            try
            {
                if (this.InvokeRequired)
                {
                    OpenImageCallback2 delOpenImg = new OpenImageCallback2(OpenImage);
                    this.Invoke(delOpenImg, strImgPath);
                }
                else
                {
                    LoadImage(strImgPath);
                }
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }
        public void m_fnUseEventRectOff()
        {
            try
            {
                m_fnUseRectDraw(false);
                m_fnUseEventRectView(0, false);
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        private void LoadImage(string strImgPath)
        {
            try
            {
                if (m_srcImage != null)
                {
                    m_srcImage.Dispose();
                }
                if (m_imgLoaded != null)
                    m_imgLoaded.Dispose();

                m_srcImage = Cv2.ImRead(strImgPath, ImreadModes.Unchanged);

                
                m_PixelSize.X = m_srcImage.Width;
                m_PixelSize.Y = m_srcImage.Height;

                double XScale = 1;
                double YScale = 1;
                if (m_PixelSize.X >= 22000 || m_PixelSize.Y >= 22000)
                {
                    XScale = (double)m_PixelSize.X / (double)22000;
                    YScale = (double)m_PixelSize.Y / (double)22000;
                }
                Sacle = XScale > YScale ? XScale : YScale;
                //Cv2.Resize(m_srcImage[rect], matResize, OpenSize, 0, 0, InterpolationFlags.Area);
                //m_imgLoaded = new Bitmap((int)((double)m_PixelSize.X / (double)Sacle), (int)((double)m_PixelSize.Y/ (double)Sacle));
                m_imgLoaded = new Bitmap(m_PixelSize.X, m_PixelSize.Y);

                m_imgRect.Width = m_PixelSize.X;
                m_imgRect.Height = m_PixelSize.Y;
                ResizeViewRect(ref m_imgRect);
                m_PixelSize.X = (int)m_imgRect.Width;
                m_PixelSize.Y = (int)m_imgRect.Height;

                m_OriginRect.X = m_srcImage.Width;
                m_OriginRect.Y = m_srcImage.Height;

                m_dRratio = (float)1.0;

                m_imgPoint = new Point(Pic_Main.Width / 2, Pic_Main.Height / 2);

                m_pointClick = m_imgPoint;
                bMinimapStartFlg = true;
                if (thread != null)
                {
                    thread.Abort();
                    thread = null;
                }

                thread = new Thread(new ThreadStart(delegate () { ThreadImagLoad(); }));
                thread.IsBackground = true;

                thread.Start();
                Pic_Main.Invalidate();
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        private void LoadImage(Bitmap strImgPath)
        {
            try
            {
                m_srcImage = OpenCvSharp.Extensions.BitmapConverter.ToMat(strImgPath);

                m_PixelSize.X = m_srcImage.Width;
                m_PixelSize.Y = m_srcImage.Height;

                double XScale = 1;
                double YScale = 1;
                if (m_PixelSize.X >= 22000 || m_PixelSize.Y >= 22000)
                {
                    XScale = (double)m_PixelSize.X / (double)22000;
                    YScale = (double)m_PixelSize.Y / (double)22000;
                }
                Sacle = XScale > YScale ? XScale : YScale;

                //m_imgLoaded = new Bitmap((int)((double)m_PixelSize.X / (double)Sacle), (int)((double)m_PixelSize.Y / (double)Sacle));
                m_imgLoaded = new Bitmap(m_PixelSize.X,m_PixelSize.Y);

                m_imgRect.Width = m_PixelSize.X;
                m_imgRect.Height = m_PixelSize.Y;

                m_dRratio = (float)1.0;

                m_imgPoint = new Point(Pic_Main.Width / 2, Pic_Main.Height / 2);

                m_pointClick = m_imgPoint;

                Thread thread = new Thread(new ThreadStart(delegate () { ThreadImagLoad(); }));
                thread.IsBackground = true;
                Pic_Main.Invalidate();
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }
        public void ThreadImagLoad()
        {
            try
            {
                if (m_imgLoaded == null) return;

                int SleepTime = 30;
                while (true)
                {
                    Thread.Sleep(SleepTime);

                    int nWidth = Pic_Main.Width;
                    int nHeight = Pic_Main.Height;

                    if (m_imgRect_Temp == m_imgRect)
                    {
                        //Pic_Main.BeginInvoke(new Action(() => Pic_Main.Invalidate()));                
                        SleepTime = 30;
                        //continue;
                    }

                    if (SleepTime > 5)
                        SleepTime -= 2;

                    m_imgLoaded = m_fnLiveImageGrab(m_imgRect.X, m_imgRect.Y, m_imgRect.Width, m_imgRect.Height, nWidth, nHeight);

                    if (this.m_bUseMiniMap && bMinimapStartFlg)
                    {
                        if (this.m_imgLoaded != null)
                        {

                            m_ucPicMiniView.BeginInvoke(new Action(() =>
                             {
                                 this.m_ucPicMiniView.SetVisible(false);
                                 this.m_ucPicMiniView.m_fnSetOringSize();
                                 this.m_imgMiniMap = Resize(OpenCvSharp.Extensions.BitmapConverter.ToBitmap(m_srcImage), this.m_ucPicMiniView.Pic_Mini.Width, this.m_ucPicMiniView.Pic_Mini.Height, false);

                                 this.m_ucPicMiniView.picMapResize(new Point(this.m_imgMiniMap.Width, this.m_imgMiniMap.Height));
                                 this.m_ucPicMiniView.m_fnSetMiniMapImage(this.m_imgMiniMap);
                                 this.m_ucPicMiniView.SetVisible(true);
                             }
                                  ));
                            bMinimapStartFlg = false;
                        }
                    }
                    m_imgRect_Temp = m_imgRect;

                    Pic_Main.BeginInvoke(new Action(() => Pic_Main.Invalidate()));
                }
            }
            catch (Exception ex)
            {
               // Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }
        public void ResizeViewRect(ref RectangleF rect)
        {
            try
            {
                float newX = 0;
                float newY = 0;
                float newWidth = m_imgLoaded.Width;
                float newHeight = m_imgLoaded.Height;
                float Ratio = 1;
                int WidthOffset = 0, HeightOffset = 0;
                float Pic_Ratio = (float)Pic_Main.Width / (float)Pic_Main.Height;
                if (newWidth > newHeight)
                {
                    Ratio = (float)m_imgLoaded.Height / (float)Pic_Main.Height;
                    newWidth = ((newHeight * Pic_Ratio));
                    if (newWidth > m_imgLoaded.Width)
                        newWidth = m_imgLoaded.Width;
                    //WidthOffset = ((m_imgLoaded.Width - newWidth) / 2);
                }
                else
                {
                    Ratio = (float)m_imgLoaded.Width / (float)Pic_Main.Width;
                    newHeight = newWidth * Pic_Ratio;
                    if (newHeight > m_imgLoaded.Height)
                        newHeight = m_imgLoaded.Height;
                    //HeightOffset = ((m_imgLoaded.Height - newHeight) / 2);
                }

                newX = WidthOffset;
                newY = HeightOffset;

                rect.X = newX;
                rect.Y = newY;
                rect.Width = newWidth;
                rect.Height = newHeight;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }
        private Bitmap m_fnLiveImageGrab(float nX, float nY, float nCropX, float nCropY, float nPicWidth, float nPicHeight)
        {

            
            Rect rect = new Rect((int)nX, (int)nY, (int)nCropX, (int)nCropY);
            Mat matResize = new Mat(rect.X, rect.Y, MatType.CV_8UC1);
            //Mat matResize = new Mat();

            OpenCvSharp.Size OpenSize = new OpenCvSharp.Size(nPicWidth, nPicHeight);

            try
            {

                if (rect.X + rect.Width > m_srcImage.Width)
                {
                    rect.X = Math.Abs(rect.Width - m_srcImage.Width);
                    m_imgRect.X = rect.X;
               
                }

              

                Cv2.Resize(m_srcImage[rect], matResize, OpenSize, 0, 0, InterpolationFlags.Area);

                Bitmap bitmap = OpenCvSharp.Extensions.BitmapConverter.ToBitmap(matResize);

                if (nPicWidth != 0 && nPicHeight != 0)
                {
                    matResize.Dispose();
                    return bitmap;
                }
                else
                    return m_imgLoaded;
            }
            catch (Exception ex)
            {
                return m_imgLoaded;
            }
        }

        public static Bitmap ConvertBitmap(byte[] frame, int width, int height, bool bIsGrayScale = true, PixelFormat pxlFormat = PixelFormat.Format8bppIndexed)
        {
            Bitmap bmp = new Bitmap(width, height, pxlFormat);
            BitmapData data = bmp.LockBits(new Rectangle(0, 0, bmp.Width, bmp.Height),
                                            ImageLockMode.WriteOnly, pxlFormat);

            try
            {
                System.Runtime.InteropServices.Marshal.Copy(frame, 0, data.Scan0, frame.Length);
            }
            catch (AccessViolationException accEx) { System.Diagnostics.Debug.WriteLine(accEx.Message); }
            catch (Exception ex) { System.Diagnostics.Debug.WriteLine(ex.Message); }

            bmp.UnlockBits(data);

            if (bIsGrayScale)
            {
                ColorPalette GrayScalepalette = bmp.Palette;
                for (int i = 0; i < 256; i++)
                    GrayScalepalette.Entries[i] = Color.FromArgb((byte)i, (byte)i, (byte)i);
                bmp.Palette = GrayScalepalette;
            }
            return bmp;
        }

        public static new Image Resize(Image image, float Width, float Height, bool needToFill)
        {
            System.Drawing.Bitmap bmPhoto = null;

            int sourceWidth = image.Width;
            int sourceHeight = image.Height;
            int sourceX = 0;
            int sourceY = 0;
            double destX = 0;
            double destY = 0;

            float nScale = 0;
            float nScaleW = 0;
            float nScaleH = 0;
            int destWidth = 0;
            int destHeight = 0;
            try
            {
                nScaleW = Width / sourceWidth;
                nScaleH = Height / sourceHeight;
                if (!needToFill)
                {
                    nScale = Math.Min(nScaleH, nScaleW);
                }
                else
                {
                    nScale = Math.Max(nScaleH, nScaleW);
                    destY = (Height - sourceHeight * nScale) / 2;
                    destX = (Width - sourceWidth * nScale) / 2;
                }

                if (nScale > 1)
                    nScale = 1;

                destWidth = (int)Math.Round(sourceWidth * nScale);
                destHeight = (int)Math.Round(sourceHeight * nScale);

                try
                {
                    bmPhoto = new System.Drawing.Bitmap(destWidth + (int)Math.Round(2 * destX), destHeight + (int)Math.Round(2 * destY));
                }
                catch (Exception ex)
                {
                    throw new ApplicationException(string.Format("destWidth:{0}, destX:{1}, destHeight:{2}, desxtY:{3}, Width:{4}, Height:{5}",
                        destWidth, destX, destHeight, destY, Width, Height), ex);
                }
                using (System.Drawing.Graphics grPhoto = System.Drawing.Graphics.FromImage(bmPhoto))
                {
                    Rectangle to = new System.Drawing.Rectangle((int)Math.Round(destX), (int)Math.Round(destY), destWidth, destHeight);
                    Rectangle from = new System.Drawing.Rectangle(sourceX, sourceY, sourceWidth, sourceHeight);
                    grPhoto.DrawImage(image, to, from, System.Drawing.GraphicsUnit.Pixel);
                }
            }
            catch (Exception ex)
            {
                throw new ApplicationException(string.Format("destWidth:{0}, destX:{1}, destHeight:{2}, desxtY:{3}, Width:{4}, Height:{5}",
                                      destWidth, destX, destHeight, destY, Width, Height), ex);
            }
            return bmPhoto;
        }

        public void m_fnSetZoomInOut(int lines, int nX, int nY)
        {
            try
            {
                PictureBox pb = Pic_Main;
                if (m_dRratio == 1)
                {
                    m_PrevSize.X = m_PixelSize.X;
                    m_PrevSize.Y = m_PixelSize.Y;
                }
                if (lines == 100)
                {
                    m_dRratio = 1;
                }
                else
                {
                    if (lines > 0)
                    {
                        if (m_dRratio >= 100.0F)
                            m_dRratio += 10.0F;
                        else if (m_dRratio >= 50.0F)
                            m_dRratio += 5.0F;
                        else if (m_dRratio >= 30.0F)
                            m_dRratio += 3.0F;
                        else if (m_dRratio >= 20.0F)
                            m_dRratio += 2.0F;
                        else if (m_dRratio >= 10.0F)
                            m_dRratio += 1.0F;
                        else if (m_dRratio >= 5.0F)
                            m_dRratio += 0.50F;
                        else
                            m_dRratio += 0.25F;
                        m_dRratio = (float)Math.Round(m_dRratio, 5);
                        if (m_dRratio > 300.0) m_dRratio = 300.0F;

                    }
                    else if (lines < 0)
                    {
                        if (m_dRratio > 100.0F)
                            m_dRratio -= 10.0F;
                        else if (m_dRratio > 50.0F)
                            m_dRratio -= 5.0F;
                        else if (m_dRratio > 30.0F)
                            m_dRratio -= 3.0F;
                        else if (m_dRratio > 20.0F)
                            m_dRratio -= 5.0F;
                        else if (m_dRratio > 10.0F)
                            m_dRratio -= 1.0F;
                        else if (m_dRratio > 5.0F)
                            m_dRratio -= 0.50F;
                        else
                            m_dRratio -= 0.25F;
                        m_dRratio = (float)Math.Round(m_dRratio, 5);
                        if (m_dRratio <= 1.0F)
                            m_dRratio = 1.0F;
                    }
                }

                RectangleF adjustedArea = new RectangleF();

                PointF pntOldCoord = new PointF();
                pntOldCoord.X = m_PixelSize.X;
                pntOldCoord.Y = m_PixelSize.Y;

                adjustedArea.Width = (float)Math.Round(m_PixelSize.X / (m_dRratio));
                adjustedArea.Height = (float)Math.Round(m_PixelSize.Y / (m_dRratio));

                m_PointOffset.X = (m_PrevSize.X - adjustedArea.Width) / 2;
                m_PointOffset.Y = (m_PrevSize.Y - adjustedArea.Height) / 2;

                adjustedArea.X = m_imgRect.X + m_PointOffset.X;
                adjustedArea.Y = m_imgRect.Y + m_PointOffset.Y;

                if (adjustedArea.X < 0) 
                    adjustedArea.X = 0;

                if (adjustedArea.X > m_OriginRect.X - adjustedArea.Width) 
                    adjustedArea.X = m_PixelSize.X - adjustedArea.Width;

                if (adjustedArea.Y < 0)
                    adjustedArea.Y = 0;

                if (adjustedArea.Y > m_OriginRect.Y - adjustedArea.Height)
                    adjustedArea.Y = m_PixelSize.Y - adjustedArea.Height;

                if (m_PixelSize.X < adjustedArea.Width)
                    adjustedArea.X = adjustedArea.X - m_imgRect.X;

                if (m_PixelSize.Y < adjustedArea.Height) 
                    adjustedArea.Y = adjustedArea.Y - m_imgRect.Y;

                if (adjustedArea.Width + adjustedArea.X > m_OriginRect.X) 
                    adjustedArea.X = m_PixelSize.X - adjustedArea.Width;

                if (adjustedArea.Y + adjustedArea.Height > m_OriginRect.Y)
                    adjustedArea.Y = m_PixelSize.Y - adjustedArea.Height;

                m_imgRect = adjustedArea;
                m_PrevSize.X = adjustedArea.Width;
                m_PrevSize.Y = adjustedArea.Height;

                if (m_imgLoaded != null)
                    m_lbNowRatio.Text = string.Format("{0:00.00}", m_dRratio);
                //Program.frm_Layers.DrawObject();
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        private void Uct_main_View_Load(object sender, EventArgs e)
        {
            // 이벤트 중복으로 인한 삭제
            //Pic_Main.MouseWheel
            //            += new System.Windows.Forms.MouseEventHandler(this.Pic_Main_MouseWheel);
        }

        private void Pic_Main_MouseWheel(object sender, MouseEventArgs e)
        {
            try
            {
                if (!m_bImgFocus || !m_bUseZoomInOut)
                {
                    Pic_Main.Invalidate();
                    return;
                }
                if (/*System.Windows.Input.Keyboard.Modifiers == System.Windows.Input.ModifierKeys.Control*/true)
                {
                    int lines = e.Delta * SystemInformation.MouseWheelScrollLines;
                    m_fnSetZoomInOut(lines, e.X, e.Y);
                }
                else
                {
                    if (e.Delta / 120 > 0)
                    {
                        if (m_imgRect.Y <= 0)
                            m_imgRect.Y = 0;
                        else
                        {
                            if ((m_imgRect.Y - Consts.ScrollSize) <= 0)
                                m_imgRect.Y = 0;
                            if(m_imgRect.Y > Consts.ScrollSize)
                                m_imgRect.Y -= Consts.ScrollSize;
                        }
                    }
                    else if (e.Delta / 120 < 0)
                    {
                        if (m_imgRect.Y + m_imgRect.Height >= m_srcImage.Height)
                            m_imgRect.Y -= ((m_imgRect.Y + m_imgRect.Height) - m_srcImage.Height);
                        else
                            m_imgRect.Y += Consts.ScrollSize;
                    }
                    //Program.frm_Layers.DrawObject();
                }
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }
        //public void GetMouseMoveEvent(ref DelegateEvent.delMouseMoveChain delMouseChain)
        //{
        //    delMouseChain += Pic_Main_MouseMove;
        //}
        //public void GetMouseDownEvent(ref DelegateEvent.delMouseDownChain delMouseChain)
        //{
        //    delMouseChain += Pic_Main_MouseDown;
        //}
        //public void GetMouseEnterEvent(ref DelegateEvent.delMouseEnterChain delMouseChain)
        //{
        //    delMouseChain += Pic_Main_MouseEnter;
        //}
        //public void GetMouseLeaveEvent(ref DelegateEvent.delMouseLeaveChain delMouseChain)
        //{
        //    delMouseChain += Pic_Main_MouseLeave;
        //}
        //public void GetMouseClickEvent(ref DelegateEvent.delMouseClickChain delMouseChain)
        //{
        //    delMouseChain += Pic_Main_MouseClick;
        //}
        //public void GetMouseWheelEvent(ref DelegateEvent.delMouseWheelChain delMouseChain)
        //{
        //    delMouseChain += Pic_Main_MouseWheel;
        //}
        public double GetRatio()
        {
            return m_dRratio;
        }
        public RectangleF GetImageRect()
        {
            return m_imgRect;
        }
        public void SetImageRect(RectangleF rect)
        {
            m_imgRect = rect;
            m_PrevSize.X = rect.Width;
            m_PrevSize.Y = rect.Height;

        }

        public void m_fnLinkNowRatio(ref Label reflbRratio)
        {
            try
            {
                m_lbNowRatio = reflbRratio;
                m_lbNowRatio.Text = string.Format("{0:00.00}", 1.00F);
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }
        public bool SetAssign(bool bClick)
        {
            bAssign = bClick;
            return bAssign;
        }

        private void Pic_Main_MouseEnter(object sender, EventArgs e)
        {
            try
            {
                Pic_Main.Focus();
                m_bImgFocus = true;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        public void SendToBackCtrl()
        {
            SendToBack();
        }

        public void BringToFrontCtrl()
        {
            BringToFront();
        }

        private void Pic_Main_MouseLeave(object sender, EventArgs e)
        {
            m_bImgFocus = false;
        }

        private void Pic_Main_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left || e.Button == MouseButtons.Right)
            {
                m_pointClick = new PointF(e.X, e.Y);
            }
            else if (e.Button == MouseButtons.Right)
            {

            }
        }

        private void Pic_Main_MouseClick(object sender, MouseEventArgs e)
        {
            try
            {
                if (m_bUseRect && e.Button == MouseButtons.Right)
                {
                    if (!m_bRectBtnDown)
                    {
                        m_pntRectRightClick = new PointF(e.X, e.Y);

                        m_pntRectClick.X = m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, Pic_Main.Width, m_dRratio);
                        m_pntRectClick.Y = m_fnCalCoordinate(m_imgRect.Y, e.Y, m_imgRect.Height, Pic_Main.Height, m_dRratio);

                        if (m_DicRect[m_fnGetUseRctNum()].GetUseRectView())
                            m_fnSetRectCoord(m_fnGetUseRctNum(), true);
                        m_bRectBtnDown = true;
                        this.Cursor = Cursors.Cross;

                    }
                    else
                    {
                        if (m_DicRect[m_fnGetUseRctNum()].GetUseRectView())
                            m_fnSetRectCoord(m_fnGetUseRctNum(), false);

                        m_bRectBtnDown = false;
                        this.Cursor = Cursors.Default;
                    }
                }

                //if (e.Button == MouseButtons.Left && Frm_Main.MagicStickMode)
                //{
                //    m_pntRectClick.X = m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, Pic_Main.Width, m_dRratio);
                //    m_pntRectClick.Y = m_fnCalCoordinate(m_imgRect.Y, e.Y, m_imgRect.Height, Pic_Main.Height, m_dRratio);

                //    Mat matBinary = new Mat();
                //    if (m_srcImage.Type() == MatType.CV_8UC3)
                //    {
                //        Mat matTemp1 = new Mat();
                //        matBinary = m_srcImage.CvtColor(ColorConversionCodes.BGR2GRAY);
                //        byte btGV = 0;
                //        for (int y = 0; y < matBinary.Rows; y++)
                //        {
                //            if (m_pntRectClick.Y != y)
                //                continue;
                //            for (int x = 0; x < matBinary.Cols; x++)
                //            {
                //                if (m_pntRectClick.X == x)
                //                    btGV = matBinary.At<byte>(y, x);
                //            }
                //        }

                //        Cv2.AdaptiveThreshold(matBinary, matBinary, 255, AdaptiveThresholdTypes.GaussianC, ThresholdTypes.BinaryInv, 51, 5);
                //    }
                //    else
                //    {
                //        Mat matTemp1 = new Mat();
                //        m_srcImage.CopyTo(matBinary);
                //        byte btGV = 0;
                //        for (int y = 0; y < matBinary.Rows; y++)
                //        {
                //            if (m_pntRectClick.Y != y)
                //                continue;
                //            for (int x = 0; x < matBinary.Cols; x++)
                //            {
                //                if (m_pntRectClick.X == x)
                //                    btGV = matBinary.At<byte>(y, x);
                //            }
                //        }

                //        Cv2.AdaptiveThreshold(matBinary, matBinary, 255, AdaptiveThresholdTypes.GaussianC, ThresholdTypes.BinaryInv, 51, 5);
                //    }


                //    Mat element = Cv2.GetStructuringElement(MorphShapes.Rect, new OpenCvSharp.Size(3, 3));
                //    //Cv2.MorphologyEx(matBinary, matBinary, MorphTypes.Close, element);
                //    Cv2.MorphologyEx(matBinary, matBinary, MorphTypes.Open, element);

                //    Mat matMask_Tmp = new Mat(matBinary.Rows, matBinary.Cols, MatType.CV_8UC1);
                //    matMask_Tmp.SetTo(0);
                //    Mat labels = new Mat();
                //    Mat stats = new Mat();
                //    Mat centroids = new Mat();
                //    int numLabels;
                //    numLabels = Cv2.ConnectedComponentsWithStats(matBinary, labels, stats, centroids);

                //    //if (numLabels < 10000)
                //    {
                //        for (int j = 1; j < numLabels; j++)
                //        {
                //            int nArea = stats.At<int>(j, (int)ConnectedComponentsTypes.Area);
                //            if (nArea < 20)
                //                continue;
                //            int nL = stats.At<int>(j, (int)ConnectedComponentsTypes.Left);
                //            int nT = stats.At<int>(j, (int)ConnectedComponentsTypes.Top);
                //            int nW = stats.At<int>(j, (int)ConnectedComponentsTypes.Width);
                //            int nH = stats.At<int>(j, (int)ConnectedComponentsTypes.Height);
                //            int nR = nL + nW;
                //            int nB = nT + nH;

                //            if (m_pntRectClick.X > nL && m_pntRectClick.X < nR && m_pntRectClick.Y > nT && m_pntRectClick.Y < nB)
                //            {
                //                for (int y = nT; y < nB; y++)
                //                {
                //                    for (int x = nL; x < nR; x++)
                //                    {
                //                        if (labels.At<int>(y, x) == j)
                //                        {
                //                            //rectContour = new Rect(nL, nT, nW, nH);
                //                            matMask_Tmp.At<int>(y, x) = 255;
                //                        }
                //                    }
                //                }
                //            }
                //        }
                //    }

                //    OpenCvSharp.Point[][] contours;
                //    HierarchyIndex[] hierarchy;
                //    Cv2.FindContours(matMask_Tmp, out contours, out hierarchy, RetrievalModes.Tree, ContourApproximationModes.ApproxTC89KCOS);

                //    List<OpenCvSharp.Point[]> new_contours = new List<OpenCvSharp.Point[]>();
                //    foreach (OpenCvSharp.Point[] p in contours)
                //    {
                //        double length = Cv2.ArcLength(p, true);
                //        //if (length < 100) continue;

                //        new_contours.Add(Cv2.ApproxPolyDP(p, length * 0.02, true));
                //    }

                //    if (new_contours.Count != 0)
                //    {
                //        List<PointF> Temp = new List<PointF>();
                //        for (int i = 0; i < new_contours[0].Length; i++)
                //        {
                //            Temp.Add(new PointF(new_contours[0][i].X, new_contours[0][i].Y));
                //        }
                //        if (!(bool)Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].Lock)
                //        {
                //            Program.frm_Layers.UnSelectedObject();
                //            Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].DrawShapes(new DieEdit.Source.Polygon_Obj(Program.frm_Layers.GetRegistedLayerList().Count, Temp) { Selected = true });
                //            Program.frm_CoordExtend.SetSelectedObjectCount(1);


                //            var SizeTemp = new List<Shape_Obj>();
                //            SizeTemp.Add(Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs()[Program.frm_Layers.GetRegistedLayerList()[Program.frm_Layers.GetSelectedRow()].GetShapeObjs().Count - 1]);
                //            Program.frm_CoordExtend.SetCurrentObjSize(SizeTemp);
                //        }
                //    }
                //    Pic_Main.Invalidate();
                //}
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }
        public RectangleF GetRect()
        {
            return this.Croprect;
        }
        private void m_fnSetRectCoord(int nKey, bool bStartEnd)
        {
            try
            {
                RectangleF rct = m_DicRect[nKey].GetRectCoord();

                if (bStartEnd)
                {
                    if (m_stCurCoord.X1 != null) rct.X = Convert.ToInt32(m_stCurCoord.X1.Text);
                    if (m_stCurCoord.Y1 != null) rct.Y = Convert.ToInt32(m_stCurCoord.Y1.Text);
                }
                else
                {
                    if (m_stCurCoord.X1 != null) rct.Width = Convert.ToInt32(m_stCurCoord.X1.Text);
                    if (m_stCurCoord.Y1 != null) rct.Height = Convert.ToInt32(m_stCurCoord.Y1.Text);
                }

                SetRectCoord(nKey, rct);
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }
        private void SetRectCoord(int nKey, RectangleF stRoiCoord)
        {
            try
            {
                struRect struRoi;
                if (m_DicRect.TryGetValue(nKey, out struRoi))
                {
                    struRoi.stRectCoord = stRoiCoord;
                    m_DicRect[nKey] = struRoi;
                }
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }
        public int m_fnGetUseRctNum()
        {
            return m_nUseRctNum;
        }
        public float m_fnCalCoordinate(float nOrignCoord, float nCurCoord, float nImgRectSize, float nFovSize, float nRatio)
        {
            try
            {
                return nOrignCoord + (float)Math.Round((Convert.ToDouble(nImgRectSize * (Math.Abs(nCurCoord))) / (nFovSize - 1)));
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
                throw ex;
            }
        }

        // Zoom In/out 의 pic box 변화 계산
        public float m_fnCalCoordinate_ZoomInOut(float nOrignCoord, float nFovSize, float dRatio)
        {
            try
            {
                return (nOrignCoord - nFovSize / 2) * (dRatio - 1);
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
                throw ex;
            }
        }

        private void Pic_Main_MouseMove(object sender, MouseEventArgs e)
        {
            try
            {

            if (!MouseMoveEvent)
                return;

            m_LastPoint = e.Location;    
            //Program.frm_Pos_GL.SetMouseGV((int)m_srcImage.At<byte>((int)m_fnCalCoordinate(m_imgRect.Y, e.Y, m_imgRect.Height, Pic_Main.Height, m_dRratio), (int)m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, Pic_Main.Width, m_dRratio)));

            if ((e.Button == MouseButtons.Left/* && Frm_Main.ExploringUserMode*/) || e.Button == MouseButtons.Right)
            {
                if (m_imgRect.Width <= m_PixelSize.X || m_imgRect.Height <= m_PixelSize.Y && (m_pointClick.X >= 0 && m_pointClick.Y >= 0))
                {
                    //Program.Painter.ClearDrawingPolygon();
                    RectangleF rectMouseMove = new RectangleF();

                    rectMouseMove.X = (float)Math.Ceiling((Convert.ToDouble(m_imgRect.Width * (m_pointClick.X - Math.Abs(e.X))) / Pic_Main.Width));
                    m_imgRect.X = m_imgRect.X + rectMouseMove.X;

                    rectMouseMove.Y = (float)Math.Ceiling((Convert.ToDouble(m_imgRect.Height * (m_pointClick.Y - Math.Abs(e.Y))) / Pic_Main.Height));
                    m_imgRect.Y = m_imgRect.Y + rectMouseMove.Y;

                    if (m_imgRect.X < 0) m_imgRect.X = 0;
                    if (m_imgRect.X > (m_OriginRect.X - m_imgRect.Width))
                        m_imgRect.X = m_OriginRect.X - m_imgRect.Width;

                    if (m_imgRect.Y < 0) m_imgRect.Y = 0;
                    if (m_imgRect.Y > (m_OriginRect.Y - m_imgRect.Height))
                        m_imgRect.Y = m_OriginRect.Y - m_imgRect.Height;

                        m_pointClick = new Point(e.X, e.Y);

                    return;
                }
                else
                {
                    return;
                }
            }

            if (m_bRectBtnDown)
            {
                if (m_imgRect.Width <= m_PixelSize.X || m_imgRect.Height <= m_PixelSize.Y && (m_pntRectClick.X >= 0 && m_pntRectClick.Y >= 0))
                {
                    //좌표계산 
                    m_RectCom.X = m_pntRectClick.X;
                    m_RectCom.Y = m_pntRectClick.Y;
                    m_RectCom.Width = m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, Pic_Main.Width, m_dRratio);
                    m_RectCom.Height = m_fnCalCoordinate((m_imgRect.Y), e.Y, m_imgRect.Height, Pic_Main.Height, m_dRratio);

                    if (m_RectCom.Width < m_pntRectClick.X)
                    {
                        m_RectCom.X = Math.Abs(m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, Pic_Main.Width, m_dRratio));
                        m_RectCom.Width = Math.Abs(m_RectCom.X - m_pntRectClick.X);
                    }
                    else m_RectCom.Width -= m_pntRectClick.X;

                    //Y좌표계산
                    if (m_RectCom.Height < m_pntRectClick.Y)
                    {
                        m_RectCom.Y = Math.Abs(m_fnCalCoordinate((m_imgRect.Y), e.Y, m_imgRect.Height, Pic_Main.Height, m_dRratio));
                        m_RectCom.Height = Math.Abs(m_RectCom.Y - m_pntRectClick.Y);
                    }
                    else m_RectCom.Height -= m_pntRectClick.Y;
                }
                else return;

                if (m_DicRect[m_fnGetUseRctNum()].GetUseRectView())
                {
                    //m_CommonRect = m_RectCom;
                    m_fnSetRectCoord(m_fnGetUseRctNum(), false);
                }
            }
            Pic_Main.Invalidate();
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        public void m_fnSetCurRGBVal(int nX, int nY)
        {
            try
            {
                Color color;

                if (nX < 0 || nY < 0) return;
                if (nX > m_imgLoaded.Width || nY > m_imgLoaded.Height) return;
                color = (m_imgLoaded).GetPixel(nX, nY);

                m_stRGBColor[0].Text = string.Format("{0:000}", color.R);
                m_stRGBColor[1].Text = string.Format("{0:000}", color.G);
                m_stRGBColor[2].Text = string.Format("{0:000}", color.B);
            }
            catch { return; }
        }

        public void m_fnMiniMapMove(Image iMapimg, RectangleF SelMap)
        {
            try
            {
                if (m_imgLoaded == null) return;
                if (iMapimg == null) return;
                int nMapX = 0, nMapY = 0;

                int nnn = (int)(SelMap.X + SelMap.Width);

                int nnn2 = (int)(SelMap.Y + SelMap.Height);

                nMapY = (int)(m_OriginRect.Y * SelMap.Y / (float)iMapimg.Height);

                if (nnn >= iMapimg.Width - 2)
                    nMapX = (int)(m_OriginRect.X - m_imgRect.Width);

                else
                    nMapX = (int)(m_OriginRect.X * SelMap.X / (float)iMapimg.Width);

                if (nnn2 >= iMapimg.Height - 2)
                    nMapY = (int)(m_OriginRect.Y - m_imgRect.Height);

                else
                    nMapY = (int)(m_OriginRect.Y * SelMap.Y / (float)iMapimg.Height);


                //nMapX = (m_OriginRect.X * nSelMapX / iMapimg.Width);
                //nMapY = (m_OriginRect.Y * nSelMapY / iMapimg.Height);
                //if (nMapY > m_OriginRect.Y - m_PixelSize.Y)
                //nMapY = m_OriginRect.Y - m_PixelSize.Y;
                //if (nMapX > m_OriginRect.X - m_PixelSize.X)
                //    nMapX = m_OriginRect.X - m_PixelSize.X;
                m_imgRect.Y = nMapY;
                m_imgRect.X = nMapX;
                if (m_imgLoaded != null)
                {
                    // m_stCurCoord.X1.Text = (m_imgRect.X + (m_imgRect.Width / 2)).ToString();
                    //m_stCurCoord.Y1.Text = (m_imgRect.Y + (m_imgRect.Height / 2)).ToString();
                }
                Pic_Main.Invalidate();
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        public void m_fnUseEventRectOn(int nKey, bool bflag)
        {
            try
            {
                m_fnUseEventRectView(0, bflag);
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        private void m_fnUseEventRectView(int nUseKey, bool bRcvFlg)
        {
            try
            {
                struRect struRoi;

                if (!m_DicRect.TryGetValue(nUseKey, out struRoi))
                {
                    struRoi.pen = new Pen(Color.White);
                    struRoi.RectList = new List<RectangleF>();
                    m_DicRect.Add(nUseKey, struRoi);
                }

                SetUseRectView(nUseKey, bRcvFlg);

                m_fnUseRectDraw(bRcvFlg);
                m_nUseRctNum = nUseKey;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }

        private void SetUseRectView(int nKey, bool bUseRectView)
        {
            try
            {
                struRect struRoi;

                if (m_DicRect.TryGetValue(nKey, out struRoi))
                {
                    struRoi.bUseRectView = bUseRectView;
                    m_DicRect[nKey] = struRoi;
                }
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }
        private void m_fnUseRectDraw(bool bRcvFlg)
        {
            m_bUseRect = bRcvFlg;
        }

        public void m_fnSetRect(Rectangle rect)
        {
            this.m_imgRect = rect;
        }

        public PictureBox GetPictureBoxObj()
        {
            return Pic_Main;
        }
    }
}
