using Cognex.VisionPro;
using Cognex.VisionPro.QuickBuild;
using CRUX_GUI_Cognex;
using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.Ex_Form;
using CRUX_GUI_Cognex.Utils;
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml.Serialization;

namespace CRUX_GUI_Cognex
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
    public static class RecipeManager
    {
        public static DataTable CvtDtAreaInfo(DataTable table, Areas areas)
        {
            try
            {
                table.Clear();
                DataTable Dt = table.Clone();
                for (int i = 0; i < areas.Area.Count; ++i)
                {
                    DataRow Dr = Dt.NewRow();
                    Dr.ItemArray = new object[] { areas.Area[i].Use, areas.Area[i].Name };
                    Dt.Rows.Add(Dr);
                }
                return Dt;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public static DataTable CvtDtPatternInfo(DataTable table, Area area)
        {
            try
            {
                table.Clear();
                DataTable Dt = table.Clone();
                for (int i = 0; i < area.Patterns.Count; ++i)
                {
                    DataRow Dr = Dt.NewRow();
                    Dr.ItemArray = new object[] { area.Patterns[i].Grab, area.Patterns[i].Vacuum, area.Patterns[i].Name };
                    Dt.Rows.Add(Dr);
                }
                return Dt;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public static DataTable CvtDtGrabCond(DataTable table, Optics optics)
        {
            try
            {
                table.Clear();
                DataTable Dt = table.Clone();
                for (int i = 0; i < optics.Camera_Data.Count; ++i)
                {
                    DataRow Dr = Dt.NewRow();
                    Dr.ItemArray = new object[] { optics.Camera_Data[i].Use, optics.Camera_Data[i].Name, optics.Camera_Data[i].CamType, optics.Camera_Data[i].Expose, optics.Camera_Data[i].Gain, optics.Camera_Data[i].PS, optics.Camera_Data[i].Delay, optics.Camera_Data[i].nCountF, optics.Camera_Data[i].nCountB, optics.Camera_Data[i].nStartF, optics.Camera_Data[i].nStartB, optics.Camera_Data[i].nStopF, optics.Camera_Data[i].nStopB, optics.Camera_Data[i].nPeriodF, optics.Camera_Data[i].nPeriodB};
                    Dt.Rows.Add(Dr);
                }
                return Dt;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public static DataTable CvtDtLightCond(DataTable table, Optics optics)
        {
            try
            {
                table.Clear();
                DataTable Dt = table.Clone();
                for (int i = 0; i < optics.Light_Data.Count; ++i)
                {
                    DataRow Dr = Dt.NewRow();
                    List<object> Temp = new List<object>();
                    Temp.Add(optics.Light_Data[i].Use);
                    Temp.Add(optics.Light_Data[i].Port_No);
                    Temp.Add(optics.Light_Data[i].Controller_No);
                    uint[] LightValueTemp = Enumerable.Repeat<uint>(0, Consts.MAX_LIGHT_CHANNEL_COUNT).ToArray();

                    for (int j = 0; j < optics.Light_Data[i].LightValue.Count; ++j)
                        LightValueTemp[j] = optics.Light_Data[i].LightValue[j];                    

                    for (int j = 0; j < LightValueTemp.Length; ++j)
                        Temp.Add(LightValueTemp[j]);                    

                    Dr.ItemArray = Temp.ToArray();
                    Dt.Rows.Add(Dr);
                }
                return Dt;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        public static void RecipeSerialize<T>(string path, string name, T recipe)
        {
            string FullPath = $@"{path}\{name}";
            //string path = @"D:\CRUX\DATA\Recipes\Test\Patterns11.xml";
            var ns = new XmlSerializerNamespaces();
            ns.Add(string.Empty, string.Empty);

            var xs = new XmlSerializer(typeof(T));
            using (var sw = new StreamWriter(FullPath))
            {
                var info = recipe;
                xs.Serialize(sw, info, ns);
            }
        }
        public static T RecipeDeserialize<T>(string path, string file_name)
        {
            string FullPath = $@"{path}{file_name}";
            if (File.Exists(FullPath))
            {
                try
                {
                    using (var sr = new StreamReader(FullPath))
                    {
                        var xs = new XmlSerializer(typeof(T));
                        var Temp = (T)xs.Deserialize(sr);
                        return Temp;
                        //this.project.Set(prj);
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                    throw ex;
                    //Systems.LogWriter.Info($@"Recipe Load Fail, Exception : {ex}");
                    //MessageBox.Show("프로젝트 파일 로딩 실패 : " + FullPath);
                }
            }
            return default(T);
        }
        public static void SaveRecipe(Recipe recipe)
        {
            try
            {
                RecipeManager.RecipeSerialize($"{ recipe.Path}{recipe.Name}", "MainRecipe.xml", recipe.Area_Data);
            }
            catch (Exception ex)
            {
                Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.CAUTION, "에러가 발생했습니다. 로그를 확인해주세요.");
                Noti.ShowDialog();

                Console.WriteLine(ex.Message);
                //Systems.LogWriter.Info($@"Recipe Save Fail, Exception : {ex}");
                throw ex;
            }
        }
        public static void SaveRecipe(Recipe recipe, string name)
        {
            try
            {
                RecipeManager.RecipeSerialize($"{ recipe.Path}{name}", "MainRecipe.xml", recipe.Area_Data);
            }
            catch (Exception ex)
            {
                Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.CAUTION, "에러가 발생했습니다. 로그를 확인해주세요.");
                Noti.ShowDialog();

                Console.WriteLine(ex.Message);
                throw ex;
                //Systems.LogWriter.Info($@"Recipe Save Fail, Exception : {ex}");
            }
        }
        public static void ReadRecipe(string path, Recipe recipe, string name)
        {
            try
            {
                string FullPath = $@"{path}{name}";
                ArrayList FileList = fileProc.GetFileNames(FullPath);
                ArrayList FullPathList = fileProc.getFileList(FullPath);
                recipe.Path = path;
                recipe.Name = name;
                Systems.RecipeData_Collection[Globals.CurrentPCno].Clear();
                for (int pc_num = 0; pc_num < Globals.MaxVisionCnt; ++pc_num)
                {
                    for (int t = 0; t < FullPathList.Count; ++t)
                    {
                        for (int i = 0; i < Globals.RecipeItem_Names.Length; ++i)
                        {
                            string[] SplitTemp = (FullPathList[t] as string).Split(new string[] { "\\" }, StringSplitOptions.None);
                            string FineName = SplitTemp[SplitTemp.Length - 1];
                            if (FineName == Globals.RecipeItem_Names[i])
                            {
                                switch (FineName)
                                {
                                    case "ROI_Property.dat":
                                        IniFile Temp = new IniFile();
                                        Temp.Load(($@"{path}Recipes\{name}\ROI_Property.dat"));

                                        foreach (IniSection item in Temp.Values)
                                        {
                                            //ROI_Property Prop = new ROI_Property();
                                            //Prop.Name = item["Name"].ToString();
                                            //Prop.LineColor = Utility.EnumUtil<CogColorConstants>.Parse(item["LineColor"].ToString());
                                            //Prop.LineStyle = Utility.EnumUtil<CogGraphicLineStyleConstants>.Parse(item["LineStyle"].ToString());
                                            //Prop.SelectedLineStyle = Utility.EnumUtil<CogGraphicLineStyleConstants>.Parse(item["SelectedLineStyle"].ToString());
                                            //Prop.SelectedLineColor = Utility.EnumUtil<CogColorConstants>.Parse(item["SelectedColor"].ToString());
                                            //Prop.DragLineStyle = Utility.EnumUtil<CogGraphicLineStyleConstants>.Parse(item["DragLineStyle"].ToString());
                                            //Prop.DragLineColor = Utility.EnumUtil<CogColorConstants>.Parse(item["DragLineColor"].ToString());
                                            //Prop.DefaultScale = item["DefaultScale"].ToDouble();
                                            //recipe.ROI_Prop.Add(Prop);
                                        }
                                        break;

                                    case "MainRecipe.xml":
                                        recipe.Area_Data = null;
                                        recipe.Area_Data = RecipeManager.RecipeDeserialize<Areas>($@"{FullPath}\", "MainRecipe.xml");
                                        break;
                                    case "ImageMergeOffset.ini":
                                        IniFile OffsetFile = new IniFile();
                                        OffsetFile.Load($@"{FullPath}\ImageMergeOffset.ini");
                                        //Dictionary<string, IniFile> Data = new Dictionary<string, IniFile>();

                                        Systems.RecipeData_Collection[pc_num].Add("ImageMergeOffset.ini", OffsetFile);
                                        //int aa = Systems.RecipeData_Collection[0]["ImageMergeOffset.ini"]["Offset"]["AllShift"].ToInt();
                                        break;
                                    case "GuideLine.ini":
                                        IniFile GuideLineData = new IniFile();
                                        GuideLineData.Load($@"{FullPath}\GuideLine.ini");
                                        //Dictionary<string, IniFile> GuideLine = new Dictionary<string, IniFile>();

                                        Systems.RecipeData_Collection[pc_num].Add("GuideLine.ini", GuideLineData);

                                        //int aa = Systems.RecipeData_Collection[0]["ImageMergeOffset.ini"]["Offset"]["AllShift"].ToInt();
                                        break;
                                }
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.CAUTION, "에러가 발생했습니다. 로그를 확인해주세요.");
                Noti.ShowDialog();

                Console.WriteLine(ex.Message);
                throw ex;
                //Systems.LogWriter.Info($@"Recipe Read Fail, Exception : {ex}");
            }
        }
        public static ST_RECIPE_INFO CreateSeqRecipeFromRecipe(Recipe recipe)
        {
            ST_RECIPE_INFO NewRecipe = new ST_RECIPE_INFO(0);
            NewRecipe.RecipeName = recipe.Name.ToString().toUniByteAry(200);
            NewRecipe.RecipePath = recipe.Path.ToString().toUniByteAry(200);
            NewRecipe.GrabCount = recipe.Area_Data.Area.Count;
            for (int i = 0; i < recipe.Area_Data.Area.Count; ++i)
            {
                ST_GRAB_AREA_INFO NewGrabInfo = new ST_GRAB_AREA_INFO(0);
                NewGrabInfo.Name = recipe.Area_Data.Area[i].Name.toUniByteAry(200);
                NewGrabInfo.PtnCount = recipe.Area_Data.Area[i].Patterns.Count;
                for (int j = 0; j < recipe.Area_Data.Area[i].Patterns.Count; ++j)
                {
                    ST_PATTERN_INFO NewPatternInfo = new ST_PATTERN_INFO(0);
                    NewPatternInfo.PatternName = recipe.Area_Data.Area[i].Patterns[j].Name.toUniByteAry(100);
                    NewPatternInfo.Grab = recipe.Area_Data.Area[i].Patterns[j].Grab;
                    NewPatternInfo.Insp = recipe.Area_Data.Area[i].Patterns[j].Insp;
                    NewPatternInfo.Vacuum = recipe.Area_Data.Area[i].Patterns[j].Vacuum;
                    NewPatternInfo.CamCondCount = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Camera_Data.Count;
                    NewPatternInfo.LightCondCount = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Light_Data.Count;
                    for (int k = 0; k < recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Camera_Data.Count; ++k)
                    {
                        ST_CAM_COND NewCamInfo = new ST_CAM_COND(0);
                        NewCamInfo.Name = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Camera_Data[k].Name.toUniByteAry(100);
                        NewCamInfo.Type = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Camera_Data[k].CamType.toUniByteAry(100);
                        NewCamInfo.Use = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Camera_Data[k].Use;
                        NewCamInfo.Expose = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Camera_Data[k].Expose;
                        NewCamInfo.Gain = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Camera_Data[k].Gain;
                        NewCamInfo.PS = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Camera_Data[k].PS;
                        NewCamInfo.Delay = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Camera_Data[k].Delay;
                        NewCamInfo.nCountF = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Camera_Data[k].nCountF;
                        NewCamInfo.nCountB = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Camera_Data[k].nCountB;
                        NewCamInfo.nStartF = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Camera_Data[k].nStartF;
                        NewCamInfo.nStartB = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Camera_Data[k].nStartB;
                        NewCamInfo.nStopF = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Camera_Data[k].nStopF;
                        NewCamInfo.nStopB = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Camera_Data[k].nStopB;
                        NewCamInfo.nPeriodF = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Camera_Data[k].nPeriodF;
                        NewCamInfo.nPeriodB = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Camera_Data[k].nPeriodB;
                        NewPatternInfo.Cam_Condition[k] = NewCamInfo;
                    }
                    for (int k = 0; k < recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Light_Data.Count; ++k)
                    {
                        ST_LIGHT_COND NewLightInfo = new ST_LIGHT_COND(0);
                        NewLightInfo.Use = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Light_Data[k].Use;
                        NewLightInfo.Port_No = (uint)recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Light_Data[k].Port_No;
                        NewLightInfo.Controller_No = (uint)recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Light_Data[k].Controller_No;
                        NewLightInfo.Modules[k] = new STRU_SERIAL_INFO_AOT(0);
                        NewLightInfo.Modules[k].nChCnt = (uint)recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Light_Data[k].LightValue.Count;
                        for (int t = 0; t < recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Light_Data[k].LightValue.Count; ++t)
                        {                                                        
                            NewLightInfo.Modules[k].nLightVal[t] = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.Light_Data[k].LightValue[t];                                             
                        }
                        NewPatternInfo.Light_Condition[k] = NewLightInfo;
                    }
                    for (int k = 0; k < recipe.Area_Data.Area[i].Patterns[j].Grab_Data.AutoFocus.Count; ++k)
                    {
                        ST_AUTOFOCUS NewAutoFocusInfo = new ST_AUTOFOCUS(0);
                        NewAutoFocusInfo.Number = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.AutoFocus[k].Number.toUniByteAry(100);
                        NewAutoFocusInfo.Use = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.AutoFocus[k].Use;
                        NewAutoFocusInfo.AxisZ = recipe.Area_Data.Area[i].Patterns[j].Grab_Data.AutoFocus[k].AxisZ;
                        NewPatternInfo.AutoFocus_Condition[k] = NewAutoFocusInfo;
                    }

                    NewGrabInfo.PatternList[j] = NewPatternInfo;
                }
                NewRecipe.GrabArea[i] = NewGrabInfo;
            }
            return NewRecipe;
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
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }
        /// <summary>
        /// Dictionary Key 변경
        /// </summary>
        /// <typeparam name="TKey"></typeparam>
        /// <typeparam name="TValue"></typeparam>
        /// <param name="dic"></param>
        /// <param name="fromKey"></param>
        /// <param name="toKey"></param>
        public static void RenameKey<TKey, TValue>(this IDictionary<TKey, TValue> dic,
                                      TKey fromKey, TKey toKey)
        {
            TValue value = dic[fromKey];
            dic.Remove(fromKey);
            dic[toKey] = value;
        }
        /// <summary>
        /// 로딩 시작
        /// </summary>
        static public void LoadingStart()
        {
            Ex_Frm_Others_Loading form = null;
            form = new Ex_Frm_Others_Loading();
            Thread thread = new Thread(() => {
            form.Location = new Point(Program.Frm_Main.Location.X + ((Program.Frm_Main.Width / 2) - (form.Width / 2)), Program.Frm_Main.Location.Y + ((Program.Frm_Main.Height / 2) - (form.Height / 2)));
            Program.LoadingForm = form;
            form.Show();
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
        /// <summary>
        /// 로딩 종료
        /// </summary>
        static public void LoadingStop()
        {
            Program.LoadingForm?.Invoke(new Action(delegate ()
            {
                Program.LoadingForm?.Dispose();
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
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
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
               // Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
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
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }

        /// <summary>
        /// 직렬화 Deep Copy
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="source"></param>
        /// <returns></returns>
        static public T DeepCopy<T>(this T source) where T : new()
        {
            if (!typeof(T).IsSerializable)
            {
                // fail
                return source;
            }

            try
            {
                object result = null;
                using (var ms = new MemoryStream())
                {
                    var formatter = new BinaryFormatter();
                    formatter.Serialize(ms, source);
                    ms.Position = 0;
                    result = (T)formatter.Deserialize(ms);
                    ms.Close();
                }

                return (T)result;
            }
            catch (Exception ex)
            {
                // fail
                return new T();
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
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
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
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
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
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
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
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }
        static public void ChangeJobImageSource(CogJob job, bool flag)
        {
            CheckState Checked;
            if (flag)
                Checked = CheckState.Checked;
            else
                Checked = CheckState.Unchecked;

            // CogJobConfiguration Config = new CogJobConfiguration(false, Recipe.MainRecipe[Systems.CurDisplayIndex].Manager.Job(0)) { TopLevel = false };
            CogJobConfiguration Config = new CogJobConfiguration(false, job) { TopLevel = false };
            var rtn = Utility.GetAllControlsRecursive(Config, "chkNormalRunMode");

            (rtn[0] as CheckBox).CheckState = Checked;
            var rtna = Utility.GetAllControlsRecursive(Config, "btnOK");
            (rtna[0] as Button).PerformClick();
            Config.CopyAll(job);
        }
        static public void AdjustCoordination(ref RectangleF rect)
        {
            try
            {
                if (rect.Width < 0)
                {
                    rect.Width = Math.Abs(rect.Width);
                    rect.X -= rect.Width;
                }
                if (rect.Height < 0)
                {
                    rect.Height = Math.Abs(rect.Height);
                    rect.Y -= rect.Height;
                }
            }
            catch (Exception ex)
            {
                //Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
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

        // Percent와 Absolute 인 row, col로 구성되어있다고 가정

        public static Size GetActualPixelSize(TableLayoutPanel panel, int col, int row)
        {

            if (panel.ColumnCount <= col || col < 0 || panel.RowCount <= row || row < 0) return Size.Empty;
            int w = panel.Width, h = panel.Height;
            int nw, nh;

            // 고정 픽셀이면 width를 바로 알수 있다.
            if (panel.ColumnStyles[col].SizeType == SizeType.Absolute)
            {
                nw = (int)panel.ColumnStyles[col].Width;
            }
            // 다른 요소들을 검사후 값을 알아낼 수 있다.

            else
            {
                int another = 0;
                for (int i = 0; i < panel.ColumnCount; ++i)
                {
                    if (panel.ColumnStyles[i].SizeType == SizeType.Absolute)
                        another += (int)panel.ColumnStyles[i].Width;
                }
                nw = (int)((w - another) * (panel.ColumnStyles[col].Width / (float)100));
            }
            // 고정 픽셀이면 height를 바로 알수있다.

            if (panel.RowStyles[row].SizeType == SizeType.Absolute)
            {
                nh = (int)panel.RowStyles[row].Height;
            }
            // 다른 요소들을 검사후 값을 알아낼 수 있다.

            else
            {
                int another = 0;
                for (int i = 0; i < panel.RowCount; ++i)
                {
                    if (panel.RowStyles[i].SizeType == SizeType.Absolute)
                        another += (int)panel.RowStyles[i].Height;
                }
                nh = (int)((h - another) * (panel.RowStyles[row].Height / (float)100));

            }
            return new Size(nw, nh);
        }
        public static Size GetActualPixelPosition(TableLayoutPanel panel, int col, int row)
        {

            if (panel.ColumnCount <= col || col < 0 || panel.RowCount <= row || row < 0) return Size.Empty;
            int w = panel.Width, h = panel.Height;
            int nw, nh;

            // 고정 픽셀이면 width를 바로 알수 있다.
            if (panel.ColumnStyles[col].SizeType == SizeType.Absolute)
            {
                nw = (int)panel.ColumnStyles[col].Width;
            }
            // 다른 요소들을 검사후 값을 알아낼 수 있다.

            else
            {
                int another = 0;
                for (int i = 0; i < panel.ColumnCount; ++i)
                {
                    if (panel.ColumnStyles[i].SizeType == SizeType.Absolute)
                        another += (int)panel.ColumnStyles[i].Width;
                }
                nw = (int)((w - another) * (panel.ColumnStyles[col].Width / (float)100));
            }
            // 고정 픽셀이면 height를 바로 알수있다.

            if (panel.RowStyles[row].SizeType == SizeType.Absolute)
            {
                nh = (int)panel.RowStyles[row].Height;
            }
            // 다른 요소들을 검사후 값을 알아낼 수 있다.

            else
            {
                int another = 0;
                for (int i = 0; i < panel.RowCount; ++i)
                {
                    if (panel.RowStyles[i].SizeType == SizeType.Absolute)
                        another += (int)panel.RowStyles[i].Height;
                }
                nh = (int)((h - another) * (panel.RowStyles[row].Height / (float)100));

            }
            return new Size(nw, nh);
        }
        public static class EnumUtil<T>
        {
            /// <summary>
            /// Enum 문자열을 Object로 반환한다.
            /// </summary>
            /// <param name="s"></param>
            /// <returns></returns>
            public static T Parse(string s)
            {
                return (T)Enum.Parse(typeof(T), s);
            }
            /// <summary>
            /// Enum 값을 Object로 반환한다.
            /// </summary>
            /// <param name="n"></param>
            /// <returns></returns>
            public static T Parse(int n)
            {
                return (T)Enum.ToObject(typeof(T), n);
            }
        }
        public static int GetRandomNum(int min, int max)
        {
            Random Obj = new Random();
            return Obj.Next(min, max);
        }

        //public static bool CheckStringValidity
    }
    public static class Ctrl_DoubleBuffer // Control DoubleBufferd
    {
        public static void DoubleBuffered(this Control control, bool enabled)
        {
            var prop = control.GetType().GetProperty("DoubleBuffered", BindingFlags.Instance | BindingFlags.NonPublic);
            prop.SetValue(control, enabled, null);
        }
    }

    public static class SystemInfomation
    {

    }

}
