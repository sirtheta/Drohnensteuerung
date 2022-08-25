using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.IO.Ports;
using System.Threading;
using UnityEngine;

public class SerialMotionController : MonoBehaviour
{
    #region INSPECTOR FIELDS
    [Header("Serial Output")]
    [SerializeField]
    private string serialMonitor;
    [SerializeField]
    private Vector3 controllerEulerAngles = new Vector3();
    [SerializeField]
    private Vector3 controllerMotionVector = new Vector3();

    [Header("Options")]
    [SerializeField]
    private int serialBaudrate = 9600;
    [SerializeField]
    private string portName = "COM3";
    [SerializeField]
    private bool rtsEnable = true;
    [SerializeField]
    private bool dtrEnable = true;
    #endregion

    #region PRIVATE FIELDS
    private bool portIsOpen = false;
    private Thread listenerThread = null;
    private SerialPort controllerPort = null;

    //Data
    private Vector3 currControllerAngles = new Vector3(0.0f,0.0f,0.0f);
    private Vector3 currControllerAcceleration = new Vector3(0.0f,0.0f,0.0f);
    private float[,] currentPIDs = new float[3, 3];

    #endregion

    #region GETSET
    public string SerialMonitor  => serialMonitor; 
    public int SerialBaudrate    => serialBaudrate; 
    public string PortName       => portName; 
    public bool RtsEnable        => rtsEnable; 
    public bool DtrEnable        => dtrEnable;
    public bool PortIsOpen       => portIsOpen;

    public Vector3  CurrControllerAngles         => currControllerAngles; 
    public Vector3  CurrControllerAcceleration   => currControllerAcceleration;
    public float[,] CurrentPIDs                  => currentPIDs;
    #endregion


    public void Connect()
    {
        if(controllerPort != null)
        {
            controllerPort.Close();
        }

        //create and open port
        controllerPort = new SerialPort();
        controllerPort.BaudRate = serialBaudrate;
        controllerPort.PortName = portName;
        controllerPort.RtsEnable = rtsEnable;
        controllerPort.DtrEnable = dtrEnable;
        try
        {
            controllerPort.Open();
        }
        catch(Exception e) 
        {
            serialMonitor = e.Message;
            return;
        }
        
        portIsOpen = true;

        listenerThread = null;
        listenerThread = new Thread(ReadSerial);
        listenerThread.Start();
    }

    public void Disconnect()
    {
        portIsOpen = false;
        controllerPort.Close();
        serialMonitor = "";
    }

    public void SetSettings(int _baudrate, string _portname, bool _rts, bool _dtr) 
    {
        if (portIsOpen)
        {
            Disconnect();
        }

        serialBaudrate = _baudrate;
        portName       = _portname;
        rtsEnable      = _rts;
        dtrEnable      = _dtr;
    }
    
    private void ReadSerial()
    {
        while (portIsOpen) 
        {
            try
            {
                string frame = controllerPort.ReadLine();
                serialMonitor = frame;
                InterpretFrame(frame);
            }
            catch { }
        }
    }

    private void InterpretFrame(string _frame)
    {

        _frame = _frame.Replace(";", "");
        string[] data = _frame.Split('|');
        float value = 0;

        if (data[0] == "ANGT")
        {
 
            switch (data[1])
            {
                case "X":
                    if(!float.TryParse(data[2], out value))
                    {
                        Debug.LogError($"Parsing to float of param: {data[2]} not possible");
                    }
                    else
                    {
                        currControllerAngles.x =value;
                    }
                    break;

                case "Y":

                    if (!float.TryParse(data[2], out value))
                    {
                        Debug.LogError($"Parsing to float of param: {data[2]} not possible");
                    }
                    else
                    {
                        currControllerAngles.y = value;
                    }
                    break;

                case "Z":
                    if (!float.TryParse(data[2], out value))
                    {
                        Debug.LogError($"Parsing to float of param: {data[2]} not possible");
                    }
                    else
                    {
                        currControllerAngles.z = value;
                    }
                    break;

                default:
                    Debug.LogError("Invalid Parameter");
                    break;
            }
    
        }
        else if (data[0] == "MOVT")
        {
            switch (data[1])
            {
                case "X":
                    if (!float.TryParse(data[2], out value))
                    {
                        Debug.LogError($"Parsing to float of param: {data[2]} not possible");
                    }
                    else
                    {
                        currControllerAcceleration.x = value;
                    }
                    break;

                case "Y":

                    if (!float.TryParse(data[2], out value))
                    {
                        Debug.LogError($"Parsing to float of param: {data[2]} not possible");
                    }
                    else
                    {
                        currControllerAcceleration.y = value;
                    }
                    break;

                case "Z":
                    Debug.Log("AngleData");
                    if (!float.TryParse(data[2], out value))
                    {
                        Debug.LogError($"Parsing to float of param: {data[2]} not possible");
                    }
                    else
                    {
                        //currControllerAcceleration.z = value;
                    }
                    break;

                default:
                    Debug.LogError("Invalid Parameter");
                    break;
            }


        }
        else if (data[0] == "PIDT")
        {
            float value_i = 0;
            float value_d = 0;
            switch (data[1])
            {
                case "X":
                    if (!(float.TryParse(data[2], out value) && float.TryParse(data[3], out value_i) && float.TryParse(data[4], out value_d)))
                    {
                        Debug.LogError($"Parsing to float of param: {data[2]} or {data[3]} or {data[4]} not possible");
                    }
                    else
                    {
                        currentPIDs[0, 0] = value/10000;
                        currentPIDs[0, 1] = value_i / 10000;
                        currentPIDs[0, 2] = value_d / 10000;
                    }
                    break;

                case "Y":

                    if (!(float.TryParse(data[2], out value) && float.TryParse(data[3], out value_i) && float.TryParse(data[4], out value_d)))
                    {
                        Debug.LogError($"Parsing to float of param: {data[2]} or {data[3]} or {data[4]} not possible");
                    }
                    else
                    {
                        currentPIDs[1, 0] = value / 10000;
                        currentPIDs[1, 1] = value_i / 10000;
                        currentPIDs[1, 2] = value_d / 10000;
                    }
                    break;

                case "Z":
                    if (!(float.TryParse(data[2], out value) && float.TryParse(data[3], out value_i) && float.TryParse(data[4], out value_d)))
                    {
                        Debug.LogError($"Parsing to float of param: {data[2]} or {data[3]} or {data[4]} not possible");
                    }
                    else
                    {
                        currentPIDs[1, 0] = value / 10000;
                        currentPIDs[1, 1] = value_i / 10000;
                        currentPIDs[1, 2] = value_d / 10000;
                    }
                    break;

                default:
                    Debug.LogError("Invalid Parameter");
                    break;
            }


        }

    }

    public void RequestPIDs()
    {
        if(!portIsOpen) { return; }

        controllerPort.Write("PIDR|X;");
        controllerPort.Write("PIDR|Y;");
        controllerPort.Write("PIDR|Z;");
    }

    public void SetPIDs(float[,] _pids)
    {
        string[] axis = { "X", "Y", "Z" };
        string[] param = { "P", "I", "D" };

        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                controllerPort.WriteLine($"PIDS|{axis[i]}|{param[j]}|{_pids[i, j] * 10000};");
            }
        }

    }

    






}

