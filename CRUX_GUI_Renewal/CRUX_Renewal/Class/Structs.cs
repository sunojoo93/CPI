using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static CRUX_Renewal.Enums;

namespace CRUX_Renewal.Class
{
    class Structs
    {
    }
    /// <summary>
    /// 검사에 필요한 정보
    /// </summary>
    class InspectInfo : IDisposable
    {
        public void Dispose ()
        {
            OriginImage.Dispose();
            //Parameter.Dispose();
        }
        //public InspectInfo DeepCopy()
        //{
        //    InspectInfo ii = new InspectInfo();
        //    ii.OriginImage = OriginImage.DeepCopy();
        //}
        public Bitmap OriginImage { get; set; } = null;
        //public RecipeParams Parameter { get; set; } = null;
        public string Position { get; set; } = string.Empty;
        public string Direction { get; set; } = string.Empty;
        public string Face { get; set; } = string.Empty;
        public string CellID { get; set; } = string.Empty;
        public string InputTime { get; set; } = string.Empty;
        public string OutputTime { get; set; } = string.Empty;
        public string FinishedTime { get; set; } = string.Empty;
        public string InspName { get; set; } = string.Empty;
    }
    /// <summary>
    /// 레시피 파라미터
    /// </summary>
    class RecipeParams : IDisposable
    {
        public RecipeParams ()
        {

        }

        public void Dispose ()
        {
            Dispose();
        }
    }
    /// <summary>
    /// 
    /// </summary>
    public struct ALIVE_STATE
    {
        public bool camrea;
        public bool light;
        public bool mainpc;
        public bool inspect;
        public bool sequence;
        public bool pgcontrol;
        public void init ()
        {
            camrea = false;
            light = false;
            mainpc = false;
            inspect = false;
            sequence = false;
            pgcontrol = false;
        }
    }
    public class ProcessSet
    {
        public Process Proc { get; set; }
        public WINDOWS_STATE State { get; set; } = WINDOWS_STATE.SW_SHOW;
        public ProcessSet (Process proc, WINDOWS_STATE state)
        {
            Proc = proc;
            State = state;
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct PARAM_INSPECT_START_ACI
    {
        uint InspType;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 100)]
        public byte[] ID;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 100)]
        public byte[] VirID;
        public int ImageNum;
        public uint ShareImgNum;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 100)]
        public byte[] Direction;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 100)]
        public byte[] Position;
        public int GrabLine;

        public PARAM_INSPECT_START_ACI (int n)
        {
            InspType = 0;
            ID = new byte[100];
            VirID = new byte[100];
            ImageNum = 0;
            ShareImgNum = 0;
            GrabLine = 0;
            Direction = new byte[100];
            Position = new byte[100];
        }
    }
    public struct CRect
    {
        public int X;
        public int Y;
        public int Width;
        public int Height;
        public CRect (int nCnt)
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
        public SMemImageInfo (int n)
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
        public RCV_TACT_TIME_DATA (int nCnt)
        {
            strPanelID = new byte[100];
            strTactName = new byte[100];
            strTactState = new byte[100];
        }
    };

    interface ITimerManager
    {
        void TimerStart ();
        void TimerStop ();
    }
}
