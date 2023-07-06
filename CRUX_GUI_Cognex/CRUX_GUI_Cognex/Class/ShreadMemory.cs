using System;
using System.Collections.Generic;
using System.IO;
using System.IO.MemoryMappedFiles;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace CRUX_GUI_Cognex.Class
{
    unsafe class SharedMem
    {
        SMemImageInfo m_ImageInfo;
        byte*[] m_pImageBuffer;

        byte* m_pSharedMemory = null;



        public SharedMem()
        {
            m_ImageInfo = new SMemImageInfo(0);
            m_pImageBuffer = new byte*[Consts.MAX_IMAGE_COUNT];
        }
        public int OpenSharedMem(int MaxCam)
        {
            try
            {
                //MemoryMapTest로 이름붙인 공유 Memory 열기
                var mappedFile = MemoryMappedFile.OpenExisting("PDC_SHARED_MEM_CAMERA_D_1", MemoryMappedFileRights.FullControl, HandleInheritability.None);

                long Pos = 0;
                // 공유 Memory에서 읽은 것을 Stream으로 받기
                MemoryMappedViewAccessor mmva = mappedFile.CreateViewAccessor();
                mmva.SafeMemoryMappedViewHandle.AcquirePointer(ref m_pSharedMemory);

                byte* ptr = GetGlobalMemoryPtr(ref Pos, Marshal.SizeOf(m_ImageInfo));

                byte[] arr = new byte[Marshal.SizeOf(m_ImageInfo)];
                Marshal.Copy((IntPtr)ptr, arr, 0, Marshal.SizeOf(m_ImageInfo));
                
                m_ImageInfo = ByteToStruct<SMemImageInfo>(arr);

                int nImageSizeX, nImageSizeY, nImageBitrate, nImageBandWidth, nImageCount;

                nImageSizeX = m_ImageInfo.nImageWidth;
                nImageSizeY = m_ImageInfo.nImageHeight;
                nImageBitrate = m_ImageInfo.nImageBitrate;
                nImageBandWidth = m_ImageInfo.nImageBandWidth;
                nImageCount = m_ImageInfo.nImageCount;

                int Bitrate;
                if (nImageBitrate == 8)
                    Bitrate = 1;
                else
                    Bitrate = 2;

                long nImageSize = nImageSizeX * Consts.MAX_IMAGE_RATIO * nImageSizeY * Consts.MAX_IMAGE_RATIO * nImageBandWidth * Bitrate;

                for (int nCnt = 0; nCnt < nImageCount; nCnt++)
                    m_pImageBuffer[nCnt] = GetGlobalMemoryPtr(ref Pos, nImageSize);

                return Consts.APP_OK;
            }
            catch (Exception ex)
            {
                //Systems.LogWriter.Error(ex);
                throw new Exception("공유메모리 접근 실패");
            }   
        }
        private T ByteToStruct<T>(byte[] buffer) where T : struct
        {
            //구조체 사이즈 
            int size = Marshal.SizeOf(typeof(T));

            if (size > buffer.Length)
            {
                throw new Exception();
            }

            IntPtr ptr = Marshal.AllocHGlobal(size);
            Marshal.Copy(buffer, 0, ptr, size);
            T obj = (T)Marshal.PtrToStructure(ptr, typeof(T));
            Marshal.FreeHGlobal(ptr);
            return obj;
        }
        unsafe byte* GetGlobalMemoryPtr(ref long nStartPos, long nReadSize)
        {

            byte* pReturn = m_pSharedMemory + nStartPos;
            nStartPos += nReadSize;

            return pReturn;
        }
        public int GetImgWidth()
        {
            return m_ImageInfo.nImageWidth;
        }

        public int GetImgHeight()
        {
            return m_ImageInfo.nImageHeight;
        }

        public int GetImgBitrate()
        {
            return m_ImageInfo.nImageBitrate;
        }

        public int GetImgBandWidth()
        {
            return m_ImageInfo.nImageBandWidth;
        }

        public byte* GetImgAddress(int nImgCnt)
        {
            return m_pImageBuffer[nImgCnt];
        }
    }
}
