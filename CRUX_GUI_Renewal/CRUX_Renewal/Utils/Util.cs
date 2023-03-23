using Cognex.VisionPro.QuickBuild;
using CRUX_Renewal;
using CRUX_Renewal.Ex_Form;
using CRUX_Renewal.Utils;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_Renewal
{
    public static class EnumHelper
    {
        public static string GetDescription(Enum en)
        {
            Type type = en.GetType();

            MemberInfo[] memInfo = type.GetMember(en.ToString());

            if (memInfo != null && memInfo.Length > 0)
            {
                //해당 text 값을 배열로 추출해 옵니다.
                object[] attrs = memInfo[0].GetCustomAttributes(typeof(DescriptionAttribute), false);

                if (attrs != null && attrs.Length > 0)
                {
                    return ((DescriptionAttribute)attrs[0]).Description;
                }
            }

            return en.ToString();
        }
        public static string GetString(Enum en)
        {
            Type type = en.GetType();

            MemberInfo[] memInfo = type.GetMember(en.ToString());

            if (memInfo != null && memInfo.Length > 0)
            {
                //해당 text 값을 배열로 추출해 옵니다.
                object[] attrs = memInfo[0].GetCustomAttributes(typeof(StringValue), false);

                if (attrs != null && attrs.Length > 0)
                {
                    return ((StringValue)attrs[0]).Value;
                }
            }

            return en.ToString();
        }
    }
    static class Utility
    {
        /// <summary>
        /// Verify that the same form exists
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        static public Form CheckForm(string name)
        {
            try
            {
                foreach (Form openForm in Application.OpenForms)
                {
                    if (openForm.Name == name) // 열린 폼의 이름 검사
                    {
                        openForm.Activate();
                        return openForm;
                    }
                }
                return null;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }
        static public void LoadingStart()
        {
            Ex_Frm_Others_Loading form = null;

            Thread thread = new Thread(() => {
                form = new Ex_Frm_Others_Loading();
                form.Location = new Point(Program.Frm_Main.Location.X + ((Program.Frm_Main.Width / 2) - (form.Width/2)), Program.Frm_Main.Location.Y + ((Program.Frm_Main.Height / 2) - (form.Height/2)));
                Program.LoadingForm = form;
                Application.Run(form);
            });

            thread.SetApartmentState(ApartmentState.STA);

            thread.Start();

            while (true)
            {
                if (form != null && form.IsHandleCreated == true)
                {
                    break;
                }
            }

            IntPtr handle = Program.Frm_Main.Handle;

            form.Invoke(new Action(delegate { WinApis.SetParent(form.Handle, handle); }));
        }
        static public void LoadingStop()
        {
            Program.LoadingForm?.Invoke(new Action(delegate ()
            {
                Program.LoadingForm?.Close();
            }));
        }
            /// <summary>
            /// Verify that the same layer name exists
            /// </summary>
            /// <param name="name"></param>
            /// <param name="list"></param>
            /// <returns></returns>
            static public Control[] GetAllControlsRecursive(Control containerControl)  // Main object부터 시작
        {
            try
            {
                List<Control> controlList = new List<Control>();

                foreach (Control ctl in containerControl.Controls)  // 자식 컨트롤을 하나씩 조회하여
                {
                    controlList.Add(ctl);   // list에 넣는다.

                    if (ctl.Controls.Count > 0) // 자식 컨트롤의 자식 컨트롤들이 0보다 많으면
                    {   // Container 이므로
                        controlList.AddRange(GetAllControlsRecursive(ctl)); // container를 재귀호출한다. 
                                                                            // 그리고 그 값들은 AddRange로 리스트에 추가된다.
                    }
                }
                return controlList.ToArray(); // 콘트롤 리스트를 반환한다.
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }
        static public Control[] GetAllControlsRecursive(Control containerControl, string content)  // Main object부터 시작
        {
            try
            {
                List<Control> controlList = new List<Control>();

                foreach (Control ctl in containerControl.Controls)  // 자식 컨트롤을 하나씩 조회하여
                {
                    if (ctl.Name.Contains(content))
                    {
                        controlList.Add(ctl);   // list에 넣는다.                        
                    }

                    if (ctl.Controls.Count > 0) // 자식 컨트롤의 자식 컨트롤들이 0보다 많으면
                    {   // Container 이므로                        
                        controlList.AddRange(GetAllControlsRecursive(ctl, content)); // container를 재귀호출한다. 
                                                                            // 그리고 그 값들은 AddRange로 리스트에 추가된다.
                    }
                }
                return controlList.ToArray(); // 콘트롤 리스트를 반환한다.
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }
        /// <summary>
        /// Find Control type in Control(Recursive)
        /// </summary>
        /// <param name="containerControl"></param>
        /// <param name="type"></param>
        /// <returns></returns>
        static public Control[] GetAllControlsRecursive(Control containerControl, Type type)  // Main object부터 시작
        {
            try
            {
                List<Control> controlList = new List<Control>();

                foreach (Control ctl in containerControl.Controls)  // 자식 컨트롤을 하나씩 조회하여
                {
                    if (ctl.GetType() == type)
                    {
                        controlList.Add(ctl);   // list에 넣는다.
                    }

                    if (ctl.Controls.Count > 0) // 자식 컨트롤의 자식 컨트롤들이 0보다 많으면
                    {   // Container 이므로
                        controlList.AddRange(GetAllControlsRecursive(ctl, type)); // container를 재귀호출한다. 
                                                                                  // 그리고 그 값들은 AddRange로 리스트에 추가된다.
                    }
                }
                return controlList.ToArray(); // 콘트롤 리스트를 반환한다.
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }
        /// <summary>
        /// Swap List Item
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="list"></param>
        /// <param name="from"></param>
        /// <param name="to"></param>
        static public void Swap<T>(List<T> list, int from, int to)
        {
            try
            {
                T tmp = list[from];
                list[from] = list[to];
                list[to] = tmp;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            }
        }

        // Pic Box 좌표<-> Image 좌표 변환
        static public float fnCalCoordinate(float nOrignCoord, float nCurCoord, float nImgRectSize, float nFovSize, double dRatio)
        {
            try
            {
                //return nOrignCoord + (float)Math.Round((Convert.ToDouble(nImgRectSize * nCurCoord) / (nFovSize - 1)));
                return nOrignCoord + (float)(nImgRectSize * nCurCoord) / (nFovSize);
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }

        // Zoom In/out 의 pic box 변화 계산
        static public float fnCalCoordinate_ZoomInOut(float nOrignCoord, float nFovSize, double dRatio)
        {
            try
            {
                return (float)((nOrignCoord - nFovSize / 2) * (dRatio - 1));
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }        
 
        static public void AdjustCoordinate(ref RectangleF rect)
        {
            try
            {
                if (rect.X > rect.Width)
                {
                    float Temp_X = rect.X;
                    rect.X = rect.Width;
                    rect.Width = Temp_X;
                }
                if (rect.Y > rect.Height)
                {
                    float Temp_Y = rect.Y;
                    rect.Y = rect.Height;
                    rect.Height = Temp_Y;
                }
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            }
        }
        static public void ChangeJobImageSource(CogJob job, bool flag)
        {
            CheckState Checked;
            if (flag)
                Checked = CheckState.Checked;
            else
                Checked = CheckState.Unchecked;

            CogJobConfiguration Config = new CogJobConfiguration(false, Systems.MainRecipe.Manager.Job(0)) { TopLevel = false };

            var rtn = Utility.GetAllControlsRecursive(Config, "chkNormalRunMode");

            (rtn[0] as CheckBox).CheckState = Checked;       
            var rtna = Utility.GetAllControlsRecursive(Config, "btnOK");
            (rtna[0] as Button).PerformClick();
            Config.CopyAll(Systems.MainRecipe.Manager.Job(0));
        }
        static public void AdjustCoordination (ref RectangleF rect)
        {
            try
            {
                if ( rect.Width < 0 )
                {
                    rect.Width = Math.Abs(rect.Width);
                    rect.X -= rect.Width;
                }
                if ( rect.Height < 0 )
                {
                    rect.Height = Math.Abs(rect.Height);
                    rect.Y -= rect.Height;
                }
            }
            catch ( Exception ex )
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            }
        }
        public static string DescriptionAttr<T>(this T source)
        {
            FieldInfo fi = source.GetType().GetField(source.ToString());

            DescriptionAttribute[] attributes = (DescriptionAttribute[])fi.GetCustomAttributes(
                typeof(DescriptionAttribute), false);

            if (attributes != null && attributes.Length > 0) return attributes[0].Description;
            else return source.ToString();
        }
    }
}
