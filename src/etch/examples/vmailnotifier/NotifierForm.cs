using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Net;


namespace vmailnotifier
{
    public class NotifierForm : Cisco.PhoneSuite.Controls.NotifyWindow.OutlookNotifier
    {
        private string callingPrefix = "Voice message from";    // calling prefix

        private System.Windows.Forms.PictureBox pictureBoxImage;
        private System.Windows.Forms.Label labelName;
        private System.Windows.Forms.Label labelTimeStamp;
        private Cisco.PhoneSuite.Controls.ImageButton.ImageButton imageButtonCallback;
        private Cisco.PhoneSuite.Controls.ImageButton.ImageButton imageButtonListen;
        private System.ComponentModel.IContainer components = null;

        #region default constructor
        /// <summary>
        /// default constructor
        /// </summary>
        public NotifierForm()
        {
            // This	call is	required by	the	Windows	Form Designer.
            InitializeComponent();
        }
        #endregion

        ///	<summary>
        ///	Clean up any resources being used.
        ///	</summary>
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (components != null)
                {
                    components.Dispose();
                }
            }
            base.Dispose(disposing);
        }

        #region	Designer generated code
        ///	<summary>
        ///	Required method	for	Designer support - do not modify
        ///	the	contents of	this method	with the code editor.
        ///	</summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(NotifierForm));
            this.pictureBoxImage = new System.Windows.Forms.PictureBox();
            this.labelName = new System.Windows.Forms.Label();
            this.labelTimeStamp = new System.Windows.Forms.Label();
            this.imageButtonCallback = new Cisco.PhoneSuite.Controls.ImageButton.ImageButton();
            this.imageButtonListen = new Cisco.PhoneSuite.Controls.ImageButton.ImageButton();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxImage)).BeginInit();
            this.SuspendLayout();
            // 
            // pictureBoxImage
            // 
            this.pictureBoxImage.BackColor = System.Drawing.Color.White;
            this.pictureBoxImage.ErrorImage = ((System.Drawing.Image)(resources.GetObject("pictureBoxImage.ErrorImage")));
            this.pictureBoxImage.Image = ((System.Drawing.Image)(resources.GetObject("pictureBoxImage.Image")));
            this.pictureBoxImage.InitialImage = ((System.Drawing.Image)(resources.GetObject("pictureBoxImage.InitialImage")));
            this.pictureBoxImage.Location = new System.Drawing.Point(12, 21);
            this.pictureBoxImage.Name = "pictureBoxImage";
            this.pictureBoxImage.Size = new System.Drawing.Size(45, 40);
            this.pictureBoxImage.TabIndex = 2;
            this.pictureBoxImage.TabStop = false;
            // 
            // labelName
            // 
            this.labelName.AutoSize = true;
            this.labelName.BackColor = System.Drawing.Color.Transparent;
            this.labelName.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold);
            this.labelName.ForeColor = System.Drawing.SystemColors.HotTrack;
            this.labelName.ImageAlign = System.Drawing.ContentAlignment.TopLeft;
            this.labelName.Location = new System.Drawing.Point(65, 25);
            this.labelName.Name = "labelName";
            this.labelName.Size = new System.Drawing.Size(202, 13);
            this.labelName.TabIndex = 3;
            this.labelName.Text = "Voice message from 408-555-5555";
            this.labelName.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // labelTimeStamp
            // 
            this.labelTimeStamp.AutoSize = true;
            this.labelTimeStamp.BackColor = System.Drawing.Color.Transparent;
            this.labelTimeStamp.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelTimeStamp.ForeColor = System.Drawing.SystemColors.HotTrack;
            this.labelTimeStamp.Location = new System.Drawing.Point(65, 48);
            this.labelTimeStamp.Name = "labelTimeStamp";
            this.labelTimeStamp.Size = new System.Drawing.Size(132, 13);
            this.labelTimeStamp.TabIndex = 5;
            this.labelTimeStamp.Text = "08/01/2007  10:00:00 AM";
            // 
            // imageButtonCallback
            // 
            this.imageButtonCallback.BackColor = System.Drawing.Color.WhiteSmoke;
            this.imageButtonCallback.ButtonForm = Cisco.PhoneSuite.Controls.ImageButton.eButtonForm.Rectangle;
            this.imageButtonCallback.HideBorder = false;
            this.imageButtonCallback.HighlightColor = System.Drawing.SystemColors.HotTrack;
            this.imageButtonCallback.HottrackImage = null;
            this.imageButtonCallback.Location = new System.Drawing.Point(277, 43);
            this.imageButtonCallback.Name = "imageButtonCallback";
            this.imageButtonCallback.NormalImage = ((System.Drawing.Image)(resources.GetObject("imageButtonCallback.NormalImage")));
            this.imageButtonCallback.OnlyShowBitmap = false;
            this.imageButtonCallback.PressedImage = ((System.Drawing.Image)(resources.GetObject("imageButtonCallback.PressedImage")));
            this.imageButtonCallback.Size = new System.Drawing.Size(23, 23);
            this.imageButtonCallback.TextAlign = Cisco.PhoneSuite.Controls.ImageButton.eTextAlign.None;
            this.imageButtonCallback.ToolTip = "Callback";
            this.imageButtonCallback.Click += new System.EventHandler(this.imageButtonCallback_Click);
            // 
            // imageButtonListen
            // 
            this.imageButtonListen.BackColor = System.Drawing.Color.WhiteSmoke;
            this.imageButtonListen.ButtonForm = Cisco.PhoneSuite.Controls.ImageButton.eButtonForm.Rectangle;
            this.imageButtonListen.HideBorder = false;
            this.imageButtonListen.HighlightColor = System.Drawing.SystemColors.HotTrack;
            this.imageButtonListen.HottrackImage = null;
            this.imageButtonListen.Location = new System.Drawing.Point(235, 43);
            this.imageButtonListen.Name = "imageButtonListen";
            this.imageButtonListen.NormalImage = ((System.Drawing.Image)(resources.GetObject("imageButtonListen.NormalImage")));
            this.imageButtonListen.OnlyShowBitmap = false;
            this.imageButtonListen.PressedImage = ((System.Drawing.Image)(resources.GetObject("imageButtonListen.PressedImage")));
            this.imageButtonListen.Size = new System.Drawing.Size(23, 23);
            this.imageButtonListen.TextAlign = Cisco.PhoneSuite.Controls.ImageButton.eTextAlign.None;
            this.imageButtonListen.ToolTip = "Listen";
            this.imageButtonListen.Click += new System.EventHandler(this.imageButtonListen_Click);
            // 
            // NotifierForm
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.BackColor = System.Drawing.Color.White;
            this.ClientSize = new System.Drawing.Size(330, 75);
            this.Controls.Add(this.imageButtonListen);
            this.Controls.Add(this.imageButtonCallback);
            this.Controls.Add(this.labelTimeStamp);
            this.Controls.Add(this.labelName);
            this.Controls.Add(this.pictureBoxImage);
            this.Name = "NotifierForm";
            this.Controls.SetChildIndex(this.pictureBoxImage, 0);
            this.Controls.SetChildIndex(this.labelName, 0);
            this.Controls.SetChildIndex(this.labelTimeStamp, 0);
            this.Controls.SetChildIndex(this.imageButtonCallback, 0);
            this.Controls.SetChildIndex(this.imageButtonListen, 0);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxImage)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }
        #endregion

        #region public interface
        /// <summary>
        /// Display notify window with input parameters
        /// </summary>
        /// <param name="cid">caller id</param>
        /// <param name="timeStamp">time stamp</param>
        /// <param name="x0">x coordinate</param>
        /// <param name="y0">y coordinate</param>
        public void Notify(string cid, string timeStamp, int x0, int y0)
        {
            labelName.Text = string.Format("{0} {1}", callingPrefix, cid);
            labelTimeStamp.Text = timeStamp;            
            base.Notify(x0, y0);
        }
        #endregion

        private void imageButtonListen_Click(object sender, EventArgs e)
        {
            // do listen here...
            System.Diagnostics.Process proc = new System.Diagnostics.Process();
            proc.EnableRaisingEvents = false;
            proc.StartInfo.FileName = "sndrec32";
            proc.StartInfo.Arguments = "/play vmail.wav";
            proc.Start();
        }

        private void imageButtonCallback_Click(object sender, EventArgs e)
        {
            HttpWebRequest req = (HttpWebRequest)WebRequest.Create("http://cuae-sandbox:8000/Callback/Initiate??me=613915123781385&other=61310059");
            HttpWebResponse res = (HttpWebResponse)req.GetResponse();
            if (res.StatusCode != HttpStatusCode.OK)
            {
                // error message
            }
        }
    }
}