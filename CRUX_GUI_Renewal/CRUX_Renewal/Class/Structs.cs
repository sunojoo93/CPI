using Cognex.VisionPro;
using Cognex.VisionPro.QuickBuild;
using CRUX_Renewal.User_Controls;
using CRUX_Renewal.Utils;
using OpenCvSharp;
using PropertyGridExt;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml.Serialization;
using static CRUX_Renewal.Enums;

namespace CRUX_Renewal.Class
{
    class Structs
    {
    }
    /// <summary>
    /// 검사에 필요한 정보
    /// </summary>
    [Serializable]
    class InspData
    {
        public List<ImageData> Datas = new List<ImageData>();
        public string Area { get; set; } = null;
        public string VirID { get; set; } = null;
        public string Face { get; set; } = null;
        public string CellID { get; set; } = null;
        public string AreaInspStartTime { get; set; } = null;
        public string AreaInspEndTime { get; set; } = null;
        public string FinishedTime { get; set; } = null;
        public string InspName { get; set; } = null;
        public string Path { get; set; } = "";
        public string PatternName { get; set; } = null;
        public bool FirstPattern { get; set; } = false;
    }

    public class ImageData: IDisposable
    {
        public CogImage8Grey OriginImage { get; set; } = null;
        public int SharedMemIdx { get; set; } = 0;
        public string PatternName { get; set; } = string.Empty;
        public string Direction { get; set; } = null;
        public void Dispose()
        {
         
        }
    }
    public class ManualImageData : ImageData
    {
        public string Area = null;
        public string Path = null;
        public bool View = false;
    }

    /// <summary>
    /// 검사에 필요한 정보
    /// </summary>
    class CommonInspData : IDisposable
    {
        public string Face { get; set; } = null;
        public string CellID { get; set; } = null;
        public string InputTime { get; set; } = null;
        public string OutputTime { get; set; } = null;
        public string ProcessedTime { get; set; } = null;

        public void Dispose()
        {
            CellID = null;
            Face = null;
            InputTime = null;
            OutputTime = null;
            ProcessedTime = null;
        }
    }
    /// <summary>
    /// 레시피 파라미터
    /// </summary>
    class RecipeParams : IDisposable
    {
        public RecipeParams()
        {

        }

        public void Dispose()
        {
            Dispose();
        }
    }
    /// <summary>
    /// 
    /// </summary>
    public struct ALIVE_STATE
    {
        public bool Camera;
        public bool Light;
        public bool MainPc;
        public bool Inspector;
        public bool Sequence;
        public bool PG;
        public bool AF;
        public void init()
        {
            Camera = false;
            Light = false;
            MainPc = false;
            Inspector = false;
            Sequence = false;
            PG = false;
            AF = false;
        }
    }
    public class ProcessSet
    {
        public Process Proc { get; set; }
        public WINDOWS_STATE State { get; set; } = WINDOWS_STATE.SW_SHOW;
        public ProcessSet(Process proc, WINDOWS_STATE state)
        {
            Proc = proc;
            State = state;
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct PARAM_INSPECT_START_AOT_CHIPPING_ALM
    {
        int PcNo;
        uint InspType;
        // 한 Area에 대한 모든 패턴의 이미지 모음
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = Consts.MAX_PATTERN_COUNT)]
        public ImageSet[] ImageData;
        // 패턴의 개수
        public int PatternCount;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 100)]
        public byte[] CellID;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 100)]
        public byte[] VirID;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 100)]
        public byte[] Area;
        public int GrabLine;
        public int CamNo;
        public bool FirstPattern;
        //public int ParticleCount;
    

        public PARAM_INSPECT_START_AOT_CHIPPING_ALM(int n)
        {
            PcNo = 0;
            InspType = 0;
            ImageData = new ImageSet[Consts.MAX_PATTERN_COUNT];
            CellID = new byte[100];
            VirID = new byte[100];
            GrabLine = 0;
            Area = new byte[100];
            PatternCount = 0;
            CamNo = 0;
            FirstPattern = false;
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct ImageSet
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 100)]
        public byte[] PatternName;
        public int SharedMemStartIdx;
        public int SharedMemEndIdx;
        public int ParticleImageCount;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 100)]
        public byte[] Direction;
        public int GrabLine;

        public ImageSet(int n)
        {
            PatternName = new byte[100];
            SharedMemStartIdx = 0;
            SharedMemEndIdx = 0;
            ParticleImageCount = 0;
            GrabLine = 0;
            Direction = new byte[100];
        }
    }    
    public struct CRect
    {
        public int X;
        public int Y;
        public int Width;
        public int Height;
        public CRect(int nCnt)
        {
            X = 0;
            Y = 0;
            Width = 0;
            Height = 0;
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public unsafe struct SMemImageInfo
    {
        public int nImageWidth;
        public int nImageHeight;
        public int nImageBitrate;
        public int nImageBandWidth;
        public int nImageCount;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 1048576)]
        byte[] byteReserved;        // 예약 공간 - 위쪽에 변수 추가 시 크기 계산하여 삭제해주어야함.
        public SMemImageInfo(int n)
        {
            nImageWidth = 0;
            nImageHeight = 0;
            nImageBitrate = 0;
            nImageBandWidth = 0;
            nImageCount = 0;
            byteReserved = new byte[1048576];
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]

    public struct RCV_TACT_TIME_DATA
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 100)]
        public byte[] strPanelID;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 100)]
        public byte[] strTactName;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 100)]
        public byte[] strTactState;
        public RCV_TACT_TIME_DATA(int nCnt)
        {
            strPanelID = new byte[100];
            strTactName = new byte[100];
            strTactState = new byte[100];
        }
    };

    interface ITimerManager
    {
        void TimerStart();
        void TimerStop();
    }

    class SafeMalloc : SafeBuffer
    {
        ///

        /// Allocates memory and initialises the SaveBuffer
        ///
        ///The number of bytes to allocate 
        public SafeMalloc(int size) : base(true)
        {
            this.SetHandle(Marshal.AllocHGlobal(size));
            this.Initialize((ulong)size);
        }

        ///

        /// Called when the object is disposed, ferr the
        /// memory via FreeHGlobal().
        ///
        ///
        protected override bool ReleaseHandle()
        {
            Marshal.FreeHGlobal(this.handle);
            return true;
        }

        ///

        /// Cast to IntPtr
        ///
        public static implicit operator IntPtr(SafeMalloc h)
        {
            return h.handle;
        }


    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class Recipe : IDisposable
    {
        public string Name { get; set; }
        public string Path { get; set; }
        public bool Opend { get; set; }
        //public CogJobManager Manager { get; set; } = null;
        //public Dictionary<string, List<ROI_Data>> ROI_List;
        public Areas Area_Data = new Areas();
        //public List<ROI_Property> ROI_Prop = new List<ROI_Property>();

        public void SaveROI_Property()
        {

        }
        public void SavePatterns()
        {

        }

        public void SaveOptics()
        {

        }
        public void SetSmartListAddEvent()
        {

        }
        public void SetSmartListRemoveEvent()
        {

        }
        public void SetSmartListInsertEvent()
        {

        }
        //public void SetRecipeData(string path, string name)
        //{
        //    Recipe Temp = Systems.RecipeContent.MainRecipe[Systems.CurDisplayIndex];
        //    //RecipeManager.RecipeDeserialize(path, name);
        //}
        #region IDisposable Support
        private bool disposedValue = false; // 중복 호출을 검색하려면

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    // TODO: 관리되는 상태(관리되는 개체)를 삭제합니다.
                }

                // TODO: 관리되지 않는 리소스(관리되지 않는 개체)를 해제하고 아래의 종료자를 재정의합니다.
                // TODO: 큰 필드를 null로 설정합니다.

                //Manager?.Shutdown();
                //Manager = null;
                disposedValue = true;
            }
        }

        // TODO: 위의 Dispose(bool disposing)에 관리되지 않는 리소스를 해제하는 코드가 포함되어 있는 경우에만 종료자를 재정의합니다.
        ~Recipe()
        {
            Systems.LogWriter.Info($@"Recipe Finalizer, Recipe Name : {Name}");
            Console.WriteLine("Finalizer");
            // 이 코드를 변경하지 마세요. 위의 Dispose(bool disposing)에 정리 코드를 입력하세요.
            Dispose(false);
        }

        // 삭제 가능한 패턴을 올바르게 구현하기 위해 추가된 코드입니다.
        public void Dispose()
        {
            //int Gen = GC.GetGeneration(Manager);
            // 이 코드를 변경하지 마세요. 위의 Dispose(bool disposing)에 정리 코드를 입력하세요.
            Dispose(true);
            // TODO: 위의 종료자가 재정의된 경우 다음 코드 줄의 주석 처리를 제거합니다.
            //GC.SuppressFinalize(this);
            //GC.WaitForPendingFinalizers();
        }
        #endregion
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class ROI_Property
    {
        private string _Name { get; set; }
        private CogGraphicLineStyleConstants _LineStyle = CogGraphicLineStyleConstants.Solid;
        private CogColorConstants _LineColor = CogColorConstants.Red;
        private CogColorConstants _DragLineColor = CogColorConstants.Cyan;
        private CogGraphicLineStyleConstants _DragLineStyle = CogGraphicLineStyleConstants.Solid;
        private CogColorConstants _SelectedLineColor = CogColorConstants.Yellow;
        private CogGraphicLineStyleConstants _SelectedLineStyle = CogGraphicLineStyleConstants.Solid;
        private double _DefaultScale = 0.8;
        private string _Description;

        public ROI_Property()
        {
            int a = 0;
        }
        [Description("ROI 이름입니다.")]
        [ReadOnly(true)]
        public string Name
        {
            get
            {
                return _Name;
            }
            set
            {
                _Name = value;
            }
        }
        [Description("Line Style을 설정합니다.")]
        [DefaultValue(CogGraphicLineStyleConstants.Solid)]
        public CogGraphicLineStyleConstants LineStyle
        {
            get
            {
                return _LineStyle;
            }
            set
            {
                _LineStyle = value;
            }
        }
        [Description("Line Color를 설정합니다.")]
        [DefaultValue(CogColorConstants.Red)]
        //[Editor(typeof(ColorEditor), typeof(ColorValuesConverter))]
        public CogColorConstants LineColor
        {
            get
            {
                return _LineColor;
            }
            set
            {
                _LineColor = value;
            }
        }
        [Description("ROI 생성 시 기본 배율입니다.")]
        [DefaultValue(0.8)]
        public double DefaultScale
        {
            get
            {
                return _DefaultScale;
            }
            set
            {
                _DefaultScale = value;
            }
        }
        [Description("Drag할 때 그려지는 GuideLine의 Color입니다.")]
        [DefaultValue(CogColorConstants.Red)]
        public CogColorConstants DragLineColor
        {
            get
            {
                return _DragLineColor;
            }
            set
            {
                _DragLineColor = value;
            }
        }
        [Description("Drag할 때 그려지는 GuideLine의 Style입니다.")]
        [DefaultValue(CogGraphicLineStyleConstants.Solid)]
        public CogGraphicLineStyleConstants DragLineStyle
        {
            get
            {
                return _DragLineStyle;
            }
            set
            {
                _DragLineStyle = value;
            }
        }
        [Description("선택된 Line의 Style입니다.")]
        [DefaultValue(CogGraphicLineStyleConstants.Solid)]
        public CogGraphicLineStyleConstants SelectedLineStyle
        {
            get
            {
                return _SelectedLineStyle;
            }
            set
            {
                _SelectedLineStyle = value;
            }
        }
        [Description("선택된 Line의 Color입니다.")]
        [DefaultValue(CogColorConstants.Red)]
        public CogColorConstants SelectedLineColor
        {
            get
            {
                return _SelectedLineColor;
            }
            set
            {
                _SelectedLineColor = value;
            }
        }
        [Description("설명입니다.")]
        public string Description
        {
            get
            {
                return _Description;
            }
            set
            {
                _Description = value;
            }
        }
    }
    public class PropertyString
    {
        private string Data { get; set; }
        Action Func;

        public void SetString(string data)
        {
            Data = data;
            Func();
        }
        public string GetString()
        {
            return Data;
        }
        public PropertyString(Action func)
        {
            Func = func;
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class Algorithm_Infomation
    {
        public string Name { get; set; } = null;
        public string Path { get; set; } = null;
        public string FileName { get; set; } = null;
    }

    public class ROI_Property_Data
    {
        public string Name { get; set; }
        public double X { get; set; }
        public double Y { get; set; }
        public double Width { get; set; }
        public double Height { get; set; }
    }
    public class ManualnspImage
    {
        public string Name { get; set; }
        public string Path { get; set; }
        public CogImage8Grey Image { get; set; }
        public bool Opend = false;
    }

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct ST_RECIPE_INFO
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 200)]
        public byte[] RecipeName;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 200)]
        public byte[] RecipePath;
        public int GrabCount;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = Consts.MAX_AREA_COUNT)]
        public ST_GRAB_AREA_INFO[] GrabArea;
        public ST_RECIPE_INFO(int num)
        {
            RecipeName = new byte[200];
            RecipePath = new byte[200];
            GrabArea = new ST_GRAB_AREA_INFO[Consts.MAX_AREA_COUNT];
            GrabCount = 0;
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct ST_GRAB_AREA_INFO
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 100)]
        public byte[] Name;
        public int PtnCount;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = Consts.MAX_PATTERN_COUNT)]
        public ST_PATTERN_INFO[] PatternList;
        public ST_GRAB_AREA_INFO(int num)
        {
            Name = new byte[100];
            PtnCount = 0;
            PatternList = new ST_PATTERN_INFO[Consts.MAX_PATTERN_COUNT];
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct ST_AUTOFOCUS
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 60)]
        public byte[] Number;
        public bool Use;
        public double AxisZ;
        public ST_AUTOFOCUS(int num)
        {
            Number = new byte[60];
            Use = true;
            AxisZ = 0;
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct ST_PATTERN_INFO
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 100)]
        public byte[] PatternName;
        public bool Grab;
        public bool Vacuum;
        public bool Insp;
        public int CamCondCount;
        public int LightCondCount;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = Consts.MAX_CAMERA_COUNT)]
        public ST_CAM_COND[] Cam_Condition;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = Consts.MAX_LIGHT_COUNT)]
        public ST_LIGHT_COND[] Light_Condition;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = Consts.MAX_AF_MODULE_COUNT)]
        public ST_AUTOFOCUS[] AutoFocus_Condition;
        public ST_PATTERN_INFO(int num)
        {
            PatternName = new byte[100];
            Grab = false;
            Vacuum = false;
            Insp = false;
            CamCondCount = 0;
            LightCondCount = 0;
            Cam_Condition = new ST_CAM_COND[Consts.MAX_CAMERA_COUNT];
            Light_Condition = new ST_LIGHT_COND[Consts.MAX_LIGHT_COUNT];
            AutoFocus_Condition = new ST_AUTOFOCUS[Consts.MAX_AF_MODULE_COUNT];
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct ST_CAM_COND
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 100)]
        public byte[] Type;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 100)]
        public byte[] Name;
        public bool Use;
        public double Expose;
        public double Gain;
        public double PS;
        public double Delay;
        public double nCountF;
        public double nCountB;
        public double nStartF;
        public double nStartB;
        public double nStopF;
        public double nStopB;
        public double nPeriodF;
        public double nPeriodB;
        public ST_CAM_COND(int num)
        {
            Type = new byte[100];
            Name = new byte[100];
            Use = false;
            Expose = 100;
            Gain = 1;
            PS = 0;
            Delay = 0;
            nCountF = 0;
            nCountB = 0;
            nStartF = 0;
            nStartB = 0;
            nStopF = 0;
            nStopB = 0;
            nPeriodF = 0;
            nPeriodB = 0;
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct STRU_SERIAL_INFO_AOT
    {
        public uint nChCnt;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = Consts.MAX_LIGHT_CHANNEL_COUNT)]
        public uint[] nLightVal;

        public STRU_SERIAL_INFO_AOT(int num)
        {
            nChCnt = 0;
            nLightVal = new uint[Consts.MAX_LIGHT_CHANNEL_COUNT];
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct ST_LIGHT_COND
    {
        public bool Use;
        public uint Port_No;
        public uint Controller_No;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = Consts.MAX_LIGHT_COUNT)]
        public STRU_SERIAL_INFO_AOT[] Modules;
        public ST_LIGHT_COND(int num)
        {
            Use = true;
            Port_No = 0;
            Controller_No = 0;
            Modules = new STRU_SERIAL_INFO_AOT[Consts.MAX_LIGHT_COUNT];
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct ST_PG_INFO
    {
        public int GrabCount;
    }

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class CameraInfo
    {
        [XmlAttribute("Name")]
        public string Name { get; set; } = string.Empty;
        [XmlAttribute("Use")]
        public bool Use { get; set; } = false;
        [XmlAttribute("Type")]
        public string CamType { get; set; } = string.Empty;
        [XmlElement("Expose")]
        public double Expose { get; set; } = 0;
        [XmlElement("Gain")]
        public double Gain { get; set; } = 0;
        [XmlElement("PS")]
        public int PS { get; set; } = 0;
        [XmlElement("Delay")]
        public int Delay { get; set; } = 0;
        [XmlElement("CountF")]
        public int nCountF { get; set; } = 0;
        [XmlElement("CountB")]
        public int nCountB { get; set; } = 0;
        [XmlElement("StartF")]
        public int nStartF { get; set; } = 0;
        [XmlElement("StartB")]
        public int nStartB { get; set; } = 0;
        [XmlElement("StopF")]
        public int nStopF { get; set; } = 0;
        [XmlElement("StopB")]
        public int nStopB { get; set; } = 0;
        [XmlElement("PeriodF")]
        public int nPeriodF { get; set; } = 0;
        [XmlElement("PeriodB")]
        public int nPeriodB { get; set; } = 0;

    }

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class LightInfo
    {
        [XmlAttribute("Use")]
        public bool Use { get; set; } = false;
        [XmlAttribute("Port_No")]
        public int Port_No { get; set; } = 0;
        [XmlAttribute("Controller_No")]
        public int Controller_No { get; set; } = 0;
        [XmlAttribute("ModuleCount")]
        public int ModuleCount { get; set; } = 0;
        [XmlArray("Modules")]
        [XmlArrayItem("Module")]
        public List<LightModule> LightModules { get; set; }
        public LightInfo()
        {
            LightModules = new List<LightModule>();
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class LightModule
    {
        [XmlAttribute("Count")]
        public int Count { get; set; } = 0;       
        [XmlArray("Ch_Value")]
        [XmlArrayItem("Ch")]
        public List<uint> Ch_Value { get; set; }
    }
    /// <summary>
    /// Patterns Struct
    /// </summary>
    /// 
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    [XmlRoot("RecipeInfo")]
    public class Areas
    {
        [XmlArray("Areas")]
        [XmlArrayItem("Area")]
        public List<Area> Area { get; set; }
        public Areas()
        {
            Area = new List<Area>();
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class ImageMergeProperty
    {
        [XmlElement("All_Shift")]
        public int All_Shift { get; set; } = 0;
        [XmlElement("ShiftX")]
        public int Shift_X { get; set; } = 0;
        [XmlElement("ShiftY")]
        public int ShiftY { get; set; } = 0;

        public ImageMergeProperty()
        {
            
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class Area
    {
        [XmlAttribute("Name")]
        public string Name { get; set; } = string.Empty;
        [XmlAttribute("Use")]
        public bool Use { get; set; } = false;
        [XmlArray("Patterns")]
        [XmlArrayItem("Pattern")]
        public List<Pattern> Patterns { get; set; }
        public Area()
        {
            Patterns = new List<Pattern>();
            
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class Pattern
    {
        [XmlAttribute("Name")]
        public string Name { get; set; } = string.Empty;
        [XmlAttribute("Grab")]
        public bool Grab { get; set; } = true;
        [XmlAttribute("Vacuum")]
        public bool Vacuum { get; set; } = true;
        [XmlAttribute("Insp")]
        public bool Insp { get; set; } = true;
        [XmlArray("InspData")]
        [XmlArrayItem("ROI")]
        public List<ROI> ROI_Data { get; set; }
        [XmlElement("Optics")]
        public Optics Grab_Data { get; set; }


        public Pattern()
        {
            ROI_Data = new List<ROI>();
            Grab_Data = new Optics();
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class Optics
    {
        [XmlArray("Cameras")]
        [XmlArrayItem("Camera")]
        public List<CameraInfo> Camera_Data { get; set; }
        [XmlArray("Lights")]
        [XmlArrayItem("Light")]
        public List<LightInfo> Light_Data { get; set; }
        [XmlArray("AutoFocus")]
        [XmlArrayItem("Module")]
        public List<AutoFocus> AutoFocus { get; set; }


        public Optics()
        {
            Camera_Data = new List<CameraInfo>();
            Light_Data = new List<LightInfo>();
            AutoFocus = new List<AutoFocus>();
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class AutoFocus
    {
        [XmlAttribute("Number")]
        public string Number { get; set; } = string.Empty;
        [XmlAttribute("Use")]
        public bool Use { get; set; } = true;
        [XmlElement("AxisZ")]
        public double AxisZ { get; set; }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class ROI
    {
        [XmlAttribute("Name")]
        public string Name { get; set; } = string.Empty;
        [XmlAttribute("Use")]
        public bool Use { get; set; } = false;
        [XmlElement("Coordinate")]
        public Coordinate Coord { get; set; }
        [XmlElement("Property")]
        public ROI_PropertyData ROI_Property { get; set; }
        [XmlArray("Algorithms")]
        [XmlArrayItem("Algorithm")]
        public List<Algorithm> Algo_List { get; set; }

        public ROI()
        {
            Coord = new Coordinate();
            ROI_Property = new ROI_PropertyData();
            Algo_List = new List<Algorithm>();
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class Coordinate
    {
        [XmlElement("X")]
        //[XmlAttribute("Name")]
        public double X { get; set; } = 0;
        [XmlElement("Y")]
        //[XmlAttribute("Name")]
        public double Y { get; set; } = 0;
        [XmlElement("Width")]
        // [XmlAttribute("Name")]
        public double Width { get; set; } = 0;
        [XmlElement("Height")]
        //[XmlAttribute("Name")]
        public double Height { get; set; } = 0;
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class ROI_PropertyData
    {
        //[Obj_Type(typeof(CogGraphicLineStyleConstants))]
        [Description("Line Style을 설정합니다.")]
        [XmlElement("LineStyle")]
        public string LineStyle { get; set; } = "0";
        //[Obj_Type(typeof(CogColorConstants))]
        [Description("Line Color를 설정합니다.")]
        [XmlElement("LineColor")]
        public string LineColor { get; set; } = "33023";
        //[Obj_Type(typeof(CogColorConstants))]
        [Description("Drag할 때 그려지는 GuideLine의 Color입니다.")]
        [XmlElement("DragLineColor")]
        public string DragLineColor { get; set; } = "16711680";
        //[Obj_Type(typeof(CogGraphicLineStyleConstants))]
        [Description("Drag할 때 그려지는 GuideLine의 Style입니다.")]
        [XmlElement("DragLineStyle")]
        public string DragLineStyle { get; set; } = "0";
        //[Obj_Type(typeof(CogColorConstants))]
        [Description("선택된 Line의 Style입니다.")]
        [XmlElement("SelectedLineColor")]
        public string SelectedLineColor { get; set; } = "65280";
        //[Obj_Type(typeof(CogGraphicLineStyleConstants))]
        [Description("선택된 Line의 Style입니다.")]
        [XmlElement("SelectedLineStyle")]
        public string SelectedLineStyle { get; set; } = "0";
        [Description("ROI 생성 시 기본 배율입니다.")]
        [XmlElement("DefaultScale")]
        public double DefaultScale { get; set; } = 0.8;
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class Algorithm
    {
        [XmlAttribute("Name")]
        public string Name { get; set; } = string.Empty;
        [XmlAttribute("Path")]
        public string Path { get; set; } = string.Empty;
        [XmlAttribute("Use")]
        public bool Use { get; set; } = false;
        [XmlArray("Parameters")]
        [XmlArrayItem("Param")]
        public List<InspParam> Param { get; set; }
        public Algorithm()
        {
            Param = new List<InspParam>();
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class InspParam
    {
        [XmlAttribute("Name")]
        public string Name { get; set; } = string.Empty;
        [XmlAttribute("Value")]
        public string Value { get; set; } = string.Empty;
    }

    public class GrabImageInfo
    {
        public CogImage8Grey Image { get; set; }
        public string PatternName { get; set; }
        public string AreaName { get; set; }
        public int CamNo;
        public int PtnNo;
        
        public GrabImageInfo(CogImage8Grey image, string name, string area, int ptn_no) 
        {
            Image = image;
            PatternName = name;
            AreaName = area;
            PtnNo = ptn_no;
            CamNo = 0;
        }
    }
    public class Recipes : IDisposable
    {
        private static Recipes Recipes_Object;
        public static Recipes Instance()
        {
            if (Recipes_Object == null)
            {
                Recipes_Object = new Recipes();
            }
            return Recipes_Object;
        }

        public void Dispose()
        {
            ViewRecipe.Dispose();
            MainRecipe.Dispose();     
        }

        public Recipe MainRecipe = new Recipe();
        public Recipe ViewRecipe = new Recipe();

        public static bool SaveLoadType = false;
    }
    public class CamList
    {

    }
}
