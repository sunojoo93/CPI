using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.MemoryMappedFiles;
using System.Runtime.InteropServices;

namespace CRUX_GUI_Cognex.Class
{
    unsafe static class ShareMemory
    {
        [STAThread]
        [DllImport("ShareData.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int CreateSharedMem(int nImageSizeX, int nImageSizeY, int nImageBitCount, int nImageWidthBytes);
        [DllImport("ShareData.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int IsInitial();
        [DllImport("ShareData.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int* GetImgAddress();
        [DllImport("ShareData.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int* SetSMemImageInfo_Ptr(int* rsm);
        [DllImport("ShareData.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetSizeX();
        [DllImport("ShareData.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetSizeY();
        [DllImport("ShareData.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetImageBitCount();
        [DllImport("ShareData.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetWidthBytes();
        [DllImport("ShareData.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetImageBufferSize();
        [DllImport("ShareData.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetUpdateFlag(int iFlag);
        [DllImport("ShareData.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetUpdateFlag();
        [DllImport("ShareData.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void DeleteMemory();
    }
}