using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SharingFormat_DLL
{
    public class SharedClass
    {
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
}
