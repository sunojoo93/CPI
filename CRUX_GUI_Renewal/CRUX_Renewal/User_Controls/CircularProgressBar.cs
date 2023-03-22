using CRUX_Renewal.Class;
using System;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
namespace CRUX_Renewal.User_Controls
{
    // 출처 :: https://stackoverflow.com/questions/4871263/how-to-create-a-circular-style-progressbar
    public class CPB : Control, ITimerManager
    {
        #region Enums

        public enum _ProgressShape
        {
            Round,
            Infinity,
            Flat
        }

        public enum _TextMode
        {
            None,
            Value,
            Percentage,
            Custom
        }
        public enum _LineStyle
        {
            Solid,
            Dash,
            DashDot,
            DashDotDot,
            Dot
        }

        #endregion

        #region Private Variables

        private double _Value;
        private long _Maximum = 100;
        private int _LineWitdh = 1;
        private float _BarWidth = 14f;
        private bool _ProgressAction = false;

        private Color _ProgressColor1 = Color.Lime;
        private Color _ProgressColor2 = Color.Aqua;
        private Color _LineColor = Color.Black;
        private LinearGradientMode _GradientMode = LinearGradientMode.ForwardDiagonal;
        private _ProgressShape ProgressShapeVal;
        private _TextMode ProgressTextMode;
        private _LineStyle _LineStyleVal = _LineStyle.Solid;
        bool Direction;
        CancellationTokenSource TokenSource = new CancellationTokenSource();
        private int _Delay = 25;
        private int _Detail = 100;
        private bool _TextView = true;
        private bool _Line = true;

        int CurIndex = 1;

        #endregion

        #region Contructor

        public CPB()
        {
            SetStyle(ControlStyles.SupportsTransparentBackColor, true);
            SetStyle(ControlStyles.Opaque, true);
            this.BackColor = SystemColors.Control;
            this.ForeColor = Color.DimGray;

            this.Size = new Size(130, 130);
            this.Font = new Font("Calibri", 72);
            this.MinimumSize = new Size(100, 100);
            this.DoubleBuffered = true;

            this.LineWidth = 1;
            this.LineColor = Color.DimGray;

            Value = 0;
            ProgressShape = _ProgressShape.Round;
            TextMode = _TextMode.Percentage;
        }
        

        #endregion

        #region Public Custom Properties

        /// <summary>Determina el Valor del Progreso</summary>
        [Description("Valor Entero que determina la posision de la Barra de Progreso."), Category("Behavior")]
        public double Value
        {
            get { return _Value; }
            set
            {
                if (value > _Maximum)
                    value = _Maximum;
                _Value = value;
                Invalidate();
            }
        }

        [Description("Obtiene o Establece el Valor Maximo de la barra de Progreso."), Category("Behavior")]
        public long Maximum
        {
            get { return _Maximum; }
            set
            {
                if (value < 1)
                    value = 1;
                _Maximum = value;
                Invalidate();
            }
        }

        [Description("Initial color of progressive bar"), Category("Appearance")]
        public Color BarColor1
        {
            get { return _ProgressColor1; }
            set
            {
                _ProgressColor1 = value;
                Invalidate();
            }
        }
        [Description("Realtime Bar Color Change"), Category("Appearance")]
        public bool ProgressAction
        {
            get { return _ProgressAction; }
            set
            {
                _ProgressAction = value;
                Invalidate();
            }
        }

        [Description("Final color of progressive bar"), Category("Appearance")]
        public Color BarColor2
        {
            get { return _ProgressColor2; }
            set
            {
                _ProgressColor2 = value;
                Invalidate();
            }
        }

        [Description("Ancho de la Barra de Progreso"), Category("Appearance")]
        public float BarWidth
        {
            get { return _BarWidth; }
            set
            {
                _BarWidth = value;
                Invalidate();
            }
        }

        [Description("Modo del Gradiente de Color"), Category("Appearance")]
        public LinearGradientMode GradientMode
        {
            get { return _GradientMode; }
            set
            {
                _GradientMode = value;
                Invalidate();
            }
        }
        [Description("DrawLine"), Category("Appearance")]
        public bool Line
        {
            get { return _Line; }
            set
            {
                _Line = value;
                Invalidate();
            }
        }

        [Description("Color de la Linea Intermedia"), Category("Appearance")]
        public Color LineColor
        {
            get { return _LineColor; }
            set
            {
                _LineColor = value;
                Invalidate();
            }
        }

        [Description("Ancho de la Linea Intermedia"), Category("Appearance")]
        public int LineWidth
        {
            get { return _LineWitdh; }
            set
            {
                _LineWitdh = value;
                Invalidate();
            }
        }

        [Description("Obtiene o Establece la Forma de los terminales de la barra de progreso."), Category("Appearance")]
        public _ProgressShape ProgressShape
        {
            get { return ProgressShapeVal; }
            set
            {
                ProgressShapeVal = value;
                Invalidate();
            }
        }
        [Description("Line Style."), Category("Appearance")]
        public _LineStyle LineStyle
        {
            get { return _LineStyleVal; }
            set
            {
                _LineStyleVal = value;
                Invalidate();
            }
        }

        [Description("Obtiene o Establece el Modo como se muestra el Texto dentro de la barra de Progreso."), Category("Behavior")]
        public _TextMode TextMode
        {
            get { return ProgressTextMode; }
            set
            {
                ProgressTextMode = value;
                Invalidate();
            }
        }
        [Description("Text View."), Category("Behavior")]
        public bool TextView
        {
            get { return _TextView; }
            set
            {
                _TextView = value;
                Invalidate();
            }
        }

        [Description("Obtiene el Texto que se muestra dentro del Control"), Category("Behavior")]
        public override string Text { get; set; }

        #region Progress shape is Only Infinity
        [Description("Highlight Delay"), Category("Behavior")]
        public int Delay
        {
            get { return _Delay; }
            set
            {
                _Delay = value;
                Invalidate();
            }
        }

        [Description("Degree of detail"), Category("Behavior")]
        public int DegreeOfDetail
        {
            get { return _Detail; }
            set
            {
                _Detail = value;
                Invalidate();
            }
        }

        #endregion
        #endregion

        #region EventArgs

        protected override void OnResize(EventArgs e)
        {
            base.OnResize(e);
            SetStandardSize();
        }

        protected override void OnSizeChanged(EventArgs e)
        {
            base.OnSizeChanged(e);
            SetStandardSize();
        }

        protected override void OnPaintBackground(PaintEventArgs p)
        {
            base.OnPaintBackground(p);
        }
        protected override void OnVisibleChanged(EventArgs e)
        {
            base.OnVisibleChanged(e);

            if (Visible && !Disposing)
                Task.Factory.StartNew(() => TimerStart());
        }
        protected override void OnHandleDestroyed(EventArgs e)
        {
            TokenSource.Cancel();
        }
        #endregion

        #region Methods
        /// <summary>
        /// ColorTimer 관리
        /// </summary>
        public void TimerStart()
        {
            while (true)
            {
                if (TokenSource.Token.IsCancellationRequested == true)
                    break;
                if (_ProgressShape.Infinity == this.ProgressShapeVal)
                {
                    //Increment(1);         
                    Thread.Sleep(_Delay);
                    //if (Value >= 100)
                    //    Decrement(99);
                    Invalidate();
                }
                else
                {
                    if (_ProgressAction)
                    {
                        if (BarColor1.A >= 200)
                            Direction = true;
                        else if (BarColor1.A <= 50)
                            Direction = false;
                        int Bar1 = BarColor1.A;
                        int Bar2 = BarColor2.A;
                        if (Direction)
                        {
                            Bar1 -= 3;
                            Bar2 -= 3;
                            BarColor1 = Color.FromArgb(Bar1, BarColor1.R, BarColor1.G, BarColor1.B);
                            BarColor2 = Color.FromArgb(Bar2, BarColor2.R, BarColor2.G, BarColor2.B);
                        }
                        else
                        {
                            Bar1 += 3;
                            Bar2 += 3;
                            BarColor1 = Color.FromArgb(Bar1, BarColor1.R, BarColor1.G, BarColor1.B);
                            BarColor2 = Color.FromArgb(Bar2, BarColor2.R, BarColor2.G, BarColor2.B);
                        }
                    }
                    Thread.Sleep(10);
                }
            }

        }
        /// <summary>
        /// ColorChange 효과를 사용한다면 폼 종료시 반드시 실행
        /// </summary>
        public void TimerStop ()
        {
            TokenSource.Cancel();
        }
        private void SetStandardSize()
        {
            int _Size = Math.Max(Width, Height);
            Size = new Size(_Size, _Size);
        }

        public void Increment(double Val)
        {
            Value += Val;
            // 실시간 갱신으로 변경
            //Refresh();
        }

        public void Decrement(int Val)
        {
            Value -= Val;
            // 실시간 갱신으로 변경
            //Refresh();
        }
        #endregion

        #region Events

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
  
            using (Bitmap bitmap = new Bitmap(this.Width, this.Height))
            {
                using (Graphics graphics = Graphics.FromImage(bitmap))
                {
                    graphics.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBilinear;
                    graphics.CompositingQuality = System.Drawing.Drawing2D.CompositingQuality.HighQuality;
                    graphics.PixelOffsetMode = System.Drawing.Drawing2D.PixelOffsetMode.HighQuality;
                    graphics.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;

                    //graphics.Clear(Color.White); //<-- this.BackColor, SystemColors.Control, Color.Transparent

                    PaintTransparentBackground(this, e);

                    //Dibuja el circulo blanco interior:
                    using (Brush mBackColor = new SolidBrush(this.BackColor))
                    {
                        graphics.FillEllipse(mBackColor,
                                18, 18,
                                (this.Width - 0x30) + 12,
                                (this.Height - 0x30) + 12);
                    }
                    // Dibuja la delgada Linea gris del medio:
                    if (_Line == true)
                    {
                        using (Pen pen2 = new Pen(LineColor, this.LineWidth) { DashStyle = (DashStyle)_LineStyleVal })
                        {
                            graphics.DrawEllipse(pen2,
                                18, 18,
                              (this.Width - 0x30) + 12,
                              (this.Height - 0x30) + 12);
                        }
                    }

                    //Dibuja la Barra de Progreso
                    using (LinearGradientBrush brush = new LinearGradientBrush(this.ClientRectangle,
                        this._ProgressColor1, this._ProgressColor2, this.GradientMode))
                    {
                        using (Pen pen = new Pen(brush, this.BarWidth))
                        {
                            switch (this.ProgressShapeVal)
                            {
                                case _ProgressShape.Round:
                                    pen.StartCap = LineCap.Round;
                                    pen.EndCap = LineCap.Round;
                                    break;
                                case _ProgressShape.Infinity:
                                    pen.StartCap = LineCap.Round;
                                    pen.EndCap = LineCap.Round;
                                    break;
                                case _ProgressShape.Flat :
                                    pen.StartCap = LineCap.Flat;
                                    pen.EndCap = LineCap.Flat;
                                    break;
                            }
                            if (this.ProgressShapeVal == _ProgressShape.Infinity)
                            {
                                for (float i = 1; i <= _Detail; ++i)
                                {
                                    if (CurIndex == i)
                                    {
                                            graphics.DrawArc(pen,
                                            (float)0x12, (float)0x12,
                                            ((float)this.Width - (float)0x23) - (float)2,
                                            ((float)this.Height - (float)0x23) - (float)2,
                                            (((float)360 / (float)_Detail) * ((float)i - (float)1)),
                                            ((float)360 / (float)_Detail) * ((float)i - (float)1) - ((float)360 / (float)_Detail) * ((float)i));
                                        graphics.DrawArc(pen,
                                            (float)0x12, (float)0x12,
                                            ((float)this.Width - (float)0x23) - (float)2,
                                            ((float)this.Height - (float)0x23) - (float)2,
                                            (((float)360 / (float)_Detail) * ((float)i - (float)1))+120,
                                            ((float)360 / (float)_Detail) * ((float)i - (float)1) - ((float)360 / (float)_Detail) * ((float)i));
                                        graphics.DrawArc(pen,
                                            (float)0x12, (float)0x12,
                                            ((float)this.Width - (float)0x23) - (float)2,
                                            ((float)this.Height - (float)0x23) - (float)2,
                                            (((float)360 / (float)_Detail) * ((float)i - (float)1))+240,
                                            ((float)360 / (float)_Detail) * ((float)i - (float)1) - ((float)360 / (float)_Detail) * ((float)i));

                                        //graphics.Flush();
                                    }
                                }
                                CurIndex++;
                                if (CurIndex >= _Detail)
                                    CurIndex = 1;                                  
                            }
                            else
                            {
                                //Aqui se dibuja realmente la Barra de Progreso                       
                                graphics.DrawArc(pen,
                                    0x12, 0x12,
                                    (this.Width - 0x23) - 2,
                                    (this.Height - 0x23) - 2,
                                    -90,
                                    (int)Math.Round((double)((360.0 / ((double)this._Maximum)) * this._Value)));
                            }
                        }
                    }

                    #region Dibuja el Texto de Progreso

                    switch (this.TextMode)
                    {
                        case _TextMode.None:
                            this.Text = string.Empty;
                            break;

                        case _TextMode.Value:
                            this.Text = _Value.ToString();
                            break;

                        case _TextMode.Percentage:
                            this.Text = Convert.ToString(Convert.ToInt32((100 / _Maximum) * _Value));
                            break;

                        default:
                            break;
                    } 

                    if (this.Text != string.Empty && _TextView)
                    {
                        using (Brush FontColor = new SolidBrush(this.ForeColor))
                        {
                            int ShadowOffset = 2;
                            SizeF MS = graphics.MeasureString(this.Text, this.Font);
                            SolidBrush shadowBrush = new SolidBrush(Color.FromArgb(100, this.ForeColor));

                            //Sombra del Texto:
                            // 텍스트 그리기 모드 : 안티앨리어싱 적용
                            graphics.TextRenderingHint = System.Drawing.Text.TextRenderingHint.AntiAlias;
                            graphics.DrawString(this.Text, this.Font, shadowBrush,
                                Convert.ToInt32(Width / 2 - MS.Width / 2) + ShadowOffset,
                                Convert.ToInt32(Height / 2 - MS.Height / 2) + ShadowOffset
                            );

                            //Texto del Control:
                            // 텍스트 그리기 모드 : 안티앨리어싱 적용
                            graphics.TextRenderingHint = System.Drawing.Text.TextRenderingHint.AntiAlias;
                            graphics.DrawString(this.Text, this.Font, FontColor,
                                Convert.ToInt32(Width / 2 - MS.Width / 2),
                                Convert.ToInt32(Height / 2 - MS.Height / 2));
                        }
                    }
                    

                    #endregion

                    //Aqui se Dibuja todo el Control:
                    e.Graphics.DrawImage(bitmap, 0, 0);
                    graphics.Dispose();
                    bitmap.Dispose();
                }
            }
        }

        private static void PaintTransparentBackground(Control c, PaintEventArgs e)
        {
            if (c.Parent == null || !Application.RenderWithVisualStyles)
                return;

            ButtonRenderer.DrawParentBackground(e.Graphics, c.ClientRectangle, c);
        }

        /// <summary>Dibuja un Circulo Relleno de Color con los Bordes perfectos.</summary>
        /// <param name="g">'Canvas' del Objeto donde se va a dibujar</param>
        /// <param name="brush">Color y estilo del relleno</param>
        /// <param name="centerX">Centro del Circulo, en el eje X</param>
        /// <param name="centerY">Centro del Circulo, en el eje Y</param>
        /// <param name="radius">Radio del Circulo</param>
        private void FillCircle(Graphics g, Brush brush, float centerX, float centerY, float radius)
        {
            g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBilinear;
            g.CompositingQuality = System.Drawing.Drawing2D.CompositingQuality.HighQuality;
            g.PixelOffsetMode = System.Drawing.Drawing2D.PixelOffsetMode.HighQuality;
            g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;

            using (System.Drawing.Drawing2D.GraphicsPath gp = new System.Drawing.Drawing2D.GraphicsPath())
            {
                g.FillEllipse(brush, centerX - radius, centerY - radius,
                          radius + radius, radius + radius);
            }
        }

        #endregion

        private void InitializeComponent ()
        {
            this.SuspendLayout();
            // 
            // CPB
            // 
            this.Margin = new System.Windows.Forms.Padding(0);
            this.Name = "CPB";
            this.ResumeLayout(false);
        }
    }
}