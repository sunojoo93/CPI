using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace BinaryFormatterStructs_DLL
{
    public class Structs
    {
    }

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class TcpDataInfo
    {
        public int nImageWidth;
        public int nImageHeight;
        public int nImageBitrate;
        public int nImageBandWidth;
        public int nImageCount;

        public byte[] byteImageBuffer;
    }
}
