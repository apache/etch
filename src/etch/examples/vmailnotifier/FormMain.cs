using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

using cisco.uc;
using Etch.Transport;
using Etch.Msg;

namespace vmailnotifier
{
    public partial class FormMain : Form
    {
        private delegate void onNewVoiceMailNotifierDelegate(string cid, string time, long mid);

        private VMailNotifierManager vmailNotifierManager = null;
        private VmailNotifierClientImpl clientImpl = null;
        private ConstsMessaging.MailStore mailbox = null;
        public ClientDeliveryService cds = null;
        public TcpConnection c = null;
        public DefaultDeliveryService<StubMessagingClient> svc = null;
        public ValueFactoryMessaging vf = null;
        public RemoteMessagingServer rfs = null;
        public StubMessagingClient sfc = null;

        public class ClientDeliveryService : DefaultDeliveryService<StubMessagingClient>
        {
            FormMain cm = null;

            public ClientDeliveryService(FormMain cm)
            {
                this.cm = cm;
            }

            protected override StubMessagingClient NewStub(MailboxMessageSource src)
            {
                return cm.sfc;
            }

            public override string ToString()
            {
                return "ClientDeliveryService";
            }
        }

        public FormMain()
        {
            InitializeComponent();
            vmailNotifierManager = VMailNotifierManager.Instance;

            clientImpl = new VmailNotifierClientImpl();
            clientImpl.onNewVoiceMailMessage += new OnNewVoiceMailMessageDelegate(clientImpl_onNewVoiceMailMessage);

            vf = new ValueFactoryMessaging();
            c = new TcpConnection(null, "10.89.31.176", 4011, 0);
            sfc = new StubMessagingClient(clientImpl, null, null);
            cds = new ClientDeliveryService(this);
            svc = DefaultDeliveryService<StubMessagingClient>.SetUp(cds, c, vf);
            rfs = new RemoteMessagingServer(svc);
        }

        private void saveFile(byte[] data, bool isNew)
        {
            FileStream fStream = new FileStream("vmail.wav", isNew? FileMode.Create : FileMode.Append);
            BinaryWriter bw = new BinaryWriter(fStream);
            int numBytes = Convert.ToInt32(data.Length);

            for (int i = 0; i < numBytes; i++)
            {
                fStream.Write(data, i, 1);
            }
            fStream.Close();
        }

        void clientImpl_onNewVoiceMailMessage(string cid, string time, long mid)
        {
            onNewVoiceMailNotifierDelegate del = new onNewVoiceMailNotifierDelegate(this.ShowVMailNotifier);
            this.BeginInvoke(del, new object[] { cid, time, mid });
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.notifyIconMain.Visible = false;
            Application.Exit();
        }

        private void ShowVMailNotifier(string cid, string timeStamp, long mid)
        {
            long? MID = new long?(mid);
            int? LEN = new int?(2048);
            rfs.startVoiceData(mailbox.id, MID);
            bool isNew = true;
            while (true)
            {
                sbyte[] sdata = rfs.getVoiceData(mailbox.id, MID, LEN);
                if (sdata == null || sdata.Length == 0)
                    break;

                Byte[] data = new Byte[sdata.Length];
                for (int i = 0; i < sdata.Length; i++)
                {
                    Int16 x = System.Convert.ToInt16(sdata[i]);
                    if (x >= 0)
                        data[i] = System.Convert.ToByte(sdata[i]);
                    else
                        data[i] = System.Convert.ToByte(x + 256);
                }
                saveFile(data, isNew);
                isNew = false;
            }

            vmailNotifierManager.AddNotifier(cid, timeStamp, -1, -1);
        }

        private void FormMain_Load(object sender, EventArgs e)
        {
            c.Start();
            c.WaitUp(4000);

            rfs.login("10.89.29.50", "bobbie", "Metreos1");
            mailbox = rfs.openMailbox("bobbie");
            rfs.subscribeMailStored(mailbox.id);
        }

        private void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            svc.ShutdownOutput();
            c.Stop();
            c.WaitDown(4000);
        }
    }
}