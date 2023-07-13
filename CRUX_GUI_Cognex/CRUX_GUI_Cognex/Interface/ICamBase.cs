using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CRUX_GUI_Cognex.Interface
{
    interface ICamBase
    {
        void LiveStart();
        void LiveStop();
        void LightOn();
        void LightOff();
    }
}
