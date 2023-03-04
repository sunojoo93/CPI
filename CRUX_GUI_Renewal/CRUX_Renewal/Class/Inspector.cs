using Cognex.VisionPro;
using Cognex.VisionPro.Blob;
using Cognex.VisionPro.ImageFile;
using Cognex.VisionPro.ImageProcessing;
using Cognex.VisionPro.Implementation;
using Cognex.VisionPro.LineMax;
using Cognex.VisionPro.QuickBuild;
using Cognex.VisionPro.QuickBuild.Implementation.Internal;
using CRUX_Renewal;
using CRUX_Renewal.Utils;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace CRUX_Renewal.Class
{
    // Inspector 객체는 싱글톤으로 생성
    class Inspector : IEnumerable, IEnumerator
    {
        private static Inspector Inspector_Object;

        int MaxInspectionCount = 5;
        int FaceCount = 8;
        int Now; // Enumerable 변수
        List<Inspection> Inspections;
        RecipeParams Recipe;

        public static Inspector Instance()
        {
            if (Inspector_Object == null)
            {
                Inspector_Object = new Inspector();
            }
            return Inspector_Object;
        }

        public void SetFaceCount(int num)
        {
            FaceCount = num;
        }

        private Inspector()
        {

        }

        public void SetInspection()
        {
            Inspections = new List<Inspection>();
            Inspections.Capacity = MaxInspectionCount;

            //Recipe = new RecipeParams();
        }
        public List<Inspection> GetInspectionList()
        {
            return Inspections;
        }

        public void SetCogManager(CogJobManager manager)
        {
            if(Inspections != null)
            {
                foreach (var item in Inspections)
                    item.Dispose();
                Inspections.RemoveRange(0, Inspections.Count);
            }
            for (int i = 0; i < MaxInspectionCount; ++i)
                Inspections.Add(new Inspection(manager));
        }
            
        public void SetRecipe(RecipeParams recipe)
        {
            Recipe = recipe;
        }

        public void StartJob (InspData insp_param, int ptn_num)
        {
            try
            {
                if ( Inspections == null )
                    throw new Exception("Inspector Initialize Fail");
                if ( Inspections != null && Inspections.Count > MaxInspectionCount )
                    throw new Exception("Over MaxJobCount");

                var Insp = Inspections?.Find(x => x.CommonData.CellID == insp_param.CellID);                
                if ( Insp != null)
                    Insp.StartInspect(insp_param, Recipe, ptn_num);
                else if(Insp == null)
                {
                    var NewInsp = Inspections?.Find(x => x.Busy != true);
                    if ( NewInsp != null )
                        NewInsp.StartInspect(insp_param, Recipe, ptn_num);
                    else
                        throw new Exception("Working All Inspector.");
                }
            }
            catch ( Exception ex )
            {
                // Working All Inspector 예외 발생 시 가장 오래된 Worker를 초기화한다.
                throw ex;
            }
        }
        public object Current
        {
            get
            {
                Inspections = Inspections ?? new List<Inspection>();

                return Inspections[Now - 1];
            }
        }

        public IEnumerator GetEnumerator ()
        {
            return this;
        }

        public bool MoveNext ()
        {
            Now++;

            if ( Now < Inspections.Count )
            {
                return true;
            }
            Reset(); //현재 위치 재설정
            return false;
        }
        public void Reset ()
        {
            Now = -1;
        }
    }
    class Judgement : IDisposable
    {
        public void Dispose ()
        {
            Dispose();
        }
    }
    /// <summary>
    /// 셀 객체
    /// </summary>
    class Inspection : IDisposable
    {
        public bool Busy { get; set; } = false;
        private bool Finished_;

        public CogJobManager JobManager { get; set; } = null;
        List<InspectionWorker> Inspection_Thread = null;

        public CommonInspData CommonData = new CommonInspData();
        public Judgement Judge = new Class.Judgement();

        private object LockObj1 = new object();

        public bool Finished
        {
            get { return Finished_; }
            set
            {
                Finished_ = value;
                if (Finished_)
                {
                    Judgement();
                }
            }
        }

        /// 판정
        /// </summary>
        public void Judgement()
        {
            Judge = new Judgement();

            /// 판정 알고리즘 ///
            CommonData.OutputTime = DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss.fff");
            // 기록
            // 폼 갱신
            Busy = false;
            Clear_Inspection();
        }

        public void Clear_Inspection()
        {
            Dispose();
            Busy = false;
            Finished = false;

        }
        public Inspection(CogJobManager source)
        {
            JobManager = new CogJobManager();

            int JobCount = source.JobCount;
            Inspection_Thread = Inspection_Thread ?? new List<InspectionWorker>();
            for (int i = 0; i < 2; ++i)
            {
                JobManager.JobAdd(new CogJob() { VisionTool = source.Job(0).VisionTool, AcqFifo = source.Job(0).AcqFifo });
                
                Inspection_Thread.Add(new InspectionWorker(JobManager.Name,JobManager.Job(i)));
            }
            Finished = false;
        }
        
        public bool CheckRunState(CogJob job)
        {
            int Count = 0;
            if((job.RunStatus as CogRunStatus).Result == CogToolResultConstants.Accept)
                Systems.LogWriter.Error($"Insp Complete JobManager Name : {JobManager.Name} Job Name : {job.Name} RunState : {job.RunStatus.Result}");
            else
                Systems.LogWriter.Error($"Occured Problem JobManager Name : {JobManager.Name} Job Name : {job.Name} RunState : {job.RunStatus.Result}");
            lock (LockObj1)
            {
                for (int i = 0; i < JobManager.JobCount; ++i)
                {
                    if (((JobManager.Job(i).RunStatus as CogRunStatus)?.Result == CogToolResultConstants.Accept))
                    {
                        Count++;
                        Systems.LogWriter.Error($"InspComplete JobManager Name : {JobManager.Name} Job Name : {JobManager.Job(i).Name}");
                    }
                }
            }
            if (Count >= JobManager.JobCount)
                return true;
            else
                return false;
        }

        public void StartInspect(InspData insp_param, RecipeParams recipe, int thread_num)
        {
            Systems.LogWriter.Info($"Inspect Start Time : {insp_param.InputTime}");
            CommonData = CommonData ?? new CommonInspData();
            CommonData.InputTime = CommonData.InputTime ?? insp_param.InputTime;
            CommonData.CellID = CommonData.CellID ?? insp_param.CellID;
            CommonData.Face = CommonData.Face ?? insp_param.Face;                     
          
            Inspection_Thread[thread_num].InspStart(insp_param);
        }
        public void StartInspect(InspData insp_param, RecipeParams recipe, int thread_num, bool initialize)
        {
            if(initialize)
            {

            }
            Systems.LogWriter.Info($"Inspect Start Time : {insp_param.InputTime}");
            CommonData = CommonData ?? new CommonInspData();
            CommonData.InputTime = CommonData.InputTime ?? insp_param.InputTime;
            CommonData.CellID = CommonData.CellID ?? insp_param.CellID;
            CommonData.Face = CommonData.Face ?? insp_param.Face;

            Inspection_Thread[thread_num].InspStart(insp_param);
        }
        public void Initialize()
        {

        }

        public void Dispose()
        {
            JobManager.Shutdown();
            Judge.Dispose();
        }
    }
    class InspectionWorker
    {
        string MainJobName { get; set; } = string.Empty;

        //Thread Worker;
        private bool Finished_;
        public bool Finished
        {
            get { return Finished_; }
            set
            {
                Finished_ = value;
                if (Finished_)
                {
                    var InspectionTemp = Systems.Inspector_.GetInspectionList().Find(x => x.JobManager.Name == MainJobName);
                    if (InspectionTemp != null)
                        if (InspectionTemp.CheckRunState(Job))
                            InspectionTemp.Finished = true;
                        else
                            Systems.LogWriter.Error($"Not Exist Inspection");
                }
            }
        }
        RecipeParams Parameter;

        Judgement Judge { get; set; }
        InspData InspectData;
        private CogJob Job { get; set; }

        public InspectionWorker (string job_name, CogJob job)
        {
            MainJobName = job_name;
            Job = job;
            SetEvent();
            Judge = new Judgement();
        }
        public void InspStart (InspData data)
        {
            InspectData = InspectData ?? new InspData();
            InspectData = data.DeepCopy<InspData>();

            (Job.AcqFifo as CogAcqFifoSynthetic).Filename = data.Path;
            Job.Run();
        }

        private void SetEvent()
        {
            Job.Stopped += new CogJob.CogJobStoppedEventHandler((sender, e) =>
            {
                var Temp = sender as CogJob;
                Console.WriteLine((Job.RunStatus as CogRunStatus).TotalTime.ToString());
                InspectData.OutputTime = DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss.fff");
                Console.WriteLine($"Origin 검사 완료, RunState : {Job.RunStatus as CogRunStatus} JobName : {Job.Name}");
                Finished = true;
            });
            Job.Running += new CogJob.CogJobRunningEventHandler((sender, e) =>
            {
                Finished = false;
                var Temp = sender as CogJob;
                Console.WriteLine("Origin 검사 시작");
                InspectData.InputTime = DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss.fff");
            });
        }
        public void Do_Judge()
        {
            // 작업 후
            Judge = new Judgement();
        }

        #region 검사
        /// <summary>
        /// 검사 메인 시퀀스
        /// </summary>
        private void Inspect()
        {
            System.Diagnostics.Stopwatch stopwatch = new System.Diagnostics.Stopwatch();
            stopwatch.Start();

            // 시간 측정용 변수
            double dTimae_function = 0.0;


            //------------------------------------------------------------------------------------------------------------------------------

            // 검사 이미지 불러오는 주소
            //string strPath_LoadImage = "C:\\Users\\PC\\Desktop\\result\\1.05_T-B_Test.bmp";
            //string strPath_LoadImage = "D:\\코그넥스 라인피팅\\1.05_T-B_Test.bmp";
            string strPath_LoadImage = "C:\\Users\\woojin\\Desktop\\result\\All.bmp";

            // 이미지 저장 주소
            string strPath_Save = "C:\\Users\\woojin\\Desktop\\result\\Aresult\\";
            //string strPath_Save = "D:\\코그넥스 라인피팅\\Main\\";

            ////////////////////////////     Parameter     ///////////////////////////////////
            bool bImageSave = false;     // 이미지 저장 플래그

            int roi_height = 5000;     // 분할된 영상의 세로길이

            int Sobel_Low_Threshold = 10;       // Sobel Low 임계값 (Default 10)
            int Sobel_High_Threshold = 20;       // Soble High 임계값 (Default 20)

            int nThreshold = 64;// 80;       // 이진화 Value (해당 파라메타보다 작으면 이진화)
            int nDefectPixel = 35;       // 불량 길이 Pixel
            double dMeanGV = 255;      // 불량의 평균 GV (해당 파라메타보다 평균 GV 값이 작아야 불량)
            int nMaxGV = 255;      // 불량의 Max GV (해당 파라메타보다 Max GV 값이 작아야 불량)

            int nThickness = 6;        // 불량 두께(Y 방향) (해당 파라메타 보다 커야 불량 정보 남김)

            int nStart_Y = 0;          // 첫번째 이미지 시작점
            int nEnd_Y = 0;          // 마지막 이미지 끝점

            // int nCell_Height = 41926; // 마지막 이미지 끝부분 찾기위한 Cell 길이, 추후 Cell 실측길이 / 레졸루션으로 계산
            //////////////////////////////////////////////////////////////////////////////////

            // 이미지 불러오기
            CogImage8Grey image8Grey = Load_Image(strPath_LoadImage);           // 원본 이미지

            CheckTime(stopwatch, "Image Load Time : ", ref dTimae_function);

            // 결과 이미지
            CogImage8Grey imageDst = new CogImage8Grey(image8Grey.Width, image8Grey.Height);

            ////////////////////////////////////////////////////// 시작점 찾기//////////////////////////////////
            CogImage8Grey stQuantize = Quantization_st(image8Grey);
            ImageSave(stQuantize, "D:\\Findstart_Quant.bmp", bImageSave);

            // Sobel
            CogImage8Grey stSobel = SobelFunction(stQuantize, Sobel_Low_Threshold, Sobel_High_Threshold);
            ImageSave(stSobel, "D:\\Findstart_Sobel.bmp", bImageSave);

            CogImage8Grey stReal_Edge = FindEdge(stSobel);
            ImageSave(stReal_Edge, "D:\\Findstart_Real_Edge.bmp", bImageSave);



            Find_Start_Edge(stReal_Edge, 10000, ref nStart_Y);

            Console.WriteLine("이미지 Corner 첫 Y좌표 = " + nStart_Y);
            CheckTime(stopwatch, "Image Start Y Find : ", ref dTimae_function);

            ////////////////////////////////////////////////////// 끝점 찾기//////////////////////////////////

            CogImage8Grey EnQuantize = Quantization_st(image8Grey);
            ImageSave(EnQuantize, "D:\\FindEnd_Quant.bmp", bImageSave);

            // Sobel
            CogImage8Grey EnSobel = SobelFunction(EnQuantize, Sobel_Low_Threshold, Sobel_High_Threshold);
            ImageSave(EnSobel, "D:\\FindEnd_Sobel.bmp", bImageSave);

            CogImage8Grey EnReal_Edge = FindEdge(EnSobel);
            ImageSave(EnReal_Edge, "D:\\FindEnd_Real_Edge.bmp", bImageSave);



            Find_End_Edge(EnReal_Edge, 10000, ref nEnd_Y);

            Console.WriteLine("이미지 Corner 마지막 Y좌표 = " + nEnd_Y);
            CheckTime(stopwatch, "Image End Y Find : ", ref dTimae_function);

            /////////////////////////////////////////////////////////////////////////////////////////////////////

            CogImage8Grey image8Grey_Temp = CogImageConvert.GetIntensityImage(image8Grey, 0, nStart_Y, image8Grey.Width, nEnd_Y - nStart_Y);
            ImageSave(image8Grey_Temp, "D:\\FindstartEnd_Ori.bmp", bImageSave);

            //////////////////////////////////////////////////// 검사 /////////////////////////////////////////////////////////////
            // 영상 분할 검사
            int img_count = image8Grey.Height / roi_height;                     // 분할된 영상의 개수

            //int nLast_roi = (image8Grey.Height - nStart_Y) % roi_height;         // 마지막 영역 나머지 픽셀길이 구하기
            int nLast_roi = (nEnd_Y - nStart_Y) % roi_height;         // 마지막 영역 나머지 픽셀길이 구하기

            if (nLast_roi != 0)                                                 // 짜투리 영역 존재시 바로 위에 영역과 합침
            {
                if ((int)(nStart_Y / roi_height) > 0)                           // 시작 위치가 분할하려는 size보다 클 경우
                    img_count = img_count - (int)(nStart_Y / roi_height) - 1;

                else
                    img_count = img_count - 1;
            }


            //nCell_Height -= ((img_count - 1) * roi_height);                     // 마지막 이미지 height 계산

            string[] strDefectInfo = new string[img_count];                     // 불량 위치등 정보 저장할 string


            /////////////////////////////////////////////////////////////////////////////// 메인 검사
            Parallel.For(0, img_count, (img_num) =>
            //for (int img_num = 0; img_num < img_count; img_num++)
            //for (int img_num = 0; img_num < 1; img_num++)
            {




                CogImage8Grey ROIImage = new CogImage8Grey();
                if (img_num == img_count - 1 && nLast_roi != 0)
                {
                    // 영상 분할
                    ROIImage = CogImageConvert.GetIntensityImage(image8Grey, 0, (img_num * roi_height) + nStart_Y, image8Grey.Width, roi_height + nLast_roi);
                    ImageSave(ROIImage, strPath_Save + img_num + "_ROI_Image.bmp", bImageSave);
                }
                else
                {
                    // 영상 분할
                    ROIImage = CogImageConvert.GetIntensityImage(image8Grey, 0, (img_num * roi_height) + nStart_Y, image8Grey.Width, roi_height);
                    ImageSave(ROIImage, strPath_Save + img_num + "_ROI_Image.bmp", bImageSave);
                }




                // 이미지 양자화
                CogImage8Grey Quantize = Quantization(ROIImage);
                ImageSave(Quantize, strPath_Save + img_num + "_Quantize_Image.bmp", bImageSave);

                // Sobel
                CogImage8Grey Sobel = SobelFunction(Quantize, Sobel_Low_Threshold, Sobel_High_Threshold);
                ImageSave(Sobel, strPath_Save + img_num + "_Sobel_Image.bmp", bImageSave);


                // Edge 찾기

                CogImage8Grey Real_Edge = FindEdge(Sobel);
                ImageSave(Real_Edge, strPath_Save + img_num + "_Real_Edge_Image.bmp", bImageSave);


                // 이상적인 Edge 그리기(직선)
                CogImage8Grey Ideal_Edge = DrawingEdge(Real_Edge, img_num);
                ImageSave(Ideal_Edge, strPath_Save + img_num + "_Ideal_Edge.bmp", bImageSave);




                // 곡선이 포함된 이미지인 경우 곡선부 그리기
                if (0 == img_num || img_num == img_count - 1)   // 0번 이미지가 곡선 이미지
                {
                    bool bType = false; // false : Top, True : Bottom

                    if (0 != img_num)
                    {
                        bType = true;
                    }


                    //  bType = true; // false : Top, True : Bottom

                    Ideal_Edge = DrawingRoundEdge(Real_Edge, Ideal_Edge, bType);
                    ImageSave(Ideal_Edge, strPath_Save + img_num + "_Ideal_Edge.bmp", bImageSave);
                }





                // 불량 검출
                CogImage8Grey Threshold_Image = FindDefect(ROIImage, Quantize, Ideal_Edge, nThreshold, nDefectPixel, dMeanGV, nMaxGV, bImageSave);
                ImageSave(Threshold_Image, strPath_Save + img_num + "_Threshold.bmp", bImageSave);

                // Image Blob -> 2차 필터(불량 두께)
                CogImage8Grey Threshold_Info = Threshold_Image.Copy();
                strDefectInfo[img_num] = DefectBlob(ref Threshold_Image, ref Threshold_Info, nDefectPixel, nThickness, img_num, roi_height, true);
                ImageSave(Threshold_Image, strPath_Save + img_num + "_Threshold_Filter.bmp", bImageSave);
                ImageSave(Threshold_Info, strPath_Save + img_num + "_Threshold_Info.bmp", bImageSave);



                // 이미지 집어넣기
                MakeDstImage(ref imageDst, Threshold_Info, img_num, roi_height, nStart_Y);



            });
            //}
            //  CheckTime(stopwatch, "Inspection Time : ", ref dTimae_function);

            ///////// 검사 완료

            // 불량 결과 출력
            ImageSave(imageDst, strPath_Save + "Last_Dst.bmp", true);    // 이미지 저장

            WriteInfo(strDefectInfo, img_count, strPath_Save + "Defect Info.txt");


            CheckTime(stopwatch, "Save Data Info Time : ", ref dTimae_function);

            stopwatch.Stop();

            Console.WriteLine("Total Time : " + (double)stopwatch.ElapsedMilliseconds / 1000 + "sec");
        }

        /// <summary>
        /// 검사 시간 확인용 함수
        /// </summary>
        /// <param name="stopwatch"></param>
        /// <param name="dTimae_function"></param>
        /// <param name="dTime_sum"></param>
        void CheckTime(System.Diagnostics.Stopwatch stopwatch, string str, ref double dTimae_function)
        {
            dTimae_function = (double)stopwatch.ElapsedMilliseconds / 1000 - dTimae_function;      // 함수 동작 시간

            Console.WriteLine(str + dTimae_function + "sec");   // 시간 출력
        }

        /// <summary>
        /// 이미지 저장
        /// </summary>
        /// <param name="InputImage"></param>
        /// <param name="Path"></param>
        /// <param name="bUse"></param>
        void ImageSave(CogImage8Grey InputImage, string Path, bool bUse)
        {
            if (!bUse)
                return;

            CogImageFileBMP save = new CogImageFileBMP();
            save.Open(Path, CogImageFileModeConstants.Write);
            save.Append(InputImage);
            save.Close();
        }

        /// <summary>
        /// 디펙 정보 저장
        /// </summary>
        /// <param name=""></param>
        /// <param name=""></param>
        void WriteInfo(string[] strDefectInfo, int nSize, string Path)
        {
            string strTXT = "";

            for (int i = 0; i < nSize; i++)
            {
                strTXT += strDefectInfo[i];
            }

            StreamWriter sw = new StreamWriter(Path, true);
            sw.WriteLine(strTXT);
            sw.Close();
        }

        /// <summary>
        /// 검사 이미지 불러오기 <CogImage8Grey>
        /// </summary>
        /// <param name="strPath"></param>
        /// <returns></returns>
        private CogImage8Grey Load_Image(string strPath)
        {
            CogImageFile img = new CogImageFile();

            img.Open(strPath, CogImageFileModeConstants.Read);
            CogImage8Grey image8Grey = CogImageConvert.GetIntensityImage(img[0], 0, 0, img[0].Width, img[0].Height);

            img.Close();

            return image8Grey;
        }

        /// <summary>
        /// 이미지 양자화
        /// </summary>
        /// <param name="InputImage"></param>
        /// <returns></returns>
        private CogImage8Grey Quantization(CogImage8Grey InputImage)
        {
            CogIPOneImageQuantize Quantize = new CogIPOneImageQuantize();
            /*  Quantize.Levels = CogIPOneImageQuantizeLevelConstants.s32;*/
            Quantize.Levels = CogIPOneImageQuantizeLevelConstants.s4;

            CogIPOneImageTool IPO = new CogIPOneImageTool();
            IPO.Operators.Add(Quantize);

            IPO.InputImage = InputImage;
            IPO.Run();

            return (CogImage8Grey)IPO.OutputImage;
        }

        /// <summary>
        /// Sobel 함수
        /// </summary>
        /// <param name="InputImage"></param>
        /// <param name="Sobel_Low_Threshold"></param>
        /// <param name="Sobel_High_Threshold"></param>
        /// <returns></returns>
        private CogImage8Grey SobelFunction(CogImage8Grey InputImage, int Sobel_Low_Threshold, int Sobel_High_Threshold)
        {
            CogSobelEdgeTool Sobel = new CogSobelEdgeTool();
            Sobel.InputImage = InputImage;


            Sobel.RunParams.PostProcessMode = CogSobelEdgePostProcessModeConstants.HysteresisThreshold;
            Sobel.RunParams.HysteresisThresholdLow = Sobel_Low_Threshold;
            Sobel.RunParams.HysteresisThresholdHigh = Sobel_High_Threshold;
            Sobel.Run();

            return (CogImage8Grey)Sobel.Result.FinalMagnitudeImage;
        }

        /// <summary>
        /// 가장 우측 Edge 찾기
        /// Unsafe 함수 -> 최적화가 안됨
        /// </summary>
        /// <param name="InputImage"></param>
        /// <returns></returns>
        unsafe private CogImage8Grey FindEdge(CogImage8Grey InputImage)
        {
            int nWidth = InputImage.Width;
            int nHeight = InputImage.Height;

            CogImage8Grey OutputImage = new CogImage8Grey(nWidth, nHeight);

            ICogImage8PixelMemory mem_Input = InputImage.Get8GreyPixelMemory(CogImageDataModeConstants.Read, 0, 0, InputImage.Width, InputImage.Height);
            ICogImage8PixelMemory mem_Output = OutputImage.Get8GreyPixelMemory(CogImageDataModeConstants.Write, 0, 0, OutputImage.Width, OutputImage.Height);


            int srcStride = mem_Input.Stride;
            int dstStride = mem_Output.Stride;


            Byte* pSrc = (Byte*)(mem_Input.Scan0);
            Byte* pDst = (Byte*)(mem_Output.Scan0);

            for (int y = 0; y < nHeight; y++)
            {
                for (int x = nWidth; x >= 0; x--)
                {
                    if (*(pSrc + x) > 0)
                    {
                        *((Byte*)(pDst + x)) = *((Byte*)(pSrc + x));
                        break;
                    }

                }
                pSrc += srcStride;
                pDst += dstStride;
            }
            mem_Input.Dispose();
            mem_Output.Dispose();

            return OutputImage;
        }

        /// <summary>
        /// 이상적인 Edge Line 그리기
        /// </summary>
        /// <param name="InputImage"></param>
        /// <returns></returns>
        private CogImage8Grey DrawingEdge(CogImage8Grey InputImage, int img_num)
        {
            /*
                        Stopwatch stopwatch_1 = new Stopwatch();
                        if (img_num == 1)
                            stopwatch_1.Start();
                            */


            CogLineMax gg = new CogLineMax();
            CogLineMaxTool cLm = new CogLineMaxTool();
            CogLineMaxEdgeDetectionParams edge = new CogLineMaxEdgeDetectionParams();

            cLm.InputImage = InputImage;

            cLm.RunParams.DistanceTolerance = 15.0;
            cLm.RunParams.Polarity = CogLineMaxPolarityConstants.Mixed;
            cLm.RunParams.FittingMode = CogLineMaxFittingModeConstants.RANSAC;
            cLm.RunParams.EdgeDetectionParams.GradientKernelSizeInPixels = 5;
            cLm.RunParams.EdgeDetectionParams.ProjectionLengthInPixels = 50;
            cLm.RunParams.ExpectedLineNormal.SelectedSpaceName = ".";
            CogRectangle aa = new CogRectangle();
            aa.X = 0;
            aa.Y = 0;

            aa.Width = InputImage.Width;
            aa.Height = InputImage.Height;

            cLm.Region = aa;

            cLm.RunParams.ExpectedLineNormal.Angle = -3.14159;


            cLm.Run();


            //  if (img_num == 1)
            //      stopwatch_1.Stop();


            //    Console.WriteLine(img_num + "번 : " + (double)stopwatch_1.ElapsedMilliseconds / 1000 + "sec");


            int X = (int)cLm.Results[0].GetLineSegment().StartX;
            int Y = (int)cLm.Results[0].GetLineSegment().StartY;
            int E_X = (int)cLm.Results[0].GetLineSegment().EndX;
            int E_Y = (int)cLm.Results[0].GetLineSegment().EndY;

            if (Y != 0)
                Y = 0;
            if (E_Y != InputImage.Height)
                E_Y = InputImage.Height;






            Bitmap bit = InputImage.ToBitmap();
            Graphics draw_line = Graphics.FromImage(bit);
            draw_line.Clear(Color.Black);

            draw_line.DrawLine(new Pen(Color.White), new Point(X, Y), new Point(E_X, E_Y));

            CogImage8Grey OutputImage = new CogImage8Grey(bit);


            return OutputImage;
        }

        /// <summary>
        /// 이상적인 Round Edge 그리기
        /// </summary>
        /// <param name="InputImage"></param>
        /// <returns></returns>
        private CogImage8Grey DrawingRoundEdge(CogImage8Grey InputImage_Round, CogImage8Grey InputImage_Line, bool bType)
        {
            // Round와 직선의 교착점 찾기
            int intersection_X = 0;         // 접점 x좌표
            int intersection_Y = 0;         // 접점 y좌표
            FindTargetPoint(InputImage_Round, InputImage_Line, ref intersection_X, ref intersection_Y, bType);
            if (bType == false)
                Console.WriteLine("처음 영역 겹치는 위치 X,Y = " + intersection_X + "," + intersection_Y);
            if (bType == true)
                Console.WriteLine("마지막 영역 겹치는 위치 X,Y = " + intersection_X + "," + intersection_Y);

            // Round 피팅을 위한 Point 샘플 구하기
            int nCnt = 0;
            int nHeight = intersection_Y;

            if (bType)
                nHeight = InputImage_Round.Height - intersection_Y;

            if (nHeight % 500 == 0)
                nCnt = nHeight / 500;
            else
                nCnt = nHeight / 500;

            Point[] pt = new Point[nCnt];
            GetSamplePoint(InputImage_Round, intersection_X, intersection_Y, nCnt, ref pt, bType);

            // 곡선(원) 그리기
            CogImage8Grey Round = DrawingCircle(InputImage_Round, intersection_Y, nCnt, pt, bType);

            // 곡선과 직선 이어붙이기
            InputImage_Round = AddRoundLine(InputImage_Line, Round, bType);

            return InputImage_Round;
        }

        /// <summary>
        /// Round와 직선의 교착점 찾기
        /// Unsafe 함수 -> 최적화가 안됨
        /// </summary>
        /// <param name="InputImage_Round"></param>
        /// <param name="InputImage_Line"></param>
        /// <param name="intersection_X"></param>
        /// <param name="intersection_Y"></param>
        unsafe private void FindTargetPoint(CogImage8Grey InputImage_Round, CogImage8Grey InputImage_Line, ref int intersection_X, ref int intersection_Y, bool bType)
        {
            int nWidth = InputImage_Round.Width;
            int nHeight = InputImage_Round.Height;

            ICogImage8PixelMemory mem_round = InputImage_Round.Get8GreyPixelMemory(CogImageDataModeConstants.Read, 0, 0, nWidth, nHeight);
            ICogImage8PixelMemory mem_line = InputImage_Line.Get8GreyPixelMemory(CogImageDataModeConstants.Read, 0, 0, nWidth, nHeight);


            int roundStride = mem_round.Stride;
            int lineStride = mem_line.Stride;



            Byte* pRound = (Byte*)(mem_round.Scan0);
            Byte* pLine = (Byte*)(mem_line.Scan0);

            if (bType)    // Bottom 검사
            {
                pRound += roundStride * (nHeight - 1);
                pLine += lineStride * (nHeight - 1);

                for (int y = nHeight - 1; y >= 0; y--)
                {
                    for (int x = 0; x < nWidth; x++)
                    {
                        if (*(pRound + x) > 0 && *(pLine + x) > 0)
                        {
                            intersection_X = x;
                            intersection_Y = y;
                            goto next; // 첫 교차점 찾은 후 이중 반복문 종료
                        }
                    }
                    pRound -= roundStride;
                    pLine -= lineStride;
                }
            }
            else    // Top 검사 위에서부터 겹치는 점 찾기
            {
                for (int y = 0; y < nHeight; y++)
                {
                    for (int x = 0; x < nWidth; x++)
                    {
                        if (*(pRound + x) > 0 && *(pLine + x) > 0)
                        {
                            intersection_X = x;
                            intersection_Y = y;
                            goto next; // 첫 교차점 찾은 후 이중 반복문 종료
                        }
                    }
                    pRound += roundStride;
                    pLine += lineStride;
                }
            }

            // 교차점을 찾지 못한경우 -> 가장 가까운 부분을 교차점으로 판단

            pRound = (Byte*)(mem_round.Scan0);
            pLine = (Byte*)(mem_line.Scan0);

            if (bType)    // Bottom 검사
            {
                pRound += roundStride * (nHeight - 1);
                pLine += lineStride * (nHeight - 1);

                int nLangth = 99999999;

                for (int y = nHeight - 1; y >= 0; y--)
                {
                    bool bRoundFlag = true;
                    bool bLineFlag = true;

                    int nX_Roune = 0;
                    int nX_Line = 0;

                    for (int x = 0; x < nWidth; x++)
                    {
                        if (*(pRound + x) > 0 && bRoundFlag)
                        {
                            nX_Roune = x;
                            bRoundFlag = false;
                        }
                        if (*(pLine + x) > 0 && bLineFlag)
                        {
                            nX_Line = x;
                            bLineFlag = false;
                        }

                        if (!bRoundFlag && !bLineFlag)
                        {
                            if (nLangth > nX_Line - nX_Roune && nX_Line - nX_Roune >= 0)      // 그려진 직선(RANSAC)보다 왼쪽에있는 선 중에서 가장 가까운 지점을 찾음
                            {
                                nLangth = nX_Line - nX_Roune;
                                intersection_X = nX_Line;
                                intersection_Y = y;
                            }
                            break;
                        }
                    }
                    pRound -= roundStride;
                    pLine -= lineStride;
                }
            }
            else    // Top 검사 위에서부터 겹치는 점 찾기
            {
                int nLangth = 99999999;

                for (int y = 0; y < nHeight; y++)
                {
                    bool bRoundFlag = true;
                    bool bLineFlag = true;

                    int nX_Roune = 0;
                    int nX_Line = 0;

                    for (int x = 0; x < nWidth; x++)
                    {
                        if (*(pRound + x) > 0 && bRoundFlag)
                        {
                            nX_Roune = x;
                            bRoundFlag = false;
                        }
                        if (*(pLine + x) > 0 && bLineFlag)
                        {
                            nX_Line = x;
                            bLineFlag = false;
                        }

                        if (!bRoundFlag && !bLineFlag)
                        {
                            if (nLangth > nX_Line - nX_Roune && nX_Line - nX_Roune >= 0)      // 그려진 직선(RANSAC)보다 왼쪽에있는 선 중에서 가장 가까운 지점을 찾음
                            {
                                nLangth = nX_Line - nX_Roune;
                                intersection_X = nX_Line;
                                intersection_Y = y;
                            }
                            break;
                        }
                    }
                    pRound += roundStride;
                    pLine += lineStride;
                }
            }
            next:
            ;

            mem_round.Dispose();
            mem_line.Dispose();
        }

        /// <summary>
        /// Round 피팅을 위해 Round 부분 샘플링 취득
        /// /// Unsafe 함수 -> 최적화가 안됨
        /// </summary>
        /// <param name="InputImage_Round"></param>
        /// <param name="intersection_Y"></param>
        /// <param name="nCnt"></param>
        /// <param name="pt"></param>
        unsafe private void GetSamplePoint(CogImage8Grey InputImage_Round, int intersection_X, int intersection_Y, int nCnt, ref Point[] pt, bool bType)
        {

            int nWidth = InputImage_Round.Width;
            int nHeight = intersection_Y;

            ICogImage8PixelMemory mem_round = InputImage_Round.Get8GreyPixelMemory(CogImageDataModeConstants.Read, 0, 0, nWidth, intersection_Y);

            if (bType)       // bottom 검사시 ROI가 다름
            {
                nHeight = InputImage_Round.Height - intersection_Y;
                mem_round = InputImage_Round.Get8GreyPixelMemory(CogImageDataModeConstants.Read, 0, intersection_Y, nWidth, nHeight);
            }


            int roundStride = mem_round.Stride;

            Byte* pRound = (Byte*)(mem_round.Scan0);

            pt[0] = new Point(intersection_X, intersection_Y);

            int testcount = 1;

            if (bType) // Bottom 검사
            {
                pRound += roundStride * (nHeight - 1);

                for (int y = InputImage_Round.Height - 1; y >= intersection_Y; y--)
                {

                    if (y == intersection_Y)
                    {
                        pRound -= roundStride * 500;
                        y = y - 500;
                        continue;
                    }

                    for (int x = 0; x < nWidth; x++)
                    {
                        if (*(pRound + x) > 0)
                        {
                            pt[testcount] = new Point(x, y);
                            testcount++;
                            y = y - 500;
                            pRound -= roundStride * 500;
                            break;
                        }
                        if (testcount == nCnt)
                            goto next; // Point 인덱스가 넘어가지 않게 샘플링 취득

                    }

                    pRound -= roundStride;

                    // pRound += roundStride;
                }
            }
            else
            {
                for (int y = 0; y < intersection_Y; y++)
                {
                    for (int x = 0; x < nWidth; x++)
                    {
                        if (*(pRound + x) > 0)
                        {
                            pt[testcount] = new Point(x, y);
                            testcount++;
                            y = y + 500;
                            pRound += roundStride * 500;
                            break;
                        }
                        if (testcount == nCnt)
                            goto next; // Point 인덱스가 넘어가지 않게 샘플링 취득
                    }

                    pRound -= roundStride;
                    // pRound += roundStride;
                }
            }

            next:
            ;

            mem_round.Dispose();
        }

        /// <summary>
        /// 원그리기를 통한 이상적인 곡선부분 그리기
        /// </summary>
        /// <param name="InputImage_Round"></param>
        /// <param name="intersection_Y"></param>
        /// <param name="nCnt"></param>
        /// <param name="pt"></param>
        /// <returns></returns>
        private CogImage8Grey DrawingCircle(CogImage8Grey InputImage_Round, int intersection_Y, int nCnt, Point[] pt, bool bType)
        {
            CogFitCircleTool FitCircle = new CogFitCircleTool();

            int nHeight = intersection_Y;

            if (bType)   // bottom 일 때
            {
                nHeight = InputImage_Round.Height - intersection_Y;
                FitCircle.InputImage = CogImageConvert.GetIntensityImage(InputImage_Round, 0, intersection_Y, InputImage_Round.Width, nHeight);
            }
            else        // Top 일 때
            {
                FitCircle.InputImage = CogImageConvert.GetIntensityImage(InputImage_Round, 0, 0, InputImage_Round.Width, nHeight);
            }

            FitCircle.RunParams.NumPoints = nCnt; // 핏팅하기 위한 점개수 설정 setPoint 함수 개수




            for (int i = 0; i < nCnt; i++)
                FitCircle.RunParams.SetPoint(i, pt[i].X, pt[i].Y);

            FitCircle.Run();

            double Radius = FitCircle.Result.GetCircle().Radius;
            double[] distance0 = new double[nCnt];
            double[] test_x = new double[nCnt];
            double[] test_y = new double[nCnt];
            bool[] test_bool = new bool[nCnt];

            double center_x = FitCircle.Result.GetCircle().CenterX;
            double center_y = FitCircle.Result.GetCircle().CenterY;

            CogImage8Grey circle_resut = new CogImage8Grey((int)InputImage_Round.Width, nHeight); // 이미지 그리는 크기는 라운드 영역 크기
            Bitmap Bit_circle = circle_resut.ToBitmap();
            Graphics circle = Graphics.FromImage(Bit_circle);

            int nSRx = (int)((Radius - center_x) * (-1));
            int nSRy = (int)((Radius - center_y) * (-1));

            if (bType)
            {
                // Rectangle 이상함 Y을 아래로 내리려면 (+)가 아니라 (-)를 해줘야함 (영상처리 좌표랑 다른듯)
                // x에 +1하는건 억지로 맞추기 위해서(1Pixel 어긋남.....)
                // center_y좌표가 이상하게 나옴 -> 해당 좌표 기준이 Height 5000인 영상 기준으로 좌표가 나옴 영상을 아무리 짤라봐도 H 5000인 영상 좌표임 -> 좌표 추가 보정해줌
                Rectangle Rt = new Rectangle((int)nSRx + 1, (int)(nSRy - intersection_Y), (int)Radius * 2, (int)Radius * 2); // Rect 위치는 원을 그리는 사각형 시작 위치여야함 (음수 가능)
                circle.DrawEllipse(Pens.White, Rt);
            }
            else
            {
                Rectangle Rt = new Rectangle((int)nSRx, (int)(nSRy), (int)Radius * 2, (int)Radius * 2); // Rect 위치는 원을 그리는 사각형 시작 위치여야함 (음수 가능)
                circle.DrawEllipse(Pens.White, Rt);
            }
            circle_resut = new CogImage8Grey(Bit_circle);

            return circle_resut;
        }

        /// <summary>
        /// Round 영역과 직선 부분을 이어 그리기
        /// Unsafe 함수 -> 최적화가 안됨
        /// </summary>
        /// <param name="InputAndOutput"></param>
        /// <param name="InputImage"></param>
        /// <returns></returns>
        unsafe private CogImage8Grey AddRoundLine(CogImage8Grey InputAndOutput, CogImage8Grey InputImage, bool bType)
        {
            int nWidth = InputImage.Width;
            int nHeight = InputImage.Height;

            ICogImage8PixelMemory mem_Dst = InputAndOutput.Get8GreyPixelMemory(CogImageDataModeConstants.Read, 0, 0, InputAndOutput.Width, InputAndOutput.Height);
            ICogImage8PixelMemory mem_src = InputImage.Get8GreyPixelMemory(CogImageDataModeConstants.Write, 0, 0, nWidth, nHeight);


            int Stride_dst = mem_Dst.Stride;
            int Stride_src = mem_src.Stride;

            Byte* pDst = (Byte*)(mem_Dst.Scan0);
            Byte* pSrc = (Byte*)(mem_src.Scan0);

            if (bType)       // Bottom 검사
            {
                pDst += Stride_dst * (InputAndOutput.Height - nHeight - 1);

                for (int y = 0; y < nHeight; y++)
                {
                    for (int x = 0; x < nWidth; x++)
                    {
                        *((Byte*)(pDst + x)) = *((Byte*)(pSrc + x));

                    }
                    pDst += Stride_dst;
                    pSrc += Stride_src;
                }
            }
            else            // Top 검사
            {
                for (int y = 0; y < nHeight; y++)
                {
                    for (int x = 0; x < nWidth; x++)
                    {
                        *((Byte*)(pDst + x)) = *((Byte*)(pSrc + x));

                    }
                    pDst += Stride_dst;
                    pSrc += Stride_src;
                }
            }

            mem_Dst.Dispose();
            mem_src.Dispose();

            return InputAndOutput;
        }

        /// <summary>
        /// 불량 찾기
        /// 1차 필터 포함 (Mean, Max GV)
        /// Unsafe 함수 -> 최적화가 안됨
        /// </summary>
        /// <param name = "InputQuantize" ></ param >
        /// < param name="InputEdge"></param>
        /// <param name = "nThreshold" ></ param >
        /// < param name="nDefectPixel"></param>
        /// <param name = "dMeanGV" ></ param >
        /// < param name="nMaxGV"></param>
        /// <param name = "bImageSvae" ></ param >
        /// < returns ></ returns >
        unsafe private CogImage8Grey FindDefect(CogImage8Grey Roi_Ori, CogImage8Grey InputQuantize, CogImage8Grey InputEdge, int nThreshold, int nDefectPixel, double dMeanGV, int nMaxGV, bool bImageSvae)
        {
            CogImage8Grey Roi_Ori_copy = new CogImage8Grey();
            Roi_Ori_copy = Roi_Ori.Copy();

            // 이부분 때문에 프로그램 죽음. 원인 찾기.
            // InputQuantize와 Roi_Ori Size는 같아야 한다.
            if (InputQuantize.Width != Roi_Ori_copy.Width || InputQuantize.Height != Roi_Ori_copy.Height)
                Roi_Ori_copy = (CogImage8Grey)Roi_Ori_copy.ScaleImage(InputQuantize.Width, InputQuantize.Height);

            // InputQuantize와 InputEdge의 Size는 같아야 한다.
            if (InputQuantize.Width != InputEdge.Width || InputQuantize.Height != InputEdge.Height)
                InputEdge = (CogImage8Grey)InputEdge.ScaleImage(InputQuantize.Width, InputQuantize.Height);


            int nWidth = InputQuantize.Width;
            int nHeight = InputQuantize.Height;

            CogImage8Grey OutputThreshold = new CogImage8Grey(nWidth, nHeight);       // 결과 출력용
            CogImage8Grey SaveImage = new CogImage8Grey(nWidth, nHeight);       // 연산 보여주기용

            ICogImage8PixelMemory mem_Quant = InputQuantize.Get8GreyPixelMemory(CogImageDataModeConstants.Read, 0, 0, nWidth, nHeight);         // 양자화 이미지
            ICogImage8PixelMemory mem_Edge = InputEdge.Get8GreyPixelMemory(CogImageDataModeConstants.Read, 0, 0, nWidth, nHeight);             // Edge 이미지
            ICogImage8PixelMemory mem_Thresh = OutputThreshold.Get8GreyPixelMemory(CogImageDataModeConstants.Write, 0, 0, nWidth, nHeight);      // 이진화 이미지(Output)
            ICogImage8PixelMemory mem_Save = SaveImage.Get8GreyPixelMemory(CogImageDataModeConstants.ReadWrite, 0, 0, nWidth, nHeight);            // 이미지 저장용
            ICogImage8PixelMemory mem_Ori = Roi_Ori_copy.Get8GreyPixelMemory(CogImageDataModeConstants.ReadWrite, 0, 0, nWidth, nHeight);

            int Stride_Quant = mem_Quant.Stride;
            int Stride_Edge = mem_Edge.Stride;
            int Stride_Thresh = mem_Thresh.Stride;
            int Stride_Save = mem_Save.Stride;
            int Stride_Ori = mem_Ori.Stride;



            Byte* pQuant = (Byte*)(mem_Quant.Scan0);
            Byte* pEdge = (Byte*)(mem_Edge.Scan0);
            Byte* pThresh = (Byte*)(mem_Thresh.Scan0);
            Byte* pSave = (Byte*)(mem_Save.Scan0);
            Byte* pOri = (Byte*)(mem_Ori.Scan0);


            for (int y = 0; y < nHeight; y++)
            {
                int nSum = 0;           // 불량 부분의 GV 합
                int nMax = 0;           // 불량 부분 최고 밝기
                int nPixel_count = 0;   // 불량 X길이 측정용
                double Ori_mean_threshold = 0; // 원본위치 평균

                for (int x = 0; x < nWidth; x++)
                {
                    bool bFlag = false;

                    // Edge 찾기
                    if (*(pEdge + x) > 0)
                    {
                        // 불량 Pixel 거리 측정
                        for (int x_before = x - nDefectPixel; x_before < x; x_before++)
                        {
                            /*
                            if (*(pQuant + x_before) < nThreshold)
                            {
                                nPixel_count++;
                                nSum += (int)(*(pQuant + x_before));

                                if (nMax < *(pQuant + x_before))
                                    nMax = (int)(*(pQuant + x_before));
                            }
                            */
                            // 양자화를 기준으로 진입하고 원본 데이터 비교
                            if (*(pQuant + x_before) < nThreshold)
                            {
                                nPixel_count++;
                                nSum += (int)(*(pOri + x_before));

                                if (nMax < *(pOri + x_before))
                                    nMax = (int)(*(pOri + x_before));
                            }
                        }
                        bFlag = true;
                    }

                    // 불량이라면
                    if (nPixel_count >= nDefectPixel)
                    {
                        Ori_mean_threshold = ((double)nSum / (double)nPixel_count) * 1.3; // 30% Threshold 값
                        for (int x_before = x - nDefectPixel; x_before < x; x_before++)
                        {
                            // 1차 필터 (Mean, Max)
                            // if (nSum / nPixel_count < dMeanGV && nMax < nMaxGV) // 해당 부분 Data화 하는거 수정 필요(파라메타 수정을 용이하게 하기 위해서)

                            if (nMax < (int)Ori_mean_threshold) //해당 불량으로 추정되는 위치 원본 데이터에서의 가로줄 평균보다 30%이상 밝으면 불량아님.
                            {
                                *(pThresh + x_before) = 255;
                            }

                            // 보여주기용 이미지 저장(필터 안거친 이미지)
                            if (bImageSvae)
                            {
                                *(pSave + x_before) = 255;
                            }
                        }
                    }

                    if (bFlag) // 이론상 x방향에 1Pixel만 밝게 올라오니까 한번 찾으면 다른 영역은 찾을 필요 없음 (조금이라도 연산 줄임)
                        break;
                }
                pQuant += Stride_Quant;
                pEdge += Stride_Edge;
                pThresh += Stride_Thresh;
                pSave += Stride_Save;
                pOri += Stride_Ori;
            }

            mem_Quant.Dispose();
            mem_Edge.Dispose();
            mem_Thresh.Dispose();
            mem_Save.Dispose();
            mem_Ori.Dispose();

            return OutputThreshold;
        }

        /// <summary>
        /// 불량 Bolb 정보 얻기
        /// 2차 필터 포함 (두께)
        /// </summary>
        /// <param name = "Threshold_Image" ></ param >
        /// < param name="Threshold_Info"></param>
        /// <param name = "nDefectPixel" ></ param >
        /// < param name="nThickness"></param>
        /// <param name = "img_num" ></ param >
        /// < param name="roi_height"></param>
        /// <param name = "bImageSave" ></ param >
        /// < returns ></ returns >
        private string DefectBlob(ref CogImage8Grey Threshold_Image, ref CogImage8Grey Threshold_Info, int nDefectPixel, int nThickness, int img_num, int roi_height, bool bImageSave)
        {
            string strInfo = "";

            CogBlobTool BlobTool = new CogBlobTool();
            BlobTool.InputImage = Threshold_Image;
            BlobTool.RunParams.SegmentationParams.Mode = CogBlobSegmentationModeConstants.HardDynamicThreshold;
            BlobTool.RunParams.SegmentationParams.Polarity = CogBlobSegmentationPolarityConstants.LightBlobs;
            BlobTool.RunParams.ConnectivityMode = CogBlobConnectivityModeConstants.GreyScale;
            BlobTool.RunParams.ConnectivityCleanup = CogBlobConnectivityCleanupConstants.Fill;
            BlobTool.RunParams.ConnectivityMinPixels = nDefectPixel * nThickness;

            BlobTool.Run();


            if (BlobTool.Results != null)
            {
                Threshold_Image = BlobTool.Results.CreateBlobImage().Copy();
                Threshold_Info = Threshold_Image.Copy();

                int nPositionX = 0;
                int nPositionY = 0;
                int nDefectThick = 0;

                int nNum = BlobTool.Results.GetBlobs().Count;

                for (int i = 0; i < nNum; i++)
                {
                    int nID = BlobTool.Results.GetBlobs()[i].ID;

                    nDefectThick = (int)(BlobTool.Results.GetBlobs()[i].Area / 35);   // Blob 높이가 안보여서 계산.....

                    nPositionX = (int)BlobTool.Results.GetBlobs()[i].CenterOfMassX /*+ img_num * roi_height*/;
                    nPositionY = (int)BlobTool.Results.GetBlobs()[i].CenterOfMassY /*+ img_num * roi_height*/;

                    double dY = nPositionY + img_num * roi_height;


                    strInfo += "\n" + "Thickness : " + nDefectThick + ", CenterX : " + nPositionX.ToString("F0") + ", CenterY : " + dY.ToString("F0");


                    if (bImageSave)
                    {
                        Bitmap Btm = Threshold_Info.ToBitmap();

                        string str = /*"No." + nID + */"Thickness : " + nDefectThick + "\nCenterX : " + nPositionX.ToString("F0") + ", CenterY : " + dY.ToString("F0");

                        float nPointSize = 22f;
                        Threshold_Info = new CogImage8Grey(WriteTextToBitmap(Btm, new Font("돋움체", nPointSize, FontStyle.Bold), Brushes.White, new Point((int)nPositionX - 500, (int)(nPositionY - nPointSize / 2 - nPointSize)), str));
                    }
                }
            }

            return strInfo;
        }

        /// <summary>
        /// 분할 이미지 합치는 작업
        /// </summary>
        /// <param name = "OutputImage" ></ param >
        /// < param name="InputImage"></param>
        /// <param name = "img_num" ></ param >
        /// < param name="roi_height"></param>
        unsafe private void MakeDstImage(ref CogImage8Grey OutputImage, CogImage8Grey InputImage, int img_num, int roi_height, int nStart_Y)
        {
            int nWidth = InputImage.Width;
            int nHeight = InputImage.Height;

            ICogImage8PixelMemory mem_Output = OutputImage.Get8GreyPixelMemory(CogImageDataModeConstants.Read, 0, nStart_Y + roi_height * img_num, nWidth, nHeight);           // 결과 이미지
            ICogImage8PixelMemory mem_Input = InputImage.Get8GreyPixelMemory(CogImageDataModeConstants.Read, 0, 0, nWidth, nHeight);                                // 분할 이미지


            int Stride_Output = mem_Output.Stride;
            int Stride_Input = mem_Input.Stride;



            Byte* pOutput = (Byte*)(mem_Output.Scan0);
            Byte* pInput = (Byte*)(mem_Input.Scan0);


            for (int y = 0; y < nHeight; y++)
            {
                for (int x = 0; x < nWidth; x++)
                {
                    *(pOutput + x) = *(pInput + x);
                }
                pOutput += Stride_Output;
                pInput += Stride_Input;
            }

            mem_Output.Dispose();
            mem_Input.Dispose();
        }

        /// <summary>
        /// 이미지에 Defect Info 작성
        /// </summary>
        /// <param name = "sourceBitmap" ></ param >
        /// < param name="font"></param>
        /// <param name = "brush" ></ param >
        /// < param name="location"></param>
        /// <param name = "text" ></ param >
        /// < returns ></ returns >
        public Bitmap WriteTextToBitmap(Bitmap sourceBitmap, Font font, Brush brush, Point location, string text)
        {
            Graphics bitmapGraphics = Graphics.FromImage(sourceBitmap);

            bitmapGraphics.DrawString(text, font, brush, location);

            bitmapGraphics.Dispose();

            return sourceBitmap;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        ///// 시작위치 찾는 부분

        /// <summary>
        /// 이미지 양자화
        /// </summary>
        /// <param name = "InputImage" ></ param >
        /// < returns ></ returns >
        private CogImage8Grey Quantization_st(CogImage8Grey InputImage)
        {
            CogIPOneImageQuantize Quantize = new CogIPOneImageQuantize();
            Quantize.Levels = CogIPOneImageQuantizeLevelConstants.s4;
            CogIPOneImageGreyMorphologyNxM Morpho = new CogIPOneImageGreyMorphologyNxM();
            Morpho.KernelWidth = 5;
            Morpho.KernelHeight = 5;
            Morpho.Operation = CogIPOneImageMorphologyOperationConstants.Erode;

            CogIPOneImageTool IPO = new CogIPOneImageTool();
            IPO.Operators.Add(Quantize);
            IPO.Operators.Add(Morpho);

            IPO.InputImage = InputImage;
            IPO.Run();

            return (CogImage8Grey)IPO.OutputImage;
        }


        /// <summary>
        /// 첫 이미지 시작점 찾기
        /// Unsafe 함수 -> 최적화가 안됨
        /// </summary>
        /// <param name = "InputImage" ></ param >

        /// < returns ></ returns >
        unsafe private void Find_Start_Edge(CogImage8Grey InputImage, int nRoi_Height, ref int Start_Y)
        {
            int nWidth = InputImage.Width;
            int nHeight = InputImage.Height;

            CogImage8Grey OutputImage = new CogImage8Grey(nWidth, nHeight);
            CogImage8Grey TempImage = InputImage.Copy(CogImageCopyModeConstants.CopyPixels);

            ICogImage8PixelMemory mem_Input = InputImage.Get8GreyPixelMemory(CogImageDataModeConstants.Read, 0, 0, InputImage.Width, InputImage.Height);
            ICogImage8PixelMemory mem_Output = OutputImage.Get8GreyPixelMemory(CogImageDataModeConstants.Write, 0, 0, OutputImage.Width, OutputImage.Height);


            int srcStride = mem_Input.Stride;
            int dstStride = mem_Output.Stride;


            Byte* pSrc = (Byte*)(mem_Input.Scan0);
            Byte* pDst = (Byte*)(mem_Output.Scan0);


            for (int y = 0; y < nHeight; y++)
            {
                for (int x = nWidth; x >= 0; x--)
                {
                    if (*(pSrc + x) > 0)
                    {
                        ICogImage8PixelMemory mem_Temp = TempImage.Get8GreyPixelMemory(CogImageDataModeConstants.Read, 0, y, InputImage.Width, nRoi_Height);
                        int tempStride = mem_Temp.Stride;
                        Byte* pTemp = (Byte*)(mem_Temp.Scan0);
                        int nCount = 0;

                        for (int y2 = 0; y2 < nRoi_Height; y2++)
                        {
                            for (int x2 = nWidth; x2 >= 0; x2--)
                            {
                                if (*(pTemp + x2) > 0)
                                {
                                    nCount++;
                                }
                            }
                            pTemp += tempStride;
                        }

                        double dPer = (double)nCount / (double)(nRoi_Height);

                        mem_Temp.Dispose();

                        if (dPer > 0.98)
                        {
                            Start_Y = y;

                            return;
                        }
                    }

                }
                pSrc += srcStride;
                pDst += dstStride;
            }

            mem_Input.Dispose();
            mem_Output.Dispose();

            return;
        }

        // <summary>
        // 마지막 이미지 끝점 찾기
        // Unsafe 함수 -> 최적화가 안됨
        // </summary>
        // <param name = "InputImage" ></ param >
        // < returns ></ returns >
        unsafe private void Find_End_Edge(CogImage8Grey InputImage, int nRoi_Height, ref int End_Y)
        {

            CogIPOneImageFlipRotate cogFilp = new CogIPOneImageFlipRotate();
            cogFilp.OperationInPixelSpace = CogIPOneImageFlipRotateOperationConstants.FlipAndRotate180Deg;

            CogIPOneImageTool IPO = new CogIPOneImageTool();
            IPO.Operators.Add(cogFilp);


            IPO.InputImage = InputImage;
            IPO.Run();

            CogImage8Grey InputImage_Rotate = (CogImage8Grey)IPO.OutputImage;

            int nWidth = InputImage_Rotate.Width;
            int nHeight = InputImage_Rotate.Height;

            CogImage8Grey OutputImage = new CogImage8Grey(nWidth, nHeight);
            CogImage8Grey TempImage = InputImage_Rotate.Copy(CogImageCopyModeConstants.CopyPixels);

            ICogImage8PixelMemory mem_Input = InputImage_Rotate.Get8GreyPixelMemory(CogImageDataModeConstants.Read, 0, 0, InputImage_Rotate.Width, InputImage_Rotate.Height);
            ICogImage8PixelMemory mem_Output = OutputImage.Get8GreyPixelMemory(CogImageDataModeConstants.Write, 0, 0, OutputImage.Width, OutputImage.Height);


            int srcStride = mem_Input.Stride;
            int dstStride = mem_Output.Stride;


            Byte* pSrc = (Byte*)(mem_Input.Scan0);
            Byte* pDst = (Byte*)(mem_Output.Scan0);


            for (int y = 0; y < nHeight; y++)
            {
                for (int x = nWidth; x >= 0; x--)
                {
                    if (*(pSrc + x) > 0)
                    {
                        ICogImage8PixelMemory mem_Temp = TempImage.Get8GreyPixelMemory(CogImageDataModeConstants.Read, 0, y, InputImage_Rotate.Width, nRoi_Height);
                        int tempStride = mem_Temp.Stride;
                        Byte* pTemp = (Byte*)(mem_Temp.Scan0);
                        int nCount = 0;

                        for (int y2 = 0; y2 < nRoi_Height; y2++)
                        {
                            for (int x2 = nWidth; x2 >= 0; x2--)
                            {
                                if (*(pTemp + x2) > 0)
                                {
                                    nCount++;
                                }
                            }
                            pTemp += tempStride;
                        }

                        double dPer = (double)nCount / (double)(nRoi_Height);

                        mem_Temp.Dispose();

                        if (dPer > 0.98)
                        {
                            End_Y = InputImage_Rotate.Height - y;

                            return;
                        }
                    }

                }
                pSrc += srcStride;
                pDst += dstStride;
            }

            mem_Input.Dispose();
            mem_Output.Dispose();

            return;
        }
        #endregion
    }
}

