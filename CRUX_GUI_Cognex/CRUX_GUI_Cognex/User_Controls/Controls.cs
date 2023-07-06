using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_GUI_Cognex.User_Controls

{
    class Controls
    {
    }

    class ColoredCombo : ComboBox
    {

        protected override void OnPaintBackground(PaintEventArgs e)
        {
            base.OnPaintBackground(e);
            using (var brush = new SolidBrush(BackColor))
            {
                e.Graphics.FillRectangle(brush, ClientRectangle);
                e.Graphics.DrawRectangle(Pens.DarkGray, 0, 0, ClientSize.Width - 1, ClientSize.Height - 1);
            }
        }
    }
    public class NoDoubleClickAutoCheckListview : ListView
    {
        private bool checkFromDoubleClick = false;

        protected override void OnItemCheck(ItemCheckEventArgs ice)
        {
            if (this.checkFromDoubleClick)
            {
                ice.NewValue = ice.CurrentValue;
                this.checkFromDoubleClick = false;
            }
            else
                base.OnItemCheck(ice);
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            // Is this a double-click?
            if ((e.Button == MouseButtons.Left) && (e.Clicks > 1))
            {
                this.checkFromDoubleClick = true;
            }
            base.OnMouseDown(e);
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            this.checkFromDoubleClick = false;
            base.OnKeyDown(e);
        }
    }
    public class CheckButton : Button
    {
        private bool _Checking;
        public event EventHandler CheckingChanged;
        Object frm_Extend;
        Type Form_Type;
        Dictionary<string, bool> States;
        public bool Checking
        {
            get
            {
                return _Checking;
            }
            set
            {
                _Checking = value;
                OnCheckingChanged();
            }
        }

        protected virtual void OnCheckingChanged()
        {
            CheckingChanged(this, EventArgs.Empty);
        }
        protected override bool ShowFocusCues
        {
            get
            {
                return false;
            }
        }

        public CheckButton()
        {
            //CheckingChanged += CheckButton_CheckingChanged;
        }

        public void CreateForm(int x, int y, int width, int height, Type type, Form form)
        {
            frm_Extend = Activator.CreateInstance(type);
            Form_Type = type;
            object[] obj = { x, y, width, height };
            object[] obj_param = { _Checking };
            object[] obj_form = { form };
            frm_Extend.GetType().GetMethod("Initialize").Invoke(frm_Extend, obj);
            frm_Extend.GetType().GetMethod("SetVisible").Invoke(frm_Extend, obj_param);
            frm_Extend.GetType().GetMethod("SetParent").Invoke(frm_Extend, obj_form);
            States = (Dictionary<string, bool>)frm_Extend.GetType().GetMethod("GetButtonState").Invoke(frm_Extend, null);
        }
        public T GetForm<T>()
        {
            return (T)frm_Extend;
        }

        public Type GetFormType()
        {
            return Form_Type;
        }
       
        //private void CheckButton_CheckingChanged(object sender, EventArgs e)
        //{
        //    OnValidated(EventArgs.Empty);
        //    if (!(frm_Extend is null))
        //    {
        //        object[] obj = { _Checking };
        //        object[] obj_ = { Program.frm_Main.Location };
        //        frm_Extend.GetType().GetMethod("SetVisible").Invoke(frm_Extend, obj);   
        //        States = (Dictionary<string, bool>)frm_Extend.GetType().GetMethod("GetButtonState").Invoke(frm_Extend, null);
        //    }
        //}
    }

    public class DoubleBufferdPictureBox : PictureBox
    {
        public DoubleBufferdPictureBox()
        {
            this.SetStyle(ControlStyles.OptimizedDoubleBuffer | ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint, true);
        }
    }

    public class DoubleBufferdListBox : ListBox
    {
        public DoubleBufferdListBox()
        {
            this.SetStyle(ControlStyles.OptimizedDoubleBuffer /*| ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint*/, true);
        }
    }

    public class SmartList<T> : List<T>
    {
        public delegate void CustomListEventHandler<T>(T listItem);
        public event CustomListEventHandler<T> onAdded;
        public event CustomListEventHandler<T> onRemoved;
        public event CustomListEventHandler<T> onInserted;

        public SmartList()
        {

        }

        public void SetListAddEvent(CustomListEventHandler<T> obj)
        {
            if(onAdded == null)
                onAdded += obj;
        }
        public void SetListRemoveEvent(CustomListEventHandler<T> obj)
        {
            if (onRemoved == null)
                onRemoved += obj;
        }
        public void SetListInsertEvent(CustomListEventHandler<T> obj)
        {
            if (onInserted == null)
                onInserted += obj;
        }

        public new void Add(T newItem)
        {
            base.Add(newItem);
            onAdded?.Invoke(newItem);
        }

        public new void Remove(T tobeRemoved)
        {
            base.Remove(tobeRemoved);

            onRemoved?.Invoke(tobeRemoved);
        }
        public new void Insert(int num,T tobeInsert)
        {            
            base.Insert(num, tobeInsert);
            onInserted?.Invoke(tobeInsert);
        }
        //public SmartList<T> DeepCopy()
        //{

        //}
    }

    public class DoubleBufferdDataGridView : DataGridView
    {
        public DoubleBufferdDataGridView()
        {
            this.SetStyle(ControlStyles.OptimizedDoubleBuffer | ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint, true);
        }
    }
    public class CenterAlignedGroupBox : GroupBox
    {
        private string _Text = "";
        public CenterAlignedGroupBox()
        {
            //set the base text to empty 
            //base class will draw empty string
            //in such way we see only text what we draw
            base.Text = "";
            this.SetStyle(ControlStyles.OptimizedDoubleBuffer | ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint, true);
        }
        //create a new property a
        [Browsable(true)]
        [Category("Appearance")]
        [DefaultValue("GroupBoxText")]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public new string Text
        {
            get
            {

                return _Text;
            }
            set
            {

                _Text = value;
                this.Invalidate();
            }
        }
        protected override void OnPaint(PaintEventArgs e)
        {

            //first let the base class to draw the control 
            base.OnPaint(e);
            //create a brush with fore color
            SolidBrush colorBrush = new SolidBrush(this.ForeColor);
            //create a brush with back color
            var backColor = new SolidBrush(this.BackColor);
            //measure the text size
            var size = TextRenderer.MeasureText(this.Text, this.Font);
            // evaluate the postiong of text from left;
            int left = (this.Width - size.Width) / 2;
            //draw a fill rectangle in order to remove the border
            e.Graphics.FillRectangle(backColor, new Rectangle(left, 0, size.Width, size.Height));
            //draw the text Now
            e.Graphics.DrawString(this.Text, this.Font, colorBrush, new PointF(left, 0));

        }
    }

    public class DoubleBufferdTableLayoutGrid : TableLayoutPanel
    {
        public DoubleBufferdTableLayoutGrid() : base()
        {
            this.SetStyle(ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint | ControlStyles.OptimizedDoubleBuffer, true);
        }
    }
    public class DoubleBufferdListView: ListView
    {
        public DoubleBufferdListView() : base()
        {
            this.SetStyle(ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint | ControlStyles.OptimizedDoubleBuffer, true);
        }
    }
    public class GradientPanel : Panel

    {
        [Browsable(true)]
        public Color GraColorA { get; set; }

        [Browsable(true)]
        public Color GraColorB { get; set; }

        [Browsable(true)]        
        public LinearGradientMode GradientFillStyle { get; set; }

        private Brush gradientBrush;
        public GradientPanel()
        {
            handlerGradientChanged = new EventHandler(GradientChanged);
            ResizeRedraw = true;
        }
        private EventHandler handlerGradientChanged;

        protected override void OnPaintBackground(System.Windows.Forms.PaintEventArgs e)
        {
            gradientBrush = new LinearGradientBrush(ClientRectangle, GraColorA, GraColorB, GradientFillStyle);
            e.Graphics.FillRectangle(gradientBrush, ClientRectangle);
        }
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (gradientBrush != null) gradientBrush.Dispose();
            }
            base.Dispose(disposing);
        }

        protected override void OnScroll(ScrollEventArgs se)
        {
            Invalidate();
        }
        private void GradientChanged(object sender, EventArgs e)
        {
            if (gradientBrush != null) 
                gradientBrush.Dispose();
            gradientBrush = null;
            Invalidate();
        }
    }
    [ToolboxItem(true)]
    public class ShadowLabel : Label
    {
        private bool _drawGradient = true;
        private Color _startColor = Color.White;
        private Color _endColor = Color.LightSkyBlue;
        private float _angle = 0;

        private bool _drawShadow = true;
        private float _yOffset = 1;
        private float _xOffset = 1;
        private Color _shadowColor = Color.Black;

        public ShadowLabel()
        {
            InitializeComponent();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            //base.OnPaint (e);

            Rectangle rectText = this.ClientRectangle;
            Rectangle rectShadow = new Rectangle(rectText.Left + (int)_xOffset, rectText.Top + (int)_yOffset, rectText.Width, rectText.Height);

            e.Graphics.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;

            if (_drawGradient == true)
            {
                LinearGradientBrush brush = new LinearGradientBrush(new Rectangle(0, 0, this.Width, this.Height), _startColor, _endColor, _angle, true);
                e.Graphics.FillRectangle(brush, 0, 0, this.Width, this.Height);
            }

            //정렬값
            TextFormatFlags flags = TextFormatFlags.HorizontalCenter | TextFormatFlags.VerticalCenter; ;
            if (this.TextAlign == ContentAlignment.BottomCenter)
            {
                flags = TextFormatFlags.Bottom | TextFormatFlags.HorizontalCenter;
            }
            else if (this.TextAlign == ContentAlignment.BottomLeft)
            {
                flags = TextFormatFlags.Bottom | TextFormatFlags.Left;
            }
            else if (this.TextAlign == ContentAlignment.BottomRight)
            {
                flags = TextFormatFlags.Bottom | TextFormatFlags.Right;
            }
            else if (this.TextAlign == ContentAlignment.MiddleCenter)
            {
                flags = TextFormatFlags.HorizontalCenter | TextFormatFlags.VerticalCenter;
            }
            else if (this.TextAlign == ContentAlignment.MiddleLeft)
            {
                flags = TextFormatFlags.VerticalCenter | TextFormatFlags.Left;
            }
            else if (this.TextAlign == ContentAlignment.MiddleRight)
            {
                flags = TextFormatFlags.VerticalCenter | TextFormatFlags.Right;
            }
            else if (this.TextAlign == ContentAlignment.TopCenter)
            {
                flags = TextFormatFlags.Top | TextFormatFlags.HorizontalCenter;
            }
            else if (this.TextAlign == ContentAlignment.TopLeft)
            {
                flags = TextFormatFlags.Top | TextFormatFlags.Left;
            }
            else if (this.TextAlign == ContentAlignment.TopRight)
            {
                flags = TextFormatFlags.Top | TextFormatFlags.Right;
            }

            //그림자 그리기
            if (_drawShadow == true)
            {
                TextRenderer.DrawText(e.Graphics, Text, this.Font, rectShadow, this._shadowColor, flags);
            }

            //텍스트 그리기
            TextRenderer.DrawText(e.Graphics, Text, this.Font, rectText, this.ForeColor, flags);
        }

        [Category("Gradient"), Description("그라디언트 사용여부"), DefaultValue(true)]
        public bool DrawGradient
        {
            get { return this._drawGradient; }
            set { this._drawGradient = value; this.Invalidate(); }
        }

        [Category("Gradient"), Description("그라디언트 시작 색상"), DefaultValue(typeof(Color), "Color.White")]
        public Color StartColor
        {
            get { return this._startColor; }
            set { this._startColor = value; this.Invalidate(); }
        }

        [Category("Gradient"), Description("그라디언트 끝 색상"), DefaultValue(typeof(Color), "Color.LightSkyBlue")]
        public Color EndColor
        {
            get { return this._endColor; }
            set { this._endColor = value; this.Invalidate(); }
        }

        [Category("Gradient"), Description("그라디언트 방향"), DefaultValue(0)]
        public float Angle
        {
            get { return this._angle; }
            set { this._angle = value; this.Invalidate(); }
        }

        [Category("Drop Shadow"), Description("그림자 사용여부"), DefaultValue(true)]
        public bool DrawShadow
        {
            get { return this._drawShadow; }
            set { this._drawShadow = value; this.Invalidate(); }
        }

        [Category("Drop Shadow"), Description("그림자 X 너비"), DefaultValue(1)]
        public float XOffset
        {
            get { return this._xOffset; }
            set { this._xOffset = value; this.Invalidate(); }
        }

        [Category("Drop Shadow"), Description("그림자 Y 너비"), DefaultValue(1)]
        public float YOffset
        {
            get { return this._yOffset; }
            set { this._yOffset = value; this.Invalidate(); }
        }

        [Category("Drop Shadow"), Description("그림자 색상"), DefaultValue(typeof(System.Drawing.Color), "Color.Black")]
        public Color ShadowColor
        {
            get { return this._shadowColor; }
            set { this._shadowColor = value; this.Invalidate(); }
        }

        private void InitializeComponent()
        {
            this.ForeColor = Color.LightSkyBlue;
        }
    }
    public class DataGridViewProgressColumn : DataGridViewImageColumn
    {
        public static Color _ProgressBarColor;

        public DataGridViewProgressColumn()
        {
            CellTemplate = new DataGridViewProgressCell(); ;
        }
        public DataGridViewProgressColumn(bool color_step)
        {
            CellTemplate = new DataGridViewProgressCell(color_step);
        }

        public override DataGridViewCell CellTemplate
        {
            get
            {
                return base.CellTemplate;
            }
            set
            {
                if (value != null &&
                    !value.GetType().IsAssignableFrom(typeof(DataGridViewProgressCell)))
                {
                    throw new InvalidCastException("Must be a DataGridViewProgressCell");
                }
                base.CellTemplate = value;
            }
        }


        [Browsable(true)]
        public Color ProgressBarColor
        {
            get
            {

                if (this.ProgressBarCellTemplate == null)
                {
                    throw new InvalidOperationException("Operation cannot be completed because this DataGridViewColumn does not have a CellTemplate.");
                }
                return this.ProgressBarCellTemplate.ProgressBarColor;

            }
            set
            {

                if (this.ProgressBarCellTemplate == null)
                {
                    throw new InvalidOperationException("Operation cannot be completed because this DataGridViewColumn does not have a CellTemplate.");
                }
                this.ProgressBarCellTemplate.ProgressBarColor = value;
                if (this.DataGridView != null)
                {
                    DataGridViewRowCollection dataGridViewRows = this.DataGridView.Rows;
                    int rowCount = dataGridViewRows.Count;
                    for (int rowIndex = 0; rowIndex < rowCount; rowIndex++)
                    {
                        DataGridViewRow dataGridViewRow = dataGridViewRows.SharedRow(rowIndex);
                        DataGridViewProgressCell dataGridViewCell = dataGridViewRow.Cells[this.Index] as DataGridViewProgressCell;
                        if (dataGridViewCell != null)
                        {
                            dataGridViewCell.SetProgressBarColor(rowIndex, value);
                        }
                    }
                    this.DataGridView.InvalidateColumn(this.Index);
                    // TODO: This column and/or grid rows may need to be autosized depending on their
                    //       autosize settings. Call the autosizing methods to autosize the column, rows, 
                    //       column headers / row headers as needed.
                }
            }
        }


        private DataGridViewProgressCell ProgressBarCellTemplate
        {
            get
            {
                return (DataGridViewProgressCell)this.CellTemplate;
            }
        }
    }
    class DataGridViewProgressCell : DataGridViewImageCell
    {
        // Used to make custom cell consistent with a DataGridViewImageCell
        static Image emptyImage;
        // Used to remember color of the progress bar
        static Color _ProgressBarColor;
        static bool _ColorStep;

        public Color ProgressBarColor
        {
            get { return _ProgressBarColor; }
            set { _ProgressBarColor = value; }
        }
        public bool ColorStep
        {
            get { return _ColorStep; }
            set { _ColorStep = value; }
        }

        static DataGridViewProgressCell()
        {

            emptyImage = new Bitmap(1, 1, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
        }
        public DataGridViewProgressCell()
        {
            this.ValueType = typeof(int);
        }
        public DataGridViewProgressCell(bool color_step)
        {
            this.ValueType = typeof(int);
            ColorStep = color_step;
        }
        // Method required to make the Progress Cell consistent with the default Image Cell.
        // The default Image Cell assumes an Image as a value, although the value of the Progress Cell is an int.
        protected override object GetFormattedValue(object value,
            int rowIndex, ref DataGridViewCellStyle cellStyle,
            TypeConverter valueTypeConverter,
            TypeConverter formattedValueTypeConverter,
            DataGridViewDataErrorContexts context)
        {
            return emptyImage;
        }

        protected override void Paint(System.Drawing.Graphics g,
            System.Drawing.Rectangle clipBounds,
            System.Drawing.Rectangle cellBounds,
            int rowIndex,
            DataGridViewElementStates cellState,
            object value, object formattedValue,
            string errorText,
            DataGridViewCellStyle cellStyle,
            DataGridViewAdvancedBorderStyle advancedBorderStyle,
            DataGridViewPaintParts paintParts)
        {
            if (Convert.ToInt16(value) == 0 || value == null)
            {
                value = 0;
            }

            int progressVal = Convert.ToInt32(value);

            float percentage = ((float)progressVal / 100.0f); // Need to convert to float before division; otherwise C# returns int which is 0 for anything but 100%.
            Brush backColorBrush = new SolidBrush(cellStyle.BackColor);
            Brush foreColorBrush = new SolidBrush(cellStyle.ForeColor);

            // Draws the cell grid
            base.Paint(g, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, (paintParts & ~DataGridViewPaintParts.ContentForeground));

            float posX = cellBounds.X;
            float posY = cellBounds.Y;

            float textWidth = TextRenderer.MeasureText(progressVal.ToString() + "%", cellStyle.Font).Width;
            float textHeight = TextRenderer.MeasureText(progressVal.ToString() + "%", cellStyle.Font).Height;

            //evaluating text position according to alignment
            switch (cellStyle.Alignment)
            {
                case DataGridViewContentAlignment.BottomCenter:
                    posX = cellBounds.X + (cellBounds.Width / 2) - textWidth / 2;
                    posY = cellBounds.Y + cellBounds.Height - textHeight;
                    break;
                case DataGridViewContentAlignment.BottomLeft:
                    posX = cellBounds.X;
                    posY = cellBounds.Y + cellBounds.Height - textHeight;
                    break;
                case DataGridViewContentAlignment.BottomRight:
                    posX = cellBounds.X + cellBounds.Width - textWidth;
                    posY = cellBounds.Y + cellBounds.Height - textHeight;
                    break;
                case DataGridViewContentAlignment.MiddleCenter:
                    posX = cellBounds.X + (cellBounds.Width / 2) - textWidth / 2;
                    posY = cellBounds.Y + (cellBounds.Height / 2) - textHeight / 2;
                    break;
                case DataGridViewContentAlignment.MiddleLeft:
                    posX = cellBounds.X;
                    posY = cellBounds.Y + (cellBounds.Height / 2) - textHeight / 2;
                    break;
                case DataGridViewContentAlignment.MiddleRight:
                    posX = cellBounds.X + cellBounds.Width - textWidth;
                    posY = cellBounds.Y + (cellBounds.Height / 2) - textHeight / 2;
                    break;
                case DataGridViewContentAlignment.TopCenter:
                    posX = cellBounds.X + (cellBounds.Width / 2) - textWidth / 2;
                    posY = cellBounds.Y;
                    break;
                case DataGridViewContentAlignment.TopLeft:
                    posX = cellBounds.X;
                    posY = cellBounds.Y;
                    break;

                case DataGridViewContentAlignment.TopRight:
                    posX = cellBounds.X + cellBounds.Width - textWidth;
                    posY = cellBounds.Y;
                    break;

            }

            if (percentage >= 0.0)
            {
                if (_ColorStep)
                {
                    if (percentage >= 0.8)
                    {
                        g.FillRectangle(new SolidBrush(Color.PaleVioletRed), cellBounds.X + 2, cellBounds.Y + 2, Convert.ToInt32((percentage * cellBounds.Width * /*0.8*/1)), cellBounds.Height / 1 - 5);
                    }
                    else if (percentage >= 0.6)
                    {
                        g.FillRectangle(new SolidBrush(Color.Orange), cellBounds.X + 2, cellBounds.Y + 2, Convert.ToInt32((percentage * cellBounds.Width * /*0.8*/1)), cellBounds.Height / 1 - 5);
                    }
                    else if (percentage < 0.6)
                    {
                        g.FillRectangle(new SolidBrush(Color.LimeGreen), cellBounds.X + 2, cellBounds.Y + 2, Convert.ToInt32((percentage * cellBounds.Width * /*0.8*/1)), cellBounds.Height / 1 - 5);
                    }
                }
                else
                {
                    g.FillRectangle(new SolidBrush(_ProgressBarColor), cellBounds.X + 2, cellBounds.Y + 2, Convert.ToInt32((percentage * cellBounds.Width * /*0.8*/1)), cellBounds.Height / 1 - 5);
                }
                // Draw the progress 
                
                //Draw text
                g.DrawString(progressVal.ToString() + "%", cellStyle.Font, foreColorBrush, posX, posY);
            }
            else
            {
                //if percentage is negative, we don't want to draw progress bar
                //wa want only text
                if (this.DataGridView.CurrentRow.Index == rowIndex)
                {
                    g.DrawString(progressVal.ToString() + "%", cellStyle.Font, new SolidBrush(cellStyle.SelectionForeColor), posX, posX);
                }
                else
                {
                    g.DrawString(progressVal.ToString() + "%", cellStyle.Font, foreColorBrush, posX, posY);
                }
            }
        }

        public override object Clone()
        {
            DataGridViewProgressCell dataGridViewCell = base.Clone() as DataGridViewProgressCell;
            if (dataGridViewCell != null)
            {
                dataGridViewCell.ProgressBarColor = this.ProgressBarColor;
            }
            return dataGridViewCell;
        }

        internal void SetProgressBarColor(int rowIndex, Color value)
        {
            this.ProgressBarColor = value;
        }

    }

}
