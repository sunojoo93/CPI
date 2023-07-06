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
using System.IO;
using System.Runtime.InteropServices;
using CRUX_Renewal.Class;

namespace CRUX_Renewal.User_Controls
{
    /// <summary>
    /// 작업일 : 2016.09.26
    /// 작성자 : 임경민 (IKM)
    /// 클래스 : Uctrl_MainPic
    /// 구  분 : User Control Class
    /// 기  능 : ROI 및 영상 호출에 사용 (ROI, Aling Mark, 원점 좌표, POL Mark, 비검사 영역 설정 가능)
    /// 기  타 : 확대, 축소, 미니맵, 좌표 이동 기능
    /// </summary>
    public partial class Uctrl_DefectMap : UserControl
    {
        //LIVE 
        ////////////////////////////////////////           
        private bool m_bLiveFlg;                        
        //test        
        private Point m_CameraPixelSize;// = new Point(6576, 4384);
        ////////////////////////////////////////

        // 원점 변수
        ////////////////////////////////////////                 
        private Point pntOriginClick;
        private Pen m_penOrigin;
        private List<Point> m_OriginPointList; 
        ////////////////////////////////////////  

        // Align 변수
        ////////////////////////////////////////          
        private bool m_bLeftAlignBtnDown = false;
        private Point pntAlignClick;
        private Pen m_penAlign;
        private List<Point> m_AlignPntList; 
        ////////////////////////////////////////

        // ROI 변수
        ////////////////////////////////////////          
        private bool m_bLeftRoiBtnDown = false;
        private Point pntRoiClick = new Point();
        private Pen m_penRoi;            
        private List<Rectangle> m_DefRectList;
        private List<Pen> m_Pnt_Line_PenList;        
        private Rectangle m_DefRect;
        ////////////////////////////////////////

        // Panel Shape 변수
        ////////////////////////////////////////           
        private Pen m_penPanalShape;
        private List<Point> m_PanelShapePointList;
        ////////////////////////////////////////

        // Pol 변수
        ////////////////////////////////////////               
        private Pen m_penPol;
        private List<Rectangle> m_PolRectList;
        private Rectangle m_PolRect;
        ////////////////////////////////////////

        // Non 변수
        ////////////////////////////////////////               
        private Pen m_penMura;
        private List<Rectangle> m_MuraRectList;
        private List<Pen> m_Mura_PenList;
        private Rectangle m_MuraRect;
        ////////////////////////////////////////

        // Pad 변수
        ////////////////////////////////////////        
        private Pen m_penPad;
        private List<Rectangle> m_PadRectList;
        private Rectangle m_PadRect;
        ////////////////////////////////////////
        // 이미지 컨트롤 변수
        ////////////////////////////////////////
        public Uctrl_MiniMap m_ucPicMiniMap;        
        private double m_dRecRatio;         
        private Point m_imgPoint;
        private Point m_LastPoint;
        private Point m_pointClick;        
        private Rectangle m_imgRect;  
        private Bitmap m_imgLoaded = null;
        private Image m_imgMiniMap;
    
        private double m_dRratio = 1F;
    
        /////////////////////////////////////////

        // Envet 사용 유무 플래그
        // Zoom In Out,Mini Map, Mouse Move, Roi, Align, 원점 좌표        
        private bool m_bUseSelCoordView = true;
        private bool  m_bUseMiniMap = false;        
        private bool m_bUseZoomInOut = false;
        private bool m_bUseDragMove = false;
        private bool m_bUseRoiLineView = false;
        private bool m_bUseRndLineView = false;
        private bool m_bUseNonLineView = false;
        private bool m_bUsePadLineView = false;
        private bool m_bUseRoiMultiLine = false;
        private bool m_bOriginMarkRegister = false;
        private bool m_bAlignMarkRegister = false;
        ////////////////////////////////////////////////

        // 라벨: 좌표 데이터 용
        /////////////////////////////////////////
        STRU_LABEL_COORD m_stCurCoord;
        STRU_LABEL_COORD m_stSelCoord;
        STRU_LABEL_COORD m_stRoiCoord;
        STRU_LABEL_COORD m_stPolCoord;
        STRU_LABEL_COORD m_stNonCoord;
        STRU_LABEL_COORD m_stPadCoord;
        STRU_LABEL_COORD m_stAlnCoord;
        STRU_LABEL_COORD m_stOrgCoord;
        Label[] m_stRGBColor = new Label[3];
        Label m_stFocus = new Label();

        public Label m_lbNowRatio = new Label();
        public struct STRU_LABEL_COORD
        {
            public Label X1;
            public Label Y1;
            public Label X2;
            public Label Y2;
        }
        /////////////////////////////////////////

        int m_nVisionNo = 0;
 
        public Uctrl_DefectMap(int nVisionNo, Form Parent, int nWidth = 870, int nHeight = 700, int nLocX = 9, int nLocY = 47, int nFovX = 6576 , int nFovY = 4384)
        {
            m_nVisionNo = nVisionNo;
            m_CameraPixelSize = new Point(nFovX , nFovY );
           
            InitializeComponent();
           
            SetStyle(ControlStyles.OptimizedDoubleBuffer, true);
            SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.ResizeRedraw, true);
            SetStyle(ControlStyles.UserPaint, true);
            //initImage( Color.FromArgb(36, 32, 30));
            Size sizeImg = new Size(nWidth, nHeight);          
       
           this.Location = new Point(nLocX, nLocY);
          // this.Size = sizeImg;
           picMainImg.Size = sizeImg;
           vScrollBar1.Location = new Point(nWidth + 1, picMainImg.Location.Y);
           hScrollBar1.Location = new Point(picMainImg.Location.X, nHeight + 1);          
           vScrollBar1.Height = nHeight;
           hScrollBar1.Width = nWidth;
           pnlBackgrd.Size = picMainImg.Size;
           pnlBackgrd.Location = picMainImg.Location;
           prbImgLoad.Location = new Point((picMainImg.Width - prbImgLoad.Width) / 2, (picMainImg.Height - prbImgLoad.Height) / 2);
           Parent.Controls.Add(this);
           //pnlBackgrd.Controls.Add(lbImgLoad);
           pnlBackgrd.Controls.Add(prbImgLoad);
           lbImgLoad.BackColor = Color.FromArgb(69, 99, 73);// Color.Transparent;
           lbImgLoad.Parent = this.picMainImg;          
           InitMuraData(new Color());
           InitPadData(new Color() );
           InitPanelMap(new Color() );
           InitDefectData(new Color());
           InitAlignData(new Color());
           InitSelDefectData(new Color());
           
        }
        private void GetPanelSizeOfCameraTask()
        {
            CmdMsgParam Param = new CmdMsgParam();
            uint nParam = 0;
            int nRet = Consts.APP_NG;
            Param.ClearOffset();
            Param.SetUInteger(nParam);
            nRet = Systems.g_Ipc.SendCommand((ushort)((m_nVisionNo + 1) * 100 + IpcConst.CAMERA_TASK), IpcConst.CAMERA_FUNC, IpcConst.LIVE_GET_WIDTH,
                                                IpcInterface.CMD_TYPE_RES, 100, Param.GetByteSize(), Param.GetParam());
            Param.SetOffset(0);
            if (nRet == Consts.APP_OK)
                m_CameraPixelSize.X = Convert.ToInt32(Param.GetUInteger());

            Param.ClearOffset();
            Param.SetUInteger(nParam);
            nRet = Systems.g_Ipc.SendCommand((ushort)((m_nVisionNo + 1) * 100 + IpcConst.CAMERA_TASK), IpcConst.CAMERA_FUNC, IpcConst.LIVE_GET_HEIGHT,
                                               IpcInterface.CMD_TYPE_RES, 100, Param.GetByteSize(), Param.GetParam());
            Param.SetOffset(0);
            if (nRet == Consts.APP_OK)
                m_CameraPixelSize.Y = (int)Param.GetUInteger();
        }

        public void SetPanelSizeOfCameraTask(int nX, int nY)
        {

            m_CameraPixelSize.X = nX;

            m_CameraPixelSize.Y = nY;
        }

        public void ClearPanelShapeData()
        {
            if (m_PanelShapePointList != null)
                m_PanelShapePointList.Clear();
            m_PanelShapePointList = null;
        }

        public void InitPanelShapeData(Color color) // 2020.01.03 yskim
        {
            if (m_PanelShapePointList == null)
            {
                m_PanelShapePointList = new List<Point>();
                m_penPanalShape = new Pen(color, 2);
                m_penPanalShape.DashStyle = System.Drawing.Drawing2D.DashStyle.Dot;
            }

            string strCurModelName = iniUtl.GetIniValue("RECIPE_INFO", "SELECT_RECIPE", "Default", Paths.NET_INITIAL_PATH[m_nVisionNo]);
            string strCurGrabVersion = iniUtl.GetIniValue("RECIPE_INFO", "SELECT_GRAB_VERSION", "Default", Paths.NET_INITIAL_PATH[m_nVisionNo]);
            string strCurInspVersion = iniUtl.GetIniValue("RECIPE_INFO", "SELECT_INSP_VERSION", "Default", Paths.NET_INITIAL_PATH[m_nVisionNo]);

            string strCurModelPath = Path.Combine(Paths.NET_DRIVE[m_nVisionNo], Paths.NET_RECIPE_PATH[m_nVisionNo], strCurModelName);
            string strCurGrabModelPath = Path.Combine(strCurModelPath, Paths.RECIPE_GRAB, strCurGrabVersion);
            string strCurInspModelPath = Path.Combine(strCurModelPath, Paths.RECIPE_INSP, strCurInspVersion);

            string corner_path = Path.Combine(strCurInspModelPath, "CornerEdge");

            

            if (!Directory.Exists(corner_path))
                return;

            // 각 파일별 코너 데이터 수집
            var pointsSet = new List<List<Point>>();
            var pointsResizeSet = new List<List<Point>>();
            var cornerIndex = new List<int>();

            string[] str_Insp_Type = new string[4];
            str_Insp_Type[0] = str_Insp_Type[1] = str_Insp_Type[2]= str_Insp_Type[3] = "";

            switch (Globals.Insp_Type)
            {
                case (int)Enums.Eqp_Type.AVI:
                    str_Insp_Type[0] = "WHITE";
					str_Insp_Type[1] = "2";
                    str_Insp_Type[2] = "0";
                    str_Insp_Type[3] = "1";
                    break;
                case (int)Enums.Eqp_Type.SVI:
                    str_Insp_Type[0] = "AlignCornerCoax.EdgePT";
					str_Insp_Type[1] = "1";
                    str_Insp_Type[2] = "1";
                    str_Insp_Type[3] = "2";
                    break;
            }

            foreach (string file in Directory.GetFiles(corner_path))
            {

                if (file.Contains(str_Insp_Type[0]))///*AlignCornerCoax.EdgePT*/
                {
                    var points = new List<Point>();
                    string[] lines = File.ReadAllLines(file);

                    cornerIndex.Add(int.Parse(lines[0].Substring(lines[0].Length - 1, 1)));
                    for (int i = Convert.ToInt32(str_Insp_Type[1]); i < lines.Length; i++)
                    {
                        int x = int.Parse(lines[i].Split(',')[Convert.ToInt32(str_Insp_Type[2])]);
                        int y = int.Parse(lines[i].Split(',')[Convert.ToInt32(str_Insp_Type[3])]);
                        points.Add(new Point(x, y));
                    }
                    pointsSet.Add(points);
                }
            }

            // 데이터가 없다면 null 값 넣고 끝내버린다.
            if (pointsSet.Count == 0)
            {
                m_PanelShapePointList.Clear();
                m_PanelShapePointList = null;
                return;
            }

            // PS인지 아닌지 체크
            int ps_multiply_value = 1;
            int ps_count = 0, none_count = 0;
            if (fileProc.FileExists(Path.Combine(strCurGrabModelPath, "CameraParameter.xml")))
            {
                var doc = new System.Xml.XmlDocument();
                doc.Load(new StringReader(fileProc.FileRead(Path.Combine(strCurGrabModelPath, "CameraParameter.xml"))));
                foreach (System.Xml.XmlNode childNode in doc.ChildNodes)
                {
                    if (childNode.Name.ToUpper().Contains("CameraRecipe".ToUpper()))
                    {
                        foreach (System.Xml.XmlNode RowTmp in childNode.ChildNodes)
                        {
                            int nCnt = RowTmp.Name.Substring(RowTmp.Name.LastIndexOf("_") + 1).toInt();
                            var dt = doc.m_fnBuildDataTableFromXml("CameraRecipe/PATTERN_" + nCnt + "/CAM", false, Consts.TRUE);
                            if (dt.Rows[0]["PS"].toBool() && dt.Rows[0]["PS"].ToString() != "None")
                                ps_count++;
                            else
                                none_count++;
                        }
                    }
                }
            }
            ps_multiply_value = ps_count > none_count ? 2 : 1;

            // Cell 사이즈 가져와서 합산 작업 (합산 해줘야 실제 좌표가 나옴)
            Point panel_size = new Point();
            string pnlinfFile = Path.Combine(strCurGrabModelPath, strCurModelName + ".pnlinf");
            int start_x = int.Parse(iniUtl.GetIniValue("CAM_1", "Edge Coord Start X", "0", pnlinfFile));
            int start_y = int.Parse(iniUtl.GetIniValue("CAM_1", "Edge Coord Start Y", "0", pnlinfFile));
            int end_x = int.Parse(iniUtl.GetIniValue("CAM_1", "Edge Coord End X", "6000", pnlinfFile));
            int end_y = int.Parse(iniUtl.GetIniValue("CAM_1", "Edge Coord End Y", "4000", pnlinfFile));
            panel_size.X = (end_x - start_x) * ps_multiply_value;
            panel_size.Y = (end_y - start_y) * ps_multiply_value;

            for (int i = 0; i < cornerIndex.Count; i++)
            {
                switch (cornerIndex[i])
                {
                    case 0: // LT - 그대로
                        for (int j = 0; j < pointsSet[i].Count; j++)
                            pointsSet[i][j] = new Point(pointsSet[i][j].X, pointsSet[i][j].Y);
                        break;
                    case 1: // RT - Width 합산
                        for (int j = 0; j < pointsSet[i].Count; j++)
                            pointsSet[i][j] = new Point(pointsSet[i][j].X + panel_size.X, pointsSet[i][j].Y);
                        break;
                    case 2: // RB - Width, Height 합산
                        for (int j = 0; j < pointsSet[i].Count; j++)
                            pointsSet[i][j] = new Point(pointsSet[i][j].X + panel_size.X, pointsSet[i][j].Y + panel_size.Y);
                        break;
                    case 3: // LB - Height 합산
                        for (int j = 0; j < pointsSet[i].Count; j++)
                            pointsSet[i][j] = new Point(pointsSet[i][j].X, pointsSet[i][j].Y + panel_size.Y);
                        break;
                }
            }

            // 각 코너 데이터 좌표 리사이징 (패널 좌표 -> 디펙맵 좌표)
            for (int i = 0; i < pointsSet.Count; i++)
            {
                var points = new List<Point>();
                int prev_x = 0, prev_y = 0;
                for (int j = 0; j < pointsSet[i].Count; j++)
                {
                    double cur_ratio_x = pointsSet[i][j].X / (double)panel_size.X;
                    double cur_ratio_y = pointsSet[i][j].Y / (double)panel_size.Y;
                    int cal_x = (int)(cur_ratio_x * (picMainImg.Width - 4)) + 2;    // -2 +2 한이유는 외관선이 좌상단변 끝에 안걸치게 하기 위함
                    int cal_y = (int)(cur_ratio_y * (picMainImg.Height - 4)) + 2;
                    cal_x = cal_x < 0 ? 0 : cal_x;
                    cal_y = cal_y < 0 ? 0 : cal_y;
                    // 중복 제거 (이전 데이터와 동일하면 무시한다)
                    if (j > 0 && cal_x == prev_x && cal_y == prev_y)
                        continue;
                    prev_x = cal_x;
                    prev_y = cal_y;

                    points.Add(new Point(cal_x, cal_y));
                }
                //KYH 2021.09.09
                if ((int)Enums.Eqp_Type.SVI == Globals.Insp_Type) DefectMapListChange(points);

                pointsResizeSet.Add(points);
            }

            if ((int)Enums.Eqp_Type.SVI == Globals.Insp_Type)
            {
                var link_line2 = new List<Point>();
                link_line2 = SetDotLinkData(pointsResizeSet[0]);
                pointsResizeSet.Add(link_line2);
            }

            //ListToExcel(pointsResizeSet);

            // 각 코너 데이터 연결하기 위한 데이터 계산 및 추가
            if((int)Enums.Eqp_Type.SVI != Globals.Insp_Type)
            {
                int corner_data_count = pointsResizeSet.Count;
                for (int i = 0; i < corner_data_count; i++)
                {
                    var link_line = new List<Point>();
                    // 마지막 데이터는 0번과 연결
                    if (i == corner_data_count - 1)
                        link_line = GetCornerLinkData(pointsResizeSet[i], pointsResizeSet[0]);
                    else
                        link_line = GetCornerLinkData(pointsResizeSet[i], pointsResizeSet[i + 1]);
                    pointsResizeSet.Add(link_line);
                }
            }
     
            // Panel Shape 리스트에 담기
            m_PanelShapePointList.Clear();
            for (int i = 0; i < pointsResizeSet.Count; i++)
                for (int j = 0; j < pointsResizeSet[i].Count; j++)
                    m_PanelShapePointList.Add(pointsResizeSet[i][j]);

            // ================================================================================

            // Hole 데이터 그리기 추가.. 2020.05.22 yskim
            string hole_path = Path.Combine(strCurInspModelPath, "CameraHole");
            if (!Directory.Exists(hole_path))
                return;

            // 각 파일별 코너 데이터 수집
            var pointsHoleSet = new List<List<Point>>();
            var pointsHoleResizeSet = new List<List<Point>>();
            var holeCornerIndex = new List<int>();
            foreach (string file in Directory.GetFiles(hole_path))
            {
                if (file.Contains("WHITE"))
                {
                    var points = new List<Point>();
                    string[] lines = File.ReadAllLines(file);
                    holeCornerIndex.Add(int.Parse(lines[0].Substring(lines[0].Length - 1, 1)));
                    for (int i = 2; i < lines.Length; i++)
                    {
                        int x = int.Parse(lines[i].Split(',')[0]);
                        int y = int.Parse(lines[i].Split(',')[1]);
                        points.Add(new Point(x, y));
                    }
                    pointsHoleSet.Add(points);
                }
            }

            // Cell 사이즈, hole 위치에 따라 좌표 합산 작업
            for (int i = 0; i < holeCornerIndex.Count; i++)
            {
                switch (holeCornerIndex[i])
                {
                    case 0: // LT - 그대로
                        for (int j = 0; j < pointsHoleSet[i].Count; j++)
                            pointsHoleSet[i][j] = new Point(pointsHoleSet[i][j].X, pointsHoleSet[i][j].Y);
                        break;
                    case 1: // RT - Width 합산
                        for (int j = 0; j < pointsHoleSet[i].Count; j++)
                            pointsHoleSet[i][j] = new Point(pointsHoleSet[i][j].X + panel_size.X, pointsHoleSet[i][j].Y);
                        break;
                    case 2: // RB - Width, Height 합산
                        for (int j = 0; j < pointsHoleSet[i].Count; j++)
                            pointsHoleSet[i][j] = new Point(pointsHoleSet[i][j].X + panel_size.X, pointsHoleSet[i][j].Y + panel_size.Y);
                        break;
                    case 3: // LB - Height 합산
                        for (int j = 0; j < pointsHoleSet[i].Count; j++)
                            pointsHoleSet[i][j] = new Point(pointsHoleSet[i][j].X, pointsHoleSet[i][j].Y + panel_size.Y);
                        break;
                }
            }

            // 각 코너 데이터 좌표 리사이징 (패널 좌표 -> 디펙맵 좌표)
            for (int i = 0; i < pointsHoleSet.Count; i++)
            {
                var points = new List<Point>();
                int prev_x = 0, prev_y = 0;
                for (int j = 0; j < pointsHoleSet[i].Count; j++)
                {
                    double cur_ratio_x = pointsHoleSet[i][j].X / (double)panel_size.X;
                    double cur_ratio_y = pointsHoleSet[i][j].Y / (double)panel_size.Y;
                    int cal_x = (int)(cur_ratio_x * (picMainImg.Width - 4)) + 2;    // -2 +2 한이유는 외관선이 좌상단변 끝에 안걸치게 하기 위함
                    int cal_y = (int)(cur_ratio_y * (picMainImg.Height - 4)) + 3;
                    cal_x = cal_x < 0 ? 0 : cal_x;
                    cal_y = cal_y < 0 ? 0 : cal_y;
                    // 중복 제거 (이전 데이터와 동일하면 무시한다)
                    if (j > 0 && cal_x == prev_x && cal_y == prev_y)
                        continue;
                    prev_x = cal_x;
                    prev_y = cal_y;
                    points.Add(new Point(cal_x, cal_y));
                }
                pointsHoleResizeSet.Add(points);
               
            }
           
            // Panel Shape 리스트에 담기
            for (int i = 0; i < pointsHoleResizeSet.Count; i++)
                for (int j = 0; j < pointsHoleResizeSet[i].Count; j++)
                    m_PanelShapePointList.Add(pointsHoleResizeSet[i][j]);

         
        }
    
        private void changeXY(List<Point> li,int nResizeX, int nResizeY, int a)
        {

            Point temp = li[a];
            temp.X += nResizeX;
            temp.Y += nResizeY;
            li[a] = temp;
        }
        private void DefectMapListChange(List<Point> Listpoint)
        {
      
            var cal_x = (from z in Listpoint select z.X).Min();
            var cal_y = (from z in Listpoint select z.Y).Min();

            int nResizeX = 2 - cal_x;
            int nResizeY = 2 - cal_y;

            for(int a = 0; a < Listpoint.Count; a++)
                changeXY(Listpoint, nResizeX, nResizeY, a);
            
        }

        static void ReleaseObject(object obj)
        {
            try
            {
                if (obj != null)
                {
                    Marshal.ReleaseComObject(obj); 
                    obj = null;
                }
            }
            catch (Exception ex)
            { obj = null; throw ex;
            }
            finally
            {
                GC.Collect(); 
            }
        }

        private List<Point> SetDotLinkData(List<Point> list1)
        {
            int LBT = 0;
            var link_line = new List<Point>();

            foreach (var T in list1)
            {
                LBT++;

                if (T.X <= 3 && (T.Y >= 30 && T.Y <= 365) && LBT <= list1.Count - 1) //LB부터 LT까지 점 개수 및 추가.
                {
                    SetListAddY(list1, T, link_line, LBT);
                }
                else if (T.X >= picMainImg.Width - 3 && (T.Y >= 250 && T.Y <= 365) && LBT <= list1.Count - 1)
                {
                    SetListAddY(list1, T, link_line, LBT);
                }
                else if ((541 <= T.X  && T.X <= 542) && (T.Y >= 160 && T.Y <= 235))
                {
                    SetListAddY(list1, T, link_line, LBT);
                }
                else if (T.X >= picMainImg.Width - 3 && (T.Y <= 140 && T.Y >= 25))
                {
                    SetListAddY(list1, T, link_line, LBT);
                }
                else if (T.Y >= picMainImg.Height - 4 && (T.X >= 20 && T.X <= 540) && LBT <= list1.Count - 1) //LB부터 RB 까지 점 개수 및 추가.
                {
                    SetListAddX(list1, T, link_line, LBT);
                }
                else if (T.Y <= 3 && (T.X >= 20 && T.X <= 540) && LBT <= list1.Count - 1) //LT부터 RT 까지 점 개수 및 추가.
                {
                    SetListAddX(list1, T, link_line, LBT);
                }

            }

            if(5 < Math.Abs(list1[0].X - list1[list1.Count - 1].X))
                SetListAddX(list1, list1[0], link_line, list1.Count -1,true);

            return link_line;
        }

        private List<Point> SetListAddX(List<Point> list1, Point list2, List<Point> link_line, int nCount, bool bDefault = false)
        {
            int nIdx, nX = 0;
            if (bDefault)
            {
                nIdx = list1[nCount].X - list1[0].X;
                nX = Math.Min(list1[nCount].X, list1[0].X);
            }
                
            else
            {
                nIdx = list1[nCount].X - list1[nCount - 1].X;
                nX = Math.Min(list1[nCount].X, list1[nCount - 1].X);
            }
              
            if (Math.Abs(nIdx) > 1)
            {
                for (int id = 0; id < Math.Abs(nIdx) - 1; id++)
                    link_line.Add(new Point(++nX, list2.Y));
            }

            return link_line;
        }
        private List<Point> SetListAddY(List<Point> list1, Point list2, List<Point> link_line, int nCount)
        {
            int nIdx = list1[nCount].Y - list1[nCount - 1].Y;
            int nY = Math.Min(list1[nCount].Y, list1[nCount - 1].Y);
            if (Math.Abs(nIdx) > 1)
            {
                for (int id = 0; id < Math.Abs(nIdx) - 1; id++)
                    link_line.Add(new Point(list2.X, ++nY));
            }
            return link_line;
        }

        private List<Point> GetCornerLinkData(List<Point> list1, List<Point> list2)
        {
           
            var link_line = new List<Point>();
            Point endPoint1, endPoint2;

            // 비교할 꼭지점들 (각 코너의 꼭지점)
            Point p1_1 = list1[0];
            Point p1_2 = list1[list1.Count - 1];
            Point p2_1 = list2[0];
            Point p2_2 = list2[list2.Count - 1];

            // 거리 계산 함수
            Func<Point, Point, double> cal_distance = (p1, p2) =>
                Math.Sqrt(Math.Pow(p1.X - p2.X, 2) + Math.Pow(p1.Y - p2.Y, 2));

            // 거리 계산
            var dist = new double[4];
            dist[0] = cal_distance(p1_1, p2_1);
            dist[1] = cal_distance(p1_1, p2_2);
            dist[2] = cal_distance(p1_2, p2_1);
            dist[3] = cal_distance(p1_2, p2_2);

            // 거리 비교
            int min_index = 0;
            for (int i = 1; i < dist.Length; i++)
                if (dist[i] < dist[min_index])
                    min_index = i;

            // 거리가 제일 작은 두 꼭지점 선택
            switch (min_index)
            {
                case 0: endPoint1 = p1_1; endPoint2 = p2_1; break;
                case 1: endPoint1 = p1_1; endPoint2 = p2_2; break;
                case 2: endPoint1 = p1_2; endPoint2 = p2_1; break;
                case 3: endPoint1 = p1_2; endPoint2 = p2_2; break;
                default: return null;
            }

            // 연결한 선이 가로선인지 세로선인지
            bool isHorizontal = Math.Abs(endPoint1.Y - endPoint2.Y) < Math.Abs(endPoint1.X - endPoint2.X);
            
            if (isHorizontal)
            {
                // 두 꼭지점의 직선 기울기 계산 및 링크 데이터 생성
                double degree = (endPoint1.Y - endPoint2.Y) / (double)(endPoint1.X - endPoint2.X);
                int sx = (endPoint1.X < endPoint2.X) ? endPoint1.X : endPoint2.X;
                int ex = (endPoint1.X < endPoint2.X) ? endPoint2.X : endPoint1.X;
                for (int x = sx; x < ex; x++)
                {
                    int y = (int) (degree * x + endPoint1.Y);
                    link_line.Add(new Point(x, y));
                }
            }
            else
            {
                // 두 꼭지점의 직선 기울기 계산 및 링크 데이터 생성
                double degree = (endPoint1.X - endPoint2.X) / (double)(endPoint1.Y - endPoint2.Y);
                int sy = (endPoint1.Y < endPoint2.Y) ? endPoint1.Y : endPoint2.Y;
                int ey = (endPoint1.Y < endPoint2.Y) ? endPoint2.Y : endPoint1.Y;
                for (int y = sy; y < ey; y++)
                {
                    int x = (int)(degree * y + endPoint1.X);
                    link_line.Add(new Point(x, y));
                }
            }

            return link_line;
        }

        public void InitDefectData(Color color)
        {
            m_DefRectList = new List<Rectangle>();
            m_Pnt_Line_PenList = new List<Pen>();
            m_DefRect = new Rectangle();
            m_penRoi = new Pen(color, 2);
            m_penRoi.DashStyle = System.Drawing.Drawing2D.DashStyle.Dot;            
        }
        public void InitPanelMap(Color color)
        {
            m_PolRectList = new List<Rectangle>();
            m_PolRect = new Rectangle();
            m_penPol = new Pen(color, 2);
            m_penPol.DashStyle = System.Drawing.Drawing2D.DashStyle.Dot;
        }
        public void InitMuraData(Color color)
        {
            m_MuraRectList = new List<Rectangle>();
            m_Mura_PenList = new List<Pen>();
            m_MuraRect = new Rectangle();
            m_penMura = new Pen(color, 2);
            m_penMura.DashStyle = System.Drawing.Drawing2D.DashStyle.Dot;
        }
        public void InitPadData(Color color)
        {
            m_PadRectList = new List<Rectangle>();
            m_PadRect = new Rectangle();
            m_penPad = new Pen(color, 2);
            m_penPad.DashStyle = System.Drawing.Drawing2D.DashStyle.Dot;
        }
        public void InitAlignData(Color color)
        {
            m_AlignPntList = new List<Point>();
            pntAlignClick = new Point();
            m_penAlign = new Pen(Color.Yellow, 3);
            m_penAlign.DashStyle = System.Drawing.Drawing2D.DashStyle.Solid;
            m_penAlign.Alignment = PenAlignment.Center;
        }

        public void InitSelDefectData(Color color)
        {
            m_OriginPointList = new List<Point>();
            pntOriginClick = new Point();
            m_penOrigin = new Pen(color, 3);
            m_penOrigin.DashStyle = System.Drawing.Drawing2D.DashStyle.Solid;
            m_penOrigin.Alignment = PenAlignment.Center;
        }

        public int m_fnCalCoordinate(int nOrignCoord, int nCurCoord, int nImgRectSize, int nFovSize, double nRatio)
        {
            //if (m_bLiveFlg)
                return nOrignCoord + (int)Math.Round((Convert.ToDouble(nImgRectSize * (Math.Abs(nCurCoord))) / (nFovSize - 1)));
//             else
//                 return (int)Math.Round((Math.Abs(nOrignCoord) + nCurCoord) / nRatio);
        }

        public void m_fnUseEventMouseWheelZoomInOut(bool bRcvFlg)
        {
            m_bUseZoomInOut = bRcvFlg;
        }
        public void m_fnUseEventDragMove(bool bRcvFlg)
        {
            m_bUseDragMove = bRcvFlg;
        }
        public void m_fnUseEventRoiLineView(bool bRcvFlg)
        {
            m_bUseRoiLineView = bRcvFlg;
        }
        public void m_fnUseEventRndLineView(bool bRcvFlg)
        {
            m_bUseRndLineView = bRcvFlg;
        }
        public void m_fnUseEventNonLineView(bool bRcvFlg)
        {
            m_bUseNonLineView = bRcvFlg;
        }
        public void m_fnUseEventPadLineView(bool bRcvFlg)
        {
            m_bUsePadLineView = bRcvFlg;
        }
        public void m_fnUseRoiMultiLine(bool bRcvFlg)
        {
            m_bUseRoiMultiLine = bRcvFlg;
        }
//         public void m_fnStartDrawRoi(bool bRcvFlg)
//         {
//             m_bUseRoiLineView = bRcvFlg;
//         }
        public void m_fnOriginMarkRegister(bool bRcvFlg)
        {
            m_bOriginMarkRegister = bRcvFlg;
        }
        public void m_fnAlignMarkRegister(bool bRcvFlg)
        {
            m_bAlignMarkRegister = bRcvFlg;
        }
        public void ucMiniPic(ref Uctrl_MiniMap ucPicMiniMap)
        {
            m_ucPicMiniMap = ucPicMiniMap;

            if (m_bUseMiniMap)
                m_ucPicMiniMap.Visible = true;
            else
                m_ucPicMiniMap.Visible = false;
        }

        /// <summary>
        /// 기  능 : 현재 비트맵을 초기화 한다.
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic    
        /// </summary>        
        public void initImage(Color color)
        {
            Image image;
            Graphics g;
            m_imgLoaded = null;
            image = new Bitmap(picMainImg.Width, picMainImg.Height);
            g = Graphics.FromImage(image);
            g.Clear(color);
            if (m_PanelShapePointList != null)
                m_fnDrawPanelShape(g, m_penPanalShape, new DelDrawPoint(getPanelShapeDataList));
            m_imgLoaded = (Bitmap)image;
            LoadImage(m_imgLoaded);
            GetPanelSizeOfCameraTask();
            picMainImg.Invalidate();
        }
        delegate void OpenImageCallback(string strImgPath);
        public void OpenImage(string strImgPath)
        {
            try
            {                
               if (this.InvokeRequired)
               {
                  
                   OpenImageCallback delOpenImg = new OpenImageCallback(OpenImage);
                   this.Invoke(delOpenImg, strImgPath);
               }
               else
               {
                   m_bLiveFlg = false;
                   LoadImage(new Bitmap(@strImgPath));                   
               }
               
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
            }
        }

        private void LoadImage(Bitmap LoadImg)
        {           
            m_imgLoaded = LoadImg;
            StartDspImageLoad();
            UpProgressValue();
            ClearRoiRectList();     
            ClearAlignPntList();
            ClearOriginPntList();

            m_dRratio = 0.05;
            m_lbNowRatio.Text = string.Format("{0:00.00}", m_dRratio);

            UpProgressValue();

            //--------------------------------------------------------------------------------------------
            // 2020.01.20 MDJ Image Load 시 Resize Add
            int nWidth = picMainImg.Width;
            int nHeight = picMainImg.Height;
            Size reSize = new Size(nWidth, nHeight);
            m_imgLoaded = new Bitmap(LoadImg, reSize);
            //--------------------------------------------------------------------------------------------

            m_imgPoint = new Point(picMainImg.Width / 2, picMainImg.Height / 2);
            m_imgRect    = new Rectangle(0, 0, m_imgLoaded.Width, m_imgLoaded.Height);
            m_pointClick = m_imgPoint;

            hScrollBar1.Minimum = 0;
            if (m_imgRect.Width - picMainImg.Width > 0)
                hScrollBar1.Maximum = m_imgRect.Width - picMainImg.Width;
            else hScrollBar1.Maximum = 0;
            vScrollBar1.Minimum = 0;
            if (m_imgRect.Height - picMainImg.Height > 0)
                vScrollBar1.Maximum = m_imgRect.Height - picMainImg.Height;
            else vScrollBar1.Maximum = 0;
           
            picMainImg.Invalidate();
            UpProgressValue();
            EndDspImageLoad();
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
            this.Cursor = Cursors.Default;
        }

        private void picMainImg_MouseDown(object sender, MouseEventArgs e)
        {
          
            if (e.Button == MouseButtons.Right)
            {

            }
            else if (e.Button == MouseButtons.Left)
            {              
                m_pointClick = new Point(e.X, e.Y);
            }           
        }

        private void picMainImg_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
           
            if (e.KeyCode == Keys.Escape)
            {
                if (m_bUseRoiLineView)
                {
                    if (m_DefRect.X != 0 || m_DefRect.Y != 0)
                    {
                        if (getRoiRectList().Count - 1 < 0 || m_DefRect != getRoiRect(getRoiRectList().Count - 1))
                        {
                            m_bLeftRoiBtnDown = false;
                            Rectangle EmptRect = new Rectangle();
                            m_DefRect = EmptRect;
                        }
                    }
                }
                if (m_bUseRndLineView)
                {
                    if (m_PolRect.X != 0 || m_PolRect.Y != 0)
                    {
                        if (getPolRectList().Count - 1 < 0 || m_PolRect != getPolRect(getPolRectList().Count - 1))
                        {
                            m_bLeftRoiBtnDown = false;
                            Rectangle EmptRect = new Rectangle();
                            m_PolRect = EmptRect;
                        }
                    }
                }
                if (m_bUseNonLineView)
                {
                    if (m_MuraRect.X != 0 || m_MuraRect.Y != 0)
                    {
                        if (getMuraRectList().Count - 1 < 0 || m_MuraRect != getNonRect(getMuraRectList().Count - 1))
                        {
                            m_bLeftRoiBtnDown = false;
                            Rectangle EmptRect = new Rectangle();
                            m_MuraRect = EmptRect;
                        }
                    }
                }
                if (m_bUsePadLineView)
                {
                    if (m_PadRect.X != 0 || m_PadRect.Y != 0)
                    {
//                         if (getPadRectList().Count - 1 < 0 || m_PadRect != getPadRect(getPadRectList().Count - 1))
//                         {
                            m_bLeftRoiBtnDown = false;
                            Rectangle EmptRect = new Rectangle();
                            m_PadRect = EmptRect;
                      //  }
                    }
                }
            }
       
            //if (m_bOriginMarkRegister && Control.ModifierKeys == Keys.Control && e.KeyCode == Keys.Z)
            //{
            //    if (getOriginPntList().Count > 0)
            //    {
            //        getOriginPntList().RemoveAt(getOriginPntList().Count - 1);
            //    }
            //}
            //if (m_bAlignMarkRegister && Control.ModifierKeys == Keys.Control && e.KeyCode == Keys.Z)
            //{
            //    if (getAlignPntList().Count > 0)
            //    {                    
            //         getAlignPntList().RemoveAt(getAlignPntList().Count - 1);
            //    }
            //}
            //if (m_bUseRoiLineView && Control.ModifierKeys  == Keys.Control && e.KeyCode == Keys.Z)
            //{                
            //    if (getRoiRectList().Count > 0)
            //        getRoiRectList().RemoveAt(getRoiRectList().Count - 1);
            //    if (!m_RoiRect.IsEmpty)
            //    {
            //        Rectangle EmptRect = new Rectangle();
            //        m_RoiRect = EmptRect;
            //    }                
            //}
            picMainImg.Invalidate();
        }
        private void picMainImg_MouseMove(object sender, MouseEventArgs e)
        {

            if (m_bLeftRoiBtnDown /*&& e.Button == MouseButtons.Right*/)//라인그리기
            {
                Rectangle tempRec = new Rectangle();
                if (m_bUseRoiLineView )
                    tempRec = m_DefRect;
                if (m_bUseRndLineView)
                    tempRec = m_PolRect;
                if (m_bUseNonLineView)
                    tempRec = m_MuraRect;
                if (m_bUsePadLineView)
                    tempRec = m_PadRect;

                tempRec.Width = m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, picMainImg.Width, m_dRratio) - pntRoiClick.X;
                tempRec.Height = m_fnCalCoordinate(m_imgRect.Y, e.Y, m_imgRect.Height, picMainImg.Height, m_dRratio) - pntRoiClick.Y;

               if (e.X - m_imgRect.X < pntRoiClick.X * m_dRratio)
               {
                   tempRec.X = m_fnCalCoordinate(Math.Abs(m_imgRect.X), e.X, m_imgRect.Width, picMainImg.Width, m_dRratio);
                   tempRec.Width = pntRoiClick.X - tempRec.X;
               }
               else tempRec.X = pntRoiClick.X;                   
               
               //Y좌표계산
               if (e.Y - m_imgRect.Y < pntRoiClick.Y * m_dRratio)
               {
                   tempRec.Y = m_fnCalCoordinate(Math.Abs(m_imgRect.X), e.X, m_imgRect.Height, picMainImg.Height, m_dRratio);
                   tempRec.Height = pntRoiClick.Y - tempRec.Y;
               }
               else tempRec.Y = pntRoiClick.Y;


               if (m_bUseRoiLineView)
               {
                   m_DefRect = tempRec;
                   m_fnSetRoiCoord(Consts.END);
               }
               if (m_bUseRndLineView)
               {
                   m_PolRect = tempRec;
                  // m_fnSetPolCoord(Consts.END);
               }
               if (m_bUseNonLineView)
               {
                   m_MuraRect = tempRec;
                //   m_fnSetNonCoord(Consts.END);
               }
               if (m_bUsePadLineView)
               {
                   m_PadRect = tempRec;
                 //  m_fnSetPadCoord(Consts.END);
               }
               
            }
            // 라이브 영상에서의 좌표 이동
            if (m_bLiveFlg && e.Button == MouseButtons.Left && m_bUseDragMove)
            {
                if (m_imgRect.Width <= m_CameraPixelSize.X || m_imgRect.Height <= m_CameraPixelSize.Y && (m_pointClick.X >= 0 && m_pointClick.Y >= 0))
                {
                    //Debug.WriteLine("{0}-{1}", m_imgRect.Width, m_imgRect.Height);
                    Rectangle rectMoveMove = new Rectangle();

                    rectMoveMove.X = (int)Math.Round((Convert.ToDouble(m_imgRect.Width * (m_pointClick.X - Math.Abs(e.X))) / picMainImg.Width));
                    m_imgRect.X = m_imgRect.X + rectMoveMove.X;
                    if (m_imgRect.X < 0) m_imgRect.X = 0;
                    if (m_imgRect.X > m_CameraPixelSize.X - m_imgRect.Width) m_imgRect.X = m_CameraPixelSize.X - m_imgRect.Width;
                    rectMoveMove.Y = (int)Math.Round((Convert.ToDouble(m_imgRect.Height * (m_pointClick.Y - Math.Abs(e.Y))) / picMainImg.Height));
                    m_imgRect.Y = m_imgRect.Y + rectMoveMove.Y;
                    if (m_imgRect.Y < 0) m_imgRect.Y = 0;
                    if (m_imgRect.Y > m_CameraPixelSize.Y - m_imgRect.Height) m_imgRect.Y = m_CameraPixelSize.Y - m_imgRect.Height;
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
            if (!m_bLiveFlg && e.Button == MouseButtons.Left && m_bUseDragMove) //이미지 이동
            {
                m_imgRect.X = m_imgRect.X + (int)Math.Round((double)(e.X - m_pointClick.X));

                if (m_imgRect.X >= 0) m_imgRect.X = 0;

                if (Math.Abs(m_imgRect.X) >= Math.Abs(m_imgRect.Width - picMainImg.Width)) m_imgRect.X = -(m_imgRect.Width - picMainImg.Width);

                m_imgRect.Y = m_imgRect.Y + (int)Math.Round((double)(e.Y - m_pointClick.Y));

                if (m_imgRect.Y >= 0) m_imgRect.Y = 0;

                if (Math.Abs(m_imgRect.Y) >= Math.Abs(m_imgRect.Height - picMainImg.Height)) m_imgRect.Y = -(m_imgRect.Height - picMainImg.Height);

                if (hScrollBar1.Maximum >= Math.Abs(m_imgRect.X)) hScrollBar1.Value = Math.Abs(m_imgRect.X);

                if (vScrollBar1.Maximum >= Math.Abs(m_imgRect.Y)) vScrollBar1.Value = Math.Abs(m_imgRect.Y);
                                
                m_pointClick.X = e.X;
                m_pointClick.Y = e.Y;
                
            }

            if (m_imgLoaded != null)
            {
                m_stCurCoord.X1.Text = m_fnCalCoordinate(m_imgRect.X, e.X, m_CameraPixelSize.X, picMainImg.Width, m_dRratio).ToString();
                m_stCurCoord.Y1.Text = m_fnCalCoordinate(m_imgRect.Y, e.Y, m_CameraPixelSize.Y, picMainImg.Height, m_dRratio).ToString();
                m_fnSetCurRGBVal(e.X, e.Y);
            }
            m_LastPoint = e.Location;
            
           
            picMainImg.Invalidate();
          
        }

        private void picMainImg_MouseClick(object sender, MouseEventArgs e)
        {

            if ((e.Button == MouseButtons.Right && m_bAlignMarkRegister))
            {
                if (getAlignPntList().Count < 0 || 2 <= (getAlignPntList().Count))
                {
                    return;
                }
                if (!m_bLeftAlignBtnDown)
                {                   
                    m_bLeftAlignBtnDown = true;
                    pntAlignClick.X = m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, picMainImg.Width, m_dRratio) ;
                    pntAlignClick.Y = m_fnCalCoordinate(m_imgRect.Y,  e.Y, m_imgRect.Height, picMainImg.Height, m_dRratio) ;
                    m_fnSetAlignCoord(1);                                            
                    m_AlignPntList.Add(pntAlignClick);                               
                    this.Cursor = Cursors.Default;                                   
                }                                                                    
                else                                                                 
                {                                                                    
                    pntAlignClick.X = m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, picMainImg.Width, m_dRratio) ;
                    pntAlignClick.Y = m_fnCalCoordinate(m_imgRect.Y,  e.Y, m_imgRect.Height, picMainImg.Height, m_dRratio) ;                  
                    m_fnSetAlignCoord(2);
                    m_AlignPntList.Add(pntAlignClick);  
                    m_bLeftAlignBtnDown = false;
                    this.Cursor = Cursors.Default;
                }               
            }

            else if (e.Button == MouseButtons.Right && m_bOriginMarkRegister)
            {

                if (getOriginPntList().Count > 0)
                {
                    getOriginPntList().RemoveAt(getOriginPntList().Count - 1);
                }
                    pntOriginClick.X = m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, picMainImg.Width, m_dRratio);
                    pntOriginClick.Y = m_fnCalCoordinate(m_imgRect.Y,  e.Y, m_imgRect.Height, picMainImg.Height, m_dRratio);
                    m_fnSetOriginCoord();
                    m_OriginPointList.Add(pntOriginClick);
                    this.Cursor = Cursors.Default;
                //}                
            }

            // ROI 등록
            else if (e.Button == MouseButtons.Right)
            {
                if (!m_bLeftRoiBtnDown)
                {                    
                    m_bLeftRoiBtnDown = true;
                    this.Cursor = Cursors.Cross;
                    pntRoiClick.X = m_fnCalCoordinate(m_imgRect.X, e.X, m_imgRect.Width, picMainImg.Width, m_dRratio);
                    pntRoiClick.Y = m_fnCalCoordinate(m_imgRect.Y,  e.Y, m_imgRect.Height, picMainImg.Height, m_dRratio);
                    if (m_bUseRoiLineView)
                        m_fnSetRoiCoord(Consts.START);
                    if (m_bUseRndLineView)
                        m_fnSetPolCoord(Consts.START);
                    if (m_bUseNonLineView)
                        m_fnSetNonCoord(Consts.START);
                    if (m_bUsePadLineView)
                    {
                        m_penPad.DashStyle = System.Drawing.Drawing2D.DashStyle.Dot;
                        m_fnSetPadCoord(Consts.START);
                    }
                 
                }
                else
                {
                    if (m_bUseRoiLineView)
                        m_fnSetRoiCoord(Consts.END);
                    if (m_bUseRndLineView)
                        m_fnSetPolCoord(Consts.END);
                    if (m_bUseNonLineView)
                        m_fnSetNonCoord(Consts.END);
                    if (m_bUsePadLineView)
                        m_fnSetPadCoord(Consts.END);
                    if (m_bUseRoiMultiLine)
                        setRoiRectList(m_DefRect);
                  
                    m_bLeftRoiBtnDown = false;
                    this.Cursor = Cursors.Default;
                }               
            }

            if (e.Button == MouseButtons.Left)
            {
                if (m_bUseSelCoordView)
                {                   
                    m_fnSetSelCoord(Consts.START);
                }
            }
                    
        }

     
        public void m_fnSetLiveZoomInOut(int lines, int nX, int nY)
        {
            try
            {
                PictureBox pb = picMainImg;

                m_dRecRatio = 1;

                if (lines > 0)
                {
                    m_dRratio += 1.0F;
                    m_dRratio = (float)Math.Round(m_dRratio, 5);
                    if (m_dRratio > 100.0) m_dRratio = 100.0F;

                }
                else if (lines < 0)
                {
                    m_dRratio -= 1.0F;
                    m_dRratio = (float)Math.Round(m_dRratio, 5);
                    if (m_dRratio <= 1.0F)
                        m_dRratio =1.0F;
                }


                Rectangle adjustedArea = new Rectangle();

                Point pntOldCoord = new Point();
                pntOldCoord.X = m_imgRect.Width;
                pntOldCoord.Y = m_imgRect.Height;
                adjustedArea.Width = (int)Math.Round( m_CameraPixelSize.X / ( m_dRratio));
                adjustedArea.Height = (int)Math.Round( m_CameraPixelSize.Y / ( m_dRratio));

              
                 adjustedArea.X = (int)m_imgRect.X + ((pntOldCoord.X - adjustedArea.Width) / 2);
                 adjustedArea.Y = (int)m_imgRect.Y + ((pntOldCoord.Y - adjustedArea.Height) / 2); //m_imgRect.Y + (m_imgRect.Height / 2);
              
             
                if (adjustedArea.X < 0) adjustedArea.X = 0;
                if (adjustedArea.X > m_CameraPixelSize.X) adjustedArea.X = m_CameraPixelSize.X;
                if (adjustedArea.Y < 0) adjustedArea.Y = 0;
                if (adjustedArea.Y > m_CameraPixelSize.Y) adjustedArea.Y = m_CameraPixelSize.Y;

                if (m_CameraPixelSize.X <= adjustedArea.Width) adjustedArea.X = 0;
                if (m_CameraPixelSize.Y <= adjustedArea.Height) adjustedArea.Y = 0;
                m_imgRect = adjustedArea;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
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
               // Rect.X = (int)Math.Round(((Rect.X + mouseLoc.X) * dRecRatio) - (picMainImg.Width / 2));
               // Rect.Y = (int)Math.Round(((Rect.Y + mouseLoc.Y) * dRecRatio) - (picMainImg.Height / 2));
                Rect.X = (int)Math.Round((Rect.X * dRecRatio) - (picMainImg.Width / 2));
                Rect.Y = (int)Math.Round((Rect.Y * dRecRatio) - (picMainImg.Height / 2));

            }
            else if (m_dRecRatio < 1)
            {
               // Rect.X = (int)Math.Round(((Rect.X + mouseLoc.X) * dRecRatio) - (picMainImg.Width / 2));
               // Rect.Y = (int)Math.Round(((Rect.Y + mouseLoc.Y) * dRecRatio) - (picMainImg.Height / 2));
                Rect.X = (int)Math.Round((Rect.X + (picMainImg.Width / 2)) * dRecRatio);
                Rect.Y = (int)Math.Round((Rect.Y + (picMainImg.Height / 2)) * dRecRatio);
            }

            if (Rect.X > 0) Rect.X = 0;
            if (Rect.Y > 0) Rect.Y = 0;

            return;
        }

        public void m_fnLinkOriginCoord(ref Label reflbCoordX, ref Label reflbCoordY)
        {
            m_stOrgCoord.X1 = reflbCoordX;
            m_stOrgCoord.Y1 = reflbCoordY;
        }

        public void m_fnSetOriginCoord()
        {          
           m_stOrgCoord.X1.Text = m_stCurCoord.X1.Text;
           m_stOrgCoord.Y1.Text = m_stCurCoord.Y1.Text;
        }

        public void m_fnLinkAlignOneCoord(ref Label reflbCoordX, ref Label reflbCoordY)
        {
            m_stAlnCoord.X1 = reflbCoordX;
            m_stAlnCoord.Y1 = reflbCoordY;
        }
        public void m_fnLinkAlignTwoCoord(ref Label reflbCoordX, ref Label reflbCoordY)
        {
            m_stAlnCoord.X2 = reflbCoordX;
            m_stAlnCoord.Y2 = reflbCoordY;
        }

        public void m_fnSetAlignCoord(int nMarkNo)
        {
            if (nMarkNo == 1)
            {
                m_stAlnCoord.X1.Text = m_stCurCoord.X1.Text;
                m_stAlnCoord.Y1.Text = m_stCurCoord.Y1.Text;
                
            }
            else
            {
                m_stAlnCoord.X2.Text = m_stCurCoord.X1.Text;
                m_stAlnCoord.Y2.Text = m_stCurCoord.Y1.Text;
            }
        }
        public void m_fnLinkRoiCoord(ref Label reflbCoordX1, ref Label reflbCoordY1, ref Label reflbCoordX2, ref Label reflbCoordY2)
        {
            m_stRoiCoord.X1 = reflbCoordX1;
            m_stRoiCoord.Y1 = reflbCoordY1;
            m_stRoiCoord.X2 = reflbCoordX2;
            m_stRoiCoord.Y2 = reflbCoordY2;
        }
        public void  m_fnLinkRndCoord(ref Label reflbCoordX1, ref Label reflbCoordY1, ref Label reflbCoordX2, ref Label reflbCoordY2)
        {
            m_stPolCoord.X1 = reflbCoordX1;
            m_stPolCoord.Y1 = reflbCoordY1;
            m_stPolCoord.X2 = reflbCoordX2;
            m_stPolCoord.Y2 = reflbCoordY2;
        }
        public void  m_fnLinkNonCoord(ref Label reflbCoordX1, ref Label reflbCoordY1, ref Label reflbCoordX2, ref Label reflbCoordY2)
        {
            m_stNonCoord.X1 = reflbCoordX1;
            m_stNonCoord.Y1 = reflbCoordY1;
            m_stNonCoord.X2 = reflbCoordX2;
            m_stNonCoord.Y2 = reflbCoordY2;
        }
        public void  m_fnLinkPadCoord(ref Label reflbCoordX1, ref Label reflbCoordY1, ref Label reflbCoordX2, ref Label reflbCoordY2)
        {
            m_stPadCoord.X1 = reflbCoordX1;
            m_stPadCoord.Y1 = reflbCoordY1;
            m_stPadCoord.X2 = reflbCoordX2;
            m_stPadCoord.Y2 = reflbCoordY2;
        }
        private void m_fnSetRoiCoord(bool bStartEnd)
        {
            if (bStartEnd)
            {
                m_stRoiCoord.X1.Text =  m_stCurCoord.X1.Text;
                m_stRoiCoord.Y1.Text = m_stCurCoord.Y1.Text;
            }
            else 
            {
                m_stRoiCoord.X2.Text = m_stCurCoord.X1.Text;        //m_RoiRect.Width.ToString();// 
                m_stRoiCoord.Y2.Text = m_stCurCoord.Y1.Text;        //m_RoiRect.Height.ToString();//
            }           
        }
        private void m_fnSetPolCoord(bool bStartEnd)
        {
            if (bStartEnd)
            {
                m_stPolCoord.X1.Text = m_stCurCoord.X1.Text;
                m_stPolCoord.Y1.Text = m_stCurCoord.Y1.Text;
            }
            else
            {
                m_stPolCoord.X2.Text = m_stCurCoord.X1.Text;// m_PolRect.Width.ToString();// 
                m_stPolCoord.Y2.Text = m_stCurCoord.Y1.Text;// m_PolRect.Height.ToString();//
            }
        }
        private void m_fnSetNonCoord(bool bStartEnd)
        {
            if (bStartEnd)
            {
                m_stNonCoord.X1.Text = m_stCurCoord.X1.Text;
                m_stNonCoord.Y1.Text = m_stCurCoord.Y1.Text;
            }
            else 
            {
                m_stNonCoord.X2.Text = m_stCurCoord.X1.Text;//m_NonRect.Width.ToString();// 
                m_stNonCoord.Y2.Text = m_stCurCoord.Y1.Text;//m_NonRect.Height.ToString();//
            }
        }
         private void m_fnSetSelCoord(bool bStartEnd)
        {
            if (bStartEnd)
            {
                m_stSelCoord.X1.Text = m_stCurCoord.X1.Text;
                m_stSelCoord.Y1.Text = m_stCurCoord.Y1.Text;
            }
            else 
            {
                m_stSelCoord.X2.Text = m_stCurCoord.X1.Text;//m_NonRect.Width.ToString();// 
                m_stSelCoord.Y2.Text = m_stCurCoord.Y1.Text;//m_NonRect.Height.ToString();//
            }
        }
        
        private void m_fnSetPadCoord(bool bStartEnd)
        {
            if (bStartEnd)
            {
                m_stPadCoord.X1.Text = m_stCurCoord.X1.Text;
                m_stPadCoord.Y1.Text = m_stCurCoord.Y1.Text;
            }
            else 
            {
                m_stPadCoord.X2.Text = m_stCurCoord.X1.Text;// m_PadRect.Width.ToString();// 
                m_stPadCoord.Y2.Text = m_stCurCoord.Y1.Text;// m_PadRect.Height.ToString();//
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
        public void m_fnLinkSelCoord(ref Label reflbCoordX, ref Label reflbCoordY)
        {
            m_stSelCoord.X1 = reflbCoordX;
            m_stSelCoord.Y1 = reflbCoordY;
        }
        
        public void m_fnLinkCurRGBVal(ref Label reflbR, ref Label reflbG, ref Label reflbB)
        {
            m_stRGBColor[0] = reflbR;
            m_stRGBColor[1] = reflbG;
            m_stRGBColor[2] = reflbB;                       
        }

        public void m_fnLinkCurFocusVal(ref Label refFocus)
        {
            m_stFocus = refFocus;

        }

        public void m_fnSetCurRGBVal(int nX, int nY)
        {
            int nXCoord = Convert.ToInt32(m_stCurCoord.X1.Text);
            int nYCoord = Convert.ToInt32(m_stCurCoord.Y1.Text);

            if (nXCoord < 0 || nYCoord < 0) return;
            if (nXCoord > m_imgLoaded.Width || nYCoord > m_imgLoaded.Height) return;

           try 
           {
               Color color ;
               if(m_bLiveFlg) 
                   color = (m_imgLoaded).GetPixel(nX, nY);
               else 
                   color = (m_imgLoaded).GetPixel(nXCoord, nYCoord);
               m_stRGBColor[0].Text = color.R.ToString();
               m_stRGBColor[1].Text = color.G.ToString();
               m_stRGBColor[2].Text = color.B.ToString();
            }
               catch { return; } 
        }

        public void m_fnMiniMapMove(Image iMapimg, int nSelMapX, int nSelMapY)
        {
            int nMapX = 0, nMapY = 0;
            if (nSelMapX > 0)
                nMapX = (int) Math.Round(m_imgLoaded.Width * nSelMapX / iMapimg.Width * m_dRratio);

            if (nSelMapY > 0)
                nMapY = (int)Math.Round(m_imgLoaded.Height * nSelMapY / iMapimg.Height * m_dRratio);

            if (nMapX > hScrollBar1.Maximum)
                hScrollBar1.Value = hScrollBar1.Maximum;
            else
                hScrollBar1.Value = nMapX;

            if (nMapY > vScrollBar1.Maximum)
                vScrollBar1.Value = vScrollBar1.Maximum;
            else
                vScrollBar1.Value = nMapY;

            m_imgRect.Y = vScrollBar1.Value * (-1);
            m_imgRect.X = hScrollBar1.Value * (-1);
            if (m_imgLoaded != null)
            {
                m_stCurCoord.X1.Text = m_fnCalCoordinate(m_imgRect.X, 0, m_imgRect.Width, picMainImg.Width, m_dRratio).ToString();
                m_stCurCoord.X1.Text = m_fnCalCoordinate(m_imgRect.Y, 0, m_imgRect.Height, picMainImg.Height,m_dRratio).ToString();
            }       
            picMainImg.Invalidate();
        }

        
        
        public delegate List<Rectangle> DelDrawRect();
        public delegate List<Pen> DelDrawPen();
        public delegate List<Point> DelDrawPoint();
        delegate void ThreadViewCallback();
      
        delegate void ThreadIPaintCallback(object sender, PaintEventArgs e);
        private void picMainImg_Paint(object sender, PaintEventArgs e)
        {
           // Graphics g = picMainImg.CreateGraphics();

            //Graphics g = Graphics.FromImage(picMainImg.Image);
            try 
            {
                if (this.InvokeRequired)
                {

                    ThreadIPaintCallback delOpenImg = new ThreadIPaintCallback(picMainImg_Paint);
                    this.Invoke(delOpenImg, sender, e);
                    return;
                }

                if (m_imgLoaded != null)
                {
                    //                 e.InterpolationMode =
                    //                    System.Drawing.Drawing2D.InterpolationMode.HighQualityBicubic;
                    e.Graphics.InterpolationMode =
                        System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
                    e.Graphics.SmoothingMode =
                        System.Drawing.Drawing2D.SmoothingMode.None;

                    if (m_bLiveFlg)
                    {
                        Rectangle tmp = new Rectangle(0, 0, picMainImg.Width, picMainImg.Height);
                        e.Graphics.DrawImage(m_imgLoaded, tmp);
                    }
                    else
                    {
                        e.Graphics.DrawImage(m_imgLoaded, m_imgRect);
                    }

                    // e.DrawImage(m_imgLoaded, picMainImg.ClientRectangle, m_imgRect, GraphicsUnit.Pixel);

                    // PAD 영역 표시 유무 플래그 필요               
                    e.Graphics.DrawRectangle(m_penPad, CalcRectCoord(m_PadRect));
                    //m_fnDrawRec(e.Graphics, m_penPad, new DelDrawRect(getPadRectList), m_PadRect);

                    // POL 마크 영역 표시 유무 플래그 필요

                    m_fnDrawRec(e.Graphics, new DelDrawPen(getPnt_Line_PenList), new DelDrawRect(getPolRectList), m_PolRect);

                    // AD 영역 표시
                    m_fnDrawAdRec(e.Graphics,new DelDrawPen(getPnt_Line_PenList), new DelDrawRect(getRoiRectList), m_DefRect);

                    // Point, Line 표시
                    m_fnDrawRec(e.Graphics, new DelDrawPen(getPnt_Line_PenList), new DelDrawRect(getRoiRectList), m_DefRect);

                    // Align 표시 유무 플래그 필요
                    String drawString = string.Format("Align Mark ");
                    m_fnDrawLine(e.Graphics, m_penAlign, new DelDrawPoint(getAlignPntList), drawString);

                    // Mura 영역 표시
                    m_fnDrawRec(e.Graphics, new DelDrawPen(getMura_PenList), new DelDrawRect(getMuraRectList), m_MuraRect);

                    // 클릭 포인트 표시                                            
                    m_fnDrawLine(e.Graphics, m_penOrigin, new DelDrawPoint(getOriginPntList));

                    if (vScrollBar1.Value >= vScrollBar1.Maximum)
                        vScrollBar1.Visible = false;
                    else vScrollBar1.Visible = true;
                    if (hScrollBar1.Value >= hScrollBar1.Maximum)
                        hScrollBar1.Visible = false;
                    else hScrollBar1.Visible = true;


                }
            }
           catch{}

        }

        private void m_fnDrawPanelShape(Graphics grp, Pen penPnt, DelDrawPoint listPnt)
        {
            foreach(Point point in listPnt())
                grp.DrawRectangle(penPnt, new Rectangle(point.X, point.Y, 1, 1));
        }

        public void m_fnDrawLine(Graphics grp, Pen penPnt, DelDrawPoint listPnt)
        {

            foreach (Point rect in listPnt())
            {
                grp.DrawLine(penPnt, CalcLineCoord(rect).X,
                                         CalcLineCoord(rect).Y - (7),
                                         CalcLineCoord(rect).X,
                                         CalcLineCoord(rect).Y + (7)
                             );

                grp.DrawLine(penPnt, CalcLineCoord(rect).X - (7),
                                         CalcLineCoord(rect).Y,
                                         CalcLineCoord(rect).X + (7),
                                         CalcLineCoord(rect).Y
                            );
            }
        }
     
        public void m_fnDrawLine(Graphics grp, Pen penPnt, DelDrawPoint listPnt, string strDescription)
        {
            int nCnt = 0;
            try
            {                
                int nFontOffSet = (int)Math.Round(7 * m_dRratio);
                int nStrRectOffSet = (int)Math.Round(140 / m_dRratio);
                foreach (Point rect in listPnt())
                {
                    string strtmp = strDescription + string.Format("{0}", ++nCnt);
                    Font font = new Font("Gadugi", nFontOffSet < 7 ? 7 : nFontOffSet, FontStyle.Bold, GraphicsUnit.Point);
                    Rectangle drawRect = CalcRectCoord(new Rectangle(rect.X, rect.Y,
                                                        140 + ((int)Math.Round(140 / m_dRratio) < 0 ? 0 : (int)Math.Round(140 / m_dRratio)),
                                                         30 + ((int)Math.Round(30 / m_dRratio) < 0 ? 0 : (int)Math.Round(30 / m_dRratio))
                                                        ));// (int)Math.Round(140 / m_dRratio), (int)Math.Round(20 / m_dRratio)));    
                    drawRect.X += (int)Math.Round(10 * m_dRratio); ;
                    StringFormat stringFormat = new StringFormat();
                    stringFormat.Alignment = StringAlignment.Near;                    
                    stringFormat.LineAlignment = StringAlignment.Near;
                    SolidBrush drawBrush = new SolidBrush(penPnt.Color);
                    grp.DrawString(strtmp, font, drawBrush, drawRect, stringFormat);
                    
                    penPnt.Width = (int)Math.Round(3 * m_dRratio);
                    grp.DrawLine(penPnt, CalcLineCoord(rect).X,
                                             CalcLineCoord(rect).Y - (int)Math.Round(25 * m_dRratio),
                                             CalcLineCoord(rect).X,
                                             CalcLineCoord(rect).Y + (int)Math.Round(25 * m_dRratio)
                                 );

                    grp.DrawLine(penPnt, CalcLineCoord(rect).X - (int)Math.Round(25 * m_dRratio),
                                             CalcLineCoord(rect).Y,
                                             CalcLineCoord(rect).X + (int)Math.Round(25 * m_dRratio),
                                             CalcLineCoord(rect).Y
                                );
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
            drawRect.X = (m_imgLoaded.Width * Rect.X / m_CameraPixelSize.X);
            drawRect.Y = (m_imgLoaded.Height * Rect.Y / m_CameraPixelSize.Y);
            return drawRect;
        }
        public void ClearOriginPntList()
        {
            m_OriginPointList.Clear();
        }
        public void setOriginPntList(Point rect)
        {
            m_OriginPointList.Add(rect);
        }
        public List<Point> getPanelShapeDataList()
        {
            return m_PanelShapePointList;
        }
        public List<Point> getOriginPntList()
        {
            return m_OriginPointList;
        }
        public Point getOriginnPnt(int nAlignNo)
        {
            if (nAlignNo >= 0)
                return m_OriginPointList[nAlignNo];
            else
            {
                Point retRec = new Point();
                return retRec;
            }
        }
        public void ClearAlignPntList()
        {   
            m_AlignPntList.Clear();
        }
        public void setAlignPntList(Point rect)
        {
            m_AlignPntList.Add(rect);
        }
        public List<Point> getAlignPntList()
        {
            return m_AlignPntList;
        }
        public Point getAlignPnt(int nAlignNo)
        {
            if (nAlignNo >= 0)
                return m_AlignPntList[nAlignNo];
            else
            {
                Point retRec = new Point();
                return retRec;
            }
        }
        public void m_fnDrawAdRec(Graphics grp, DelDrawPen penRect, DelDrawRect listRect, Rectangle RoiRect)
        {
            int nClrCnt = 0;
            foreach (Rectangle rect in listRect())
            {
                //penRect.DashStyle = System.Drawing.Drawing2D.DashStyle.Solid;
                if (Consts.DEFECT_MAP_VIEW_OFFSET * -1 >= rect.X && Consts.DEFECT_MAP_VIEW_OFFSET >= rect.Width
                    && Consts.DEFECT_MAP_VIEW_OFFSET * -1 >= rect.Y && Consts.DEFECT_MAP_VIEW_OFFSET >= rect.Height)
                {
                    // Fill rectangle to screen.
                    grp.FillRectangle(new SolidBrush(penRect()[nClrCnt].Color), (new Rectangle(0, 0, this.picMainImg.Width, this.picMainImg.Height)));
                    break;
                }
                nClrCnt++;
            }
        }
        public void m_fnDrawRec(Graphics grp, DelDrawPen penRect, DelDrawRect listRect, Rectangle RoiRect)
        {  
//             if (lstRect.Count > 30000) return;                    
            int nClrCnt = 0;
            foreach (Rectangle rect in listRect())
            {
                //penRect.DashStyle = System.Drawing.Drawing2D.DashStyle.Solid;
                //if (m_CameraPixelSize.X + 5 >= rect.Width && m_CameraPixelSize.Y + 5 >= rect.Height)               
                if (Consts.DEFECT_MAP_VIEW_OFFSET < rect.Width
                    && Consts.DEFECT_MAP_VIEW_OFFSET < rect.Height)                
                {
                    // Fill rectangle to screen.
                    grp.FillRectangle(new SolidBrush(penRect()[nClrCnt].Color), CalcRectCoord(rect));
                }
                nClrCnt++;
            }
        }
       
        private Rectangle CalcRectCoord(Rectangle Rect)
        {
           // Rectangle adjustedArea = new Rectangle();

            if (m_CameraPixelSize.X == 0 || (m_CameraPixelSize.Y == 0)) //new Rectangle(0, 0, 0, 0);
            {
                m_CameraPixelSize.X = 1;
                m_CameraPixelSize.Y = 1;
            }
            Rectangle drawrec = new Rectangle();
            drawrec.X = (m_imgLoaded.Width * Rect.X / m_CameraPixelSize.X) - 1;
            drawrec.Y = (m_imgLoaded.Height * Rect.Y / m_CameraPixelSize.Y) - 1;
            drawrec.Width = Math.Abs(drawrec.X - (m_imgLoaded.Width * Rect.Width / m_CameraPixelSize.X)) ;
            drawrec.Height = Math.Abs(drawrec.Y - (m_imgLoaded.Height * Rect.Height / m_CameraPixelSize.Y)) ;
//             drawrec.X = (int)Math.Round((m_imgRect.X) + (Rect.X * m_dRratio));
//             drawrec.Y = (int)Math.Round((m_imgRect.Y) + (Rect.Y * m_dRratio));
//             drawrec.Width = (int)Math.Round(Rect.Width * m_dRratio);
//             drawrec.Height = (int)Math.Round(Rect.Height * m_dRratio);

            return drawrec;
        }

        /// <summary>
        /// ROI Rect List
        /// </summary>
        public void ClearRoiRectList()
        {
            Rectangle EmptRect = new Rectangle();
            m_DefRect = EmptRect;
            m_DefRectList.Clear();
        }
        public void setRoiRectList(Rectangle rect)
        {
            m_DefRectList.Add(rect);
        }
        public List<Rectangle> getRoiRectList()
        {
            return m_DefRectList;
        }
        public List<Pen> getPnt_Line_PenList()
        {
            return m_Pnt_Line_PenList;
        }
        public List<Pen> getMura_PenList()
        {
            return m_Mura_PenList;
        }
        public Rectangle getRoiRect(int nRoiNo)
        {
            if (nRoiNo >= 0)  
                return m_DefRectList[nRoiNo];
            else
            {
                Rectangle retRec = new Rectangle();
                return retRec;
            }
        }
        /// <summary>
        /// POL Rect List
        /// </summary>
        public void ClearPolRectList()
        {
            Rectangle EmptRect = new Rectangle();
            m_PolRect = EmptRect;
            m_PolRectList.Clear();
        }
        public void setPolRectList(Rectangle rect)
        {
            m_PolRectList.Add(rect);
        }
        public List<Rectangle> getPolRectList()
        {
            return m_PolRectList;
        }
        public Rectangle getPolRect(int nNo)
        {
            if (nNo >= 0)
                return m_PolRectList[nNo];
            else
            {
                Rectangle retRec = new Rectangle();
                return retRec;
            }
        }
        /// <summary>
        /// NON Rect List
        /// </summary>
        public void ClearNonRectList()
        {
            Rectangle EmptRect = new Rectangle();
            m_MuraRect = EmptRect;
            m_MuraRectList.Clear();
        }
        public void setMuraRectList(Rectangle rect)
        {
            m_MuraRectList.Add(rect);
        }
        public List<Rectangle> getMuraRectList()
        {
            return m_MuraRectList;
        }
        public Rectangle getNonRect(int nNo)
        {
            if (nNo >= 0)
                return m_MuraRectList[nNo];
            else
            {
                Rectangle retRec = new Rectangle();
                return retRec;
            }
        }
        /// <summary>
        /// PAD Rect List
        /// </summary>
        public void ClearPadRectList()
        {
            Rectangle EmptRect = new Rectangle();
            m_PadRect = EmptRect;
            m_PadRectList.Clear();
        }
        public void setPadRectList(Rectangle rect)
        {
            m_PadRectList.Add(rect);
        }
        public List<Rectangle> getPadRectList()
        {
            return m_PolRectList;
        }
        public Rectangle getPadRect(int nNo)
        {
            if (nNo >= 0)
                return m_PadRectList[nNo];
            else
            {
                Rectangle retRec = new Rectangle();
                return retRec;
            }
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
            Rectangle EmptRect = new Rectangle();
            m_DefRect = EmptRect;
            m_DefRectList.Clear();

           
            m_PolRect = EmptRect;
            m_PolRectList.Clear();

            
            m_MuraRect = EmptRect;
            m_MuraRectList.Clear();

            
//             m_PadRect = EmptRect;
//             m_PadRectList.Clear();
        }
        /// <summary>
        /// 기  능 : 레시피 데이터의 ROI 좌표를 등록
        /// /반환값 : 
        /// /입  력 : int nX, int nY
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        /// <summary>
        /// 기  능 : 레시피 데이터의 원점 좌표를 등록
        /// /반환값 : 
        /// /입  력 : int nX, int nY
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Uctrl_MainPic      
        /// </summary>
        public void SelDefectCoord(int nX, int nY)
        {
            if (m_imgLoaded != null)
            {
                Point pntCoord = new Point(nX, nY);
                ClearOriginPntList();
                setOriginPntList(pntCoord);
                picMainImg.Invalidate();
            }            
        }
        /// <summary>
        /// 기  능 : 레시피 데이터의 ROI 좌표를 등록
        /// /반환값 : 
        /// /입  력 : int nX, int nY
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void DrawDefectMap(List<List<Rectangle>> AreaList)
        {
            try
            {
                if (m_imgLoaded != null)
                {
                    List<Rectangle> tmpLst = new List<Rectangle>();
                    if (AreaList.Count() >= 1) m_DefRectList = AreaList[0];
                    else m_DefRectList = tmpLst;
                    if (AreaList.Count() >= 2) m_PolRectList = AreaList[1];
                    else m_PolRectList = tmpLst;
                    if (AreaList.Count() >= 3) m_MuraRectList = AreaList[2];
                    else m_MuraRectList = tmpLst;
                 
                    m_DefRect = new Rectangle(0, 0, 0, 0);
                    m_PolRect = new Rectangle(0, 0, 0, 0);
                    m_MuraRect = new Rectangle(0, 0, 0, 0);
                    //picMainImg_View();
                    picMainImg.Invalidate();
                }
             }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
                //Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
            }
        }

        /// <summary>
        /// 기  능 : 레시피 데이터의 ROI 좌표를 등록
        /// /반환값 : 
        /// /입  력 : int nX, int nY
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void DrawDefectMap(Dictionary<Rectangle[], Pen[]> AreaDic)
        {
            try
            {
                if (m_imgLoaded != null)
                {
                    List<Rectangle[]> AreaList = new List<Rectangle[]>();
                    List<Pen[]> PenList = new List<Pen[]>();                    
                    
                    foreach (KeyValuePair<Rectangle[], Pen[]> kv in AreaDic)
                    {
                        AreaList.Add(kv.Key);
                        PenList.Add(kv.Value);
                    }

                    List<Rectangle> tmpLst = new List<Rectangle>();
                    List<Pen> tmpPenLst = new List<Pen>();
                    if (AreaList.Count() >= 1)
                    {
                        m_DefRectList = AreaList[0].OfType<Rectangle>().ToList();
                        m_Pnt_Line_PenList = PenList[0].OfType<Pen>().ToList();
                    }
                    else
                    {
                        m_DefRectList = tmpLst;
                        m_Pnt_Line_PenList = tmpPenLst;
                    }
//                     if (AreaList.Count() >= 2) m_PolRectList = AreaList[1].OfType<Rectangle>().ToList();
//                     else m_PolRectList = tmpLst;
                    if (AreaList.Count() >= 2)
                    {
                        m_MuraRectList = AreaList[1].OfType<Rectangle>().ToList();
                        m_Mura_PenList = PenList[1].OfType<Pen>().ToList();
                    }
                    else
                    {
                        m_MuraRectList = tmpLst;
                       // m_Pnt_Line_PenList = tmpPenLst;                    
                    }

                    m_DefRect = new Rectangle(0, 0, 0, 0);
                    m_PolRect = new Rectangle(0, 0, 0, 0);
                    m_MuraRect = new Rectangle(0, 0, 0, 0);
                    //picMainImg_View();
                    picMainImg.Invalidate();
                }
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
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
        public void DataToSetPadArea(Rectangle PadList)
        {
            try
            {
                if (m_imgLoaded == null) return;
                m_PadRect = PadList;
                picMainImg.Invalidate();
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
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
        public void DataToSetPadArea(Rectangle PadList, string strSavePath, bool bUseSaveImg = false)
        {
            try
            {
                if (m_imgLoaded == null) return;
                m_PadRect = PadList;

                Bitmap bit = new Bitmap(this.m_imgLoaded, this.m_imgLoaded.Width, m_imgLoaded.Height);

                using (Graphics grPhoto = Graphics.FromImage(bit))
                {
                    if (bUseSaveImg && !PadList.IsEmpty)
                    {             
                        // 이미지 저장
                        Bitmap convertImage = new Bitmap(PadList.Width, PadList.Height);                       
                        Graphics g = Graphics.FromImage(convertImage);
                        Rectangle rectSave = new Rectangle(PadList.X, PadList.Y, Math.Abs(PadList.X + PadList.Width), Math.Abs(PadList.Y + PadList.Height));                        
                        g.DrawImage(m_imgLoaded,
                         new Rectangle(m_PadRect.X * -1, m_PadRect.Y * -1, m_imgLoaded.Width, m_imgLoaded.Height)
                            //,
                            //(m_PadRect),
                            //GraphicsUnit.Pixel
                            );

                        convertImage.Save(@strSavePath, System.Drawing.Imaging.ImageFormat.Bmp);
                        convertImage.Dispose();
                        m_penPad.DashStyle = System.Drawing.Drawing2D.DashStyle.Solid;
                    }
                  
                }
                picMainImg.Invalidate();
                //
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
                //Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
            }
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
        /// 기  능 : 현재 이미지를 해당 경로에 저장한다.
        /// /반환값 : 
        /// /입  력 : string strSavePath
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        /// 
        public int ImageSave(string strSavePath, float fAngle = 0.0F)
        {
            int nRet = Consts.FALSE;
            try
            {                
                if (m_imgLoaded == null) return Consts.FALSE;


                //Systems.m_fnAddLog(Globals.SelPcNo, (int)Enums.LogLevel.ALL, string.Format("SavePath : {0}", strSavePath));

                Bitmap bmp = new Bitmap(picMainImg.Width, picMainImg.Height);
                using (Graphics g = Graphics.FromImage(bmp))
                {
                    if (m_imgLoaded != null)
                    {
                        g.InterpolationMode =
                            System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
                        g.SmoothingMode =
                            System.Drawing.Drawing2D.SmoothingMode.None;

                        g.DrawImage(m_imgLoaded, m_imgRect);

                        // AD 영역 표시
                        m_fnDrawAdRec(g,new DelDrawPen(getPnt_Line_PenList), new DelDrawRect(getRoiRectList), m_DefRect);

                        // Point, Line 표시
                        m_fnDrawRec(g, new DelDrawPen(getPnt_Line_PenList), new DelDrawRect(getRoiRectList), m_DefRect);

                        // Mura 영역 표시
                        m_fnDrawRec(g, new DelDrawPen(getMura_PenList), new DelDrawRect(getMuraRectList), m_MuraRect);

                       

                        if (getPanelShapeDataList() == null)
                        {
                           // Systems.m_fnAddLog(m_nVisionNo, string.Format("PanelShapeDataList Data Null.."));
                            InitPanelShapeData(Color.Black);
                        }
                        else
                        {
                            //Systems.m_fnAddLog(m_nVisionNo, string.Format("PanelShapeDataList Data : {0}", m_PanelShapePointList.Count.ToString()));
                            
                        }
                        
                        // Panel Shape 표시
                        if (m_penPanalShape != null)
                            m_fnDrawPanelShape(g, m_penPanalShape, new DelDrawPoint(getPanelShapeDataList));
                    }                    
                }
              
                ImageCodecInfo jpgEncoder = GetEncoder(ImageFormat.Jpeg);

                System.Drawing.Imaging.Encoder myEncoder =  System.Drawing.Imaging.Encoder.Quality;

                EncoderParameters myEncoderParameters = new EncoderParameters(1);
                EncoderParameter myEncoderParameter = new EncoderParameter(myEncoder, 100L);
                myEncoderParameters.Param[0] = myEncoderParameter;

                Bitmap bitmap = new Bitmap(bmp, new Size(640, 480));

                // 2017.08.07 add by ikm
                if (fileProc.CreateDirectory(Path.GetDirectoryName(@strSavePath)))
                Utils.ImageProc.fnRotateImage(bitmap, fAngle).Save(@strSavePath, jpgEncoder, myEncoderParameters);
                                
               
               // bmp.Save(@strSavePath, jpgEncoder, myEncoderParameters);
                
                nRet = Consts.TRUE;
            }
            catch (Exception ex) 
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
                //Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
                nRet = Consts.FALSE;
            }

            return nRet;
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
            Bitmap bitmapRet = null;
            try
            {               
                CmdMsgParam Param = new CmdMsgParam();
                Byte[] dat = new Byte[nPicWidth * nPicHeight];
                Param.SetParam(dat);
                Param.SetUInteger(Convert.ToUInt32(Math.Abs(nX)));       //ULONG : X Offset	Crop Image 시작 X
                Param.SetUInteger(Convert.ToUInt32(Math.Abs(nY)));       //ULONG : Y Offset	Crop Image 시작 Y
                Param.SetUInteger(Convert.ToUInt32(nCropX));   //ULONG : X Crop Size	Crop Image X Size
                Param.SetUInteger(Convert.ToUInt32(nCropY));  //ULONG : Y Crop Size	Crop Image Y Size
                Param.SetUInteger(Convert.ToUInt32(nPicWidth));   //ULONG : X Window Size	반환받을 Byte[] X Size
                Param.SetUInteger(Convert.ToUInt32(nPicHeight));  //ULONG : Y Window Size	반환받을 Byte[] Y Size

                nRet = Systems.g_Ipc.SendCommand((ushort)((Globals.SelPcNo + 1) * 100 + IpcConst.CAMERA_TASK), IpcConst.CAMERA_FUNC, IpcConst.LIVE_REQ,
                                                    IpcInterface.CMD_TYPE_RES, IpcInterface.CMD_TIMEOUT, Param.GetByteSize(), Param.GetParam());

                if (nRet == Consts.APP_OK)
                    bitmapRet = (Bitmap)Utils.ImageProc.ConvertBitmap(Param.GetParam(), nPicWidth, nPicHeight);
                else
                    bitmapRet = m_imgLoaded;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(m_nVisionNo, ex.StackTrace, ex.Message);
                //Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
            }

            return bitmapRet;
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
           return m_imgLoaded;           
        }

        /// <summary>
        /// 기  능 : 이미지 비트맵을 현재 이미지로 설정한다.
        /// /반환값 : Bitmap
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void m_fnSetBitmap(Bitmap image)
        {
            //m_imgLoaded = null;
            LoadImage(image);    
            picMainImg.Invalidate();
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
            return new Point(m_imgLoaded.Width, m_imgLoaded.Height);
        }
    }
}
