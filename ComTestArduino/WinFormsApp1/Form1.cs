using System.IO.Ports;

namespace WinFormsApp1
{
  public partial class Form1 : Form
  {
    public Form1()
    {
      InitializeComponent();
      label2.Text = "not connected";
      send.Enabled = false;
      btnDisconnect.Enabled = false;
    }

    public SerialPort myport = new();

    private void send_Click(object sender, EventArgs e)
    {
      string input = textBox1.Text;
      myport.Write(input);

    }

    void Disconnect()
    {
      myport.Close();
    }

    void Connect()
    {
      try
      {
        myport.BaudRate = 9600;
        myport.PortName = "COM4";
        myport.DtrEnable = true;
        myport.RtsEnable = true;
        myport.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);
        myport.Open();
      }
      catch (Exception e)
      {
        label1.Text = e.Message;
        return;
      }
      label2.Text = "Connected";
      send.Enabled = true;
      btnConnect.Enabled = false;
      btnDisconnect.Enabled = true;
    }

    private void AddInputToTextBox(string indata)
    {
      label1.Invoke(new Action(() =>
      {
        label1.Text = indata;
      }));
    }

    private void DataReceivedHandler(
                       object sender,
                       SerialDataReceivedEventArgs e)
    {
      SerialPort sp = (SerialPort)sender;
      string indata = sp.ReadExisting();
   
      AddInputToTextBox(indata);
    }

    private void btnDisconnect_Click(object sender, EventArgs e)
    {
      Disconnect();
      label2.Text = "not connected";
      send.Enabled = false;
      btnConnect.Enabled = true;
      btnDisconnect.Enabled = false;
    }

    private void btnConnect_Click(object sender, EventArgs e)
    {
      Connect();
    }
  }
}