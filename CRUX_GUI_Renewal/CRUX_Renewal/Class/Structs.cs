﻿using Cognex.VisionPro;
using Cognex.VisionPro.QuickBuild;
using CRUX_Renewal.Utils;
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
    [Serializable]
    class InspData : IDisposable
    {
        public void Dispose()
        {
            //OriginImage.Dispose();   


        }
        public InspData DeepCopy()
        {
            InspData Temp = new InspData();
            Temp = this;
            Temp.OriginImage = ImageProc.DeepCopyBitmap_Stream(OriginImage);
            return Temp;
        }
        public CogImage8Grey OriginImage { get; set; } = null;
        public string Position { get; set; } = null;
        public string Direction { get; set; } = null;
        public string Face { get; set; } = null;
        public string CellID { get; set; } = null;
        public string InputTime { get; set; } = null;
        public string OutputTime { get; set; } = null;
        public string FinishedTime { get; set; } = null;
        public string InspName { get; set; } = null;
        public string Path { get; set; } = "";
        public int ImageNo { get; set; } = 0;
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
        public bool camrea;
        public bool light;
        public bool mainpc;
        public bool inspect;
        public bool sequence;
        public bool pgcontrol;
        public void init()
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
        public ProcessSet(Process proc, WINDOWS_STATE state)
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

        public PARAM_INSPECT_START_ACI(int n)
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
        
        public CogJobManager Manager { get; set; } = null;
        public Optical_Cam Camera;
        public Optical_Light Light;

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

                Manager.Shutdown();
                Manager = null;
                disposedValue = true;             
            }
        }

        // TODO: 위의 Dispose(bool disposing)에 관리되지 않는 리소스를 해제하는 코드가 포함되어 있는 경우에만 종료자를 재정의합니다.
        ~Recipe()
        {
            Console.WriteLine("Finalizer");
            // 이 코드를 변경하지 마세요. 위의 Dispose(bool disposing)에 정리 코드를 입력하세요.
            Dispose(false);
        }

        // 삭제 가능한 패턴을 올바르게 구현하기 위해 추가된 코드입니다.
        public void Dispose()
        {
            int Gen = GC.GetGeneration(Manager);
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
    public class Optical_Cam
    {
        int aa = 0;
        int bb = 1;
        string cc = "테스트";
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class Optical_Light
    {
        int aa = 0;
        int bb = 1;
        string cc = "테스트";
    }
}
