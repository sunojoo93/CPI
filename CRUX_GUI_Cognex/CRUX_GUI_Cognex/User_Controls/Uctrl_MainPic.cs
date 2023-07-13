using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading.Tasks;
using System.Drawing.Drawing2D;
using System.Threading;
using System.Diagnostics;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.IO;
using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.Utils;

namespace CRUX_GUI_Cognex.User_Controls
{

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct struRect
    {
        public Pen pen;
        public List<Rectangle> RectList;
        public bool bUseRectView;
        public Rectangle stRectCoord;


        public Pen GetPenVal()
        {
            return this.pen;
        }
        public List<Rectangle> GetRectListVal()
        {
            return this.RectList;
        }
        public bool GetUseRectView()
        {
            return this.bUseRectView;
        }

        public Rectangle GetRectCoord()
        {
            return this.stRectCoord;
        }
    }


    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct struPoint
    {
        public int nViewKind;
        public Pen pen;
        public List<Point> PointList;
        public Dictionary<int, List<Point>> PolygonList;
        public bool bUsePointView;
        public Point stPointCoord;
        public bool bEndPolygon;
        public bool bUsePolygon;
        public int nCurPolygonKey;
        public int GetViewKind()
        {
            return this.nViewKind;
        }
        public Pen GetPenVal()
        {
            return this.pen;
        }
        public List<Point> GetPointListVal()
        {
            return this.PointList;
        }
        public bool GetUsePointView()
        {
            return this.bUsePointView;
        }
        public Point GetPointCoord()
        {
            return this.stPointCoord;
        }
        public Dictionary<int, List<Point>> GetPolygonListVal()
        {
            return this.PolygonList;
        }
        public bool IsEndPolygon()
        {
            return this.bEndPolygon;
        }

    }


    /// <summary>
    /// 작업일 : 2016.09.26
    /// 작성자 : 임경민 (IKM)
    /// 클래스 : Uctrl_MainPic
    /// 구  분 : User Control Class
    /// 기  능 : ROI 및 영상 호출에 사용 (ROI, Aling Mark, 원점 좌표, POL Mark, 비검사 영역 설정 가능)
    /// 기  타 : 확대, 축소, 미니맵, 좌표 이동 기능
    /// </summary>
    public partial class Uctrl_MainPic : UserControl
    {
        //LIVE 
        ////////////////////////////////////////   
        //private float Ori_ratio = 1;
        private int m_nCamIndex = 0;
        private bool m_bLiveFlg;
        private bool _threadFlg;
        private bool _DoWork;
        //private Bitmap m_imgLive;
        /// <summary>기  능 : 카메라 티칭용 분할 화면 오프셋 /작성자 : 임경민 (IKM) </summary>
        private bool m_bDivisionModeFlg;
        // private Rectangle m_recLive;
        //test
        private Point m_PixelSize = new Point(6576, 4384);
        ////////////////////////////////////////

        /// <summary>기  능 : 영상 컨트롤의 상태값 표시 /작성자 : 임경민 (IKM) </summary>
        private Label m_strImageSate;

        //         // ROI 변수
        //         ////////////////////////////////////////     
        private int m_nUseRctNum;
        private int m_nUsePntNum;

        private Dictionary<int, struRect> m_DicRect;
        private Dictionary<int, struPoint> m_DicPnt;
        //KYH 2021.11.01
        private Dictionary<int, struRect> m_DicFocusROI;

        ////////////////////////////////////////
        // Guide Line 변수
        ////////////////////////////////////////                      
        private Pen m_penGuideLine;
        private Pen m_penGuideLine2;
        private Pen m_penGuideLine3;
        private Rectangle m_GuideLineRect;
        //2021.10.28 KYH
        private Rectangle[] m_GuideLineRect2;
        private Rectangle m_GuideLineCenter;
        private Rectangle[] m_GuideLineRect3;
        /// <summary>기  능 : 카메라 티칭용 가이드 라인의 각도에 대한 오프셋 /작성자 : 임경민 (IKM) </summary>
        private double m_dGuideLineOffsetT = 0.0F;
        ////////////////////////////////////////

        // Line Profile 변수
        ////////////////////////////////////////                             
        private Pen m_penLineProfile;
        ////////////////////////////////////////

        // Line Draw 변수
        ////////////////////////////////////////   
        private string m_strLineName;
        private Point m_pntLineRightClick;



        private bool m_bLineBtnDown = false;
        Rectangle m_Linerect = new Rectangle();
        Rectangle m_LastLinerect = new Rectangle();
        private Point pntLineClick = new Point();
        private Pen m_penLine;
        ////////////////////////////////////////

        // Rectangle Draw 변수
        ////////////////////////////////////////    
        private Point m_pntRectRightClick;
        private bool m_bRectBtnDown = false;
        private bool m_bFocusDown = false;
        private Rectangle m_RectCom = new Rectangle();
        private Point m_pntRectClick = new Point();
        ////////////////////////////////////////

        private Pen m_penUniformity;


        // 이미지 컨트롤 변수
        ////////////////////////////////////////
        private string m_strImgPath = "";
        private int m_nVisionNo = 0;
        private int m_nImgeID = 0;
        private Thread m_thread;
        public Uctrl_MiniMap m_ucPicMiniMap;
        private double m_dRecRatio;
        private Point m_imgPoint;
        private Point m_LastPoint;
        private Point m_pointClick;
        private Point m_pointClick2;
        private Point m_pointRightClick;
        private Point m_pointROIClick;
        private Point m_pointROIRIGHTClick;
        private Point m_pointROIRIGHT_MOVE_Click;
        private Point m_pointROIRIGHT_MOVE_Click2;
        private Point m_pntDivisionModeClick;
        private Rectangle m_imgRect;
        private Bitmap m_imgLoaded = null;
        private STRU_DIVISION_COORD m_DivLoad = new STRU_DIVISION_COORD();
        private int m_nDivStartX = 0;
        private int m_nDivStartY = 0;
        private int m_nDivEndX = 0;
        private int m_nDivEndY = 0;
        private Image m_imgMiniMap;
        private double m_dRratio = 1F;
        private double[] m_dFocusVal = new double[5];
        private PixelFormat m_pxlForamt = System.Drawing.Imaging.PixelFormat.Format8bppIndexed;
        bool m_bIsGrayScale = true;
        private double m_dResolution = 0.0F;
        private int m_nCamPsModeRatio = 1;
        /////////////////////////////////////////

        // Envet 사용 유무 플래그
        // Zoom In Out,Mini Map, Mouse Move, Roi, Align, 원점 좌표        
        private bool m_bUseMiniMap = false;
        private bool m_bImgFocus = false;
        private bool m_bUseZoomInOut = true;
        private bool m_bUseDragMove = true;
        private bool m_bUsePoint = false;
        private bool m_bUseLine = false;
        private bool m_bUseRect = false;

        private bool m_bUseRoiMultiLine = true;
        private bool m_bUseGuideLineView = true;
        private bool m_bUseFocusLineView = false;
        private bool m_bunfocus = true;
        private bool m_bUseGvView = false;
        private bool m_bUseLineProfile = false;
        private bool m_bUseContrast = false;

        private bool m_bUseFocusValeView = false;
        private bool m_bUseDistanceView = false;

        private bool m_bFoucs = false;
        private bool m_bFocusClick = false;

        private const int ndownclick = 0;

        private bool m_bCenter = false;
        private bool m_bL_T = false;
        private bool m_bL_B = false;
        private bool m_bR_T = false;
        private bool m_bR_B = false;

        ////////////////////////////////////////////////

        // 카메라에 송신 할 영상 회전 축
        double m_setRotateAngle = 0.0F;

        // 라벨: 좌표 데이터 용
        /////////////////////////////////////////
        //KYH 2021.11.01
        STRU_LABEL_COORD m_stFocusCurCoord;
        STRU_LABEL_COORD m_stCurCoord;
        STRU_LABEL_COORD m_stPointCoord;
        STRU_LABEL_COORD m_stGdOrgCoord;
        STRU_LABEL_COORD m_stLineProfileCoord;
        STRU_LABEL_COORD m_stDrawLineCoord;

        Label m_lbContrastCoord;
        Label[] m_stRGBColor = new Label[3];
        Label m_stFocus = new Label();

        public Label m_lbNowRatio = new Label();
        private Rectangle[] rect2;

        public struct STRU_LABEL_COORD
        {
            public Label X1;
            public Label Y1;
            public Label X2;
            public Label Y2;
        }

        public struct STRU_DIVISION_COORD
        {
            public Bitmap[] m_imgDivLoaded;
            public Rectangle[] m_imgRect;
        }




        public enum E_DIV_NUM : int
        {
            E_LEFT_TOP = 0,
            E_RIGHT_TOP,
            E_LEFT_BOTTOM,
            E_RIGHT_BOTTOM,
            E_CENTER,
        };

        public enum E_ROI_NUM : int
        {
            R_LEFT_TOP = 0,
            R_LEFT_BOTTOM,
            R_RIGHT_TOP,
            R_RIGHT_BOTTOM,
            R_CENTER,
        };




        /////////////////////////////////////////
        /// <summary>기  능 : 이미지 처리 클래스 초기화 /작성자 : 임경민 (IKM) </summary>
        public Uctrl_MainPic()
        {

            InitializeComponent();
            //             SetStyle(ControlStyles.OptimizedDoubleBuffer, true);
            //             SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            //             SetStyle(ControlStyles.ResizeRedraw, true);
            //             SetStyle(ControlStyles.UserPaint, true);



            //Size sizeImg = new Size(nWidth / 4 * 4, nHeight / 4 * 4);
            //m_bUseMiniMap = bUseMiniMap;
            //m_ucPicMiniMap = ucPicMiniMap;// new Uctrl_MiniMap(this);

            //if (m_bUseMiniMap) m_ucPicMiniMap.Visible = true;
            //else m_ucPicMiniMap.Visible = false;

            //this.Location = new Point(nLocX, nLocY);
            //this.Size = sizeImg;
            //picMainImg.Size = sizeImg;

            //LinkStrImageState(ref strImageSate);

            ///////////////////////////////////////////////////////////////////////////////
            // ruler control
            //            rulerControl_H.Width = picMainImg.Width;            
            //            rulerControl_H.MajorInterval = 10;         
            //            rulerControl_V.Height = picMainImg.Height;
            //            rulerControl_V.MajorInterval = 10;         
            ///////////////////////////////////////////////////////////////////////////////

            vScrollBar1.Location = new Point(Width + 1, picMainImg.Location.Y);
            hScrollBar1.Location = new Point(picMainImg.Location.X, Height + 1);
            vScrollBar1.Height = Height;
            hScrollBar1.Width = Width;
            //pnlBackgrd.Size = picMainImg.Size;
            //pnlBackgrd.Location = picMainImg.Location;
            //prbImgLoad.Location = new Point((picMainImg.Width - prbImgLoad.Width) / 2, (picMainImg.Height - prbImgLoad.Height) / 2);
            //Parent.Controls.Add(this);
            //pnlBackgrd.Controls.Add(lbImgLoad);
            //pnlBackgrd.Controls.Add(prbImgLoad);
            //lbImgLoad.BackColor = Color.Transparent;
            //lbImgLoad.Parent = this.picMainImg;

            InitRectData();
            InitPointData();
            InitGuideLineData(new Color());
            InitLineProfileData(new Color());
            InitContrastData(new Color());
        }
        public void Initialize(Form Parent, ref Uctrl_MiniMap ucPicMiniMap, ref Label strImageSate, int nImgeID, int nVisionNo, bool bUseMiniMap = false, int nWidth = 870, int nHeight = 700, int nLocX = 9, int nLocY = 47)
        {
            m_nImgeID = nImgeID;
            m_nVisionNo = nVisionNo;

            Size sizeImg = new Size(nWidth / 4 * 4, nHeight / 4 * 4);
            m_bUseMiniMap = bUseMiniMap;
            m_ucPicMiniMap = ucPicMiniMap;// new Uctrl_MiniMap(this);

            if (m_bUseMiniMap) m_ucPicMiniMap.Visible = true;
            else m_ucPicMiniMap.Visible = false;

            this.Location = new Point(nLocX, nLocY);
            this.Size = sizeImg;
            picMainImg.Size = sizeImg;

            LinkStrImageState(ref strImageSate);

            ///////////////////////////////////////////////////////////////////////////////
            // ruler control
            //            rulerControl_H.Width = picMainImg.Width;            
            //            rulerControl_H.MajorInterval = 10;         
            //            rulerControl_V.Height = picMainImg.Height;
            //            rulerControl_V.MajorInterval = 10;         
            ///////////////////////////////////////////////////////////////////////////////

            vScrollBar1.Location = new Point(nWidth + 1, picMainImg.Location.Y);
            hScrollBar1.Location = new Point(picMainImg.Location.X, nHeight + 1);
            vScrollBar1.Height = nHeight;
            hScrollBar1.Width = nWidth;
            //pnlBackgrd.Size = picMainImg.Size;
            //pnlBackgrd.Location = picMainImg.Location;
            //prbImgLoad.Location = new Point((picMainImg.Width - prbImgLoad.Width) / 2, (picMainImg.Height - prbImgLoad.Height) / 2);
            Parent.Controls.Add(this);
            //pnlBackgrd.Controls.Add(lbImgLoad);
            //pnlBackgrd.Controls.Add(prbImgLoad);
            //lbImgLoad.BackColor = Color.Transparent;
            //lbImgLoad.Parent = this.picMainImg;

            InitRectData();
            InitPointData();
            InitGuideLineData(new Color());
            InitLineProfileData(new Color());
            InitContrastData(new Color());
        }
        /// 기  능  : 이미지의 현재 상태를 텍스트로 반환
        /// /반환값 : string
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Form_RcpTeach    
        /// </summary>
        private void LinkStrImageState(ref Label strImageSate)
        {
            m_strImageSate = strImageSate;
        }

        /// <summary>
        /// 기  능  : 이미지의 현재 상태를 텍스트로 반환
        /// /반환값 : string
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Form_RcpTeach    
        /// </summary>
        private void SetImageState(Label strImageSate)
        {
            m_strImageSate = strImageSate;
        }

        // Rect 초기화
        /// /////////////////////////////////////////////////////////////////////////////////        
        private void InitRectData()
        {
            m_DicRect = new Dictionary<int, struRect>();
            m_DicFocusROI = new Dictionary<int, struRect>();
            //m_CommonRect = new Rectangle();
            //m_penRoi = new Pen(new Color(), 2);
            // m_penRoi.DashStyle = System.Drawing.Drawing2D.DashStyle.Dot;        		
        }
        public void InitRectColor(int nKey, Color color)
        {
            Pen penRoi = new Pen(color, 2);
            penRoi.DashStyle = System.Drawing.Drawing2D.DashStyle.Dot;
            SetRectPenVal(nKey, penRoi);
        }
        private int m_fnGetUseTotRectCount()
        {
            return m_DicRect.Count;
        }
        /// /////////////////////////////////////////////////////////////////////////////////

        // Point 초기화
        /// /////////////////////////////////////////////////////////////////////////////////
        private void InitPointData()
        {
            m_DicPnt = new Dictionary<int, struPoint>();
            //m_CommonPoint = new Point();           
        }
        public void InitPointColor(int nKey, Color color, int nViewKind = (int)Enums.E_PNT_VIEW_KIND.CROSS)
        {
            Pen pen = new Pen(color, 2);
            pen.DashStyle = System.Drawing.Drawing2D.DashStyle.Solid;
            SetPntPenVal(nKey, pen);
            SetViewKind(nKey, nViewKind);
        }
        private int m_fnGetUseTotPointCount()
        {
            return m_DicPnt.Count;
        }
        /// /////////////////////////////////////////////////////////////////////////////////            


        public void InitGuideLineData(Color color)
        {
            m_penGuideLine = new Pen(color, 2);
            m_penGuideLine2 = new Pen(color, 1);
            m_penGuideLine3 = new Pen(color, 1);

            m_penGuideLine.DashStyle = System.Drawing.Drawing2D.DashStyle.Solid;
            m_penGuideLine.Alignment = PenAlignment.Center;

            m_penGuideLine2.DashStyle = System.Drawing.Drawing2D.DashStyle.Dot;
            m_penGuideLine2.Alignment = PenAlignment.Center;

            m_penGuideLine3.DashStyle = System.Drawing.Drawing2D.DashStyle.Dot;
            m_penGuideLine3.Alignment = PenAlignment.Center;
        }


        public void InitLineProfileData(Color color)
        {
            pntLineClick = new Point();
            m_penLineProfile = new Pen(color, 1);

            m_penLineProfile.DashStyle = System.Drawing.Drawing2D.DashStyle.Solid;
            m_penLineProfile.Alignment = PenAlignment.Center;
        }

        public void InitContrastData(Color color)
        {
            m_pntRectClick = new Point();
            m_penUniformity = new Pen(color, 1);

            m_penUniformity.DashStyle = System.Drawing.Drawing2D.DashStyle.Solid;
            m_penUniformity.Alignment = PenAlignment.Center;
        }

        public void InitLinePen(string strLineName, Color color)
        {
            m_pntRectClick = new Point();
            m_penLine = new Pen(color, 3);
            m_strLineName = strLineName;

            m_penLine.DashStyle = System.Drawing.Drawing2D.DashStyle.Solid;
            m_penLine.Alignment = PenAlignment.Center;
        }

        public string GetLineName()
        {
            return m_strLineName;
        }

        public Color GetLinePenColor()
        {
            return m_penLine.Color;
        }

        public void m_fnSetResolution(double dResolution)
        {
            m_dResolution = dResolution;
        }

        public double m_fnGetResolution()
        {
            return m_dResolution;
        }

        ///  /// <summary>
        /// 기  능 : Picture Box 상의 좌표를 실제 사이즈의 픽셀 좌표 변환
        /// /반환값 : 
        /// /입  력 : int 표시되는 영역의 시작 점, int 변환할 Picture Box 상의 좌표, int 실제 영상의 사이즈, int Picture Box의 사이즈, double 확대/축소 배율
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary>      
        public int m_fnCalCoordinate(int nOrignCoord, int nCurCoord, int nImgRectSize, int nFovSize, double nRatio)
        {

            if (m_bDivisionModeFlg)
            {
                int nPosition = 0;

                if (nCurCoord > nFovSize)
                    nPosition = nCurCoord - nFovSize;
                else
                    nPosition = nCurCoord;

                return (int)Math.Round(nOrignCoord + (nPosition / nRatio));
            }
            else
                return nOrignCoord + (int)Math.Round((Convert.ToDouble(nImgRectSize * (Math.Abs(nCurCoord))) / (nFovSize - 1)));

        }


        /// <summary>기  능 : 마우스 휠 이용 확대/축소 기능 사용 유무 /작성자 : 임경민 (IKM) </summary>
        public void m_fnUseEventMouseWheelZoomInOut(bool bRcvFlg)
        {
            m_bUseZoomInOut = bRcvFlg;
        }
        /// <summary>기  능 : 마우스 Drag & Move 기능 사용 유무 /작성자 : 임경민 (IKM) </summary>
        public void m_fnUseEventDragMove(bool bRcvFlg)
        {
            m_bUseDragMove = bRcvFlg;
        }
        /// <summary>기  능 : ROI 영역 선택 기능 사용 유무 /작성자 : 임경민 (IKM) </summary>
        private void m_fnUseEventRectView(int nUseKey, bool bRcvFlg)
        {
            struRect struRoi;

            if (!m_DicRect.TryGetValue(nUseKey, out struRoi))
            {
                struRoi.pen = new Pen(Color.White);
                struRoi.RectList = new List<Rectangle>();
                m_DicRect.Add(nUseKey, struRoi);
            }
            SetUseRectView(nUseKey, bRcvFlg);

            if (bRcvFlg)
            {
                m_fnUseRectDraw(bRcvFlg);
                m_nUseRctNum = nUseKey;
            }
        }

        private void m_fnUseEventFocusView(int nUseKey, bool bRcvFlg)
        {
            struRect strufocusRoi;

            if (!m_DicFocusROI.TryGetValue(nUseKey, out strufocusRoi))
            {
                strufocusRoi.pen = new Pen(Color.Red);
                strufocusRoi.RectList = new List<Rectangle>();
                m_DicFocusROI.Add(nUseKey, strufocusRoi);
            }
            SetUseFocusView(nUseKey, bRcvFlg);
        }

        public void m_fnUseEventRectOn(int nKey)
        {
            for (int nPntNum = 0; nPntNum < (int)Enums.DrawPnt.MAX; nPntNum++)
            {
                m_fnUseEventPntView(nPntNum, false);
            }
            for (int nRoiNum = 0; nRoiNum < (int)Enums.DrawRct.MAX; nRoiNum++)
            {
                if (nKey != nRoiNum)
                    m_fnUseEventRectView(nRoiNum, false);
                else
                    m_fnUseEventRectView(nRoiNum, true);
            }

        }

//         public void m_fnUseEventFocusROI(int nKey)
//         {
// 
//             for (int nRoiNum = 0; nRoiNum < (int)Enums.DrawFocusROI.MAX; nRoiNum++)
//             {
//                 if (nKey != nRoiNum)
//                     m_fnUseEventFocusView(nRoiNum, false);
//                 else
//                     m_fnUseEventFocusView(nRoiNum, true);
//             }
// 
//         }

        public void m_fnUseEventRectOff()
        {
            m_fnUseRectDraw(false);

            for (int nPntNum = 0; nPntNum < (int)Enums.DrawPnt.MAX; nPntNum++)
            {
                m_fnUseEventPntView(nPntNum, false);
            }
            for (int nRoiNum = 0; nRoiNum < (int)Enums.DrawRct.MAX; nRoiNum++)
            {
                m_fnUseEventRectView(nRoiNum, false);
            }
        }

        /// <summary>기  능 : ROI 영역 선택 기능 사용 유무 /작성자 : 임경민 (IKM) </summary>
        public int m_fnGetUseRctNum()
        {
            return m_nUseRctNum;
        }


        /// <summary>기  능 : ROI 영역 선택 기능 사용 유무 /작성자 : 임경민 (IKM) </summary>
        private void m_fnUseEventPntView(int nUseKey, bool bRcvFlg)
        {
            struPoint struPnt;

            if (!m_DicPnt.TryGetValue(nUseKey, out struPnt))
            {
                struPnt.pen = new Pen(Color.White);
                struPnt.PointList = new List<Point>();
                m_DicPnt.Add(nUseKey, struPnt);
            }
            SetUsePntView(nUseKey, bRcvFlg);
            if (bRcvFlg)
            {
                m_fnUsePointDraw(bRcvFlg);
                m_nUsePntNum = nUseKey;
            }
        }

        /// <summary>기  능 : ROI 영역 선택 기능 사용 유무 /작성자 : 임경민 (IKM) </summary>
        public int m_fnGetUsePntNum()
        {
            return m_nUsePntNum;
        }

        public void m_fnUseEventPointOn(int nKey)
        {
            for (int nRoiNum = 0; nRoiNum < (int)Enums.DrawRct.MAX; nRoiNum++)
            {
                m_fnUseEventRectView(nRoiNum, false);
            }

            for (int nPntNum = 0; nPntNum < (int)Enums.DrawPnt.MAX; nPntNum++)
            {
                if (nKey != nPntNum)
                    m_fnUseEventPntView(nPntNum, false);
                else
                    m_fnUseEventPntView(nPntNum, true);
            }
        }

        public void m_fnUseEventPointOff()
        {
            m_fnUsePointDraw(false);

            for (int nRoiNum = 0; nRoiNum < (int)Enums.DrawRct.MAX; nRoiNum++)
            {
                m_fnUseEventRectView(nRoiNum, false);
            }

            for (int nPntNum = 0; nPntNum < (int)Enums.DrawPnt.MAX; nPntNum++)
            {
                m_fnUseEventPntView(nPntNum, false);
            }
        }

        public void m_fnUseEventFocusRect()
        {
            for (int nRoiNum = 0; nRoiNum < (int)Enums.DrawFocusROI.MAX; nRoiNum++)
            {
                m_fnUseEventFocusView(nRoiNum, false);
            }
        }

        /// <summary>기  능 : 티칭용 가이드 라인 표시 기능 사용 유무 /작성자 : 임경민 (IKM) </summary>
        public void m_fnUseEventGuideLineView(bool bRcvFlg)
        {
            m_bUseGuideLineView = bRcvFlg;
        }
        public void m_fnUseEventFocusView(bool bRcvFlg)
        {
            m_bUseFocusLineView = bRcvFlg;
            m_bFocusClick = bRcvFlg;
            m_bunfocus = bRcvFlg;

            if (bRcvFlg)
            {
                for (int nRoiNum = 0; nRoiNum < (int)Enums.DrawFocusROI.MAX; nRoiNum++)
                {
                    m_fnUseEventFocusView(nRoiNum, false);
                }
            }
        

        }
        /// <summary>기  능 : 선택 영역 GV값 표시 기능 사용 유무 /작성자 : 임경민 (IKM) </summary>
        public void m_fnUseGvView(bool bRcvFlg)
        {
            m_bUseGvView = bRcvFlg;
        }
        /// <summary>기  능 : 라인 프로파일 기능 사용 유무 /작성자 : 임경민 (IKM) </summary>
        public void m_fnUseLineProfile(bool bRcvFlg)
        {
            m_bUseLineProfile = bRcvFlg;
            m_bUseLine = bRcvFlg;
        }
        /// <summary>기  능 : 포커스 수치 측정 기능 사용 유무 /작성자 : 임경민 (IKM) </summary>
        public void m_fnUseFocusVale(bool bRcvFlg)
        {
            m_bUseFocusValeView = bRcvFlg;
        }
        /// <summary>기  능 : 라인의 길이 값 표시 기능 사용 유무 /작성자 : 임경민 (IKM) </summary>
        public void m_fnUseDistanceView(bool bRcvFlg)
        {
            m_bUseDistanceView = bRcvFlg;
        }
        /// <summary>기  능 : 포인트 기능 사용 유무 /작성자 : 임경민 (IKM) </summary>
        private void m_fnUsePointDraw(bool bRcvFlg)
        {
            m_bUsePoint = bRcvFlg;
            m_bUseRect = false;
            m_bUseLine = false;
        }
        /// <summary>기  능 : LINE Draw 기능 사용 유무 /작성자 : 임경민 (IKM) </summary>
        public void m_fnUseLineDraw(bool bRcvFlg)
        {
            m_bUseLine = bRcvFlg;
            m_bUseRect = false;
            m_bUsePoint = false;
        }

        /// <summary>기  능 : Rectangle Draw 기능 사용 유무 /작성자 : 임경민 (IKM) </summary>
        private void m_fnUseRectDraw(bool bRcvFlg)
        {
            m_bUseRect = bRcvFlg;
            m_bUseLine = false;
            m_bUsePoint = false;
        }

        /// <summary>기  능 : 대비 측정 기능 사용 유무 /작성자 : 임경민 (IKM) </summary>
        public void m_fnUseUniformity(bool bRcvFlg)
        {
            m_bUseContrast = bRcvFlg;
            m_bUseRect = bRcvFlg;
        }

        /// <summary>기  능 : ROI 다중 선택 기능 사용 유무 /작성자 : 임경민 (IKM) </summary>
        public void m_fnUseRoiMultiLine(bool bRcvFlg)
        {
            m_bUseRoiMultiLine = bRcvFlg;
        }

        public void ucMiniPic(ref Uctrl_MiniMap ucPicMiniMap)
        {
            m_ucPicMiniMap = ucPicMiniMap;

            if (m_bUseMiniMap)
                m_ucPicMiniMap.Visible = true;
            else
                m_ucPicMiniMap.Visible = false;
        }

        private void ucMainPic_Load(object sender, EventArgs e)
        {
            picMainImg.MouseWheel
                        += new System.Windows.Forms.MouseEventHandler(this.picMainImg_MouseWheel);
        }

        /// <summary>
        /// 기  능 : LIVE 영상 ON 
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary>      
        delegate bool LiveStartCallback(int nCamIndex, int nWidth, int nHeight/*byte[] liveimage*/);
        public bool LiveStart(int nCamIndex, int nWidth, int nHeight/*byte[] liveimage*/)
        {
            try
            {
                // bool bRet = false;

                m_fnEndImageLoadThread();// 이전에 진행 중인 이미지 호출 스레드가 있으면 종료 한다.

                //                 if (this.InvokeRequired)
                //                 {
                // 
                //                     LiveStartCallback delOpenImg = new LiveStartCallback(LiveStart);
                //                     this.Invoke(delOpenImg, nCamIndex, nWidth, nHeight /*, liveimage*/);
                //                     return true;
                //                 }


                //m_fnSetPixelForamt(m_fnBandwidthToPixelformat());

                m_nCamIndex = nCamIndex;

                // 호출된 비트맵이 컬러 이미지인지 그레이 이미지 인지 구분

                m_DivLoad.m_imgDivLoaded = new Bitmap[5];
                m_DivLoad.m_imgRect = new Rectangle[5];

                m_imgRect = new Rectangle(0, 0, 0, 0);

                m_PixelSize = m_fnGetCameraFovSize();

                CmdMsgParam Param = new CmdMsgParam();

                int nRet = Consts.APP_NG;

                Param.ClearOffset();
                Param.SetUInteger(Consts.TRUE);
                nRet = Systems.g_Ipc.SendCommand((ushort)((m_fnGetVisionNo() + 1) * 100 + IpcConst.CAMERA_TASK + m_nCamIndex), IpcConst.CAMERA_FUNC, IpcConst.LIVE_START_END,
                                                    IpcInterface.CMD_TYPE_RES, 100, Param.GetByteSize(), Param.GetParam());
                if (nRet == Consts.APP_NG) return false;
                Param.SetOffset(0);

                int nChannelCnt = Param.GetInteger();
                m_fnSetPixelForamt(m_fnBandwidthToPixelformat(nChannelCnt));
                m_bIsGrayScale = (m_fnGetPixelForamt().ToString().ToUpper().Contains("RGB")) ? false : true;

                m_imgRect.Width = m_PixelSize.X;
                m_imgRect.Height = m_PixelSize.Y;

                EndDspImageLoad();
                ClearAllArea();

                m_imgLoaded = new Bitmap(m_PixelSize.X, m_PixelSize.Y);
                // m_imgLive = new Bitmap(picMainImg.Width, picMainImg.Height);

                m_dRratio = 1.0;

                m_lbNowRatio.Text = string.Format("{0:00.00}", m_dRratio);

                m_imgPoint = new Point(picMainImg.Width / 2, picMainImg.Height / 2);

                m_imgRect = new Rectangle(0, 0, m_PixelSize.X, m_PixelSize.Y);
                //m_imgRect = new Rectangle(0, 0, m_imgLive.Width, m_imgLive.Height);
                m_pointClick = m_imgPoint;

                hScrollBar1.Minimum = 0;
                if (m_imgRect.Width - m_PixelSize.X > 0)
                    hScrollBar1.Maximum = -m_PixelSize.X - m_imgRect.Width;
                else hScrollBar1.Maximum = 0;
                vScrollBar1.Minimum = 0;
                if (m_imgRect.Height - m_PixelSize.Y > 0)
                    vScrollBar1.Maximum = -m_PixelSize.Y - m_imgRect.Height;
                else vScrollBar1.Maximum = 0;


                SetImageLoadThreadFlg(true);
                SetThreadWork(true);
                SetLiveFlg(true);

                Thread thread = new Thread(new ThreadStart(delegate () { ThreadImagLoad(); }));
                thread.IsBackground = true;
                m_thread = thread;
                m_thread.Start();
                picMainImg.Invalidate();

            }
            catch (Exception)
            {
                //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E091", Globals.nLanguageFlg), true);
            }

            return true;
        }

        /// <summary>
        /// 기  능 : 카메라의 FOV 사이즈를 반환한다.
        /// /반환값 : Point
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary> 
        public Point m_fnGetCameraFovSize()
        {
            CmdMsgParam Param = new CmdMsgParam();
            uint nParam = 0;
            int nRet = Consts.APP_NG;

            Point PixelSize = new Point(6576, 4384);

            Param.ClearOffset();
            Param.SetUInteger(nParam);
            nRet = Systems.g_Ipc.SendCommand((ushort)((m_fnGetVisionNo() + 1) * 100 + IpcConst.CAMERA_TASK + m_nCamIndex), IpcConst.CAMERA_FUNC, IpcConst.LIVE_GET_WIDTH,
                                                IpcInterface.CMD_TYPE_RES, 100, Param.GetByteSize(), Param.GetParam());
            if (nRet == Consts.APP_NG) return PixelSize;

            Param.SetOffset(0);
            if (nRet == Consts.APP_OK)
                PixelSize.X = Convert.ToInt32(Param.GetUInteger());

            Param.ClearOffset();
            Param.SetUInteger(nParam);
            nRet = Systems.g_Ipc.SendCommand((ushort)((m_fnGetVisionNo() + 1) * 100 + IpcConst.CAMERA_TASK + m_nCamIndex), IpcConst.CAMERA_FUNC, IpcConst.LIVE_GET_HEIGHT,
                                                IpcInterface.CMD_TYPE_RES, 100, Param.GetByteSize(), Param.GetParam());
            if (nRet == Consts.APP_NG) return PixelSize;

            Param.SetOffset(0);
            if (nRet == Consts.APP_OK)
                PixelSize.Y = (int)Param.GetUInteger();

            return PixelSize;
        }

        /// <summary>
        /// 기  능 : 카메라의 PS 모드 배율을 설정 한다.
        /// /반환값 : 
        /// /입  력 : Point
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary> 
        public void m_fnSetCameraFovRatio(Point pntCamFovSize)
        {
            m_nCamPsModeRatio = 1;

            if (m_imgLoaded != null)
            {
                if (m_imgLoaded.Width >= pntCamFovSize.X * 2 - 1 && m_imgLoaded.Height >= pntCamFovSize.Y * 2 - 1)
                    m_nCamPsModeRatio = 2;
                else if (m_imgLoaded.Width >= pntCamFovSize.X * 3 - 1 && m_imgLoaded.Height >= pntCamFovSize.Y * 3 - 1)
                    m_nCamPsModeRatio = 3;
            }

        }

        /// <summary>
        /// 기  능 : 카메라의 PS 모드 배율을 반환 한다.
        /// /반환값 : int
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary> 
        public int m_fnGetCameraFovRatio()
        {
            return m_nCamPsModeRatio;
        }
        /// <summary>
        /// 기  능 : LIVE 영상 OFF 
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary>      
        delegate int LiveEndCallback();
        public int LiveEnd()
        {
            int nRet = Consts.APP_OK;
            try
            {
                if (this.InvokeRequired)
                {

                    LiveEndCallback delOffImg = new LiveEndCallback(LiveEnd);
                    this.Invoke(delOffImg);
                    return nRet;
                }

                m_imgRect = new Rectangle(0, 0, 0, 0);

                m_fnEndImageLoadThread();

                Thread.Sleep(100);

                CmdMsgParam Param = new CmdMsgParam();
                
                Param.ClearOffset();
                Param.SetUInteger(Consts.FALSE);
                nRet = Systems.g_Ipc.SendCommand((ushort)((m_fnGetVisionNo() + 1) * 100 + IpcConst.CAMERA_TASK + m_nCamIndex), IpcConst.CAMERA_FUNC, IpcConst.LIVE_START_END,
                                                    IpcInterface.CMD_TYPE_RES, IpcInterface.CMD_TIMEOUT, Param.GetByteSize(), Param.GetParam());
                Param.SetOffset(0);

                m_bDivisionModeFlg = false;


                SetLiveFlg(false);   
            }
            catch (Exception)
            {
                //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E091", Globals.nLanguageFlg), true);
            }

            return nRet;
        }


        /// <summary>
        /// 기  능 : LIVE 영상 호출 플래그 설정.
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary>        
        public void SetLiveFlg(bool bFlg)
        {
            m_bLiveFlg = bFlg;
        }
        /// <summary>
        /// 기  능 : 이미지 호출 스레드 ON/OFF 플래그 설정
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary>      
        public void SetImageLoadThreadFlg(bool bFlg)
        {
            _threadFlg = bFlg;
        }
        /// <summary>
        /// 기  능 : 이미지 호출 스레드 Wait 상태 설정
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary>      
        public void SetThreadWork(bool bFlg)
        {
            _DoWork = bFlg;

            m_ucPicMiniMap.SetMinimapThreadWork(bFlg);
            ///////////////////////////////////////////////////////////////////////////////
            // ruler control

            // rulerControl_H.Visible = bFlg;
            // rulerControl_V.Visible = bFlg;
            ///////////////////////////////////////////////////////////////////////////////

        }
        /// <summary>
        /// 기  능 : LIVE 영상 호출 플래그 반환.
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary>      
        private bool GetLiveFlg()
        {
            return m_bLiveFlg;
        }
        /// <summary>
        /// 기  능 : 이미지 호출 스레드 ON/OFF 플래그 반환
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary>      
        public bool GetLiveThreadFlg()
        {
            return _threadFlg;
        }
        /// <summary>
        /// 기  능 : 이미지 호출 스레드 Wait 상태 반환
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary>      
        public bool GetThreadDoWork()
        {
            return _DoWork;
        }

        // delegate void ThreadImagLoadCallback();
        /// <summary>
        /// 기  능 : 이미지 로드 스레드 (카메라 테스크로 부터 실시간 영상 호출 스레드)
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary> 
        public void ThreadImagLoad()
        {
            if (m_imgLoaded == null) return;
            try
            {
                bool bMinimapStartFlg = true;

                while (GetLiveThreadFlg())
                {
                    if (Modes.NET_SIMULATION_MODE) Thread.Sleep(50);
                    else Thread.Sleep(30);

                    if (GetThreadDoWork())
                        if (this.InvokeRequired)
                        {
                            int nWidth = picMainImg.Width;
                            int nHeight = picMainImg.Height;

                            if (m_imgRect.X >= 0 && m_imgRect.Y >= -100)
                            {
                                if (m_bDivisionModeFlg)
                                {
                                    nWidth = nWidth / 2 / 4 * 4;
                                    nHeight = nHeight / 2 / 4 * 4;
       
                                    Point pntOldCoord = new Point();

                                    pntOldCoord.X = m_nDivEndX;
                                    pntOldCoord.Y = m_nDivEndY;

                                    m_nDivEndX = (int)Math.Round((nWidth) / m_dRratio);
                                    m_nDivEndY = (int)Math.Round((nHeight) / m_dRratio);

                                    for (int i = 0; i < 5; i++)
                                    {
                                        m_nDivStartX = (int)m_DivLoad.m_imgRect[i].X + ((pntOldCoord.X - m_nDivEndX) / 2);
                                        m_nDivStartY = (int)m_DivLoad.m_imgRect[i].Y + ((pntOldCoord.Y - m_nDivEndY) / 2);
                                        m_DivLoad.m_imgRect[i].X = m_nDivStartX;
                                        m_DivLoad.m_imgRect[i].Y = m_nDivStartY;
                                        m_DivLoad.m_imgRect[i].Width = m_nDivEndX;
                                        m_DivLoad.m_imgRect[i].Height = m_nDivEndY;

                                        m_DivLoad.m_imgDivLoaded[i] = m_fnLiveImageGrab(m_nDivStartX, m_nDivStartY, m_nDivEndX, m_nDivEndY, nWidth, nHeight);
                                    }
                                }
                                else
                                {
                                    m_imgLoaded = m_fnLiveImageGrab(m_imgRect.X, m_imgRect.Y, m_imgRect.Width, m_imgRect.Height, nWidth, nHeight);
                                }


                                if (GetLiveFlg())
                                {
                                    if (this.m_bUseMiniMap && bMinimapStartFlg)
                                    {
                                        m_ucPicMiniMap.LiveStart(m_fnGetImageID(), GetLiveFlg(), m_bIsGrayScale, m_fnGetPxlFormatIndex(), m_pxlForamt, m_PixelSize, m_nCamIndex);
                                        bMinimapStartFlg = false;
                                    }
                                }
                                else
                                {
                                    if (this.m_bUseMiniMap && bMinimapStartFlg)
                                    {
                                        if (this.m_imgLoaded != null)
                                        {
                                            m_ucPicMiniMap.BeginInvoke(new Action(() =>
                                            {
                                                this.m_ucPicMiniMap.m_fnSetOringSize();
                                                this.m_imgMiniMap = Utils.ImageProc.Resize(this.m_imgLoaded,
                                                        this.m_ucPicMiniMap.picMinImg.Width,
                                                        this.m_ucPicMiniMap.picMinImg.Height, false);
                                                this.m_ucPicMiniMap.picMapResize(new Point(this.m_imgMiniMap.Width, this.m_imgMiniMap.Height));
                                                this.m_ucPicMiniMap.m_fnSetMiniMapImage(this.m_imgMiniMap);
                                            }
                                            ));

                                            bMinimapStartFlg = false;
                                        }
                                    }
                                }
                                picMainImg.BeginInvoke(new Action(() => picMainImg.Invalidate()));
                            }
                            else
                            {
                                lbImgLoad.BeginInvoke(new Action(() => lbImgLoad.Enabled = true));

                            }
                        }


                }

                bMinimapStartFlg = true;
            }
            catch (Exception ex)
            {

                System.Diagnostics.Debug.WriteLine(ex.Message);
                //Systems.m_fnAddLog(m_fnGetVisionNo(), ex.StackTrace, ex.Message);
                m_imgLoaded.Dispose();
                // MessageBox.Show( ex.StackTrace, ex.Message);
            }
        }

        /// <summary>
        /// 기  능 : 현재 비트맵을 초기화 한다.
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary>        
        public void initImage()
        {
            Image image;
            Graphics g;
            m_imgLoaded = null;


            image = new Bitmap(picMainImg.Width, picMainImg.Height);
            g = Graphics.FromImage(image);
            g.Clear(Color.Black);
            m_imgLoaded = (Bitmap)image;
            picMainImg.Invalidate();
            if (m_bUseMiniMap && m_imgMiniMap != null)
            {
                image = new Bitmap(m_imgMiniMap.Width, m_imgMiniMap.Height);
                g = Graphics.FromImage(image);
                g.Clear(Color.Black);
                m_ucPicMiniMap.m_fnSetMiniMapImage(image);
            }

        }

        /// <summary>
        /// 기  능 : 경로 상의 이미지 오픈
        /// /반환값 : 
        /// /입  력 : string strImgPath
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary> 
        delegate void OpenImageCallback(string strImgPath, bool bFocus = false, bool bAlign = false);
        public void OpenImage(string strImgPath, bool bFocus = false, bool bAlign = false)
        {
            try
            {
                m_strImgPath = strImgPath;
                m_bFoucs = bFocus;

                if (this.InvokeRequired)
                {
                    OpenImageCallback delOpenImg = new OpenImageCallback(OpenImage);
                    this.Invoke(delOpenImg, strImgPath);
                }
                else
                {
                    if (!bAlign) setRotateAngle(0.0F);
                    LoadImage(strImgPath);

                }

            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine(ex.Message);
            }

        }

        public void Seq_Focus_thread()
        {

            double[] dFocusVal_Th = new double[5];

            int nRet = Consts.APP_OK;

            int nMeanStd = 0;

            if (m_bUseFocusValeView) nMeanStd = 1;
            else nMeanStd = 0;

            try
            {
                while(m_bUseFocusValeView)
                {
                    for (int i = 0; i < 5; i++)
                    {
                        CmdMsgParam Param = new CmdMsgParam();
                        Thread.Sleep(100);

                        Point ptimgSize = m_fnGetImgSize();

                        Bitmap bitTmp = m_fnGetDivBitmap(i);

                        Rectangle Test = new Rectangle(0, 0, 100, 100);

                        if(this.picMainImg.InvokeRequired)
                        {
                            Invoke(new MethodInvoker(delegate ()
                            {
                                bitTmp = bitTmp.Clone(Test, bitTmp.PixelFormat);
                            }));
                        }
                        else
                            bitTmp = bitTmp.Clone(Test, bitTmp.PixelFormat);


                        if (bitTmp == null) return;
                        Byte[] byteImg = Utils.ImageProc.loadBitmap(bitTmp, bitTmp.Width, bitTmp.Height, m_fnGetPixelForamt());
                        Param.SetInteger(bitTmp.Width);
                        Param.SetInteger(bitTmp.Height);
                        Param.SetInteger(nMeanStd);
                        Param.SetBytes(byteImg);

                        nRet = Systems.g_Ipc.SendCommand((ushort)((m_nVisionNo + 1) * 100 + IpcConst.ALGORITHM_TASK), IpcConst.ALGORITHM_FUNC, IpcConst.ALGORITHM_FOCUS_FIGURE,
                                                            IpcInterface.CMD_TYPE_RES, IpcInterface.CMD_TIMEOUT, Param.GetByteSize(), Param.GetParam());

                        if (nRet == Consts.APP_OK)
                        {
                            Param.SetOffset(0);
                            dFocusVal_Th[i] = Param.GetDbl();

                        }
                    }
                    m_fnSetFocusValue(dFocusVal_Th);
                }


            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine(ex.Message);
            }

        }

        /// <summary>
        /// 기  능 : 오픈한 이미지의 경로를 반환
        /// /반환값 : 
        /// /입  력 : Bitmap LoadImg
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary> 
        public string GetIOpenImagePath()
        {
            return m_strImgPath;
        }

        /// <summary>
        /// 기  능 : 오픈한 이미지의 경로를 변경
        /// /반환값 : 
        /// /입  력 : Bitmap LoadImg
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary> 
        public void SetIOpenImagePath(string strImgPath)
        {
            m_strImgPath = strImgPath;
        }

        /// <summary>
        /// 기  능 : 비트맵 원본 이미지를 카메라 테스크에 셋팅하고 이미지 로드 스레드를 호출 한다.
        /// /반환값 : 
        /// /입  력 : Bitmap LoadImg
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary> 
        private void LoadImage(string strImgPath)
        {

            SetLiveFlg(false);
            m_fnEndImageLoadThread();

            m_imgLoaded = null; //LoadImg;
            m_DivLoad.m_imgDivLoaded = new Bitmap[5];
            m_DivLoad.m_imgRect = new Rectangle[5];
            m_imgRect = new Rectangle(0, 0, 0, 0);

            CmdMsgParam Param = new CmdMsgParam();

            int nRet = Consts.APP_NG;

            Param.SetOffset(0);
            Param.SetChars(strImgPath.toUniCharAry(1000));
            Param.SetInteger(m_fnGetImageID());                  //int   : 이미지 버퍼 인덱스로 사용할 아이디
            Param.SetDbl(GetRotateAngle());
            nRet = Systems.g_Ipc.SendCommand((ushort)((m_fnGetVisionNo() + 1) * 100 + IpcConst.CAMERA_TASK + m_nCamIndex), IpcConst.CAMERA_FUNC, IpcConst.LOAD_IMAGE,
                                                IpcInterface.CMD_TYPE_RES, 10000/*IpcInterface.CMD_TIMEOUT*/, Param.GetByteSize(), Param.GetParam(), (int)Enums.CompressLevel.NONE);
            Param.SetOffset(0);

            if (nRet != Consts.APP_OK)
            {
                //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E092", Globals.nLanguageFlg), true);

                return;
            }

            int nChannelCnt = Param.GetInteger();
            
            m_fnSetPixelForamt(m_fnBandwidthToPixelformat(nChannelCnt));
            m_bIsGrayScale = (m_fnGetPixelForamt().ToString().ToUpper().Contains("RGB")) ? false : true;

            m_PixelSize.X = Param.GetInteger();
            m_PixelSize.Y = Param.GetInteger();

            m_imgLoaded = new Bitmap(m_PixelSize.X, m_PixelSize.Y);

            m_fnSetCameraFovRatio(m_fnGetCameraFovSize());

            m_imgRect.Width = m_PixelSize.X;
            m_imgRect.Height = m_PixelSize.Y;



            EndDspImageLoad();
            ClearAllArea();

            m_dRratio = 1.0;

            m_lbNowRatio.Text = string.Format("{0:00.00}", m_dRratio);

            m_imgPoint = new Point(picMainImg.Width / 2, picMainImg.Height / 2);

            m_pointClick = m_imgPoint;

            hScrollBar1.Minimum = 0;
            if (m_imgRect.Width - m_PixelSize.X > 0)
                hScrollBar1.Maximum = -m_PixelSize.X - m_imgRect.Width;
            else hScrollBar1.Maximum = 0;
            vScrollBar1.Minimum = 0;
            if (m_imgRect.Height - m_PixelSize.Y > 0)
                vScrollBar1.Maximum = -m_PixelSize.Y - m_imgRect.Height;
            else vScrollBar1.Maximum = 0;

            Thread thread = new Thread(new ThreadStart(delegate () { ThreadImagLoad(); }));
            thread.IsBackground = true;
            m_thread = thread;
            m_thread.Start();

            SetImageLoadThreadFlg(true);
            SetThreadWork(true);

            picMainImg.Invalidate();
        }

        private void DispminiMap()
        {
            if (this.InvokeRequired)
            {
                this.BeginInvoke(new Action(() =>
                {
                    if (this.m_bUseMiniMap)
                    {
                        this.m_imgMiniMap = Utils.ImageProc.Resize(this.m_imgLoaded,
                                    this.m_ucPicMiniMap.picMinImg.Width,
                                    this.m_ucPicMiniMap.picMinImg.Height, false);
                    }
                }));
            }

        }
        private void StartDspImageLoad()
        {
            pnlBackgrd.Visible = true;
            lbImgLoad.Visible = true;
            prbImgLoad.Value = 0;
            prbImgLoad.Visible = true;
        }
        private void EndDspImageLoad()
        {
            pnlBackgrd.Visible = false;
            prbImgLoad.Visible = false;
            lbImgLoad.Visible = false;
        }
        private void UpProgressValue()
        {
            prbImgLoad.Value++;
        }

        private void picMainImg_MouseUp(object sender, MouseEventArgs e)
        {
            //bLeftButtonDown = false;
            //this.Cursor = Cursors.Default;
            m_bCenter = false;
            m_bL_B = false;
            m_bL_T = false;
            m_bR_B = false;
            m_bR_T = false;

        }

        private void picMainImg_MouseDown(object sender, MouseEventArgs e)
        {

            if (e.Button == MouseButtons.Left)
            {
                m_pointClick = new Point(e.X, e.Y);
                //m_pointClick2 = new Point(e.X, e.Y);

                m_pointROIClick.X = m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, picMainImg.Width, m_dRratio);
                m_pointROIClick.Y = m_fnCalCoordinate(m_imgRect.Y, e.Y, m_imgRect.Width, picMainImg.Width, m_dRratio);

                if (m_bDivisionModeFlg)
                {
                    m_pntDivisionModeClick = new Point(e.X, e.Y);
                }
            }
            else if (e.Button == MouseButtons.Right)
            {
                m_pointROIRIGHTClick.X = m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, picMainImg.Width, m_dRratio);
                m_pointROIRIGHTClick.Y = m_fnCalCoordinate(m_imgRect.Y, e.Y, m_imgRect.Width, picMainImg.Width, m_dRratio);
                m_pointRightClick = new Point(e.X, e.Y);

                //Center
                if (m_pointROIRIGHTClick.Y > (m_GuideLineCenter.Y + m_GuideLineCenter.Height - 500) && m_pointROIRIGHTClick.X > m_GuideLineCenter.X - 600 && m_GuideLineCenter.X + 500 > m_pointROIRIGHTClick.X && (m_GuideLineCenter.X + m_GuideLineCenter.Width + 800) > m_pointROIRIGHTClick.X && m_GuideLineCenter.Y - 500 < m_pointROIRIGHTClick.Y && (m_GuideLineCenter.Y + m_GuideLineCenter.Height + 1000) > m_pointROIRIGHTClick.Y)
                {
                    m_bCenter = true;
                }
                //LEFT_TOP
                else if (m_GuideLineRect.Width / 2 > m_pointROIRIGHTClick.X - m_GuideLineRect.Y && (m_GuideLineRect.Height / 2) + m_GuideLineRect.Y > m_pointROIRIGHTClick.Y)
                {
                    m_bL_T = true;
                }
                //LEFT_BOTTOM
                else if (m_GuideLineRect.Width / 2 > m_pointROIRIGHTClick.X - m_GuideLineRect.Y && (m_GuideLineRect.Height / 2) + m_GuideLineRect.Y < m_pointROIRIGHTClick.Y)
                {
                    m_bL_B = true;
                }
                //RIGHT_TOP
                else if (m_GuideLineRect.Width / 2 < m_pointROIRIGHTClick.X && (m_GuideLineRect.Height / 2) + m_GuideLineRect.Y > m_pointROIRIGHTClick.Y)
                {
                    m_bR_T = true;
                }
                //RIGHT_BOTTOM
                else if (m_GuideLineRect.Width / 2 < m_pointROIRIGHTClick.X && (m_GuideLineRect.Height / 2) + m_GuideLineRect.Y < m_pointROIRIGHTClick.Y)
                {
                    m_bR_B = true;
                }
            }
            

        }

        private void picMainImg_MouseEnter(object sender, EventArgs e)
        {
            picMainImg.Focus();
            m_bImgFocus = true;
        }
        private void picMainImg_MouseLeave(object sender, EventArgs e)
        {
            m_bImgFocus = false;
        }
        private void picMainImg_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {

            if (e.KeyCode == Keys.Escape)
            {

                if (m_DicPnt[m_fnGetUsePntNum()].GetUsePointView())
                {
                    if (m_DicPnt[m_fnGetUsePntNum()].nViewKind == (int)Enums.E_PNT_VIEW_KIND.POLYGON)
                    {
                        SetPntCoord(m_fnGetUsePntNum(), new Point(0, 0));
                        SetEndPolygon(m_fnGetUsePntNum(), true);
                    }
                }

                if (m_bUseLine)
                {
                    if (m_Linerect.Width != 0 || m_Linerect.Height != 0)
                    {
                        m_bLineBtnDown = false;
                        Rectangle EmptRect = new Rectangle();
                        pntLineClick.X = 0;
                        pntLineClick.Y = 0;
                        m_Linerect = EmptRect;
                        this.Cursor = Cursors.Default;
                    }
                }

                if (m_bUseRect)
                {
                    if (m_RectCom.Width != 0 || m_RectCom.Height != 0)
                    {
                        m_bRectBtnDown = false;
                        Rectangle EmptRect = new Rectangle();
                        m_pntRectClick.X = 0;
                        m_pntRectClick.Y = 0;
                        m_RectCom = EmptRect;
                        this.Cursor = Cursors.Default;
                    }
                }
            }

            if (Control.ModifierKeys == Keys.Control && e.KeyCode == Keys.Z)
            {
                if (m_DicPnt[m_fnGetUsePntNum()].GetUsePointView())
                {
                    if (m_DicPnt[m_fnGetUsePntNum()].nViewKind == (int)Enums.E_PNT_VIEW_KIND.POLYGON)
                    {
                        Dictionary<int, List<Point>> dicPolygon = m_DicPnt[m_fnGetUsePntNum()].GetPolygonListVal();

                        List<Point> lstPolygon;
                        int nKey = m_DicPnt[m_fnGetUsePntNum()].nCurPolygonKey;
                        if (dicPolygon.ContainsKey(nKey))
                        {
                            if (dicPolygon.TryGetValue(nKey, out lstPolygon))
                            {
                                if (lstPolygon.Count() > 0)
                                {
                                    if (lstPolygon.Count() <= 2) lstPolygon.Clear();
                                    else
                                        lstPolygon.RemoveAt(lstPolygon.Count() - 1);
                                    SetPolygonListVal(m_fnGetUsePntNum(), dicPolygon, nKey);
                                    m_fnSetPointCoord(-1, -1);

                                    // 호출 클래스의 datatable row 삭제
                                    m_strImageSate.Text = string.Format("{0},{1},DELETE", nKey, lstPolygon.Count());
                                }
                            }
                        }
                    }
                }
            }
            picMainImg.Invalidate();
        }
        private void picMainImg_MouseMove(object sender, MouseEventArgs e)
        {
            // Thread.Sleep(10);

            //////////////////////////////////////
            //시작점에서의 거리 재기
            m_LastPoint = e.Location;
         
            // 라이브 영상에서의 좌표 이동
            if (m_bDivisionModeFlg && e.Button == MouseButtons.Left && m_bUseDragMove)
            {
                DivisionCoordMouseMove(e.X, e.Y);

                m_pointClick.X = e.X;
                m_pointClick.Y = e.Y;
     
                return;
            }
            else if (e.Button == MouseButtons.Right && m_bUseDragMove && m_bUseFocusLineView)
            {

                m_pointROIRIGHT_MOVE_Click.X = m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, picMainImg.Width, m_dRratio);
                m_pointROIRIGHT_MOVE_Click.Y = m_fnCalCoordinate(m_imgRect.Y, e.Y, m_imgRect.Width, picMainImg.Width, m_dRratio);

//                 DivisionCoord_Focus_MouseMove(e.X, e.Y);
// 
//                 m_pointClick2.X = e.X;
//                 m_pointClick2.Y = e.Y;

                SetRectROI(m_GuideLineRect2, m_pointROIRIGHT_MOVE_Click.X, m_pointROIRIGHT_MOVE_Click.Y, m_bLiveFlg, m_dRratio);
            }
       

            // 라이브 영상에서의 좌표 이동
            if (!m_bDivisionModeFlg && e.Button == MouseButtons.Left && m_bUseDragMove)
            {
                if (m_imgRect.Width <= m_PixelSize.X || m_imgRect.Height <= m_PixelSize.Y && (m_pointClick.X >= 0 && m_pointClick.Y >= 0))
                {
                    Rectangle rectMouseMove = new Rectangle();

                    rectMouseMove.X = (int)Math.Round((Convert.ToDouble(m_imgRect.Width * (m_pointClick.X - Math.Abs(e.X))) / picMainImg.Width));
                    m_imgRect.X = m_imgRect.X + rectMouseMove.X;

                    rectMouseMove.Y = (int)Math.Round((Convert.ToDouble(m_imgRect.Height * (m_pointClick.Y - Math.Abs(e.Y))) / picMainImg.Height));
                    m_imgRect.Y = m_imgRect.Y + rectMouseMove.Y;

                    if (m_imgRect.X < 0) m_imgRect.X = 0;
                    if (m_imgRect.X > m_PixelSize.X - m_imgRect.Width) m_imgRect.X = m_PixelSize.X - m_imgRect.Width;

                    if (m_imgRect.Y < 0) m_imgRect.Y = 0;
                    if (m_imgRect.Y > m_PixelSize.Y - m_imgRect.Height) m_imgRect.Y = m_PixelSize.Y - m_imgRect.Height;
                    if (hScrollBar1.Maximum >= Math.Abs(m_imgRect.X)) hScrollBar1.Value = Math.Abs(m_imgRect.X);

                    if (vScrollBar1.Maximum >= Math.Abs(m_imgRect.Y)) vScrollBar1.Value = Math.Abs(m_imgRect.Y);

                    m_pointClick.X = e.X;
                    m_pointClick.Y = e.Y;

                    return;
                }
                else
                {
                    return;
                }
            }

            // 라이브 영상에서의 라인 프로파일 좌표 이동
            if (m_bLineBtnDown)
            {
                if (!m_bDivisionModeFlg)
                {
                    if (m_imgRect.Width <= m_PixelSize.X || m_imgRect.Height <= m_PixelSize.Y && (pntLineClick.X >= 0 && pntLineClick.Y >= 0))
                    {

                        if (m_bUseLineProfile)
                        {
                            if (Math.Abs(m_pntLineRightClick.X - e.X) > Math.Abs(m_pntLineRightClick.Y - e.Y))
                            {
                                m_Linerect.Width = m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, picMainImg.Width, m_dRratio);
                                m_Linerect.Height = pntLineClick.Y;
                            }
                            else
                            {
                                m_Linerect.Width = pntLineClick.X;
                                m_Linerect.Height = m_fnCalCoordinate((m_imgRect.Y), e.Y, m_imgRect.Height, picMainImg.Height, m_dRratio);
                            }
                            m_stLineProfileCoord.X2.Text = m_Linerect.Width.ToString();
                            m_stLineProfileCoord.Y2.Text = m_Linerect.Height.ToString();
                        }
                        else
                        {
                            if ((Control.ModifierKeys & Keys.Shift) != Keys.None)
                            {
                                if (Math.Abs(m_pntLineRightClick.X - e.X) > Math.Abs(m_pntLineRightClick.Y - e.Y))
                                {
                                    m_Linerect.Width = m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, picMainImg.Width, m_dRratio);
                                    m_Linerect.Height = pntLineClick.Y;
                                }
                                else
                                {
                                    m_Linerect.Width = pntLineClick.X;
                                    m_Linerect.Height = m_fnCalCoordinate((m_imgRect.Y), e.Y, m_imgRect.Height, picMainImg.Height, m_dRratio);
                                }
                            }

                            else
                            {
                                //좌표계산 
                                m_Linerect.Width = m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, picMainImg.Width, m_dRratio);
                                m_Linerect.Height = m_fnCalCoordinate((m_imgRect.Y), e.Y, m_imgRect.Height, picMainImg.Height, m_dRratio);
                            }
                        }

                    }
                    else return;
                }
                else
                {
                    Rectangle tempRect = GetRectCurDiv(m_pntLineRightClick.X, m_pntLineRightClick.Y);

                    if (tempRect.Width <= m_PixelSize.X || tempRect.Height <= m_PixelSize.Y && (pntLineClick.X >= 0 && pntLineClick.Y >= 0))
                    {
                        //좌표계산 
                        if (Math.Abs(m_pntLineRightClick.X - e.X) > Math.Abs(m_pntLineRightClick.Y - e.Y))
                        {
                            m_Linerect.Width = m_fnCalCoordinate(tempRect.X, e.X, m_nDivEndX, picMainImg.Width / 2, m_dRratio);
                            m_Linerect.Height = pntLineClick.Y;
                            if ((picMainImg.Width / 3 - 1 < m_pntLineRightClick.X
                                && picMainImg.Height / 3 - 1 < m_pntLineRightClick.Y)
                                && (picMainImg.Width / 3 * 2 - 1 >= m_pntLineRightClick.X
                                && picMainImg.Height / 3 * 2 - 1 >= m_pntLineRightClick.Y))
                            {
                                m_Linerect.Width = (int)Math.Round(tempRect.X + ((e.X - picMainImg.Width / 3) / m_dRratio));
                            }
                        }
                        else
                        {
                            m_Linerect.Width = pntLineClick.X;
                            m_Linerect.Height = m_fnCalCoordinate((tempRect.Y), e.Y, m_nDivEndY, picMainImg.Height / 2, m_dRratio);
                            if ((picMainImg.Width / 3 - 1 < m_pntLineRightClick.X
                               && picMainImg.Height / 3 - 1 < m_pntLineRightClick.Y)
                               && (picMainImg.Width / 3 * 2 - 1 >= m_pntLineRightClick.X
                               && picMainImg.Height / 3 * 2 - 1 >= m_pntLineRightClick.Y))
                            {
                                m_Linerect.Height = (int)Math.Round(tempRect.Y + ((e.Y - picMainImg.Height / 3) / m_dRratio));
                            }
                        }
                        m_stLineProfileCoord.X2.Text = m_Linerect.Width.ToString();
                        m_stLineProfileCoord.Y2.Text = m_Linerect.Height.ToString();
                    }
                    else return;

                }

            }

            // 라이브 영상에서의 콘트라스트 취득 좌표 이동
            if (m_bRectBtnDown)
            {

                // Rectangle tempRec = new Rectangle();
                if (!m_bDivisionModeFlg)
                {
                    if (m_imgRect.Width <= m_PixelSize.X || m_imgRect.Height <= m_PixelSize.Y && (m_pntRectClick.X >= 0 && m_pntRectClick.Y >= 0))
                    {
                        //좌표계산 
                        m_RectCom.X = m_pntRectClick.X;
                        m_RectCom.Y = m_pntRectClick.Y;
                        m_RectCom.Width = m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, picMainImg.Width, m_dRratio);
                        m_RectCom.Height = m_fnCalCoordinate((m_imgRect.Y), e.Y, m_imgRect.Height, picMainImg.Height, m_dRratio);

                        if (m_RectCom.Width < m_pntRectClick.X)
                        {

                            m_RectCom.X = Math.Abs(m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, picMainImg.Width, m_dRratio));

                            m_RectCom.Width = Math.Abs(m_RectCom.X - m_pntRectClick.X);

                        }
                        else m_RectCom.Width -= m_pntRectClick.X;

                        //Y좌표계산
                        if (m_RectCom.Height < m_pntRectClick.Y)
                        {
                            m_RectCom.Y = Math.Abs(m_fnCalCoordinate((m_imgRect.Y), e.Y, m_imgRect.Height, picMainImg.Height, m_dRratio));
                            m_RectCom.Height = Math.Abs(m_RectCom.Y - m_pntRectClick.Y);
                        }
                        else m_RectCom.Height -= m_pntRectClick.Y;
                    }
                    else return;
                }
                else
                {
                    Rectangle tempRect = GetRectCurDiv(m_pntRectRightClick.X, m_pntRectRightClick.Y);

                    if (tempRect.Width <= m_PixelSize.X || tempRect.Height <= m_PixelSize.Y && (pntLineClick.X >= 0 && m_pntRectClick.Y >= 0))
                    {
                        //좌표계산 
                        m_RectCom.X = m_pntRectClick.X;
                        m_RectCom.Y = m_pntRectClick.Y;
                        m_RectCom.Width = m_fnCalCoordinate(tempRect.X, e.X, m_nDivEndX, picMainImg.Width / 2, m_dRratio);
                        m_RectCom.Height = m_fnCalCoordinate((tempRect.Y), e.Y, m_nDivEndY, picMainImg.Height / 2, m_dRratio);

                        if ((picMainImg.Width / 3 - 1 < m_pntRectRightClick.X
                            && picMainImg.Height / 3 - 1 < m_pntRectRightClick.Y)
                            && (picMainImg.Width / 3 * 2 - 1 >= m_pntRectRightClick.X
                            && picMainImg.Height / 3 * 2 - 1 >= m_pntRectRightClick.Y))
                        {
                            m_RectCom.Width = (int)Math.Round(tempRect.X + ((e.X - picMainImg.Width / 3) / m_dRratio));
                            m_RectCom.Height = (int)Math.Round(tempRect.Y + ((e.Y - picMainImg.Height / 3) / m_dRratio));
                        }

                        m_RectCom.Width -= m_pntRectClick.X;
                        m_RectCom.Height -= m_pntRectClick.Y;

                    }
                    else return;
                }
                if (m_DicRect[m_fnGetUseRctNum()].GetUseRectView())
                {
                 
                    m_fnSetRectCoord(m_fnGetUseRctNum(), Consts.END);
                }
            }

            if (m_bUsePoint)
            {
                if (m_DicPnt[m_fnGetUsePntNum()].GetUsePointView())
                {
                    if (m_DicPnt[m_fnGetUsePntNum()].GetViewKind() == (int)Enums.E_PNT_VIEW_KIND.POLYGON)
                    {
                        if ((Control.ModifierKeys & Keys.Shift) != Keys.None)
                        {
                            List<Point> lp;
                            m_DicPnt[m_fnGetUsePntNum()].GetPolygonListVal().TryGetValue(m_DicPnt[m_fnGetUsePntNum()].nCurPolygonKey, out lp);

                            if (lp != null && lp.Count() > 0)
                            {
                                int nX = CalcRectCoord(lp[lp.Count() - 1].X, m_imgRect.X, m_PixelSize.X, picMainImg.Width);
                                int nY = CalcRectCoord(lp[lp.Count() - 1].Y, m_imgRect.Y, m_PixelSize.Y, picMainImg.Height);


                                if (Math.Abs(nX - e.X) > Math.Abs(nY - e.Y))
                                {
                                    Point pnt = new Point();
                                    pnt.X = m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, picMainImg.Width, m_dRratio);
                                    pnt.Y = lp[lp.Count() - 1].Y;
                                    m_stCurCoord.X1.Text = pnt.X.ToString();
                                    m_stCurCoord.Y1.Text = pnt.Y.ToString();
                                }
                                else
                                {
                                    Point pnt = new Point();
                                    pnt.X = lp[lp.Count() - 1].X;
                                    pnt.Y = m_fnCalCoordinate(m_imgRect.Y, e.Y, m_imgRect.Height, picMainImg.Height, m_dRratio);
                                    m_stCurCoord.X1.Text = pnt.X.ToString();
                                    m_stCurCoord.Y1.Text = pnt.Y.ToString();
                                }

                                m_fnSetPointCoord(m_fnGetUsePntNum()); return;
                            }
                        }
                        else
                            m_fnSetPointCoord(m_fnGetUsePntNum());
                    }

                    else
                        m_fnSetPointCoord(m_fnGetUsePntNum());
                }
            }
            if (m_imgLoaded != null)
            {
                if (m_bDivisionModeFlg)
                {
                    Rectangle tempRect = GetRectCurDiv(e.X, e.Y);

                    m_stCurCoord.X1.Text = m_fnCalCoordinate(tempRect.X, e.X, m_nDivEndX, picMainImg.Width / 2, m_dRratio).ToString();
                    m_stCurCoord.Y1.Text = m_fnCalCoordinate(tempRect.Y, e.Y, m_nDivEndY, picMainImg.Height / 2, m_dRratio).ToString();

                    if ((picMainImg.Width / 3 - 1 < m_pntLineRightClick.X
                       && picMainImg.Height / 3 - 1 < m_pntLineRightClick.Y)
                       && (picMainImg.Width / 3 * 2 - 1 >= m_pntLineRightClick.X
                       && picMainImg.Height / 3 * 2 - 1 >= m_pntLineRightClick.Y))
                    {

                        m_stCurCoord.X1.Text = (tempRect.X + ((e.X - picMainImg.Width / 3) / m_dRratio)).ToString();
                        m_stCurCoord.Y1.Text = (tempRect.Y + ((e.Y - picMainImg.Height / 3) / m_dRratio)).ToString();

                    }
                }
                else
                {
                    m_stCurCoord.X1.Text = m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, picMainImg.Width, m_dRratio).ToString();
                    m_stCurCoord.Y1.Text = m_fnCalCoordinate(m_imgRect.Y, e.Y, m_imgRect.Height, picMainImg.Height, m_dRratio).ToString();
                }
                //if(!m_bUseFocusLineView)
                m_fnSetCurRGBVal(e.X, e.Y);

                m_pointROIRIGHT_MOVE_Click2.X = m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, picMainImg.Width, m_dRratio);
                m_pointROIRIGHT_MOVE_Click2.Y = m_fnCalCoordinate(m_imgRect.Y, e.Y, m_imgRect.Width, picMainImg.Width, m_dRratio);

               // m_fnSetCurFocusVal(m_pointROIRIGHT_MOVE_Click2.X, m_pointROIRIGHT_MOVE_Click2.Y);
            }


            picMainImg.Invalidate();
          
        }

        private void DivisionCoordMouseMove(int nCoordX, int nCoordY)
        {
            // Rectangle rect = checkDivisonArea(m_pointClick.X, m_pointClick.Y, nCoordX, nCoordY);
            Rectangle rectMouseMove = GetRectCurDiv(m_pntDivisionModeClick.X, m_pntDivisionModeClick.Y);
            Rectangle tmpRectMouseMove = new Rectangle();

            tmpRectMouseMove.X = (int)Math.Round((Convert.ToDouble(m_nDivEndX * (m_pointClick.X - Math.Abs(nCoordX))) / (picMainImg.Width / 2 / 4 * 4)));
            rectMouseMove.X = rectMouseMove.X + tmpRectMouseMove.X;
            if (rectMouseMove.X < 0) rectMouseMove.X = 0;
            if (rectMouseMove.X > m_PixelSize.X - m_nDivEndX) rectMouseMove.X = m_PixelSize.X - m_nDivEndX;


            tmpRectMouseMove.Y = (int)Math.Round((Convert.ToDouble(m_nDivEndY * (m_pointClick.Y - Math.Abs(nCoordY))) / (picMainImg.Height / 2 / 4 * 4)));
            rectMouseMove.Y = rectMouseMove.Y + tmpRectMouseMove.Y;
            if (rectMouseMove.Y < 0) rectMouseMove.Y = 0;
            if (rectMouseMove.Y > m_PixelSize.Y - m_nDivEndY) rectMouseMove.Y = m_PixelSize.Y - m_nDivEndY;
            //             if (hScrollBar1.Maximum >= Math.Abs(rectMouseMove.X)) hScrollBar1.Value = Math.Abs(rectMouseMove.X);
            //             if (vScrollBar1.Maximum >= Math.Abs(rectMouseMove.Y)) vScrollBar1.Value = Math.Abs(rectMouseMove.Y);



            SetRectCurDiv(rectMouseMove, m_pntDivisionModeClick.X, m_pntDivisionModeClick.Y);
          //  SetRectFocusDiv(rectMouseMove, m_pntDivisionModeClick.X, m_pntDivisionModeClick.Y, m_pointClick, m_dRratio);
        }

        private void DivisionCoord_Focus_MouseMove(int nCoordX, int nCoordY)
        {
            // Rectangle rect = checkDivisonArea(m_pointClick.X, m_pointClick.Y, nCoordX, nCoordY);
            Rectangle rectMouseMove = GetRectCurDiv(m_pntDivisionModeClick.X, m_pntDivisionModeClick.Y);
            Rectangle tmpRectMouseMove = new Rectangle();

            tmpRectMouseMove.X = (int)Math.Round((Convert.ToDouble(m_nDivEndX * (m_pointClick.X - Math.Abs(nCoordX))) / (picMainImg.Width / 2 / 4 * 4)));
            rectMouseMove.X = rectMouseMove.X + tmpRectMouseMove.X;
            if (rectMouseMove.X < 0) rectMouseMove.X = 0;
            if (rectMouseMove.X > m_PixelSize.X - m_nDivEndX) rectMouseMove.X = m_PixelSize.X - m_nDivEndX;


            tmpRectMouseMove.Y = (int)Math.Round((Convert.ToDouble(m_nDivEndY * (m_pointClick.Y - Math.Abs(nCoordY))) / (picMainImg.Height / 2 / 4 * 4)));
            rectMouseMove.Y = rectMouseMove.Y + tmpRectMouseMove.Y;
            if (rectMouseMove.Y < 0) rectMouseMove.Y = 0;
            if (rectMouseMove.Y > m_PixelSize.Y - m_nDivEndY) rectMouseMove.Y = m_PixelSize.Y - m_nDivEndY;
            //             if (hScrollBar1.Maximum >= Math.Abs(rectMouseMove.X)) hScrollBar1.Value = Math.Abs(rectMouseMove.X);
            //             if (vScrollBar1.Maximum >= Math.Abs(rectMouseMove.Y)) vScrollBar1.Value = Math.Abs(rectMouseMove.Y);

            SetRectROI(m_GuideLineRect2, m_pntDivisionModeClick.X, m_pntDivisionModeClick.Y, m_bLiveFlg, m_dRratio);
            // SetRectFocusDiv(rectMouseMove, m_pntDivisionModeClick.X, m_pntDivisionModeClick.Y, m_pointClick, m_dRratio);
        }

        private void picMainImg_MouseClick(object sender, MouseEventArgs e)
        {

            if (e.Button == MouseButtons.Right)
            {

                m_bFocusClick = false;

            
                if (m_bUsePoint)
                {
                    SetEndPolygon(m_fnGetUsePntNum(), false);
                    m_fnSetPointCoord();
                    this.Cursor = Cursors.Default;
                }
                if (m_bUseLine)
                {
                    if (!m_bLineBtnDown /*&& !m_bUseLineProfile*/)
                    {
                        m_pntLineRightClick = new Point(e.X, e.Y);

                        if (m_bDivisionModeFlg)
                        {
                            Rectangle tempRect = GetRectCurDiv(e.X, e.Y);
                            pntLineClick.X = m_fnCalCoordinate(tempRect.X, e.X, tempRect.Width, picMainImg.Width / 2, m_dRratio);
                            pntLineClick.Y = m_fnCalCoordinate(tempRect.Y, e.Y, tempRect.Height, picMainImg.Height / 2, m_dRratio);

                            if ((picMainImg.Width / 3 - 1 < m_pntLineRightClick.X
                                  && picMainImg.Height / 3 - 1 < m_pntLineRightClick.Y)
                                  && (picMainImg.Width / 3 * 2 - 1 >= m_pntLineRightClick.X
                                  && picMainImg.Height / 3 * 2 - 1 >= m_pntLineRightClick.Y))
                            {
                                pntLineClick.X = (int)Math.Round(tempRect.X + ((e.X - picMainImg.Width / 3) / m_dRratio));
                                pntLineClick.Y = (int)Math.Round(tempRect.Y + ((e.Y - picMainImg.Height / 3) / m_dRratio));
                            }

                        }
                        else
                        {
                            pntLineClick.X = m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, picMainImg.Width, m_dRratio);
                            pntLineClick.Y = m_fnCalCoordinate(m_imgRect.Y, e.Y, m_imgRect.Height, picMainImg.Height, m_dRratio);

                        }
                        if (m_bUseLineProfile)
                        {
                            m_stLineProfileCoord.X1.Text = pntLineClick.X.ToString();
                            m_stLineProfileCoord.Y1.Text = pntLineClick.Y.ToString();

                        }
                        else
                        {
                            m_fnSetDrawLineCoord(Consts.START);
                        }
                        m_bLineBtnDown = true;
                        this.Cursor = Cursors.Cross;
                    }
                    else
                    {
                        m_bLineBtnDown = false;
                        if (!m_bUseLineProfile)
                        {
                            m_fnSetDrawLineCoord(Consts.END);
                        }
                        this.Cursor = Cursors.Default;

                    }
                }

                if (m_bUseRect)
                {
                    if (!m_bRectBtnDown)
                    {
                        m_pntRectRightClick = new Point(e.X, e.Y);

                        if (m_bDivisionModeFlg)
                        {
                            Rectangle tempRect = GetRectCurDiv(e.X, e.Y);
                            m_pntRectClick.X = m_fnCalCoordinate(tempRect.X, e.X, tempRect.Width, picMainImg.Width / 2, m_dRratio);
                            m_pntRectClick.Y = m_fnCalCoordinate(tempRect.Y, e.Y, tempRect.Height, picMainImg.Height / 2, m_dRratio);

                            if ((picMainImg.Width / 3 - 1 < m_pntRectRightClick.X
                                  && picMainImg.Height / 3 - 1 < m_pntRectRightClick.Y)
                                  && (picMainImg.Width / 3 * 2 - 1 >= m_pntRectRightClick.X
                                  && picMainImg.Height / 3 * 2 - 1 >= m_pntRectRightClick.Y))
                            {
                                m_pntRectClick.X = (int)Math.Round(tempRect.X + ((e.X - picMainImg.Width / 3) / m_dRratio));
                                m_pntRectClick.Y = (int)Math.Round(tempRect.Y + ((e.Y - picMainImg.Height / 3) / m_dRratio));
                            }

                        }
                        else
                        {
                            m_pntRectClick.X = m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, picMainImg.Width, m_dRratio);
                            m_pntRectClick.Y = m_fnCalCoordinate(m_imgRect.Y, e.Y, m_imgRect.Height, picMainImg.Height, m_dRratio);

                        }

                        ///////////////////////////////////////////////////////////////////////////////////////////////
                        // 기능 별 좌표 취득
                        // if (m_bUseContrast) 
                        if (m_DicRect[m_fnGetUseRctNum()].GetUseRectView())
                            m_fnSetRectCoord(m_fnGetUseRctNum(), Consts.START);
                        m_bRectBtnDown = true;
                        this.Cursor = Cursors.Cross;

                    }
                    else
                    {
                        ///////////////////////////////////////////////////////////////////////////////////////////////
                        // 기능 별 좌표 취득
                        if (m_bUseContrast)
                            m_fnCalcContrast();
                        if (m_DicRect[m_fnGetUseRctNum()].GetUseRectView())
                            m_fnSetRectCoord(m_fnGetUseRctNum(), Consts.END);

                        m_bRectBtnDown = false;
                        this.Cursor = Cursors.Default;
                        ///////////////////////////////////////////////////////////////////////////////////////////////

                    }
                }








            }
            if (e.Button == MouseButtons.Left)
            {

            }
        }

        private void picMainImg_MouseWheel(object sender, MouseEventArgs e)
        {
            try
            {
                //Thread.Sleep(10);
                if (!m_bImgFocus || !m_bUseZoomInOut)
                {
                    picMainImg.Invalidate();
                    return;
                }
                int lines = e.Delta * SystemInformation.MouseWheelScrollLines/* / 120*/;
                m_fnSetZoomInOut(lines, e.X, e.Y);

            }
            catch (Exception)
            {
                //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E093", Globals.nLanguageFlg), true);
            }
        }


        public void m_fnSetZoomInOut(int lines, int nX, int nY)
        {
            try
            {
                PictureBox pb = picMainImg;

                m_dRecRatio = 1;

                if (lines > 0)
                {
                    if (m_dRratio >= 100.0F)
                        m_dRratio += 10.0;
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
                        m_dRratio += 0.25F;// 1.0F;
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
                        m_dRratio -= 0.25F;//1.0F;
                    m_dRratio = (float)Math.Round(m_dRratio, 5);
                    if (m_dRratio <= 1.0F)
                        m_dRratio = 1.0F;
                }

                Rectangle adjustedArea = new Rectangle();

                Point pntOldCoord = new Point();
                pntOldCoord.X = m_imgRect.Width;
                pntOldCoord.Y = m_imgRect.Height;


                adjustedArea.Width = (int)Math.Round(m_PixelSize.X / (m_dRratio));
                adjustedArea.Height = (int)Math.Round(m_PixelSize.Y / (m_dRratio));






                // 마우스 포인트 중심 좌표 확대 - 축소
                //int nMouseCurX = m_stCurCoord.X1.Text.toInt();
                //int nMouseCurY = m_stCurCoord.Y1.Text.toInt();
                //adjustedArea.X = nMouseCurX - (int)Math.Round(nMouseCurX / m_dRratio);
                //adjustedArea.Y = nMouseCurY - (int)Math.Round(nMouseCurY / m_dRratio);
                //int nMousseX = CalcRectCoord(nMouseCurX, m_imgRect.X, m_PixelSize.X, picMainImg.Width);
                //int nMousseY = CalcRectCoord(nMouseCurY, m_imgRect.Y, m_PixelSize.Y, picMainImg.Height);
                //m_stCurCoord.X1.Text = m_fnCalCoordinate(m_imgRect.X, nX, m_imgRect.Width, picMainImg.Width, m_dRratio).ToString();
                //m_stCurCoord.Y1.Text = m_fnCalCoordinate(m_imgRect.Y, nY, m_imgRect.Height, picMainImg.Height, m_dRratio).ToString();






                adjustedArea.X = (int)m_imgRect.X + ((pntOldCoord.X - adjustedArea.Width) / 2);
                adjustedArea.Y = (int)m_imgRect.Y + ((pntOldCoord.Y - adjustedArea.Height) / 2); //m_imgRect.Y + (m_imgRect.Height / 2);

                if (adjustedArea.X < 0) adjustedArea.X = 0;
                if (adjustedArea.X > m_PixelSize.X) adjustedArea.X = m_PixelSize.X - adjustedArea.Width;
                if (adjustedArea.Y < 0) adjustedArea.Y = 0;
                if (adjustedArea.Y > m_PixelSize.Y) adjustedArea.Y = m_PixelSize.Y - adjustedArea.Height;

                if (m_PixelSize.X <= adjustedArea.Width) adjustedArea.X = 0;
                if (m_PixelSize.Y <= adjustedArea.Height) adjustedArea.Y = 0;

                if (adjustedArea.Width + adjustedArea.X > m_PixelSize.X) adjustedArea.X = m_PixelSize.X - adjustedArea.Width;
                if (adjustedArea.Y + adjustedArea.Height > m_PixelSize.Y) adjustedArea.Y = m_PixelSize.Y - adjustedArea.Height;
                if (m_PixelSize.Y <= adjustedArea.Height) adjustedArea.Y = 0;
                
                m_imgRect = adjustedArea;


                m_lbNowRatio.Text = string.Format("{0:00.00}", m_dRratio);


                // m_fnSetImgPosition();

                //m_lbNowRatio.Text = string.Format("{0:00.00}", m_dRratio);

                //                 hScrollBar1.Minimum = 0;
                //                 if (m_LivePixelSize.X - m_recLive.Width > 0)
                //                     hScrollBar1.Maximum = m_LivePixelSize.X - m_recLive.Width;
                //                 else hScrollBar1.Maximum = 0;
                //                 vScrollBar1.Minimum = 0;
                //                 if (m_LivePixelSize.Y - m_recLive.Height > 0)
                //                     vScrollBar1.Maximum = m_LivePixelSize.Y - m_recLive.Height;
                //                 else vScrollBar1.Maximum = 0;

            }
            catch (Exception ex)
            {
                throw ex;
                //Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
            }

        }

        private void vScrollBar1_Scroll(object sender, ScrollEventArgs e)
        {
            m_imgRect.Y = vScrollBar1.Value * (-1);
            m_imgPoint.Y = m_imgRect.Y * (-1);
            picMainImg.Invalidate();
        }

        private void hScrollBar1_Scroll(object sender, ScrollEventArgs e)
        {
            m_imgRect.X = hScrollBar1.Value * (-1);
            m_imgPoint.X = m_imgRect.X * (-1);
            picMainImg.Invalidate();
        }

        private void CalcZoomInOutCoord(ref Rectangle Rect, double dRecRatio)
        {
            Point mouseLoc = picMainImg.PointToClient(Cursor.Position);
            if (m_dRecRatio > 1)
            {
                Rect.X = (int)Math.Round((Rect.X * dRecRatio) - (picMainImg.Width / 2));
                Rect.Y = (int)Math.Round((Rect.Y * dRecRatio) - (picMainImg.Height / 2));

            }
            else if (m_dRecRatio < 1)
            {
                Rect.X = (int)Math.Round((Rect.X + (picMainImg.Width / 2)) * dRecRatio);
                Rect.Y = (int)Math.Round((Rect.Y + (picMainImg.Height / 2)) * dRecRatio);
            }

            if (Rect.X > 0) Rect.X = 0;
            if (Rect.Y > 0) Rect.Y = 0;

            return;
        }

        public void m_fnLinkPointCoord(ref Label reflbCoordX, ref Label reflbCoordY)
        {
            m_stPointCoord.X1 = reflbCoordX;
            m_stPointCoord.Y1 = reflbCoordY;
        }
        public void m_fnLinkGdOriginCoord(ref Label reflbCoordX, ref Label reflbCoordY)
        {
            m_stGdOrgCoord.X1 = reflbCoordX;
            m_stGdOrgCoord.Y1 = reflbCoordY;
        }
        private void m_fnSetPointCoord()
        {
            m_stPointCoord.X1.Text = m_stCurCoord.X1.Text;
            m_stPointCoord.Y1.Text = m_stCurCoord.Y1.Text;
        }
        private void m_fnSetPointCoord(int nX, int nY)
        {
            m_stPointCoord.X1.Text = nX.ToString();
            m_stPointCoord.Y1.Text = nY.ToString();
        }
        private void m_fnSetGdOriginCoord()
        {
            m_stGdOrgCoord.X1.Text = m_stCurCoord.X1.Text;
            m_stGdOrgCoord.Y1.Text = m_stCurCoord.Y1.Text;
        }

        public Rectangle m_fnGetRoiCoord(int nKey)
        {
            return m_DicRect[nKey].GetRectCoord();
        }

        public void m_fnLinkLineProfileCoord(ref Label reflbCoordX1, ref Label reflbCoordY1, ref Label reflbCoordX2, ref Label reflbCoordY2)
        {
            m_stLineProfileCoord.X1 = reflbCoordX1;
            m_stLineProfileCoord.Y1 = reflbCoordY1;
            m_stLineProfileCoord.X2 = reflbCoordX2;
            m_stLineProfileCoord.Y2 = reflbCoordY2;
        }
        public void m_fnLinkDrawLineCoord(ref Label reflbCoordX1, ref Label reflbCoordY1, ref Label reflbCoordX2, ref Label reflbCoordY2)
        {
            m_stDrawLineCoord.X1 = reflbCoordX1;
            m_stDrawLineCoord.Y1 = reflbCoordY1;
            m_stDrawLineCoord.X2 = reflbCoordX2;
            m_stDrawLineCoord.Y2 = reflbCoordY2;
        }

        public void m_fnLinkUniformityValue(ref Label reflbContVal)
        {
            m_lbContrastCoord = reflbContVal;
        }

        private void m_fnSetRectCoord(int nKey, bool bStartEnd)
        {
            Rectangle rct = m_DicRect[nKey].GetRectCoord();

            if (bStartEnd)
            {
                if (m_stCurCoord.X1 != null) rct.X = m_stCurCoord.X1.Text.toInt();
                if (m_stCurCoord.Y1 != null) rct.Y = m_stCurCoord.Y1.Text.toInt();
            }
            else
            {
                if (m_stCurCoord.X1 != null) rct.Width = m_stCurCoord.X1.Text.toInt();
                if (m_stCurCoord.Y1 != null) rct.Height = m_stCurCoord.Y1.Text.toInt();
            }

            SetRectCoord(nKey, rct);
        }
        //KYH 2021.11.01 
        private void m_fnSetFocusRectCoord(int nKey, bool bStartEnd)
        {
            Rectangle rct = m_DicFocusROI[nKey].GetRectCoord();

            if (bStartEnd)
            {
                if (m_pointROIClick.X != 0) rct.X = m_pointROIClick.X;
                if (m_pointROIClick.Y != 0) rct.Y = m_pointROIClick.Y;
            }
            else
            {
                if (m_pointROIClick.X != 0) rct.Width = m_pointROIClick.X;
                if (m_pointROIClick.Y != 0) rct.Height = m_pointROIClick.Y;
            }

            SetFocusRectCoord(nKey, rct);
        }

        private void m_fnSetPointCoord(int nKey)
        {
            Point pnt = m_DicPnt[nKey].GetPointCoord();

            if (m_stCurCoord.X1 != null) pnt.X = m_stCurCoord.X1.Text.toInt();
            if (m_stCurCoord.Y1 != null) pnt.Y = m_stCurCoord.Y1.Text.toInt();
            SetPntCoord(nKey, pnt);
        }

        private void m_fnSetDrawLineCoord(bool bStartEnd)
        {
            if (bStartEnd)
            {
                m_stDrawLineCoord.X1.Text = m_stCurCoord.X1.Text;
                m_stDrawLineCoord.Y1.Text = m_stCurCoord.Y1.Text;
            }
            else
            {
                m_stDrawLineCoord.X2.Text = m_stCurCoord.X1.Text;
                m_stDrawLineCoord.Y2.Text = m_stCurCoord.Y1.Text;
            }
        }

        public void m_fnLinkNowRatio(ref Label reflbRratio)
        {
            m_lbNowRatio = reflbRratio;
            m_lbNowRatio.Text = string.Format("{0:00.00}", 1.00F);
        }
        public void m_fnLinkCurCoord(ref Label reflbCoordX, ref Label reflbCoordY)
        {
            m_stCurCoord.X1 = reflbCoordX;
            m_stCurCoord.Y1 = reflbCoordY;
        }

        public void m_fnLinkCurRGBVal(ref Label reflbR, ref Label reflbG, ref Label reflbB)
        {
            m_stRGBColor[0] = reflbR;
            m_stRGBColor[1] = reflbG;
            m_stRGBColor[2] = reflbB;
        }

        public void m_fnLinkCurFocusVal(ref Label refFoccus)
        {
            m_stFocus = refFoccus;

        }

        public void m_fnSetCurRGBVal(int nX, int nY)
        {
            try
            {
                Color color;

                if (nX < 0 || nY < 0) return;
                if (nX > m_imgLoaded.Width || nY > m_imgLoaded.Height) return;
                color = (m_imgLoaded).GetPixel(nX, nY);


                if (m_bDivisionModeFlg)
                {
                    if ((picMainImg.Width / 3 < nX && picMainImg.Height / 3 < nY) && (picMainImg.Width / 3 * 2 >= nX && picMainImg.Height / 3 * 2 >= nY))
                        color = (m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_CENTER]).GetPixel(nX - picMainImg.Width / 3, nY - picMainImg.Height / 3);
                    else if (picMainImg.Width / 2 >= nX && picMainImg.Height / 2 >= nY)
                        color = (m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_LEFT_TOP]).GetPixel(nX, nY);
                    else if (picMainImg.Width / 2 < nX && picMainImg.Height / 2 >= nY)
                        color = (m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_RIGHT_TOP]).GetPixel(nX - picMainImg.Width / 2, nY);
                    else if (picMainImg.Width / 2 >= nX && picMainImg.Height / 2 < nY)
                        color = (m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_LEFT_BOTTOM]).GetPixel(nX, nY - picMainImg.Height / 2);
                    else if (picMainImg.Width / 2 < nX && picMainImg.Height / 2 < nY)
                        color = (m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_RIGHT_BOTTOM]).GetPixel(nX - picMainImg.Width / 2, nY - picMainImg.Height / 2);

                }
                m_stRGBColor[0].Text = string.Format("{0:000}", color.R);
                m_stRGBColor[1].Text = string.Format("{0:000}", color.G);
                m_stRGBColor[2].Text = string.Format("{0:000}", color.B);
            }
            catch { return; }
        }

        public void m_fnSetCurFocusVal(int nX, int nY)
        {
            try
            {
               

                if (nX < 0 || nY < 0) return;
               // if (nX > m_imgLoaded.Width || nY > m_imgLoaded.Height) return;
                int a = 0;


                if (m_bUseFocusLineView)
                {
                    if ((picMainImg.Width / 3 < nX && picMainImg.Height / 3 < nY) && (picMainImg.Width / 3 * 2 >= nX && picMainImg.Height / 3 * 2 >= nY))
                        a = nX/*(m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_CENTER]).GetPixel(nX - picMainImg.Width / 3, nY - picMainImg.Height / 3)*/;
                    else if (picMainImg.Width / 2 >= nX && picMainImg.Height / 2 >= nY)
                        a = nX;/*(m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_LEFT_TOP]).GetPixel(nX, nY);*/
                    else if (picMainImg.Width / 2 < nX && picMainImg.Height / 2 >= nY)
                        a = nX;/*(m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_RIGHT_TOP]).GetPixel(nX - picMainImg.Width / 2, nY);*/
                    else if (picMainImg.Width / 2 >= nX && picMainImg.Height / 2 < nY)
                        a = nX;/*(m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_LEFT_BOTTOM]).GetPixel(nX, nY - picMainImg.Height / 2);*/
                    else if (picMainImg.Width / 2 < nX && picMainImg.Height / 2 < nY)
                        a = nX;/*(m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_RIGHT_BOTTOM]).GetPixel(nX - picMainImg.Width / 2, nY - picMainImg.Height / 2);*/

                }
                m_stFocus.Text = string.Format("{0} {1}",nX,nY);

            }
            catch { return; }
        }


        public void m_fnMiniMapMove(Image iMapimg, int nSelMapX, int nSelMapY)
        {
            if (m_imgLoaded == null) return;
            if (iMapimg == null) return;
            int nMapX = 0, nMapY = 0;


            nMapX = (m_PixelSize.X * nSelMapX / iMapimg.Width);
            nMapY = (m_PixelSize.Y * nSelMapY / iMapimg.Height);
            m_imgRect.Y = nMapY;
            m_imgRect.X = nMapX;
            if (m_imgLoaded != null)
            {
                m_stCurCoord.X1.Text = (m_imgRect.X + (m_imgRect.Width / 2)).ToString();
                m_stCurCoord.Y1.Text = (m_imgRect.Y + (m_imgRect.Height / 2)).ToString();
            }


            picMainImg.Invalidate();
        }



        public delegate List<Rectangle> DelDrawRect();

        public delegate List<Point> DelDrawPoint();
        delegate void ThreadIPaintCallback(object sender, PaintEventArgs e);
        private void picMainImg_Paint(object sender, PaintEventArgs e)
        {
            try
            {
                if (this.InvokeRequired)
                {
                    ThreadIPaintCallback delOpenImg = new ThreadIPaintCallback(picMainImg_Paint);
                    this.Invoke(delOpenImg, sender, e);
                    return;
                }

                if (m_imgLoaded != null || m_DivLoad.m_imgDivLoaded != null)
                {
                    m_imgMiniMap = m_ucPicMiniMap.GetMinimapImg();
                    Pen penTemp = new Pen(Color.Red);
                    if (m_bUseLine) penTemp = m_penLine;
                    if (m_bUseLineProfile) penTemp = m_penLineProfile;
                    if (m_bUseContrast) penTemp = m_penUniformity;

                    //if (m_DicRect.Count() > 0)
                    {
                        if (m_DicRect[m_fnGetUseRctNum()].bUseRectView) penTemp = m_DicRect[m_fnGetUseRctNum()].GetPenVal();
                        if (m_DicPnt[m_fnGetUsePntNum()].bUsePointView) penTemp = m_DicPnt[m_fnGetUsePntNum()].GetPenVal();
                    }

                    //                 e.Graphics.InterpolationMode =
                    //                    System.Drawing.Drawing2D.InterpolationMode.HighQualityBicubic;
                    e.Graphics.InterpolationMode =
                        System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
                    e.Graphics.SmoothingMode =
                        System.Drawing.Drawing2D.SmoothingMode.None;


                    if (!m_bDivisionModeFlg)
                    {
                        Rectangle tmp = new Rectangle(0, 0, picMainImg.Width, picMainImg.Height);
                        e.Graphics.DrawImage(m_imgLoaded, tmp);
                        // Guide Line 영역 표시 유무 플래그
                        if (m_bUseGuideLineView)
                        {
                            Rectangle rect = CalcRectCoord(m_GuideLineRect);
                      
                         
                            e.Graphics.TranslateTransform((float)(rect.Width / 2) + rect.X, (float)(rect.Height / 2) + rect.Y);
                            e.Graphics.RotateTransform((float)m_dGuideLineOffsetT);
                            e.Graphics.TranslateTransform(-(float)((rect.Width / 2) + rect.X), -(float)((rect.Height / 2) + rect.Y));
                            e.Graphics.DrawRectangle(m_penGuideLine, rect);


                            m_ucPicMiniMap.m_fnDrawGuideLine(/*m_imgMiniMap,*/ m_PixelSize, (m_GuideLineRect), m_dGuideLineOffsetT, GetLiveFlg());
                        }
                        else
                        {
                            m_ucPicMiniMap.m_fnDrawGuideLine(/*m_imgMiniMap,*/ m_PixelSize, (new Rectangle(0, 0, 0, 0)), m_dGuideLineOffsetT, GetLiveFlg());
                        }

                        if (m_bUseLine)
                        {
                            int nX = CalcRectCoord(pntLineClick.X, m_imgRect.X, m_PixelSize.X, picMainImg.Width);
                            int nY = CalcRectCoord(pntLineClick.Y, m_imgRect.Y, m_PixelSize.Y, picMainImg.Height);
                            int nW = CalcRectCoord(m_Linerect.Width, m_imgRect.X, m_PixelSize.X, picMainImg.Width);
                            int nH = CalcRectCoord(m_Linerect.Height, m_imgRect.Y, m_PixelSize.Y, picMainImg.Height);
                            e.Graphics.DrawLine(penTemp, nX, nY, nW, nH);

                            if (m_bUseDistanceView)
                            {
                                double dResolution = m_fnGetResolution();
                                string strTmp = string.Format("({0:000.000} , {1:000.000}) mm/pixel ", Math.Abs((m_Linerect.Width - pntLineClick.X).PixelToMicro(dResolution)),
                                    Math.Abs((m_Linerect.Height - pntLineClick.Y).PixelToMicro(dResolution)));
                                if (nW > 0 || nH > 0)
                                    m_fnDrawStringOnLine(e.Graphics, nW, nH, strTmp);
                            }
                        }

                        if (m_bUseRect && penTemp != null)
                        {
                            Rectangle rectTmp = m_RectCom;

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
                            Rectangle rect = CalcRectCoord(rectTmp);
                            e.Graphics.DrawRectangle(penTemp, rect);
                        }
                
                    }
                    else
                    {
                        DisplayDivisionImage(e.Graphics);

                        if (m_bUseLine)
                        {
                            int nWidth = picMainImg.Width / 2 / 4 * 4;
                            int nHeight = picMainImg.Height / 2 / 4 * 4;

                            Rectangle rectXY = new Rectangle();
                            Rectangle tempRect = new Rectangle();

                            tempRect = GetRectCurDiv(m_pntLineRightClick.X, m_pntLineRightClick.Y);
                            m_LastLinerect = tempRect;
                            rectXY.X = CalcRectCoord(pntLineClick.X, tempRect.X, nWidth, nWidth);
                            rectXY.Y = CalcRectCoord(pntLineClick.Y, tempRect.Y, nHeight, nHeight);
                            rectXY.Width = CalcRectCoord(m_Linerect.Width, tempRect.X, nWidth, nWidth);
                            rectXY.Height = CalcRectCoord(m_Linerect.Height, tempRect.Y, nHeight, nHeight);

                            Rectangle rect = checkDivisonArea(rectXY.X, rectXY.Y, rectXY.Width, rectXY.Height, m_pntLineRightClick);

                            e.Graphics.DrawLine(penTemp, rect.X, rect.Y, rect.Width, rect.Height);
                            if (m_bUseDistanceView)
                            {
                                double dResolution = m_fnGetResolution();
                                string strTmp = string.Format("({0:000.000} , {1:000.000}) ㎛/pixel ", Math.Abs((m_Linerect.Width - pntLineClick.X).PixelToMicro(dResolution)),
                                    Math.Abs((m_Linerect.Height - pntLineClick.Y).PixelToMicro(dResolution)));
                                if (rectXY.Width > 0 || rectXY.Height > 0)
                                    m_fnDrawStringOnLine(e.Graphics, rectXY.Width, rectXY.Height, strTmp);
                            }
                        }

                        if (m_bUseRect)
                        {
                            int nWidth = picMainImg.Width / 2 / 4 * 4;
                            int nHeight = picMainImg.Height / 2 / 4 * 4;

                            Rectangle rectXY = new Rectangle();
                            Rectangle tempRect = new Rectangle();

                            tempRect = GetRectCurDiv(m_pntRectRightClick.X, m_pntRectRightClick.Y);

                            rectXY.X = CalcRectCoord(m_pntRectClick.X, tempRect.X, nWidth, nWidth);
                            rectXY.Y = CalcRectCoord(m_pntRectClick.Y, tempRect.Y, nHeight, nHeight);
                            rectXY.Width = CalcRectCoord(m_pntRectClick.X + m_RectCom.Width, tempRect.X, nWidth, nWidth);
                            rectXY.Height = CalcRectCoord(m_pntRectClick.Y + m_RectCom.Height, tempRect.Y, nHeight, nHeight);

                            Rectangle rect = checkDivisonArea(rectXY.X, rectXY.Y, rectXY.Width, rectXY.Y, m_pntRectRightClick);
                            e.Graphics.DrawLine(penTemp, rect.X, rect.Y, rect.Width, rect.Y);
                            rect = checkDivisonArea(rectXY.X, rectXY.Y, rectXY.X, rectXY.Height, m_pntRectRightClick);
                            e.Graphics.DrawLine(penTemp, rect.X, rect.Y, rect.X, rect.Height);
                            rect = checkDivisonArea(rectXY.Width, rectXY.Y, rectXY.Width, rectXY.Height, m_pntRectRightClick);
                            e.Graphics.DrawLine(penTemp, rect.Width, rect.Y, rect.Width, rect.Height);
                            rect = checkDivisonArea(rectXY.X, rectXY.Height, rectXY.Width, rectXY.Height, m_pntRectRightClick);
                            e.Graphics.DrawLine(penTemp, rect.X, rect.Height, rect.Width, rect.Height);
                        }
                    }


                    if (m_bUseFocusLineView && m_bunfocus)
                    {
                        rect2 = CalcRectCoord(m_GuideLineRect2);
                        e.Graphics.DrawRectangles(m_penGuideLine, rect2);
                    }

                    //각 ROI 표시 
                    for (int nRoiCnt = 0; nRoiCnt < (int)Enums.DrawRct.MAX; nRoiCnt++)
                    {
                        if (!m_DicRect.ContainsKey(nRoiCnt)) continue;
                        m_fnDrawRect(e.Graphics, m_DicRect[nRoiCnt].GetPenVal(), m_DicRect[nRoiCnt].GetRectListVal()/*, m_RoiRect*/);
                    }

                    //각 Point 좌표 출력 
                    for (int nRoiCnt = 0; nRoiCnt < (int)Enums.DrawPnt.MAX; nRoiCnt++)
                    {
                        if (!m_DicPnt.ContainsKey(nRoiCnt)) continue;


                        if (m_DicPnt[nRoiCnt].GetViewKind() != (int)Enums.E_PNT_VIEW_KIND.POLYGON)
                            m_fnDrawPoint(e.Graphics, m_DicPnt[nRoiCnt].GetPenVal(), (m_DicPnt[nRoiCnt].GetPointListVal()), m_DicPnt[nRoiCnt].GetViewKind());
                        else
                            m_fnDrawPolygon(e.Graphics, m_DicPnt[nRoiCnt].GetPenVal(), m_DicPnt[nRoiCnt].GetPolygonListVal(), m_DicPnt[nRoiCnt].GetViewKind());
                    }

                    if (m_bUseGvView)
                    {
                        m_fnDrawStringMousePoint(e.Graphics, string.Format("GV : {0}", m_stRGBColor[0].Text));
                    }

                    //if(m_bUseFocusLineView)
                    //{
                    //    m_fnDrawStringFocusValue(e.Graphics, string.Format("Focus : {0}", m_stFocus.Text));
                    //}

                    if (vScrollBar1.Value >= vScrollBar1.Maximum)
                        vScrollBar1.Visible = false;
                    else vScrollBar1.Visible = true;
                    if (hScrollBar1.Value >= hScrollBar1.Maximum)
                        hScrollBar1.Visible = false;
                    else hScrollBar1.Visible = true;

                    if (m_bUseMiniMap)
                    {
                        Rectangle recFovRec = new Rectangle();

                        if (m_bDivisionModeFlg)
                        {
                            //                             for (int i = 0; i < 5; i++)
                            //                             {
                            //                                 m_ucPicMiniMap.picMinImg_Paint(m_imgMiniMap, m_LivePixelSize,
                            //                                 m_DivLoad.m_imgRect[i],
                            //                                 getOriginPntList(),
                            //                                 getAlignPntList(),
                            //                                 getRoiRectList(),
                            //                                 picMainImg.Width, picMainImg.Height, m_dRratio,
                            //                                 m_bLiveFlg);
                            //                             }
                        }
                        else
                        {
                            recFovRec = m_imgRect;
                        }


                        m_ucPicMiniMap.picMinImg_Paint(/*m_imgMiniMap,*/ m_PixelSize,
                         recFovRec,
                         m_DicPnt,
                         m_DicRect
                            /*getRoiRectList(m_fnGetUseRoiNum())*/,
                         picMainImg.Width, picMainImg.Height, m_dRratio,
                         true);
                    }

                    ///////////////////////////////////////////////////////////////////////////////
                    // ruler control
                    //                     if (m_imgRect == null || m_imgRect.Width == 0 || m_imgRect.Height == 0) return;
                    //                     rulerControl_H.StartValue = m_imgRect.X * 0.032;
                    //                     rulerControl_H.DpiX = (float)SetRuler_ppi(0.032, m_imgRect.Width);
                    // 
                    //                     rulerControl_V.StartValue = m_imgRect.Y * 0.032;
                    //                     rulerControl_V.DpiX = rulerControl_H.DpiX;
                    ///////////////////////////////////////////////////////////////////////////////

                }
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine(ex.Message);//string.Format("{0}", (DateTime.Now - dateTime)));
            }

        }

        private double SetRuler_ppi(double fResolution, int nWindowPixelCnt, double dZoom = 0.0, double dStart = 0.0)
        {
            double dRet = 0.0;

            //window disp length
            double length = fResolution * 0.1 * (double)nWindowPixelCnt;

            double win_cm = (double)picMainImg.Width / length;

            //ppc -> ppi
            dRet = win_cm * 2.54;

            return dRet;

        }
        public void m_fnDrawStringOnLine(Graphics grp, int nW, int nH, string strDescription)
        {
            Font font = new Font("Gadugi", 12, FontStyle.Bold, GraphicsUnit.Point);

            int nDrawX = 0;
            int nDrawY = nH;
            if (nW >= 140) nDrawX = nW - 140;
            if (nW >= picMainImg.Width - 70) nDrawX = picMainImg.Width - 220;
            if (nH >= picMainImg.Height - 20) nDrawY = picMainImg.Height - 20;

            Rectangle drawRect = new Rectangle(nDrawX, nDrawY, 300, 40);
            StringFormat stringFormat = new StringFormat();
            stringFormat.Alignment = StringAlignment.Near;
            stringFormat.LineAlignment = StringAlignment.Near;
            SolidBrush drawBrush = new SolidBrush(Color.Yellow);
            grp.DrawString(strDescription, font, drawBrush, drawRect, stringFormat);
        }
        public void m_fnDrawStringMousePoint(Graphics grp, string strDescription)
        {
            //  if (m_bUseGvView)
            {
                Font font = new Font("Gadugi", 12, FontStyle.Bold, GraphicsUnit.Point);
                Rectangle drawRect = new Rectangle(m_LastPoint.X + 10, m_LastPoint.Y - 3, 140, 40);
                StringFormat stringFormat = new StringFormat();
                stringFormat.Alignment = StringAlignment.Near;
                stringFormat.LineAlignment = StringAlignment.Near;
                SolidBrush drawBrush = new SolidBrush(Color.Yellow);
                grp.DrawString(strDescription, font, drawBrush, drawRect, stringFormat);
            }
        }

        public void m_fnDrawStringFocusValue(Graphics grp, string strDescription)
        {
            int nVlaue = 0;
            Font font = new Font("Gadugi", 12, FontStyle.Bold, GraphicsUnit.Point);


            if (m_bUseGvView)
                nVlaue = 15;
            else
                nVlaue = -3;

            Rectangle drawRect = new Rectangle(m_LastPoint.X + 10, m_LastPoint.Y + nVlaue, 140, 40);
            StringFormat stringFormat = new StringFormat();
            stringFormat.Alignment = StringAlignment.Near;
            stringFormat.LineAlignment = StringAlignment.Near;
            SolidBrush drawBrush = new SolidBrush(Color.LimeGreen);
            grp.DrawString(strDescription, font, drawBrush, drawRect, stringFormat);

        }

        public void m_fnDrawPoint(Graphics grp, Pen penPnt, Point pnt, int nViewKind, string strDescription = "")
        {
            //int nCnt = 0;
            try
            {
                int nFontOffSet = (int)Math.Round(5 * m_dRratio);
                int nStrRectOffSet = (int)Math.Round(200 / m_dRratio);


                string strtmp = strDescription;// +string.Format("{0}", ++nCnt);
                Font font = new Font("Gadugi", nFontOffSet < 5 ? 5 : nFontOffSet, FontStyle.Bold, GraphicsUnit.Point);

                Rectangle drawRect = new Rectangle();

                switch (nViewKind)
                {
                    case (int)Enums.E_PNT_VIEW_KIND.CROSS:
                        {
                            drawRect.X = CalcRectCoord(pnt.X, m_imgRect.X, m_PixelSize.X, picMainImg.Width) + (int)Math.Round(5 * m_dRratio);
                            drawRect.Y = CalcRectCoord(pnt.Y, m_imgRect.Y, m_PixelSize.Y, picMainImg.Height) - (int)Math.Round(15 * m_dRratio);
                            StringFormat stringFormat = new StringFormat();
                            stringFormat.Alignment = StringAlignment.Near;
                            stringFormat.LineAlignment = StringAlignment.Near;
                            SolidBrush drawBrush = new SolidBrush(penPnt.Color);
                            grp.DrawString(strtmp, font, drawBrush, drawRect, stringFormat);

                            penPnt.Width = 2;

                            grp.DrawLine
                                        (
                                            penPnt,
                                            CalcRectCoord(pnt.X, m_imgRect.X, m_PixelSize.X, picMainImg.Width),
                                            CalcRectCoord(pnt.Y, m_imgRect.Y, m_PixelSize.Y, picMainImg.Height) - (int)Math.Round(15 * m_dRratio),
                                            CalcRectCoord(pnt.X, m_imgRect.X, m_PixelSize.X, picMainImg.Width),
                                            CalcRectCoord(pnt.Y, m_imgRect.Y, m_PixelSize.Y, picMainImg.Height) + (int)Math.Round(15 * m_dRratio)
                                        );
                            grp.DrawLine
                                        (
                                            penPnt,
                                            CalcRectCoord(pnt.X, m_imgRect.X, m_PixelSize.X, picMainImg.Width) - (int)Math.Round(15 * m_dRratio),
                                            CalcRectCoord(pnt.Y, m_imgRect.Y, m_PixelSize.Y, picMainImg.Height),
                                            CalcRectCoord(pnt.X, m_imgRect.X, m_PixelSize.X, picMainImg.Width) + (int)Math.Round(15 * m_dRratio),
                                            CalcRectCoord(pnt.Y, m_imgRect.Y, m_PixelSize.Y, picMainImg.Height)
                                        );
                        }
                        break;

                    case (int)Enums.E_PNT_VIEW_KIND.POINT:
                        {
                            Rectangle rectTmp = new Rectangle(pnt.X - 1, pnt.Y - 1, 2, 2);
                            Rectangle rect = CalcRectCoord(rectTmp);
                            //e.Graphics.DrawRectangle(penTemp, rect);
                            grp.FillRectangle(penPnt.Brush, rect);
                        }
                        break;

                    case (int)Enums.E_PNT_VIEW_KIND.POLYGON:
                        {
                            Rectangle rectTmp = new Rectangle(pnt.X - 1, pnt.Y - 1, 2, 2);
                            Rectangle rect = CalcRectCoord(rectTmp);
                            grp.FillRectangle(penPnt.Brush, rect);
                        }
                        break;
                }
            }
            catch
            {
                //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E090", Globals.nLanguageFlg), true);
            }

        }

        public void m_fnDrawPoint(Graphics grp, Pen penPnt, List<Point> listPnt, int nViewKind, string strDescription = "")
        {
            //int nCnt = 0;
            try
            {
                int nFontOffSet = (int)Math.Round(5 * m_dRratio);
                int nStrRectOffSet = (int)Math.Round(200 / m_dRratio);

                foreach (Point pnt in listPnt)
                {
                    string strtmp = strDescription;// +string.Format("{0}", ++nCnt);
                    Font font = new Font("Gadugi", nFontOffSet < 5 ? 5 : nFontOffSet, FontStyle.Bold, GraphicsUnit.Point);

                    Rectangle drawRect = new Rectangle();

                    switch (nViewKind)
                    {
                        case (int)Enums.E_PNT_VIEW_KIND.CROSS:
                            drawRect.X = CalcRectCoord(pnt.X, m_imgRect.X, m_PixelSize.X, picMainImg.Width) + (int)Math.Round(5 * m_dRratio);
                            drawRect.Y = CalcRectCoord(pnt.Y, m_imgRect.Y, m_PixelSize.Y, picMainImg.Height) - (int)Math.Round(15 * m_dRratio);
                            StringFormat stringFormat = new StringFormat();
                            stringFormat.Alignment = StringAlignment.Near;
                            stringFormat.LineAlignment = StringAlignment.Near;
                            SolidBrush drawBrush = new SolidBrush(penPnt.Color);
                            grp.DrawString(strtmp, font, drawBrush, drawRect, stringFormat);

                            penPnt.Width = 2;

                            grp.DrawLine
                                        (
                                            penPnt,
                                            CalcRectCoord(pnt.X, m_imgRect.X, m_PixelSize.X, picMainImg.Width),
                                            CalcRectCoord(pnt.Y, m_imgRect.Y, m_PixelSize.Y, picMainImg.Height) - (int)Math.Round(15 * m_dRratio),
                                            CalcRectCoord(pnt.X, m_imgRect.X, m_PixelSize.X, picMainImg.Width),
                                            CalcRectCoord(pnt.Y, m_imgRect.Y, m_PixelSize.Y, picMainImg.Height) + (int)Math.Round(15 * m_dRratio)
                                        );
                            grp.DrawLine
                                       (
                                           penPnt,
                                           CalcRectCoord(pnt.X, m_imgRect.X, m_PixelSize.X, picMainImg.Width) - (int)Math.Round(15 * m_dRratio),
                                           CalcRectCoord(pnt.Y, m_imgRect.Y, m_PixelSize.Y, picMainImg.Height),
                                           CalcRectCoord(pnt.X, m_imgRect.X, m_PixelSize.X, picMainImg.Width) + (int)Math.Round(15 * m_dRratio),
                                           CalcRectCoord(pnt.Y, m_imgRect.Y, m_PixelSize.Y, picMainImg.Height)
                                       );
                            break;

                        case (int)Enums.E_PNT_VIEW_KIND.POINT:
                            {
                                Rectangle rectTmp = new Rectangle(pnt.X - 1, pnt.Y - 1, 2, 2);
                                Rectangle rect = CalcRectCoord(rectTmp);
                                //e.Graphics.DrawRectangle(penTemp, rect);
                                grp.FillRectangle(penPnt.Brush, rect);
                            }
                            break;

                    }
                }

            }
            catch
            {
                //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E090", Globals.nLanguageFlg), true);
            }

        }

        public void m_fnDrawPolygon(Graphics grp, Pen penPnt, Dictionary<int, List<Point>> dicPnt, int nViewKind)
        {
            try
            {
                if (dicPnt == null) return;
                switch (nViewKind)
                {
                    case (int)Enums.E_PNT_VIEW_KIND.POLYGON:
                        {

                            for (int nDicCnt = 1; nDicCnt <= dicPnt.Count; nDicCnt++)
                            {

                                List<Point> listPolygon = new List<Point>();
                                List<Point> listPnt;
                                if (dicPnt.TryGetValue(nDicCnt, out listPnt))
                                {
                                    if (listPnt == null) continue;
                                    foreach (Point pnt in listPnt)
                                    {
                                        Rectangle rectTmp = new Rectangle(pnt.X, pnt.Y, 1, 1);
                                        Rectangle rect = CalcRectCoord(rectTmp);
                                        listPolygon.Add(new Point(rect.X, rect.Y));
                                    }

                                    Point[] pntVal = listPolygon.ToList().ToArray();

                                    int nLastPoint = pntVal.Count() - 1;

                                    if (pntVal.Count() > 0)
                                    {
                                        bool bEndPolygon = false;
                                        if (pntVal.Count() > 1)
                                        {
                                            if (pntVal[0].X == pntVal[nLastPoint].X && pntVal[0].Y == pntVal[nLastPoint].Y)
                                            {
                                                bEndPolygon = true;//SetEndPolygon(m_fnGetUsePntNum(), true);
                                            }
                                            grp.DrawLines(penPnt, pntVal);
                                        }

                                        if (!bEndPolygon && !m_DicPnt[m_fnGetUsePntNum()].IsEndPolygon() & m_DicPnt[m_fnGetUsePntNum()].nCurPolygonKey == nDicCnt)
                                        {
                                            Point pnt = m_DicPnt[m_fnGetUsePntNum()].GetPointCoord();
                                            int nX = CalcRectCoord(pnt.X, m_imgRect.X, m_PixelSize.X, picMainImg.Width);
                                            int nY = CalcRectCoord(pnt.Y, m_imgRect.Y, m_PixelSize.Y, picMainImg.Height);
                                            grp.DrawLine(penPnt, pntVal[nLastPoint], new Point(nX, nY));

                                        }
                                    }

                                }
                            }
                        }
                        break;

                }



            }
            catch
            {
                //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E090", Globals.nLanguageFlg), true);
            }

        }
        private Point CalcLineCoord(Point Rect)
        {

            Point drawRect = new Point();

            drawRect.X = (int)Math.Round((m_imgRect.X) + (Rect.X * m_dRratio));
            drawRect.Y = (int)Math.Round((m_imgRect.Y) + (Rect.Y * m_dRratio));

            return drawRect;
        }

        public void m_fnDrawRect(Graphics grp, Pen penRect, /*DelDrawRect*/List<Rectangle> listRect/*, Rectangle RoiRect*/)
        {
            //     grp.DrawRectangle(penRect, CalcRectCoord(RoiRect));

            //     Systems.m_fnAddLog(Globals.SelPcNo, (int)Enums.LogView.MANUAL, string.Format("Count '{0}':", DateTime.Now));

            //             Rectangle[] rect = listRect().ToArray();
            //             penRect.DashStyle = System.Drawing.Drawing2D.DashStyle.Solid;
            //             grp.DrawRectangles(penRect, CalcRectCoord(rect));
            //             penRect.DashStyle = System.Drawing.Drawing2D.DashStyle.Dot;

            //             Parallel.For(0, rect.Count(), new ParallelOptions { MaxDegreeOfParallelism = Environment.ProcessorCount },
            //                                     nCnt =>
            //                                     {
            //                                         //penRect.DashStyle = System.Drawing.Drawing2D.DashStyle.Solid;
            //                                         lock(grp) grp.DrawRectangle(penRect, CalcRectCoord(rect[nCnt]));
            //                                        // penRect.DashStyle = System.Drawing.Drawing2D.DashStyle.Dot;
            //                                     });
            foreach (Rectangle rect in listRect)
            {
                penRect.DashStyle = System.Drawing.Drawing2D.DashStyle.Solid;
                grp.DrawRectangle(penRect, CalcRectCoord(rect));
                penRect.DashStyle = System.Drawing.Drawing2D.DashStyle.Dot;
            }
            // Systems.m_fnAddLog(Globals.SelPcNo, (int)Enums.LogView.MANUAL, string.Format("Count '{0}':", DateTime.Now));
        }
        private int CalcRectCoord(int CurCoord, int StartCoord, int EndCoord, int ImgLen)
        {
            int retCoord = 0;


            // if (m_bDivisionModeFlg)
            {
                retCoord = (int)Math.Round((CurCoord * ImgLen) * m_dRratio / EndCoord);
                retCoord = ((int)Math.Round(((((StartCoord * ImgLen) * m_dRratio / EndCoord) - retCoord) * -1)));
            }


            return retCoord;
        }
        private Rectangle CalcRectCoord(Rectangle Rect)
        {

            Rectangle drawrec = new Rectangle();


            drawrec.X = (int)Math.Round((Rect.X * picMainImg.Width) * m_dRratio / m_PixelSize.X);
            drawrec.Y = (int)Math.Round((Rect.Y * picMainImg.Height) * m_dRratio / m_PixelSize.Y);
            drawrec.X = (int)Math.Round(((((m_imgRect.X * picMainImg.Width) * m_dRratio / m_PixelSize.X) - drawrec.X) * -1));
            drawrec.Y = (int)Math.Round(((((m_imgRect.Y * picMainImg.Height) * m_dRratio / m_PixelSize.Y) - drawrec.Y) * -1));

            drawrec.Width = (int)Math.Round(((picMainImg.Width * Rect.Width) * m_dRratio) / m_PixelSize.X);
            drawrec.Height = (int)Math.Round(((picMainImg.Height * Rect.Height) * m_dRratio) / m_PixelSize.Y);


            return drawrec;
        }
        private Point[] CalFocusPoint(Rectangle point)
        {
            Point[] pp = new Point[4];


            pp[0].X = point.X * 2;
            pp[0].Y = point.Y;
            pp[1].X = 1000;
            pp[1].Y = 1000;


            return pp;
        }
        private Rectangle CalcRectCoordDiv(Rectangle Rect, int ndiv)
        {


            Rectangle drawrec = new Rectangle();

           
               drawrec.X = (int)Math.Round(((Rect.X * picMainImg.Width) * m_dRratio / m_PixelSize.X));
               drawrec.Y = (int)Math.Round((Rect.Y * picMainImg.Height) * m_dRratio / m_PixelSize.Y);
               drawrec.X = (int)Math.Round(((((m_imgRect.X * picMainImg.Width) * m_dRratio / m_PixelSize.X) - drawrec.X) * -1));
               drawrec.Y = (int)Math.Round((((m_imgRect.Y * picMainImg.Height) * m_dRratio / m_PixelSize.Y) - drawrec.Y) * -1);

               drawrec.Y = (drawrec.Y * 2) + (drawrec.Y / 2);
               drawrec.Width = (int)Math.Round((((picMainImg.Width * Rect.Width) * m_dRratio) / m_PixelSize.X) / ndiv);
               drawrec.Height = (int)Math.Round((((picMainImg.Height * Rect.Height) * m_dRratio) / m_PixelSize.Y) / ndiv);
               drawrec.X = (drawrec.X) + (drawrec.Width / 3 * 4) - (drawrec.Width / 3) + 2;


            return drawrec; 
        }

        private Rectangle[] CalcRectCoord(Rectangle[] Rect)
        {

            Rectangle[] drawrec = new Rectangle[Rect.Count()];

            Parallel.For(0, Rect.Count(), new ParallelOptions { MaxDegreeOfParallelism = Environment.ProcessorCount },
            nCnt =>
            {
                drawrec[nCnt].X = (int)Math.Round((Rect[nCnt].X * picMainImg.Width) * m_dRratio / m_PixelSize.X);
                drawrec[nCnt].Y = (int)Math.Round((Rect[nCnt].Y * picMainImg.Height) * m_dRratio / m_PixelSize.Y);
                drawrec[nCnt].X = (int)Math.Round(((((m_imgRect.X * picMainImg.Width) * m_dRratio / m_PixelSize.X) - drawrec[nCnt].X) * -1));
                drawrec[nCnt].Y = (int)Math.Round(((((m_imgRect.Y * picMainImg.Height) * m_dRratio / m_PixelSize.Y) - drawrec[nCnt].Y) * -1));

                drawrec[nCnt].Width = (int)Math.Round(((picMainImg.Width * Rect[nCnt].Width) * m_dRratio) / m_PixelSize.X);
                drawrec[nCnt].Height = (int)Math.Round(((picMainImg.Height * Rect[nCnt].Height) * m_dRratio) / m_PixelSize.Y);

            });

            return drawrec;
        }

        private void DisplayDivisionImage(Graphics grp)
        {
            Font font = new Font("Gadugi", 22, FontStyle.Bold, GraphicsUnit.Point);
            Rectangle drawFoucsRect;
            StringFormat stringFormat = new StringFormat();
            stringFormat.Alignment = StringAlignment.Near;
            stringFormat.LineAlignment = StringAlignment.Near;
            SolidBrush drawBrush = new SolidBrush(Color.GreenYellow);

            int nWidth =  picMainImg.Width / 2 / 4 * 4;
            int nHeight = picMainImg.Height / 2 / 4 * 4;
            Pen penDivGuieLine = m_penGuideLine;

            //가이드 라인 왼쪽 상단 표시
            Rectangle recLTDiv = new Rectangle(0, 0, nWidth, nHeight);
            Rectangle recLTDiv2 = new Rectangle(0, 0, 100,100);

            if (m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_LEFT_TOP] != null && !m_bUseFocusLineView)
                grp.DrawImage(m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_LEFT_TOP], recLTDiv, recLTDiv, GraphicsUnit.Pixel);
            else
                grp.DrawImage(m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_LEFT_TOP], recLTDiv, recLTDiv2, GraphicsUnit.Pixel);

            penDivGuieLine.Width = 1;// (int)Math.Round(3 * m_dRratio);
            recLTDiv.X = CalcRectCoord(m_GuideLineRect.X, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_TOP].X, nWidth, nWidth);
            recLTDiv.Y = CalcRectCoord(m_GuideLineRect.Y, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_TOP].Y, nHeight, nHeight);
            recLTDiv.Width = nWidth;
            recLTDiv.Height = CalcRectCoord(m_GuideLineRect.Y, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_TOP].Y, nHeight, nHeight);
            if (m_bUseGuideLineView)
                grp.DrawLine(m_penGuideLine, recLTDiv.X, recLTDiv.Y, recLTDiv.Width, recLTDiv.Height);

            recLTDiv.X = CalcRectCoord(m_GuideLineRect.X, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_TOP].X, nWidth, nWidth);
            recLTDiv.Y = CalcRectCoord(m_GuideLineRect.Y, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_TOP].Y, nHeight, nHeight);
            recLTDiv.Width = CalcRectCoord(m_GuideLineRect.X, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_TOP].X, nWidth, nWidth);
            recLTDiv.Height = nHeight;
            if (m_bUseGuideLineView)
                grp.DrawLine(m_penGuideLine, recLTDiv.X, recLTDiv.Y, recLTDiv.Width, recLTDiv.Height);
            // 포커스 수치값 표시
            if (m_bUseFocusValeView)
            {
                drawFoucsRect = new Rectangle(nWidth / 2 - 50, nHeight / 2 - 20, 300, 140);
                grp.DrawString(string.Format("{0:0.000}", m_fnGetFocusValue()[(int)E_DIV_NUM.E_LEFT_TOP]), font, drawBrush, drawFoucsRect, stringFormat);
            }

            //가이드 라인 오른쪽 상단 표시
            Rectangle recRTDiv = new Rectangle(nWidth - 1, 0, nWidth, nHeight);
            if (m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_RIGHT_TOP] != null && !m_bUseFocusLineView)
                grp.DrawImage(m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_RIGHT_TOP], recRTDiv, 0, 0, recRTDiv.Width, recRTDiv.Height, GraphicsUnit.Pixel);
            else
                grp.DrawImage(m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_RIGHT_TOP], recRTDiv, 0, 0, recLTDiv2.Width, recLTDiv2.Height, GraphicsUnit.Pixel);

            recRTDiv.X = nWidth;
            recRTDiv.Y = CalcRectCoord(m_GuideLineRect.Y, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_TOP].Y, nHeight, nHeight);
            recRTDiv.Width = (int)Math.Round((m_GuideLineRect.Width) * m_dRratio) + nWidth
                                + CalcRectCoord(m_GuideLineRect.X, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_TOP].X, nWidth, nWidth);
            recRTDiv.Height = CalcRectCoord(m_GuideLineRect.Y, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_TOP].Y, nHeight, nHeight);
            if (recRTDiv.X < nWidth) { recRTDiv.X = 0; recRTDiv.Width = 0; }
            if (recRTDiv.Width < nWidth) { recRTDiv.X = 0; recRTDiv.Width = 0; }
            if (m_bUseGuideLineView)
                grp.DrawLine(m_penGuideLine, recRTDiv.X, recRTDiv.Y, recRTDiv.Width, recRTDiv.Height);

            recRTDiv.X = (int)Math.Round((m_GuideLineRect.Width) * m_dRratio) + nWidth
                                + CalcRectCoord(m_GuideLineRect.X, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_TOP].X, nWidth, nWidth);
            recRTDiv.Y = CalcRectCoord(m_GuideLineRect.Y, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_TOP].Y, nHeight, nHeight);
            recRTDiv.Width = (int)Math.Round((m_GuideLineRect.Width) * m_dRratio) + nWidth
                                + CalcRectCoord(m_GuideLineRect.X, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_TOP].X, nWidth, nWidth);
            recRTDiv.Height = nHeight;
            if (recRTDiv.X < nWidth) { recRTDiv.X = 0; recRTDiv.Width = 0; }
            if (recRTDiv.Width < nWidth) { recRTDiv.X = 0; recRTDiv.Width = 0; }
            if (m_bUseGuideLineView)
                grp.DrawLine(m_penGuideLine, recRTDiv.X, recRTDiv.Y, recRTDiv.Width, recRTDiv.Height);

            // 포커스 수치값 표시
            if (m_bUseFocusValeView)
            {
                drawFoucsRect = new Rectangle(nWidth - 1 + nWidth / 2 - 50, nHeight / 2 - 20, 300, 140);
                grp.DrawString(string.Format("{0:0.000}", m_fnGetFocusValue()[(int)E_DIV_NUM.E_RIGHT_TOP]), font, drawBrush, drawFoucsRect, stringFormat);
            }
            //가이드 라인 왼쪽 하단 표시
            Rectangle recLBDiv = new Rectangle(0, nHeight - 1, nWidth, nHeight);
            if (m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_LEFT_BOTTOM] != null && !m_bUseFocusLineView)
                grp.DrawImage(m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_LEFT_BOTTOM], recLBDiv, 0, 0, recLBDiv.Width, recLBDiv.Height, GraphicsUnit.Pixel);
            else
                grp.DrawImage(m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_LEFT_BOTTOM], recLBDiv, 0, 0, recLTDiv2.Width, recLTDiv2.Height, GraphicsUnit.Pixel);

            recLBDiv.X = CalcRectCoord(m_GuideLineRect.X, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_BOTTOM].X, nWidth, nWidth);
            recLBDiv.Y = (int)Math.Round((m_GuideLineRect.Height) * m_dRratio) + nHeight
                + CalcRectCoord(m_GuideLineRect.Y, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_BOTTOM].Y, nHeight, nHeight);
            recLBDiv.Width = nWidth;
            recLBDiv.Height = (int)Math.Round((m_GuideLineRect.Height) * m_dRratio) + nHeight
                + CalcRectCoord(m_GuideLineRect.Y, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_BOTTOM].Y, nHeight, nHeight);
            if (recLBDiv.Y < nHeight) { recLBDiv.Y = 0; recLBDiv.Height = 0; }
            if (recLBDiv.Height < nHeight) { recLBDiv.Y = 0; recLBDiv.Height = 0; }
            if (m_bUseGuideLineView)
                grp.DrawLine(m_penGuideLine, recLBDiv.X, recLBDiv.Y, recLBDiv.Width, recLBDiv.Height);

            recLBDiv.X = CalcRectCoord(m_GuideLineRect.X, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_BOTTOM].X, nWidth, nWidth);
            recLBDiv.Y = nHeight;
            recLBDiv.Width = CalcRectCoord(m_GuideLineRect.X, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_BOTTOM].X, nWidth, nWidth);
            recLBDiv.Height = (int)Math.Round((m_GuideLineRect.Height) * m_dRratio) + nHeight
                + CalcRectCoord(m_GuideLineRect.Y, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_BOTTOM].Y, nHeight, nHeight);
            if (recLBDiv.Y < nHeight) { recLBDiv.Y = 0; recLBDiv.Height = 0; }
            if (recLBDiv.Height < nHeight) { recLBDiv.Y = 0; recLBDiv.Height = 0; }
            if (m_bUseGuideLineView)
                grp.DrawLine(m_penGuideLine, recLBDiv.X, recLBDiv.Y, recLBDiv.Width, recLBDiv.Height);

            // 포커스 수치값 표시
            if (m_bUseFocusValeView)
            {
                drawFoucsRect = new Rectangle(nWidth / 2 - 50, nHeight - 1 + nHeight / 2 - 20, 300, 140);
                grp.DrawString(string.Format("{0:0.000}", m_fnGetFocusValue()[(int)E_DIV_NUM.E_LEFT_BOTTOM]), font, drawBrush, drawFoucsRect, stringFormat);
            }
            //가이드 라인 오른쪽 하단 표시
            Rectangle recRBDiv = new Rectangle(nWidth - 1, nHeight - 1, nWidth, nHeight);
            if (m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_RIGHT_BOTTOM] != null && !m_bUseFocusLineView)
                grp.DrawImage(m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_RIGHT_BOTTOM], recRBDiv, 0, 0, recRBDiv.Width, recRBDiv.Height, GraphicsUnit.Pixel);
            else
                grp.DrawImage(m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_RIGHT_BOTTOM], recRBDiv, 0, 0, recLTDiv2.Width, recLTDiv2.Height, GraphicsUnit.Pixel);

            recRBDiv.X = (int)Math.Round((m_GuideLineRect.Width) * m_dRratio) + nWidth
                + CalcRectCoord(m_GuideLineRect.X, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_BOTTOM].X, nWidth, nWidth);
            recRBDiv.Y = (int)Math.Round((m_GuideLineRect.Height) * m_dRratio) + nHeight
                + CalcRectCoord(m_GuideLineRect.Y, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_BOTTOM].Y, nHeight, nHeight);
            recRBDiv.Width = nWidth;
            recRBDiv.Height = (int)Math.Round((m_GuideLineRect.Height) * m_dRratio) + nHeight
                + CalcRectCoord(m_GuideLineRect.Y, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_BOTTOM].Y, nHeight, nHeight);
            if (recRBDiv.X < nWidth) { recRBDiv.X = 0; recRBDiv.Width = 0; }
            if (recRBDiv.Y < nHeight) { recRBDiv.Y = 0; recRBDiv.Height = 0; }
            if (recRBDiv.Width < nWidth) { recRBDiv.X = 0; recRBDiv.Width = 0; }
            if (recRBDiv.Height < nHeight) { recRBDiv.Y = 0; recRBDiv.Height = 0; }
            if (m_bUseGuideLineView)
                grp.DrawLine(m_penGuideLine, recRBDiv.X, recRBDiv.Y, recRBDiv.Width, recRBDiv.Height);

            recRBDiv.X = (int)Math.Round((m_GuideLineRect.Width) * m_dRratio) + nWidth
                + CalcRectCoord(m_GuideLineRect.X, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_BOTTOM].X, nWidth, nWidth);
            recRBDiv.Y = (int)Math.Round((m_GuideLineRect.Height) * m_dRratio) + nHeight
                + CalcRectCoord(m_GuideLineRect.Y, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_BOTTOM].Y, nHeight, nHeight);
            recRBDiv.Width = (int)Math.Round((m_GuideLineRect.Width) * m_dRratio) + nWidth
                + CalcRectCoord(m_GuideLineRect.X, m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_BOTTOM].X, nWidth, nWidth);
            recRBDiv.Height = nHeight;
            if (recRBDiv.X < nWidth) { recRBDiv.X = 0; recRBDiv.Width = 0; }
            if (recRBDiv.Y < nHeight) { recRBDiv.Y = 0; recRBDiv.Height = 0; }
            if (recRBDiv.Width < nWidth) { recRBDiv.X = 0; recRBDiv.Width = 0; }
            if (recRBDiv.Height < nHeight) { recRBDiv.Y = 0; recRBDiv.Height = 0; }
            if (m_bUseGuideLineView)
                grp.DrawLine(m_penGuideLine, recRBDiv.X, recRBDiv.Y, recRBDiv.Width, recRBDiv.Height);

            // 포커스 수치값 표시
            if (m_bUseFocusValeView)
            {
                drawFoucsRect = new Rectangle(nWidth - 1 + nWidth / 2 - 50, nHeight - 1 + nHeight / 2 - 20, 300, 140);
                grp.DrawString(string.Format("{0:0.000}", m_fnGetFocusValue()[(int)E_DIV_NUM.E_RIGHT_BOTTOM]), font, drawBrush, drawFoucsRect, stringFormat);
            }
            //grp.PageScale = 2.0F;
            //가이드 라인 중앙 표시
            Rectangle rectCenterDiv = new Rectangle(picMainImg.Width / 3 - 1, picMainImg.Height / 3 - 1, picMainImg.Width / 3 - 1, picMainImg.Height / 3 - 1);
            if (m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_CENTER] != null && !m_bUseFocusLineView)
            {
                grp.DrawImage(m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_CENTER], rectCenterDiv, 0, 0, rectCenterDiv.Width, rectCenterDiv.Height, GraphicsUnit.Pixel);
            }
            else
                grp.DrawImage(m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_CENTER], rectCenterDiv, 0, 0, recLTDiv2.Width, recLTDiv2.Height, GraphicsUnit.Pixel);

            // 포커스 수치값 표시
            if (m_bUseFocusValeView)
            {
                drawFoucsRect = new Rectangle((picMainImg.Width / 3 - 1) + (picMainImg.Width / 3 - 1) / 2 - 50, (picMainImg.Height / 3 - 1) + (picMainImg.Height / 3 - 1) / 2 - 20, 300, 140);
                grp.DrawString(string.Format("{0:0.000}", m_fnGetFocusValue()[(int)E_DIV_NUM.E_CENTER]), font, drawBrush, drawFoucsRect, stringFormat);
            }
        }

        private Rectangle checkDivisonArea(int nStartX, int nStartY, int nEndX, int nEndY, Point pntClick)
        {
            int nTmpStartX = 0, nTmpStartY = 0, nTmpEndX = 0, nTmpEndY = 0;
            int nWidth = picMainImg.Width / 2 / 4 * 4;
            int nHeight = picMainImg.Height / 2 / 4 * 4;
            nTmpStartX = nStartX; nTmpStartY = nStartY; nTmpEndX = nEndX; nTmpEndY = nEndY;

            if ((picMainImg.Width / 3 - 1 < pntClick.X && picMainImg.Height / 3 - 1 < pntClick.Y)
                && (picMainImg.Width / 3 * 2 - 1 >= pntClick.X && picMainImg.Height / 3 * 2 - 1 >= pntClick.Y))
            {
                nStartX += (picMainImg.Width / 3) - 1;
                nStartY += (picMainImg.Height / 3) - 1;
                nEndX += (picMainImg.Width / 3) - 1;
                nEndY += (picMainImg.Height / 3) - 1;
                int nTmp = 0;
                if (nStartX > nEndX) { nTmp = nStartX; nStartX = nEndX; nEndX = nTmp; }
                if (nStartY > nEndY) { nTmp = nStartY; nStartY = nEndY; nEndY = nTmp; }
                nTmpStartX = nStartX; nTmpStartY = nStartY; nTmpEndX = nEndX; nTmpEndY = nEndY;

                if (nTmpStartY == nTmpEndY && nTmpEndX > picMainImg.Width / 3 * 2 - 1) nEndX = (picMainImg.Width / 3) * 2 - 1;
                if (nTmpStartY == nTmpEndY && nTmpStartX < picMainImg.Width / 3 - 1) nStartX = (picMainImg.Width / 3) - 1;
                if (nTmpStartY == nTmpEndY && nTmpStartX > picMainImg.Width / 3 * 2 - 1) { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }
                if (nTmpStartY == nTmpEndY && nTmpStartY > picMainImg.Height / 3 * 2 - 1) { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }
                if (nTmpStartY == nTmpEndY && nTmpEndX < picMainImg.Width / 3 - 1) { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }
                if (nTmpStartY == nTmpEndY && nTmpStartY < picMainImg.Height / 3 - 1) { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }

                if (nTmpStartX == nTmpEndX && nTmpStartY < picMainImg.Height / 3 - 1) { nStartY = picMainImg.Height / 3 + 1; }
                if (nTmpStartX == nTmpEndX && nTmpEndY > picMainImg.Height / 3 * 2 - 2) nEndY = picMainImg.Height / 3 * 2 - 2;
                if (nTmpStartX == nTmpEndX && nTmpStartX > picMainImg.Width / 3 * 2 - 2) { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }
                if (nTmpStartX == nTmpEndX && nTmpStartX < picMainImg.Width / 3 - 1) { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }
                if (nTmpStartX == nTmpEndX && nTmpStartY > picMainImg.Height / 3 * 2 - 1) { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }
                if (nTmpStartX == nTmpEndX && nTmpEndY < picMainImg.Height / 3 - 1) { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }
            }
            else if (picMainImg.Width / 2 - 1 >= pntClick.X && picMainImg.Height / 2 - 1 >= pntClick.Y)
            {

                if (picMainImg.Width / 3 - 1 < nStartX && picMainImg.Height / 3 - 1 < nStartY
                    || (picMainImg.Width / 3 - 1 < nEndX && picMainImg.Height / 3 - 1 < nEndY))
                {
                    if (nTmpStartX == nTmpEndX && nTmpStartX > (picMainImg.Width / 3) - 1 & nTmpStartY > picMainImg.Height / 3 - 1)
                    { nStartY = (picMainImg.Height / 3) - 1; }
                    if (nTmpStartX == nTmpEndX && nTmpEndX > (picMainImg.Width / 3) - 1 & nTmpEndY > picMainImg.Height / 3 - 1)
                    { nEndY = (picMainImg.Height / 3) - 1; }
                    if (nTmpStartX == nTmpEndX && nTmpEndX > nWidth)
                    { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }

                    if (nTmpStartY == nTmpEndY && nTmpStartX > (picMainImg.Width / 3) - 1 & nTmpStartY > picMainImg.Height / 3 - 1)
                    { nStartX = (picMainImg.Width / 3) - 1; }
                    if (nTmpStartY == nTmpEndY && nTmpEndX > (picMainImg.Width / 3) - 1 & nTmpEndY > picMainImg.Height / 3 - 1)
                    { nEndX = (picMainImg.Width / 3) - 1; }
                    if (nTmpStartY == nTmpEndY && nTmpEndY > nHeight)
                    { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }

                }
                else
                {
                    if (nTmpStartY == nTmpEndY && nTmpStartX > nWidth) nStartX = nWidth;
                    if (nTmpStartY == nTmpEndY && nTmpEndX > nWidth) nEndX = nWidth;
                    if (nTmpStartY == nTmpEndY && nTmpStartY > nHeight) nStartY = picMainImg.Height + 1;
                    if (nTmpStartY == nTmpEndY && nTmpEndY > nHeight) nEndY = picMainImg.Height + 1;

                    if (nTmpStartX == nTmpEndX && nTmpStartX > nWidth) nStartX = 0;
                    if (nTmpStartX == nTmpEndX && nTmpEndX > nWidth) nEndX = 0;
                    if (nTmpStartX == nTmpEndX && nTmpStartY > nHeight) nStartY = nHeight;
                    if (nTmpStartX == nTmpEndX && nTmpEndY > nHeight) nEndY = nHeight;
                }
            }
            else if (picMainImg.Width / 2 - 1 < pntClick.X && picMainImg.Height / 2 - 1 >= pntClick.Y)
            {
                if (picMainImg.Width / 3 * 2 > nStartX + nWidth - 1 && picMainImg.Height / 3 < nStartY
                    || (picMainImg.Width / 3 * 2 > nEndX + nWidth - 1 && picMainImg.Height / 3 < nEndY))
                {
                    nStartX += nWidth - 1;
                    nEndX += nWidth - 1;
                    int nTmp = 0;
                    if (nStartX > nEndX) { nTmp = nStartX; nStartX = nEndX; nEndX = nTmp; }
                    if (nStartY > nEndY) { nTmp = nStartY; nStartY = nEndY; nEndY = nTmp; }
                    nTmpStartX = nStartX; nTmpStartY = nStartY; nTmpEndX = nEndX; nTmpEndY = nEndY;

                    if (nTmpStartX == nTmpEndX && nTmpEndY > picMainImg.Height / 3 - 1)
                    { nEndY = (picMainImg.Height / 3 - 1); }
                    if (nTmpStartX == nTmpEndX && nTmpStartY > picMainImg.Height / 3 - 1)
                    { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }
                    if (nTmpStartX == nTmpEndX && nTmpStartX <= picMainImg.Width / 2 - 1)
                    { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }

                    if (nTmpStartY == nTmpEndY && nTmpEndX > (picMainImg.Width / 3) * 2 - 1 && nTmpEndY > picMainImg.Height / 3 - 1)
                    { nStartX = (picMainImg.Width / 3) * 2 - 1; }
                    if (nTmpStartY == nTmpEndY && nTmpEndX <= (picMainImg.Width / 3) * 2 - 1 && nTmpEndY >= picMainImg.Height / 3 - 1)
                    { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }
                    if (nTmpStartY == nTmpEndY && nTmpEndY >= nHeight)
                    { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }
                }
                else
                {
                    nStartX += nWidth - 1;
                    nEndX += nWidth - 1;
                    int nTmp = 0;
                    if (nStartX > nEndX) { nTmp = nStartX; nStartX = nEndX; nEndX = nTmp; }
                    if (nStartY > nEndY) { nTmp = nStartY; nStartY = nEndY; nEndY = nTmp; }
                    nTmpStartX = nStartX; nTmpStartY = nStartY; nTmpEndX = nEndX; nTmpEndY = nEndY;

                    if (nTmpStartY == nTmpEndY && nTmpStartX <= nWidth) nStartX = nWidth;
                    if (nTmpStartY == nTmpEndY && nTmpEndX <= nWidth) nEndX = nWidth;
                    if (nTmpStartY == nTmpEndY && nTmpStartY > nHeight) nStartY = picMainImg.Height + 1;
                    if (nTmpStartY == nTmpEndY && nTmpEndY > nHeight) nEndY = picMainImg.Height + 1;

                    if (nTmpStartX == nTmpEndX && nTmpStartX <= nWidth) nStartX = 0;
                    if (nTmpStartX == nTmpEndX && nTmpEndX <= nWidth) nEndX = 0;
                    if (nTmpStartX == nTmpEndX && nTmpStartY > nHeight) nStartY = nHeight;
                    if (nTmpStartX == nTmpEndX && nTmpEndY > nHeight) nEndY = nHeight;
                }


            }
            else if (picMainImg.Width / 2 - 1 >= pntClick.X && picMainImg.Height / 2 - 1 < pntClick.Y)
            {
                if (picMainImg.Width / 3 - 1 < nStartX && picMainImg.Height / 3 * 2 - 1 > nStartY + nHeight - 1
                    || (picMainImg.Width / 3 - 1 < nEndX && picMainImg.Height / 3 * 2 - 1 > nEndY + nHeight - 1))
                {
                    nStartY += nHeight - 1;
                    nEndY += nHeight - 1;

                    int nTmp = 0;
                    if (nStartX > nEndX) { nTmp = nStartX; nStartX = nEndX; nEndX = nTmp; }
                    if (nStartY > nEndY) { nTmp = nStartY; nStartY = nEndY; nEndY = nTmp; }
                    nTmpStartX = nStartX; nTmpStartY = nStartY; nTmpEndX = nEndX; nTmpEndY = nEndY;

                    if (nTmpStartY == nTmpEndY && nTmpEndX > (picMainImg.Width / 3) - 1 && nTmpEndY > picMainImg.Height / 3 - 1)
                    { nEndX = (picMainImg.Width / 3) - 1; }
                    if (nTmpStartY == nTmpEndY && nTmpStartX >= (picMainImg.Width / 3) - 1 && nTmpEndY >= picMainImg.Height / 3 - 1)
                    { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }
                    if (nTmpStartY == nTmpEndY && nTmpStartY <= picMainImg.Height / 2 - 1)
                    { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }

                    if (nTmpStartX == nTmpEndX && nTmpEndY > picMainImg.Height / 3 - 1)
                    { nStartY = (picMainImg.Height / 3 * 2 - 1); }
                    if (nTmpStartX == nTmpEndX && nTmpEndY <= picMainImg.Height / 3 * 2 - 1)
                    { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }
                    if (nTmpStartX == nTmpEndX && nTmpStartX > picMainImg.Width / 2 - 1)
                    { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }
                }
                else
                {
                    nStartY += nHeight - 1;
                    nEndY += nHeight - 1;

                    int nTmp = 0;
                    if (nStartX > nEndX) { nTmp = nStartX; nStartX = nEndX; nEndX = nTmp; }
                    if (nStartY > nEndY) { nTmp = nStartY; nStartY = nEndY; nEndY = nTmp; }
                    nTmpStartX = nStartX; nTmpStartY = nStartY; nTmpEndX = nEndX; nTmpEndY = nEndY;

                    if (nTmpStartY == nTmpEndY && nTmpStartX > nWidth) nStartX = nWidth;
                    if (nTmpStartY == nTmpEndY && nTmpEndX > nWidth) nEndX = nWidth;
                    if (nTmpStartY == nTmpEndY && nTmpStartY <= nHeight) nStartY = picMainImg.Height + 1;
                    if (nTmpStartY == nTmpEndY && nTmpEndY <= nHeight) nEndY = picMainImg.Height + 1;

                    if (nTmpStartX == nTmpEndX && nTmpStartX > nWidth) nStartX = 0;
                    if (nTmpStartX == nTmpEndX && nTmpEndX > nWidth) nEndX = 0;
                    if (nTmpStartX == nTmpEndX && nTmpStartY <= nHeight) nStartY = nHeight;
                    if (nTmpStartX == nTmpEndX && nTmpEndY <= nHeight) nEndY = nHeight;
                }
            }
            else if (picMainImg.Width / 2 - 1 < pntClick.X && picMainImg.Height / 2 - 1 < pntClick.Y)
            {
                if (picMainImg.Width / 3 / 2 - 1 > nEndX && picMainImg.Height / 3 * 2 - 1 > nEndY + nHeight - 1
                    || (picMainImg.Width / 3 / 2 - 1 > nStartX && picMainImg.Height / 3 * 2 - 1 > nStartY + nHeight - 1))
                {
                    nStartX += nWidth - 1;
                    nStartY += nHeight - 1;
                    nEndX += nWidth - 1;
                    nEndY += nHeight - 1;

                    int nTmp = 0;
                    if (nStartX > nEndX) { nTmp = nStartX; nStartX = nEndX; nEndX = nTmp; }
                    if (nStartY > nEndY) { nTmp = nStartY; nStartY = nEndY; nEndY = nTmp; }
                    nTmpStartX = nStartX; nTmpStartY = nStartY; nTmpEndX = nEndX; nTmpEndY = nEndY;

                    if (nTmpStartX == nTmpEndX && nTmpEndY > picMainImg.Height / 3 * 2 - 1)
                    { nStartY = (picMainImg.Height / 3 * 2 - 1); }
                    if (nTmpStartX == nTmpEndX && nTmpEndY <= picMainImg.Height / 3 * 2 - 1)
                    { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }
                    if (nTmpStartX == nTmpEndX && nTmpStartX <= picMainImg.Width / 2 - 1)
                    { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }

                    if (nTmpStartY == nTmpEndY && nStartX < picMainImg.Width / 3 * 2 - 1)
                    { nStartX = (picMainImg.Width / 3) * 2 - 1; }
                    if (nTmpStartY == nTmpEndY && nEndX <= picMainImg.Width / 3 * 2 - 1)
                    { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; }
                    if (nTmpStartY == nTmpEndY && nStartY <= picMainImg.Height / 2 - 1)
                    { nStartX = 0; nEndX = 0; nStartY = 0; nEndY = 0; } //nStartY = picMainImg.Height + 1;

                }
                else
                {
                    nStartX += nWidth - 1;
                    nStartY += nHeight - 1;
                    nEndX += nWidth - 1;
                    nEndY += nHeight - 1;

                    int nTmp = 0;
                    if (nStartX > nEndX) { nTmp = nStartX; nStartX = nEndX; nEndX = nTmp; }
                    if (nStartY > nEndY) { nTmp = nStartY; nStartY = nEndY; nEndY = nTmp; }
                    nTmpStartX = nStartX; nTmpStartY = nStartY; nTmpEndX = nEndX; nTmpEndY = nEndY;

                    if (nTmpStartY == nTmpEndY && nTmpStartX <= nWidth) nStartX = nWidth;
                    if (nTmpStartY == nTmpEndY && nTmpEndX <= nWidth) nEndX = nWidth;
                    if (nTmpStartY == nTmpEndY && nTmpStartY <= nHeight) nStartY = picMainImg.Height + 1;
                    if (nTmpStartY == nTmpEndY && nTmpEndY <= nHeight) nEndY = picMainImg.Height + 1;

                    if (nTmpStartX == nTmpEndX && nTmpStartX <= nWidth) nStartX = 0;
                    if (nTmpStartX == nTmpEndX && nTmpEndX <= nWidth) nEndX = 0;
                    if (nTmpStartX == nTmpEndX && nTmpStartY <= nHeight) nStartY = nHeight;
                    if (nTmpStartX == nTmpEndX && nTmpEndY <= nHeight) nEndY = nHeight;
                }
            }

            return new Rectangle(nStartX, nStartY, nEndX, nEndY);
        }



        /// <summary>
        /// 기  능 : 모드 설정 좌표를 클리어
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic      
        /// </summary>
        public void ClearAllArea()
        {
            for (int nRoiCnt = 0; nRoiCnt < m_DicRect.Count; nRoiCnt++)
            {
                m_DicRect[nRoiCnt].RectList.Clear();
            }

            for (int nPntCnt = 0; nPntCnt < m_DicPnt.Count; nPntCnt++)
            {
                m_DicPnt[nPntCnt].PointList.Clear();
                if (m_DicPnt[nPntCnt].PolygonList == null) continue;
                m_DicPnt[nPntCnt].PolygonList.Clear();
            }

            //picMainImg.Invalidate();
        }

        /// <summary>
        /// 기  능 : 레시피 데이터의 ROI 좌표를 등록
        /// /반환값 : 
        /// /입  력 : int nX, int nY
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void DataToSetRectArea(List<Rectangle[]> AreaList)
        {
            try
            {
                if (m_imgLoaded != null)
                {
                    List<Rectangle> tmpLst = new List<Rectangle>();
                    struRect struRoi;
                    for (int nRoiCnt = 0; nRoiCnt < m_fnGetUseTotRectCount(); nRoiCnt++)
                    {
                        m_DicRect.TryGetValue(nRoiCnt, out struRoi);
                        if (AreaList.Count() >= 1)
                            struRoi.RectList = AreaList[nRoiCnt].OfType<Rectangle>().ToList();
                        else
                            struRoi.RectList = tmpLst;

                        //m_DicRoi[nRoiCnt] = struRoi;
                    }
                }
            }
            catch (Exception ex)
            {
                throw ex;
                //Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
                //Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
            }
        }

        /// <summary>
        /// 기  능  : Dictionary<string, Pen> 형식에서 string 명으로 검색하여 Pen 데이터 반환.
        /// 
        /// /반환값 :  
        /// /입  력 : int nKey, Pen pen
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        private void SetRectPenVal(int nKey, Pen pen)
        {
            struRect struRoi;
            if (m_DicRect.TryGetValue(nKey, out struRoi))
            {
                struRoi.pen = pen;
                m_DicRect[nKey] = struRoi;
            }
        }
        /// <summary>
        /// 기  능  : 해당 키의 Rectangle 값을 셋팅
        /// 
        /// /반환값 :  
        /// /입  력 : int nKey, Rectangle stRoiCoord
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        private void SetRectCoord(int nKey, Rectangle stRoiCoord)
        {
            struRect struRoi;
            if (m_DicRect.TryGetValue(nKey, out struRoi))
            {
                struRoi.stRectCoord = stRoiCoord;
                m_DicRect[nKey] = struRoi;
            }
        }

        //KYH 2021.11.01
        private void SetFocusRectCoord(int nKey, Rectangle stRoiCoord)
        {
            struRect strufocusRoi;
            if (m_DicFocusROI.TryGetValue(nKey, out strufocusRoi))
            {
                strufocusRoi.stRectCoord = stRoiCoord;
                m_DicFocusROI[nKey] = strufocusRoi;
            }
        }

        /// <summary>
        /// 기  능  : 해당 키의 List<Rectangle> 값을 셋팅
        /// 
        /// /반환값 :  
        /// /입  력 : int nKey, List<Rectangle> AreaList
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        private void SetRectListVal(int nKey, List<Rectangle> AreaList)
        {
            struRect struRoi;
            if (m_DicRect.TryGetValue(nKey, out struRoi))
            {
                struRoi.RectList = AreaList;
                m_DicRect[nKey] = struRoi;
            }
        }
        /// <summary>
        /// 기  능  : 해당 키의 Rect 값을 출력 리스트에 추가
        /// 
        /// /반환값 :  
        /// /입  력 : int nKey, Rectangle rect
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        private void SetAddRectVal(int nKey, Rectangle rect)
        {
            // List<Rectangle> AreaList = new List<Rectangle>();
            //AreaList.Add(rect);
            struRect struRoi;
            if (m_DicRect.TryGetValue(nKey, out struRoi))
            {
                struRoi.RectList.Add(rect);// = AreaList;
                m_DicRect[nKey] = struRoi;
            }
        }
        /// <summary>
        /// 기  능  : 해당 키의 사용 유무 셋팅.
        /// 
        /// /반환값 :  
        /// /입  력 : int nKey, bool bUseRectView
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        private void SetUseRectView(int nKey, bool bUseRectView)
        {
            struRect struRoi;
            if (m_DicRect.TryGetValue(nKey, out struRoi))
            {
                struRoi.bUseRectView = bUseRectView;
                m_DicRect[nKey] = struRoi;
            }
        }

        private void SetUseFocusView(int nKey, bool bUseRectView)
        {
            struRect strufocusRoi;
            if (m_DicFocusROI.TryGetValue(nKey, out strufocusRoi))
            {
                strufocusRoi.bUseRectView = bUseRectView;
                m_DicFocusROI[nKey] = strufocusRoi;
            }
        }

        /// 기  능  : 해당 키의 십자 마크 사용 유무를 셋팅
        /// 
        /// /반환값 :  
        /// /입  력 : int nKey, bool bCross
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>   
        private void SetViewKind(int nKey, int nViewKind)
        {
            struPoint struPnt;
            if (m_DicPnt.TryGetValue(nKey, out struPnt))
            {
                struPnt.nViewKind = nViewKind;
                m_DicPnt[nKey] = struPnt;
            }
        }
        /// <summary>
        /// 기  능  : Dictionary<string, Pen> 형식에서 string 명으로 검색하여 Pen 데이터 반환.
        /// 
        /// /반환값 :  
        /// /입  력 : int nKey, Pen pen
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        private void SetPntPenVal(int nKey, Pen pen)
        {
            struPoint struPnt;
            if (m_DicPnt.TryGetValue(nKey, out struPnt))
            {
                struPnt.pen = pen;
                m_DicPnt[nKey] = struPnt;
            }
        }
        /// <summary>
        /// 기  능  : 해당 키의 Point 값을 셋팅
        /// 
        /// /반환값 :  
        /// /입  력 : int nKey, Point stRoiCoord
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        private void SetPntCoord(int nKey, Point stCoord, int nPolygonNo = 0)
        {
            struPoint struPnt;
            if (m_DicPnt.TryGetValue(nKey, out struPnt))
            {
                struPnt.stPointCoord = stCoord;
                m_DicPnt[nKey] = struPnt;
            }
        }

        /// <summary>
        /// 기  능  : 해당 키의 List<Point> 값을 셋팅
        /// 
        /// /반환값 :  
        /// /입  력 : int nKey, List<Point> AreaList
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        private void SetPntListVal(int nKey, List<Point> AreaList)
        {
            struPoint struPnt;
            if (m_DicPnt.TryGetValue(nKey, out struPnt))
            {
                struPnt.PointList = AreaList;
                m_DicPnt[nKey] = struPnt;
            }
        }

        /// <summary>
        /// 기  능  : 해당 키의 Polygon Roi 값을 셋팅
        /// 
        /// /반환값 :  
        /// /입  력 : int nKey, List<Point> AreaList
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary> 
        private void SetPolygonListVal(int nKey, Dictionary<int, List<Point>> PolygonList, int nCurPolygonKey)
        {
            struPoint struPnt;
            if (m_DicPnt.TryGetValue(nKey, out struPnt))
            {
                struPnt.nCurPolygonKey = nCurPolygonKey;
                struPnt.PolygonList = PolygonList;
                m_DicPnt[nKey] = struPnt;
            }
        }

        /// <summary>
        /// 기  능  : 해당 키의 Point 값을 출력 리스트에 추가
        /// 
        /// /반환값 :  
        /// /입  력 : int nKey, Point pnt
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        private void SetAddPntVal(int nKey, Point pnt)
        {
            //List<Point> AreaList = new List<Point>();
            //AreaList.Add(pnt);
            struPoint struPnt;
            if (m_DicPnt.TryGetValue(nKey, out struPnt))
            {
                struPnt.PointList.Add(pnt);// = AreaList;
                m_DicPnt[nKey] = struPnt;
            }
        }
        /// <summary>
        /// 기  능  : 해당 키의 사용 유무 셋팅.
        /// 
        /// /반환값 :  
        /// /입  력 : int nKey, bool bUseRectView
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        private void SetUsePntView(int nKey, bool bUsePointView)
        {
            struPoint struPnt;
            if (m_DicPnt.TryGetValue(nKey, out struPnt))
            {
                struPnt.bUsePointView = bUsePointView;
                m_DicPnt[nKey] = struPnt;
            }
        }

        /// <summary>
        /// 기  능  : 해당 키의 다각형 완료 유무.
        /// 
        /// /반환값 :  
        /// /입  력 : int nKey, bool bUseRectView
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        private void SetEndPolygon(int nKey, bool bIsEndPolygon)
        {
            struPoint struPnt;
            if (m_DicPnt.TryGetValue(nKey, out struPnt))
            {
                struPnt.bEndPolygon = bIsEndPolygon;
                m_DicPnt[nKey] = struPnt;
            }
        }

        /// <summary>
        /// 기  능  : 다각형을 사용 중인 포인트 구조체 개수
        /// 
        /// /반환값 : int  
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        public int m_fnGetUsePolygonCount()
        {
            int nRetCount = 0;
            for (int nKey = 0; nKey < m_DicPnt.Count; nKey++)
            {
                if (m_DicPnt.ContainsKey(nKey))
                {
                    if (m_DicPnt[nKey].GetViewKind() == (int)Enums.E_PNT_VIEW_KIND.POLYGON) nRetCount++;
                }
            }
            return nRetCount;
        }

        /// <summary>
        /// 기  능  : 다각형을 사용 중인 포인트 구조체 개수
        /// 
        /// /반환값 : int  
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        public int m_fnGetUsePolygonKey(int nIndex)
        {
            int nRetKey = 0;
            int nCnt = 0;
            for (int nKey = 0; nKey < m_DicPnt.Count; nKey++)
            {
                if (m_DicPnt.ContainsKey(nKey))
                {
                    if (m_DicPnt[nKey].GetViewKind() == (int)Enums.E_PNT_VIEW_KIND.POLYGON)
                    {
                        nRetKey = nKey;
                        nCnt++;
                    }
                    if (nCnt > nIndex) break;
                }
            }
            return nRetKey;
        }

        /// <summary>
        /// 기  능 : 레시피 데이터의 ROI 좌표를 등록
        /// /반환값 : 
        /// /입  력 : int nKey, List<Rectangle> AreaList
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void DataToSetRectArea(int nKey, List<Rectangle> AreaList)
        {
            try
            {
                if (m_imgLoaded != null)
                {
                    SetRectListVal(nKey, AreaList);

                    picMainImg.Invalidate();
                }
            }
            catch (Exception ex)
            {
                throw ex;
                //Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
                //Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
            }
        }
        /// <summary>
        /// 기  능 : 레시피 데이터의 ROI 좌표를 등록
        /// /반환값 : 
        /// /입  력 : Dictionary<int, List<Rectangle>> AreaList
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void DataToSetRectArea(Dictionary<int, List<Rectangle>> AreaList)
        {
            try
            {
                if (m_imgLoaded != null)
                {
                    for (int nRoiCnt = 0; nRoiCnt < AreaList.Count; nRoiCnt++)
                        SetRectListVal(nRoiCnt, AreaList[nRoiCnt]);
                    picMainImg.Invalidate();
                }
            }
            catch (Exception ex)
            {
                throw ex;
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
                //Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
            }
        }


        /// <summary>
        /// 기  능 : 레시피 데이터의 Polygon 좌표를 등록
        /// /반환값 : 
        /// /입  력 : int nKey, List<Point> AreaList
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void DataToSetPolygonArea(int nKey, Dictionary<int, List<Point>> dPolygonList, int nCurPolygonKey)
        {
            try
            {
                if (m_imgLoaded != null)
                {
                    SetPolygonListVal(nKey, dPolygonList, nCurPolygonKey);
                    picMainImg.Invalidate();
                }
            }
            catch (Exception ex)
            {
                throw ex;
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
                //Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
            }
        }

        /// <summary>
        /// 기  능 : 레시피 데이터의 Point 좌표를 등록
        /// /반환값 : 
        /// /입  력 : int nKey, List<Point> AreaList
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void DataToSetPointArea(int nKey, List<Point> AreaList)
        {
            try
            {
                if (m_imgLoaded != null)
                {
                    SetPntListVal(nKey, AreaList);
                    picMainImg.Invalidate();
                }
            }
            catch (Exception ex)
            {
                throw ex;
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
                //Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
            }
        }
        /// <summary>
        /// 기  능 : 레시피 데이터의 Point 좌표를 등록
        /// /반환값 : 
        /// /입  력 : Dictionary<int, List<Point>> AreaList
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void DataToSetPointArea(Dictionary<int, List<Point>> AreaList)
        {
            try
            {
                if (m_imgLoaded != null)
                {
                    for (int nRoiCnt = 0; nRoiCnt < AreaList.Count; nRoiCnt++)
                    {
                        SetPntListVal(nRoiCnt, AreaList[nRoiCnt]);
                    }
                    picMainImg.Invalidate();
                }
            }
            catch (Exception ex)
            {
                throw ex;
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
                //Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
            }
        }

        /// <summary>
        /// 기  능 : Pad 좌표를 등록하고 이미지를 저장한다
        /// /반환값 : 
        /// /입  력 : Rectangle PadList, string strSavePath, bool bUseSaveImg
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void m_fnRectAreaImageSave(int nKey, Rectangle rect, string strSavePath = "", bool bUseSaveImg = false)
        {
            try
            {
                if (m_imgLoaded == null) return;

                //SetRectRectVal(nKey, rect);
                Pen pen = m_DicRect[nKey].GetPenVal();
                pen.DashStyle = System.Drawing.Drawing2D.DashStyle.Solid;
                SetRectPenVal(nKey, pen);

                if (bUseSaveImg && !rect.IsEmpty)
                {
                    ImageSave(strSavePath, rect.X, rect.Y, rect.Width, rect.Height);
                }
                picMainImg.Invalidate();

            }
            catch (Exception ex)
            {
                throw ex;
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
                //Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
            }
        }
        /// <summary>
        /// 기  능 : Pad 좌표를 등록하고 이미지를 저장한다
        /// /반환값 : 
        /// /입  력 : Rectangle PadList, string strSavePath, bool bUseSaveImg
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void DataToSetGuideLineArea(Rectangle rectGuideLine, double nGuideLineOffsetRotate)
        {
            try
            {
                m_dGuideLineOffsetT = nGuideLineOffsetRotate;
                m_GuideLineRect = rectGuideLine;

                if (m_imgLoaded == null) return;
                picMainImg.Invalidate();
            }
            catch (Exception ex)
            {
                throw ex;
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
                //Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
            }
        }

        public void DataToSetGuideLineArea2(Rectangle[] rectGuideLine, Rectangle rectGuideLine2, double nGuideLineOffsetRotate)
        {
            try
            {
                m_dGuideLineOffsetT = nGuideLineOffsetRotate;
                m_GuideLineRect2 = rectGuideLine;
                m_GuideLineCenter = rectGuideLine2;
                if (m_imgLoaded == null) return;
                picMainImg.Invalidate();
            }
            catch (Exception ex)
            {
                throw ex;
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
                //Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
            }
        }

        public void DataToSetGuideLineArea3(Rectangle[] rectGuideLine, double nGuideLineOffsetRotate)
        {
            try
            {
                m_dGuideLineOffsetT = nGuideLineOffsetRotate;
                m_GuideLineRect3 = rectGuideLine;
                if (m_imgLoaded == null) return;
                picMainImg.Invalidate();
            }
            catch (Exception ex)
            {
                throw ex;
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
                //Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
            }
        }

        /// <summary>
        /// 기  능 : 현재 라이브 영상을 bitmap 이미지로 저장한다.
        /// /반환값 : 
        /// /입  력 : string strSavePath
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public int ImageSave(string strSavePath, int nX = 0, int nY = 0, int nCropX = 0, int nCropY = 0)
        {
            int nRet = Consts.FALSE;
            try
            {
                if (nCropX == 0) nCropX = m_PixelSize.X;
                if (nCropY == 0) nCropY = m_PixelSize.Y;
                int nPixelOffSet = m_fnGetPxlFormatIndex();

                m_fnImgThreadSuspend();

                CmdMsgParam Param = new CmdMsgParam();
                Param.SetInteger(m_fnGetImageID());
                Param.SetInteger(Convert.ToInt32(GetLiveFlg()));         //int   : 카메라 영상 1 , BMP 이미지 호출 0
                //Param.SetInteger(Convert.ToInt32(m_bIsGrayScale));       //int   : RGB 0 , RAW 1
                Param.SetUInteger(Convert.ToUInt32(Math.Abs(nX)));       //ULONG : X Offset	Crop Image 시작 X
                Param.SetUInteger(Convert.ToUInt32(Math.Abs(nY)));       //ULONG : Y Offset	Crop Image 시작 Y
                Param.SetUInteger(Convert.ToUInt32(nCropX));   //ULONG : X Crop Size	Crop Image X Size
                Param.SetUInteger(Convert.ToUInt32(nCropY));  //ULONG : Y Crop Size	Crop Image Y Size
                Param.SetChars(strSavePath.toUniCharAry(500));
                nRet = Systems.g_Ipc.SendCommand((ushort)((m_fnGetVisionNo() + 1) * 100 + IpcConst.CAMERA_TASK + m_nCamIndex), IpcConst.CAMERA_FUNC, IpcConst.IMAGE_SAVE_REQ,
                                                    IpcInterface.CMD_TYPE_RES, IpcInterface.CMD_TIMEOUT, Param.GetByteSize(), Param.GetParam(), (int)Enums.CompressLevel.NONE);


                m_fnImgThreadResume();
                picMainImg.Invalidate();

                nRet = Consts.TRUE;
            }
            catch (Exception)
            {
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
                //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E008", Globals.nLanguageFlg), true);
                nRet = Consts.FALSE;
            }

            return nRet;
        }

        private ImageCodecInfo GetEncoder(ImageFormat format)
        {

            ImageCodecInfo[] codecs = ImageCodecInfo.GetImageDecoders();

            foreach (ImageCodecInfo codec in codecs)
            {
                if (codec.FormatID == format.Guid)
                {
                    return codec;
                }
            }
            return null;
        }

        /// <summary>
        /// 기  능  : 이미지 수신 스레드를 일시 정지 한다.
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void m_fnImgThreadSuspend()
        {
            try
            {
                SetThreadWork(false);
                Thread.Sleep(500);
                //                 if (m_thread.IsAlive)
                //                     m_thread.Suspend();
            }
            catch (Exception ex)
            {
                throw ex;
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
            }
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
                throw ex;
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
            }
        }
        /// <summary>
        /// 기  능  : 이미지 수신 스레드를 종료 한다.
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void m_fnEndImageLoadThread()
        {
            try
            {
                SetImageLoadThreadFlg(false);
                SetThreadWork(false);

                m_fnWaitImgThreadEnd();

                if (m_bUseMiniMap)
                {
                    m_ucPicMiniMap.SetimgThreadEnd(false);
                    m_ucPicMiniMap.m_fnWaitImgThreadEnd();
                }
            }
            catch (Exception ex)
            {
                throw ex;
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
            }
        }
        /// <summary>
        /// 기  능 : 이미지 수신 스레드를 다시 시작 한다.
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void m_fnImgThreadResume()
        {
            try
            {
                //m_thread.Resume();

                SetThreadWork(true);
            }
            catch (Exception ex)
            {
                throw ex;
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
            }
        }

        /// <summary>
        /// 기  능 : 현재 라이브 영상을 bitmap 이미지로 변화하여 화면에 송출한다.
        /// /반환값 : 
        /// /입  력 : string strSavePath
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        private Bitmap m_fnLiveImageGrab(int nX, int nY, int nCropX, int nCropY, int nPicWidth, int nPicHeight)
        {
            int nRet = Consts.FALSE;
            // Bitmap bitmapRet = null;

            try
            {
                int nPixelOffSet = m_fnGetPxlFormatIndex();

                CmdMsgParam Param = new CmdMsgParam();

                Byte[] dat = new Byte[nPicWidth * nPicHeight * nPixelOffSet];//12 bit 면 * 2
                Param.SetParam(dat);
                Param.SetInteger(m_fnGetImageID());
                Param.SetInteger(Convert.ToInt32(GetLiveFlg()));        //int   : 카메라 영상 1 , BMP 이미지 호출 0               
                Param.SetUInteger(Convert.ToUInt32(Math.Abs(nX)));      //ULONG : X Offset	Crop Image 시작 X
                Param.SetUInteger(Convert.ToUInt32(Math.Abs(nY)));      //ULONG : Y Offset	Crop Image 시작 Y
                Param.SetUInteger(Convert.ToUInt32(nCropX));            //ULONG : X Crop Size	Crop Image X Size
                Param.SetUInteger(Convert.ToUInt32(nCropY));            //ULONG : Y Crop Size	Crop Image Y Size
                Param.SetUInteger(Convert.ToUInt32(nPicWidth));         //ULONG : X Window Size	반환받을 Byte[] X Size
                Param.SetUInteger(Convert.ToUInt32(nPicHeight));        //ULONG : Y Window Size	반환받을 Byte[] Y Size

                DateTime dateTime = DateTime.Now;


                nRet = Systems.g_Ipc.SendCommand((ushort)((m_fnGetVisionNo() + 1) * 100 + IpcConst.CAMERA_TASK + m_nCamIndex), IpcConst.CAMERA_FUNC, IpcConst.LIVE_REQ,
                                                    IpcInterface.CMD_TYPE_RES, 10000, Param.GetByteSize(), Param.GetParam(), (int)Enums.CompressLevel.NONE, 1000);

                //System.Diagnostics.Debug.WriteLine(string.Format ("{0}", (DateTime.Now - dateTime)));
                if (nRet == Consts.APP_OK && nPicWidth != 0 && nPicHeight != 0)
                    return (Bitmap)Utils.ImageProc.ConvertBitmap(Param.GetParam(), nPicWidth, nPicHeight, m_bIsGrayScale, m_pxlForamt);
                else
                    return m_imgLoaded;

            }
            catch (Exception ex)
            {
                throw ex;
                //Systems.m_fnAddLog(m_fnGetVisionNo(), ex.StackTrace, ex.Message);
                return m_imgLoaded;
            }
            //            
            //             return bitmapRet;
        }

        /// <summary>
        /// 기  능 : 이미지 비트맵을 반환한다.
        /// /반환값 : Bitmap
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public Bitmap m_fnGetBitmap()
        {
            m_fnImgThreadSuspend();
            Bitmap bitImg = m_fnLiveImageGrab(0, 0, m_PixelSize.X, m_PixelSize.Y, m_PixelSize.X, m_PixelSize.Y);

            //bitImg.Save("D:\\Align.bmp", System.Drawing.Imaging.ImageFormat.Bmp);
            m_fnImgThreadResume();
            return bitImg;
            //return m_imgLoaded;
        }
        public byte[] m_fnGetImageBuffer()
        {
            m_fnImgThreadSuspend();

            int nPixelOffSet = m_fnGetPxlFormatIndex();

            CmdMsgParam Param = new CmdMsgParam();
            Byte[] dat = new Byte[m_PixelSize.X * m_PixelSize.Y * nPixelOffSet];
            Param.SetParam(dat);

            Param.SetInteger(0);                                     //int   : 카메라 영상 1 , BMP 이미지 호출 0
            Param.SetUInteger(0);       //ULONG : X Offset	Crop Image 시작 X
            Param.SetUInteger(0);       //ULONG : Y Offset	Crop Image 시작 Y
            Param.SetUInteger(Convert.ToUInt32(m_PixelSize.X));   //ULONG : X Crop Size	Crop Image X Size
            Param.SetUInteger(Convert.ToUInt32(m_PixelSize.Y));  //ULONG : Y Crop Size	Crop Image Y Size
            Param.SetUInteger(Convert.ToUInt32(m_PixelSize.X));   //ULONG : X Window Size	반환받을 Byte[] X Size
            Param.SetUInteger(Convert.ToUInt32(m_PixelSize.Y));  //ULONG : Y Window Size	반환받을 Byte[] Y Size

            int nRet = Systems.g_Ipc.SendCommand((ushort)((m_fnGetVisionNo() + 1) * 100 + IpcConst.CAMERA_TASK + m_nCamIndex), IpcConst.CAMERA_FUNC, IpcConst.LIVE_REQ,
                                                IpcInterface.CMD_TYPE_RES, IpcInterface.CMD_TIMEOUT, Param.GetByteSize(), Param.GetParam());

            byte[] bRet = Param.GetParam();

            m_fnImgThreadResume();

            return bRet;

        }
        // <summary>
        /// 기  능 : 분할 모드에서의 이미지 비트맵을 반환한다.
        /// /반환값 : Bitmap
        /// /입  력 : nPos
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public Bitmap m_fnGetDivBitmap(int nPos)
        {
            return m_DivLoad.m_imgDivLoaded[nPos];
        }

        public Bitmap cropAtRect(Bitmap b, Rectangle r)
        {
            using (Bitmap nb = new Bitmap(r.Width, r.Height))
            using (Graphics g = Graphics.FromImage(nb))
            {
                Rectangle recLTDiv = new Rectangle(0, 0, r.Width, r.Height);
                Rectangle recLTDiv2 = new Rectangle(0, 0, 100, 100);
                g.DrawImage(m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_LEFT_TOP], recLTDiv, recLTDiv2, GraphicsUnit.Pixel);
                return nb;
            }
        }

        // <summary>
        /// 기  능 : 분할 모드에서의 Rectangle을 반환한다.
        /// /반환값 : Rectangle
        /// /입  력 : nPos
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public Rectangle m_fnGetDivRect(int nPos)
        {
            return m_DivLoad.m_imgRect[nPos];
        }

        public Rectangle m_fnGetFocusDivRect(int nPos)
        {
            return m_GuideLineRect2[nPos];
        }
        /// <summary>
        /// 기  능 : 이미지 비트맵을 현재 이미지로 설정한다.
        /// /반환값 : Bitmap
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void m_fnSetBitmap(string strImgPath, double dAngle = 0.0F)
        {
            //m_imgLoaded = null;
            // string strImgPath = Paths.DEFAULT_DRIVE + "ResponseImage.bmp";
            // image.Save(strImgPath, System.Drawing.Imaging.ImageFormat.Bmp);          
            setRotateAngle(dAngle);
            LoadImage(strImgPath);
            picMainImg.Invalidate();
        }

        private void setRotateAngle(double dAngle) { m_setRotateAngle = dAngle; }
        private double GetRotateAngle() { return m_setRotateAngle; }

        /// <summary>
        /// 기  능 : 출력 화면의 크기를 반환한다.
        /// /반환값 : Point
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public Point m_fnGetPictureBoxSize()
        {
            Point pnt = new Point(picMainImg.Width, picMainImg.Height);
            return pnt;
        }

        /// <summary>
        /// 기  능 : 화면에 표시되고 있는 영상 부분의 사이즈를 반환한다.
        /// /반환값 : Point
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public Point m_fnGetViewCropImgSize()
        {
            Point retPnt = new Point(m_imgRect.Width, m_imgRect.Height);
            return retPnt;
        }

        /// <summary>
        /// 기  능 : 이미지의 사이즈를 반환한다.
        /// /반환값 : Point
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public Point m_fnGetImgSize()
        {
            Point retPnt = new Point(m_imgRect.Width, m_imgRect.Height);
            return m_PixelSize;
        }

        /// <summary>
        /// 기  능 : Picture Box의 사이즈를 변경한다.
        /// /반환값 : 
        /// /입  력 : int nWidth, int nHeight
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void m_fnSetImgSize(int nImgWidth, int nImgHeight, int nPicWidth, int nPicHeight)
        {
            m_imgRect.Width = nImgWidth;
            m_imgRect.Height = nImgHeight;
            picMainImg.Width = nPicWidth;
            picMainImg.Height = nPicHeight;

        }
        /// <summary>
        /// 기  능  : 출력되는 영상의 위치를 변경한다.
        /// /반환값 : 
        /// /입  력 : int nWidth, int nHeight
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void m_fnSetImgPosition(int nX, int nY)
        {
            if (m_imgRect.Width + nX > m_PixelSize.X)
                nX = m_PixelSize.X - m_imgRect.Width;
            if (nX < 0) nX = 0;
            if (m_imgRect.Height + nY > m_PixelSize.Y)
                nY = m_PixelSize.Y - m_imgRect.Height;
            if (nY < 0) nY = 0;

            m_imgRect.X = nX;
            m_imgRect.Y = nY;
        }
        /// <summary>
        /// 기  능 : 현재 영상의 배율을 설정한다.
        /// /반환값 : double ratio
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void m_fnSetRatio(double dRatio)
        {
            m_dRratio = dRatio;
            m_lbNowRatio.Text = string.Format("{0:00.00}", m_dRratio);
            //picMainImg.Invalidate();
            m_fnSetZoomInOut(0, 0, 0);
        }
        /// <summary>
        /// 기  능 : 현재 영상의 배율을 반환한다.
        /// /반환값 : double ratio
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public double m_fnGetRatio()
        {
            return m_dRratio;
        }
        /// <summary>
        /// 기  능 : 화면 불할 모드 유무 설정.
        /// /반환값 : 
        /// /입  력 : bool bDivisionModeFlg
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void m_fnUseDivisionMode(bool bDivisionModeFlg, Rectangle recOffSet,bool bFoucs)
        {
            if (m_DivLoad.m_imgRect == null) return;
            m_bDivisionModeFlg = bDivisionModeFlg;
            m_bunfocus = !bDivisionModeFlg;
            if (bFoucs)
            {
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_TOP].X = m_GuideLineRect2[0].X;
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_TOP].Y = m_GuideLineRect2[0].Y;
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_TOP].X = m_GuideLineRect2[2].X/* + m_GuideLineRect2[2].Width*/;
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_TOP].Y = m_GuideLineRect2[2].Y;
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_BOTTOM].X = m_GuideLineRect2[1].X;
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_BOTTOM].Y = m_GuideLineRect2[1].Y;
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_BOTTOM].X = m_GuideLineRect2[3].X;
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_BOTTOM].Y = m_GuideLineRect2[3].Y;
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_CENTER].X = m_GuideLineRect2[4].X;
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_CENTER].Y = m_GuideLineRect2[4].Y;
                m_nDivEndX = picMainImg.Width / 2 / 4 * 4;
                m_nDivEndY = picMainImg.Height / 2 / 4 * 4;
      
                //m_GuideLineRect2[0].X = m_GuideLineRect2[0].X - (recOffSet.X);
                //m_GuideLineRect2[0].Y = m_GuideLineRect2[0].Y - (recOffSet.Y);

            }
            else
            {
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_TOP].X = m_GuideLineRect.X - (recOffSet.X);
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_TOP].Y = m_GuideLineRect.Y - (recOffSet.Y);
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_TOP].X = m_GuideLineRect.X + m_GuideLineRect.Width - (recOffSet.Width);
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_TOP].Y = m_GuideLineRect.Y - (recOffSet.Y);
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_BOTTOM].X = m_GuideLineRect.X - (recOffSet.X);
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_BOTTOM].Y = m_GuideLineRect.Y + m_GuideLineRect.Height - (recOffSet.Height);
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_BOTTOM].X = m_GuideLineRect.X + m_GuideLineRect.Width - (recOffSet.Width);
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_BOTTOM].Y = m_GuideLineRect.Y + m_GuideLineRect.Height - (recOffSet.Height);
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_CENTER].X = m_GuideLineRect.X + m_GuideLineRect.Width / 2 - (100);
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_CENTER].Y = m_GuideLineRect.Y + m_GuideLineRect.Height / 2 - (100);
                m_nDivEndX = picMainImg.Width / 2 / 4 * 4;
                m_nDivEndY = picMainImg.Height / 2 / 4 * 4;
            }
         
        }

        private Point transformation(int nX, int nY,
                                    int px, int py,
                                    int rad)
        {
            Point retPnt = new Point();
            retPnt.X = (int)Math.Round((px - nX) * Math.Cos(rad) - (py - nY) * Math.Sin(rad) + nX);
            retPnt.Y = (int)Math.Round((px - nX) * Math.Sin(rad) + (py - nY) * Math.Cos(rad) + nY);
            return retPnt;


        }

        private Rectangle GetRectCurDiv(int nCurX, int nCurY)
        {

            if ((picMainImg.Width / 3 - 1 < nCurX && picMainImg.Height / 3 - 1 < nCurY) && (picMainImg.Width / 3 * 2 - 1 >= nCurX && picMainImg.Height / 3 * 2 - 1 >= nCurY))
            {
                return m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_CENTER];
            }
            else if (picMainImg.Width / 2 - 1 >= nCurX && picMainImg.Height / 2 - 1 >= nCurY)
            {
                return m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_TOP];
            }
            else if (picMainImg.Width / 2 - 1 < nCurX && picMainImg.Height / 2 - 1 >= nCurY)
            {
                return m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_TOP];
            }
            else if (picMainImg.Width / 2 - 1 >= nCurX && picMainImg.Height / 2 - 1 < nCurY)
            {
                return m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_BOTTOM];
            }
            else if (picMainImg.Width / 2 - 1 < nCurX && picMainImg.Height / 2 - 1 < nCurY)
            {
                return m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_BOTTOM];
            }
            else return new Rectangle();
        }

        private void SetRectCurDiv(Rectangle rect, int nCurX, int nCurY)
        {

            if ((picMainImg.Width / 3 - 1 < nCurX && picMainImg.Height / 3 - 1 < nCurY)
            && (picMainImg.Width / 3 * 2 - 1 >= nCurX && picMainImg.Height / 3 * 2 - 1 >= nCurY))
            {
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_CENTER] = rect;
            }
            else if (picMainImg.Width / 2 >= nCurX && picMainImg.Height / 2 >= nCurY)
            {
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_TOP] = rect;
            }
            else if (picMainImg.Width / 2 < nCurX && picMainImg.Height / 2 >= nCurY)
            {
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_TOP] = rect;
            }
            else if (picMainImg.Width / 2 >= nCurX && picMainImg.Height / 2 < nCurY)
            {
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_BOTTOM] = rect;
            }
            else if (picMainImg.Width / 2 < nCurX && picMainImg.Height / 2 < nCurY)
            {
                m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_BOTTOM] = rect;
            }

        }


        private void SetRectROI(Rectangle[] rect, int nCurX, int nCurY, bool Liveflag, double m_dRratio)
        {

            if (m_bCenter && nCurY > (m_GuideLineCenter.Y + m_GuideLineCenter.Height - 500) && nCurX > m_GuideLineCenter.X - 600 && m_GuideLineCenter.X + 500 > nCurX && (m_GuideLineCenter.X + m_GuideLineCenter.Width + 800) > nCurX && m_GuideLineCenter.Y - 500 < nCurY && (m_GuideLineCenter.Y + m_GuideLineCenter.Height + 1000) > nCurY)
            {
                m_GuideLineRect2[(int)E_ROI_NUM.R_CENTER].X = nCurX - 100;

                if (!Liveflag)
                    m_GuideLineRect2[(int)E_ROI_NUM.R_CENTER].Y = nCurY - (int)(600 / m_dRratio);
                else
                m_GuideLineRect2[(int)E_ROI_NUM.R_CENTER].Y = nCurY/* - (int)(600 / m_dRratio)*/;
            }

           // }
            else if (m_bL_T/* && !m_bCenter && m_GuideLineRect.Width / 2 >= m_pointROIRIGHT_MOVE_Click.X - m_GuideLineRect.X && m_GuideLineRect.Height / 2 >= m_pointROIRIGHT_MOVE_Click.Y - m_GuideLineRect.Y - 150*/)
            {

                m_GuideLineRect2[(int)E_ROI_NUM.R_LEFT_TOP].X = nCurX - 100;

                if (!Liveflag)
                    m_GuideLineRect2[(int)E_ROI_NUM.R_LEFT_TOP].Y = nCurY - (int)(350 / m_dRratio);
                else
                    m_GuideLineRect2[(int)E_ROI_NUM.R_LEFT_TOP].Y = nCurY;

            }
            else if (m_bL_B/* && !m_bCenter && m_GuideLineRect.Width / 2 >= m_pointROIRIGHT_MOVE_Click.X - m_GuideLineRect.X && m_GuideLineRect.Height / 2 <= m_pointROIRIGHT_MOVE_Click.Y - m_GuideLineRect.Y - 300*/)
            {

                m_GuideLineRect2[(int)E_ROI_NUM.R_LEFT_BOTTOM].X = nCurX - 100;

                if (!Liveflag)
                    m_GuideLineRect2[(int)E_ROI_NUM.R_LEFT_BOTTOM].Y = nCurY - (int)(800 / m_dRratio);
                else
                    m_GuideLineRect2[(int)E_ROI_NUM.R_LEFT_BOTTOM].Y = nCurY + 250;
            }
            else if (m_bR_T/*&& !m_bCenter && m_GuideLineRect.Width / 2 <= m_pointROIRIGHTClick.X && m_GuideLineRect.Height / 2 >= m_pointROIRIGHT_MOVE_Click.Y - m_GuideLineRect.Y - 150/*m_GuideLineRect.Height / 2 >= m_pointROIRIGHTClick.Y*//* - m_GuideLineRect.Y**/)
            {
                m_GuideLineRect2[(int)E_ROI_NUM.R_RIGHT_TOP].X = nCurX - 100;

                if (!Liveflag)
                    m_GuideLineRect2[(int)E_ROI_NUM.R_RIGHT_TOP].Y = nCurY - (int)(350 / m_dRratio);
                else
                m_GuideLineRect2[(int)E_ROI_NUM.R_RIGHT_TOP].Y = nCurY /*- (int)(350 / m_dRratio)*/;
            }
            else if (m_bR_B/* &&!m_bCenter && m_GuideLineRect.Width / 2 <= m_pointROIRIGHTClick.X && m_GuideLineRect.Height / 2 <= m_pointROIRIGHT_MOVE_Click.Y - m_GuideLineRect.Y - 500*/)
            {
                m_GuideLineRect2[(int)E_ROI_NUM.R_RIGHT_BOTTOM].X = nCurX - 100;

                if (!Liveflag)
                    m_GuideLineRect2[(int)E_ROI_NUM.R_RIGHT_BOTTOM].Y = nCurY - (int)(800 / m_dRratio);
                else
                    m_GuideLineRect2[(int)E_ROI_NUM.R_RIGHT_BOTTOM].Y = nCurY + 250;
            }

        }

        private void SetRectFocusDiv(Rectangle rect, int nCurX, int nCurY, Point abc, double m_dRratio)
        {
            bool bTEST = false;

            if ((m_GuideLineRect.Width / 3 - 1 < m_pointROIRIGHT_MOVE_Click.X && m_GuideLineRect.Height / 3 - 1 < m_pointROIRIGHT_MOVE_Click.Y)
            && (m_GuideLineRect.Width / 3 * 2 - 1 >= m_pointROIRIGHT_MOVE_Click.X && m_GuideLineRect.Height / 3 * 2 - 1 >= m_pointROIRIGHT_MOVE_Click.Y - 800))
            {
                m_GuideLineRect2[(int)E_ROI_NUM.R_CENTER].X = rect.X;
                m_GuideLineRect2[(int)E_ROI_NUM.R_CENTER].Y = rect.Y;

                //bTEST = true;
            }
            else if (picMainImg.Width / 2 >= nCurX && picMainImg.Height / 2 >= nCurY)
            {
                int distanceX = m_GuideLineRect2[0].X - nCurX;
                int distanceY = m_GuideLineRect2[0].Y - nCurY;

                m_GuideLineRect2[(int)E_ROI_NUM.R_LEFT_TOP].X = rect.X;
                m_GuideLineRect2[(int)E_ROI_NUM.R_LEFT_TOP].Y = rect.Y;
            }
            else if (!bTEST && m_GuideLineRect.Width / 2 >= m_pointROIRIGHT_MOVE_Click.X - m_GuideLineRect.X && m_GuideLineRect.Height / 2 <= m_pointROIRIGHT_MOVE_Click.Y - m_GuideLineRect.Y - 500)
            {

                m_GuideLineRect2[(int)E_ROI_NUM.R_LEFT_BOTTOM].X = rect.X;
                m_GuideLineRect2[(int)E_ROI_NUM.R_LEFT_BOTTOM].Y = rect.Y;
            }
            else if (!bTEST && m_GuideLineRect.Width / 2 <= m_pointROIRIGHTClick.X && m_GuideLineRect.Height / 2 >= m_pointROIRIGHTClick.Y)
            {
                //int distanceX = m_GuideLineRect.X - nCurX + 1400;
                //int distanceY = m_GuideLineRect.Y - nCurY + 700;

                m_GuideLineRect2[(int)E_ROI_NUM.R_RIGHT_TOP].X = rect.X;
                m_GuideLineRect2[(int)E_ROI_NUM.R_RIGHT_TOP].Y = rect.Y;
            }
            else if (!bTEST && m_GuideLineRect.Width / 2 <= m_pointROIRIGHTClick.X && m_GuideLineRect.Height / 2 <= m_pointROIRIGHT_MOVE_Click.Y - m_GuideLineRect.Y - 500)
            {
                m_GuideLineRect2[(int)E_ROI_NUM.R_RIGHT_BOTTOM].X = rect.X;
                m_GuideLineRect2[(int)E_ROI_NUM.R_RIGHT_BOTTOM].Y = rect.Y;
            }

        }



        public Dictionary<int, List<int>> m_fnGetLineProfileVal()
        {
            List<int> retListX = new List<int>();
            List<int> retListY = new List<int>();
            int nXYFlg = 0;
            Dictionary<int, List<int>> retDic = new Dictionary<int, List<int>>();
            if (m_imgLoaded == null) return null;
            Bitmap bitTmp = m_imgLoaded;

            int nDivOffSetX = 0;
            int nDivOffSetY = 0;
            int nStartX = 0;
            int nStartY = 0;
            int nEndX = 0;
            int nEndY = 0;

            int nWidth = picMainImg.Width;
            int nHeight = picMainImg.Height;


            if (m_bDivisionModeFlg)
            {
                Rectangle tempRect = new Rectangle();
                int nDivWidth = picMainImg.Width / 2 / 4 * 4;
                int nDivnHeight = picMainImg.Height / 2 / 4 * 4;
                tempRect = GetRectCurDiv(m_pntLineRightClick.X, m_pntLineRightClick.Y);
                nStartX = CalcRectCoord(pntLineClick.X, tempRect.X, nDivWidth, nDivWidth);
                nStartY = CalcRectCoord(pntLineClick.Y, tempRect.Y, nDivnHeight, nDivnHeight);
                nEndX = CalcRectCoord(m_Linerect.Width, tempRect.X, nDivWidth, nDivWidth);
                nEndY = CalcRectCoord(m_Linerect.Height, tempRect.Y, nDivnHeight, nDivnHeight);

                if ((picMainImg.Width / 3 < m_pntLineRightClick.X && picMainImg.Height / 3 < m_pntLineRightClick.Y
                    && (picMainImg.Width / 3 * 2 >= m_pntLineRightClick.X && picMainImg.Height / 3 * 2 >= m_pntLineRightClick.Y)))
                {
                    bitTmp = (m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_CENTER]);
                    nDivOffSetX = picMainImg.Width / 3;
                    nDivOffSetY = picMainImg.Height / 3;
                }
                else if (picMainImg.Width / 2 >= m_pntLineRightClick.X && picMainImg.Height / 2 >= m_pntLineRightClick.Y)
                {
                    bitTmp = (m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_LEFT_TOP]);
                }
                else if (picMainImg.Width / 2 < m_pntLineRightClick.X && picMainImg.Height / 2 >= m_pntLineRightClick.Y)
                {
                    bitTmp = (m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_RIGHT_TOP]);
                    nDivOffSetX = picMainImg.Width / 2;
                }
                else if (picMainImg.Width / 2 >= m_pntLineRightClick.X && picMainImg.Height / 2 < m_pntLineRightClick.Y)
                {
                    bitTmp = (m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_LEFT_BOTTOM]);
                    nDivOffSetY = picMainImg.Height / 2;
                }
                else if (picMainImg.Width / 2 < m_pntLineRightClick.X && picMainImg.Height / 2 < m_pntLineRightClick.Y)
                {
                    bitTmp = (m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_RIGHT_BOTTOM]);
                    nDivOffSetX = picMainImg.Width / 2;
                    nDivOffSetY = picMainImg.Height / 2;
                }

                if (nStartX >= nDivWidth) nStartX = nDivWidth - 1;
                if (nEndX >= nDivWidth) nEndX = nDivWidth - 1;
                if (nStartY >= nDivnHeight) nStartY = nDivnHeight - 1;
                if (nEndY >= nDivnHeight) nEndY = nDivnHeight - 1;

                if (nStartX < 0) nStartX = 0;
                if (nEndX < 0) nEndX = 0;
                if (nStartY < 0) nStartY = 0;
                if (nEndY < 0) nEndY = 0;

            }
            else
            {
                nStartX = CalcRectCoord(pntLineClick.X, m_imgRect.X, m_PixelSize.X, picMainImg.Width);
                nStartY = CalcRectCoord(pntLineClick.Y, m_imgRect.Y, m_PixelSize.Y, picMainImg.Height);
                nEndX = CalcRectCoord(m_Linerect.Width, m_imgRect.X, m_PixelSize.X, picMainImg.Width);
                nEndY = CalcRectCoord(m_Linerect.Height, m_imgRect.Y, m_PixelSize.Y, picMainImg.Height);

                if (nStartX >= picMainImg.Width) nStartX = picMainImg.Width - 1;
                if (nEndX >= picMainImg.Width) nEndX = picMainImg.Width - 1;
                if (nStartY >= picMainImg.Height) nStartY = picMainImg.Height - 1;
                if (nEndY >= picMainImg.Height) nEndY = picMainImg.Height - 1;

                if (nStartX < 0) nStartX = 0;
                if (nEndX < 0) nEndX = 0;
                if (nStartY < 0) nStartY = 0;
                if (nEndY < 0) nEndY = 0;

            }

            if (nStartX < 0 || nEndX < 0 || nStartY < 0 || nEndY < 0) return null;
            if (bitTmp == null) return null;
            int nTmp = 0;
            if (nStartX > nEndX) { nTmp = nStartX; nStartX = nEndX; nEndX = nTmp; }
            if (nStartY > nEndY) { nTmp = nStartY; nStartY = nEndY; nEndY = nTmp; }

            retListX.Clear();
            nStartX += nDivOffSetX;
            nEndX += nDivOffSetX;
            for (int i = 0; i < nWidth; i++)
            {
                if (i >= (nStartX - 1 < 0 ? 0 : nStartX - 1) && i < nEndX)
                    retListX.Add(bitTmp.GetPixel(i - nDivOffSetX < 0 ? 0 : i - nDivOffSetX, nStartY).R);
                else
                    retListX.Add(0);
            }
            nXYFlg = 0;
            retDic.Add(nXYFlg, retListX);

            retListY.Clear();
            nStartY += nDivOffSetY;
            nEndY += nDivOffSetY;
            for (int i = 0; i < nHeight; i++)
            {
                if (i >= (nStartY - 1 < 0 ? 0 : nStartY - 1) && i < nEndY)
                    retListY.Add(bitTmp.GetPixel(nStartX - nDivOffSetX, i - nDivOffSetY < 0 ? 0 : i - nDivOffSetY).R);
                else
                    retListY.Add(0);

            }
            nXYFlg = 1;
            retDic.Add(nXYFlg, retListY);

            return retDic;
        }

        public Rectangle m_fnGetImageFov()
        {
            if (m_bDivisionModeFlg)
            {
                return GetRectCurDiv(m_pntLineRightClick.X, m_pntLineRightClick.Y);
            }
            else
            {
                return m_imgRect;
            }
        }

        public double m_fnCalcContrast()
        {
            double dRet = 0.0F;

            List<int> retListX = new List<int>();
            List<int> retListY = new List<int>();

            Dictionary<int, List<int>> retDic = new Dictionary<int, List<int>>();
            if (m_imgLoaded == null) return 0.0F;
            Bitmap bitTmp = m_imgLoaded;

            int nDivOffSetX = 0;
            int nDivOffSetY = 0;
            int nStartX = 0;
            int nStartY = 0;
            int nEndX = 0;
            int nEndY = 0;

            int nWidth = picMainImg.Width;
            int nHeight = picMainImg.Height;


            if (m_bDivisionModeFlg)
            {
                int nDivWidth = picMainImg.Width / 2 / 4 * 4;
                int nDivnHeight = picMainImg.Height / 2 / 4 * 4;

                nStartX = m_RectCom.X;
                nStartY = m_RectCom.Y;
                nEndX = m_RectCom.Width;
                nEndY = m_RectCom.Height;


                if (m_RectCom.Width < 0)
                {
                    nStartX = m_RectCom.X + m_RectCom.Width;
                    nEndX = m_RectCom.X - nStartX;
                }
                if (m_RectCom.Height < 0)
                {
                    nStartY = m_RectCom.Y + m_RectCom.Height;
                    nEndY = m_RectCom.Y - nStartY;
                }
                if ((picMainImg.Width / 3 < m_pntRectRightClick.X && picMainImg.Height / 3 < m_pntRectRightClick.Y
                    && (picMainImg.Width / 3 * 2 >= m_pntRectRightClick.X && picMainImg.Height / 3 * 2 >= m_pntRectRightClick.Y)))
                {
                    bitTmp = (m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_CENTER]);
                    nDivOffSetX = picMainImg.Width / 3;
                    nDivOffSetY = picMainImg.Height / 3;
                }
                else if (picMainImg.Width / 2 >= m_pntRectRightClick.X && picMainImg.Height / 2 >= m_pntRectRightClick.Y)
                {
                    bitTmp = (m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_LEFT_TOP]);
                }
                else if (picMainImg.Width / 2 < m_pntRectRightClick.X && picMainImg.Height / 2 >= m_pntRectRightClick.Y)
                {
                    bitTmp = (m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_RIGHT_TOP]);
                    nDivOffSetX = picMainImg.Width / 2;
                }
                else if (picMainImg.Width / 2 >= m_pntRectRightClick.X && picMainImg.Height / 2 < m_pntRectRightClick.Y)
                {
                    bitTmp = (m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_LEFT_BOTTOM]);
                    nDivOffSetY = picMainImg.Height / 2;
                }
                else if (picMainImg.Width / 2 < m_pntRectRightClick.X && picMainImg.Height / 2 < m_pntRectRightClick.Y)
                {
                    bitTmp = (m_DivLoad.m_imgDivLoaded[(int)E_DIV_NUM.E_RIGHT_BOTTOM]);
                    nDivOffSetX = picMainImg.Width / 2;
                    nDivOffSetY = picMainImg.Height / 2;
                }
            }
            else
            {
                nStartX = m_RectCom.X;
                nStartY = m_RectCom.Y;
                nEndX = m_RectCom.Width;
                nEndY = m_RectCom.Height;
            }

            if (nEndX == 0 || nEndY == 0) return dRet;

            //m_bLiveFlg = false;

            nWidth = nEndX;
            nHeight = nEndY;
            if (nWidth > 1920) nWidth = 1920;
            if (nHeight > 1080) nHeight = 1080;


            bitTmp = m_fnLiveImageGrab(nStartX, nStartY, nEndX, nEndY
                , (nWidth / 4 * 4), (nHeight / 4 * 4));
            //bitTmp.Save(("D:\\Contrast.bmp"), System.Drawing.Imaging.ImageFormat.Bmp);
            // m_bLiveFlg = true;

            m_lbContrastCoord.Text = string.Format("{0:0.000}", bitTmp.Uniformity() /** 100*/);

            // Contrast = 최대 GV - 최소 GV / 최대 GV + 최소 GV
            return dRet;
        }

        public void m_fnSetFocusValue(double[] dFocusVal)
        {
            m_dFocusVal = dFocusVal;
        }

        public double[] m_fnGetFocusValue()
        {
            return m_dFocusVal;
        }

        public void m_fnSetRectOriginDiv()
        {
            Rectangle rect = new Rectangle(0, 0, picMainImg.Width / 2, picMainImg.Height / 2);


            m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_CENTER] = rect;

            m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_TOP] = rect;

            m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_TOP] = rect;

            m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_LEFT_BOTTOM] = rect;

            m_DivLoad.m_imgRect[(int)E_DIV_NUM.E_RIGHT_BOTTOM] = rect;

        }

        /// <summary>
        /// 기  능 : 현재 픽셀의 포멧을 취득하여 이미지비트의 오프셋 값을 반환한다.
        /// /반환값 : int PixelOffSet
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary>        
        public int m_fnGetPxlFormatIndex()
        {
            int nPixelOffSet = 1;
            string strPixFormat = m_fnGetPixelForamt().ToString();
            if (strPixFormat.Contains("8")) nPixelOffSet = 1;
            if (strPixFormat.Contains("16")) nPixelOffSet = 2;
            if (strPixFormat.Contains("24")) nPixelOffSet = 3;
            if (strPixFormat.Contains("32")) nPixelOffSet = 4;
            if (strPixFormat.Contains("64")) nPixelOffSet = 8;
            return nPixelOffSet;
        }

        /// <summary>
        /// 기  능 : 현재 픽셀의 포멧을 반환한다.
        /// /반환값 : int PixelOffSet
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary>    
        public PixelFormat m_fnGetPixelForamt()
        {
            return m_pxlForamt;
        }
        /// <summary>
        /// 기  능 : 현재 픽셀의 포멧을 설정한다.
        /// /반환값 : int PixelOffSet
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary>    
        public void m_fnSetPixelForamt(PixelFormat pxlForamt)
        {
            m_pxlForamt = pxlForamt;
        }
        /// <summary>
        /// 기  능 : 호출 영상의 Band Width를 반환한다.
        /// /반환값 : 
        /// /입  력 : int BandWidth
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary>    
        public uint m_fnGetImageBandwidth()
        {
            uint nParam = 0;
            int nRet = Consts.APP_NG;
            CmdMsgParam Param = new CmdMsgParam();

            Param.ClearOffset();
            Param.SetUInteger(nParam);
            nRet = Systems.g_Ipc.SendCommand((ushort)((m_fnGetVisionNo() + 1) * 100 + IpcConst.CAMERA_TASK + m_nCamIndex), IpcConst.CAMERA_FUNC, IpcConst.LIVE_GET_BANDWIDTH,
                                                IpcInterface.CMD_TYPE_RES, 100, Param.GetByteSize(), Param.GetParam());
            if (nRet == Consts.APP_OK)
            {
                Param.SetOffset(0);
                nParam = Param.GetUInteger();
            }
            return nParam;
        }

        /// 기  능  :  입력된 Band Width로 픽셀 포멧을 반환한다.
        /// /반환값 : int nBandWidth
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary>    
        public PixelFormat m_fnBandwidthToPixelformat(int nChannelcnt)
        {
            int nBandWidth = nChannelcnt;//m_fnGetImageBandwidth();
            switch (nBandWidth)
            {
                case 1: return PixelFormat.Format8bppIndexed;
                case 3: return PixelFormat.Format24bppRgb;
                case 4: return PixelFormat.Format32bppRgb;
                default: return PixelFormat.Format8bppIndexed;
            }
        }

        /// 기  능  :  입력된 Band Width로 픽셀 포멧을 반환한다.
        /// /반환값 : int nBandWidth
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary>    
        public string m_fnLoadImageExtens()
        {
            int nBandWidth = (int)m_fnGetImageBandwidth();
            string strExtens = "";
            if (nBandWidth == 8) strExtens = Paths.RECIPE_EXTENS_BMP;
            if (nBandWidth == 16) strExtens = Paths.RECIPE_EXTENS_TIFF;
            return strExtens;
        }

        /// <summary>
        /// 기  능 : 현재 이미지 아이디를 반환한다.
        /// /반환값 : int PixelOffSet
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary>    
        public int m_fnGetImageID()
        {
            return m_nImgeID;
        }
        /// <summary>
        /// 기  능 : 현재 PC No를 반환한다.
        /// /반환값 : int PixelOffSet
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary>    
        public int m_fnGetVisionNo()
        {
            return m_nVisionNo;
        }
        /// <summary>
        /// 기  능 : 선택된 영역을 클리어 한다.
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary>    
        public void SelAreaClear()
        {
            m_RectCom = new Rectangle();
        }


    }
}
