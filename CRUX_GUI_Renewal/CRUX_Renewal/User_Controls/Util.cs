using DieEdit.Controls;
using DieEdit.Source;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DieEdit.Other
{
    static class Util
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
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }
        /// <summary>
        /// Verify that the same layer name exists
        /// </summary>
        /// <param name="name"></param>
        /// <param name="list"></param>
        /// <returns></returns>
        static public string CheckLayerName(string name, List<Layer> list)
        {
            try
            {
                if (list.Find(x => x.Name == name) == null)
                {
                    return name;
                }
                else
                {
                    string Name = IncreaseLayerName(name);
                    return CheckLayerName(Name, list);
                }
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }
        /// <summary>
        /// Verify that the same priority exists
        /// </summary>
        /// <param name="list"></param>
        /// <returns></returns>
        static public int CheckLayerPriority(List<Layer> list)
        {
            try
            {
                List<int> PriorityList = new List<int>();
                if (list.Count == 0)
                    return 0;
                foreach (Layer layer in list)
                {
                    PriorityList.Add(layer.Priority);
                }
                PriorityList.Sort((PriorityA, PriorityB) => PriorityA.CompareTo(PriorityB));


                return PriorityList.Last() + 1;
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }

        /// <summary>
        /// Verify that the same name exists
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        static public string IncreaseLayerName(string name)
        {
            try
            {
                string[] Name_Split = name.Split('(');
                if (Name_Split.Length > 1)
                {
                    string LayerCount = Name_Split[1].Replace(")", "");
                    int Count = int.Parse(LayerCount);
                    ++Count;
                    return String.Format("{0}({1})", Name_Split[0], Count);
                }
                else
                {
                    string Name = string.Format("{0}(1)", name);
                    return Name;
                }
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }
        /// <summary>
        /// Find Control name in Control(Recursive)
        /// </summary>
        /// <param name="containerControl"></param>
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
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
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
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
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
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
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
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
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
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
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
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            }
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
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            }
        }
        static public int GetSelectedObject(Point pntClickPoint, List<Point> list)
        {
            try
            {
                if (list == null)
                    return 0;

                int Count = 0;
                List<int> Temp = new List<int>();

                int nCrosses = 0;
                int nConvexCount = list.Count;
                //float[] X_Arr = new float[nConvexCount];
                //float[] Y_Arr = new float[nConvexCount];
                //for (int i = 0; i < nConvexCount; ++i)
                //{
                //    X_Arr[i] = fnCalCoordinate(0, list[i].X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                //    Y_Arr[i] = fnCalCoordinate(0, list[i].Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                //}

                for (int i = 0; i < nConvexCount; ++i)
                {
                    int j = (i + 1) % nConvexCount;
                    if ((list[i].Y > pntClickPoint.Y) != (list[j].Y > pntClickPoint.Y))
                    {
                        double atX = (((list[j].X - list[i].X) / (list[j].Y - list[i].Y)) * ((int)pntClickPoint.Y - list[i].Y)) + list[i].X;
                        if ((int)pntClickPoint.X < atX)
                            nCrosses++;
                    }
                }
                if ((nCrosses % 2) != 0)
                {
                    Count += 1;
                }

                return Count;
            }
            catch (Exception ex)
            {
                throw ex;
                //Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }
        static public int GetSelectedObject(RectangleF rectDragArea, SmartList<Layer> list, ref List<Shape_Obj> objs)
        {
            try
            {
                if (list == null)
                    return 0;

                int Count = 0;
                List<int> Temp = new List<int>();
                foreach (Layer layer in list)
                {
                    if (layer.Selected)
                    {
                        if ((layer.Visible is bool))
                        {
                            SmartList<Shape_Obj> Objs = layer.GetShapeObjs();
                            foreach (Shape_Obj obj in Objs)
                            {
                                int nCrosses = 0;
                                int nConvexCount = obj.GetPolyonList().Count;
                                int[] X_Arr = new int[nConvexCount];
                                int[] Y_Arr = new int[nConvexCount];
                                for (int i = 0; i < obj.GetPolyonList().Count; ++i)
                                {
                                    X_Arr[i] = (int)fnCalCoordinate(0, obj.GetPolyonList()[i].X - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().X, Program.frm_Main.LayerBoard.GetPictureBox().Width, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Width, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                                    Y_Arr[i] = (int)fnCalCoordinate(0, obj.GetPolyonList()[i].Y - Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Y, Program.frm_Main.LayerBoard.GetPictureBox().Height, Program.frm_Main.m_UcRcpTeachPic2.GetImageRect().Height, Program.frm_Main.m_UcRcpTeachPic2.GetRatio());
                                }

                                int nMinX = X_Arr.Min();
                                int nMaxX = X_Arr.Max();
                                int nMinY = Y_Arr.Min();
                                int nMaxY = Y_Arr.Max();


                                for (int m = (int)rectDragArea.Y; m <= (int)rectDragArea.Height; m++)
                                {
                                    if (m < nMinY || m > nMaxY)
                                        continue;
                                    bool bBreak = false;
                                    for (int n = (int)rectDragArea.X; n <= (int)rectDragArea.Width; n++)
                                    {
                                        if (n < nMinX || n > nMaxX)
                                            continue;
                                        for (int j = 0; j < nConvexCount; ++j)
                                        {
                                            int k = (j + 1) % nConvexCount;

                                            if ((Y_Arr[j] > m) != (Y_Arr[k] > m))
                                            {
                                                double atX = (((X_Arr[k] - X_Arr[j]) / (Y_Arr[k] - Y_Arr[j])) * ((int)m - Y_Arr[j])) + X_Arr[j];
                                                if ((int)n < atX)
                                                {
                                                    nCrosses++;
                                                }
                                            }
                                        }
                                        if ((nCrosses % 2) != 0)
                                        {
                                            obj.Selected = true;
                                            Count += 1;
                                            objs.Add(obj);

                                            bBreak = true;
                                            break;
                                        }
                                    }
                                    if (bBreak)
                                        break;
                                }
                            }
                        }
                    }
                }
                return Count;
            }
            catch (Exception ex)
            {
                throw ex;
                //Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
            }
        }
        static public Rectangle FindCoordMinMaxFromObject(Shape_Obj obj)
        {
            try
            {
                List<int> CoordX = new List<int>();
                List<int> CoordY = new List<int>();

                int MaxX = 0;
                int MaxY = 0;
                int MinX = 0;
                int MinY = 0;

                SmartList<Shape_Obj> Temp = new SmartList<Shape_Obj>();
                foreach (PointF p in obj.GetPolyonList())
                {
                    CoordX.Add((int)p.X);
                    CoordY.Add((int)p.Y);
                }
                Temp.Add(obj);

                MinX = CoordX.Min();
                MaxX = CoordX.Max();
                MinY = CoordY.Min();
                MaxY = CoordY.Max();

                return new Rectangle(new Point(MinX, MinY), new Size(MaxX, MaxY));
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }
    
        static public int FindEmptyObjectId(SmartList<Shape_Obj> objs)
        {
            try
            {
                List<int> IdList = new List<int>();

                foreach (Shape_Obj obj in objs)
                {
                    IdList.Add(obj.Id);
                }
                return 0;
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }

        }  

        static public double GetPolygonArea(List<PointF> sourceList)
        {
            try
            {
                double polygonArea = 0d;

                int firstIndex;
                int secondIndex;
                int sourceCount = sourceList.Count;

                PointF firstPoint;
                PointF secondPoint;

                double factor = 0d;

                for (firstIndex = 0; firstIndex < sourceCount; firstIndex++)
                {
                    secondIndex = (firstIndex + 1) % sourceCount;

                    firstPoint = sourceList[firstIndex];
                    secondPoint = sourceList[secondIndex];

                    factor = ((firstPoint.X * secondPoint.Y) - (secondPoint.X * firstPoint.Y));

                    polygonArea += factor;
                }

                polygonArea /= 2d;

                return Math.Abs(polygonArea);
            }
            catch (Exception ex)
            {
                Program.LogMgr.PrintLog(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }
        static public void LinkData(object obj1, object obj2)
        {
            obj1 = obj2;
        }
    }
}
