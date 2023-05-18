using Cognex.VisionPro;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace SharingFormat_DLL
{
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class SharedClass
    {
        public Dictionary<string, ROI_Property_Data> ROI_List = new Dictionary<string, ROI_Property_Data>();
        public Dictionary<string, string> Parameters = new Dictionary<string, string>();
        public int Number1 = 9;
        public int Number2= 0;
        public string Data1 = "asda";
        public List<string> Listt;
        // 모든 함수는 public 한정자 사용할 것
        public int Test(int a, int b)
        {
            return a + b;
        }
    }
    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class ROI_Property_Data
    {
        public string Name { get; set; }
        public double X { get; set; }
        public double Y { get; set; }
        public double Width { get; set; }
        public double Height { get; set; }
    }
    public class CommonParam
    {
        public Dictionary<string, string> Params = new Dictionary<string, string>();
    }
    public class Insp_Images
    {
        public Dictionary<string, CogImage8Grey> Images;
    }
}
