using System.Collections;
using System.Collections.Generic;
using System.IO.Ports;
using System.Threading;
using UnityEngine;

public class SerialReader : MonoBehaviour
{
    private SerialPort port = new SerialPort();
    public bool isClosed = false;

    [Range(0.01f,0.99f)]
    public float smoothing = 0.9f;
    public string package;
    public float dataX;
    public float dataY;
    public float dataZ;

    public float dataMoveX;
    public float dataMoveY;

    Vector3 targetAngles = Vector3.zero;

    // Start is called before the first frame update
    void Start()
    {
        port.BaudRate = 9600;
        port.PortName = "COM3";
        //port.ReadTimeout = 500;
        port.RtsEnable = true;
        port.DtrEnable = true;

        try
        {
            port.Open();
        }
        catch { }


        Thread Hilo = new Thread(ListenSerial);
        Hilo.Start();

    }

    private void ListenSerial()
    {
        while (!isClosed)
        {
            try
            {
                //read to data from arduino
                package = port.ReadLine();
                string[] data = package.Split(':');
                float.TryParse(data[0], out dataX );
                float.TryParse(data[1], out dataY);
                float.TryParse(data[2], out dataZ);
                float.TryParse(data[3], out dataMoveX);
                float.TryParse(data[4], out dataMoveY);
            }
            catch { }
        }
    }

    // Update is called once per frame
    void Update()
    {

        if (isClosed)
        {
            if (port.IsOpen)
                port.Close();

        }

        Rotate();

    }

    void Rotate()
    {
        targetAngles.x = dataX - 90;
        targetAngles.z = 90 - dataY;
        //targetAngles.y = Mathf.Lerp(90 - dataZ, targetAngles.y,  smoothing);

        this.transform.eulerAngles = targetAngles;
        transform.localPosition = Vector3.Lerp(this.transform.localPosition + new Vector3(-dataMoveY,0,-dataMoveX), transform.localPosition, smoothing);


    }

    public void SendString(string message)
    {
        port.WriteLine(message);
    }

    public void SendInt(int message)
    {
        port.WriteLine("Test:");
    }


}
