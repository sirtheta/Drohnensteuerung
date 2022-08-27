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

    [Header("Options")]
    [SerializeField]
    private bool connectOnStartup;
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
    [SerializeField]
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

    private void Start()
    {
        if (connectOnStartup)
        {
            Connect();
        }
    }

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

        if (data[0] == "DATT")
        {
            float xAVal = 0, yAVal = 0, zAVal = 0, xMVal = 0, yMVal = 0, zMVal = 0;

            if(float.TryParse(data[1], out xAVal) &&
               float.TryParse(data[2], out yAVal) &&
               float.TryParse(data[4], out xMVal) &&
               float.TryParse(data[5], out yMVal))
            {
                currControllerAngles.x = xAVal;
                currControllerAngles.y = yAVal;
                currControllerAcceleration.x = xMVal;
                currControllerAcceleration.y = yMVal;
            }

        
        }
        else if (data[0] == "PIDT")
        {
           
            switch (data[1])
            {
                case "X":
                    if (!(float.TryParse(data[3], out value)))
                    {
                        Debug.LogError($"Parsing to float of param: {data[3]} not possible");
                    }
                    else
                    {
                        switch (data[2])
                        {
                            case "P":
                                Debug.Log("Received P of X");
                                currentPIDs[0, 0] = value / 10000;
                                break;
                            case "I":
                                currentPIDs[0, 1] = value / 10000;
                                break;
                            case "D":
                                currentPIDs[0, 2] = value / 10000;
                                break;
                            default:
                                break;
                        }
  
                    }
                    break;

                case "Y":

                    if (!(float.TryParse(data[3], out value)))
                    {
                        Debug.LogError($"Parsing to float of param:{data[3]} not possible");
                    }
                    else
                    {
                        switch (data[2])
                        {
                            case "P":
                                currentPIDs[1, 0] = value / 10000;
                                break;
                            case "I":
                                currentPIDs[1, 1] = value / 10000;
                                break;
                            case "D":
                                currentPIDs[1, 2] = value / 10000;
                                break;
                            default:
                                break;
                        }
                    }
                    break;

                case "Z":
                    if (!(float.TryParse(data[3], out value)))
                    {
                        Debug.LogError($"Parsing to float of param:  {data[3]} not possible");
                    }
                    else
                    {
                        switch (data[2])
                        {
                            case "P":
                                currentPIDs[2, 0] = value / 10000;
                                break;
                            case "I":
                                currentPIDs[2, 1] = value / 10000;
                                break;
                            case "D":
                                currentPIDs[2, 2] = value / 10000;
                                break;
                            default:
                                break;
                        }
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

        controllerPort.Write("PIDR|X|P;");
        controllerPort.Write("PIDR|X|D;");
        controllerPort.Write("PIDR|X|I;");
        controllerPort.Write("PIDR|Y|P;");
        controllerPort.Write("PIDR|Y|D;");
        controllerPort.Write("PIDR|Y|I;");
        controllerPort.Write("PIDR|Z|P;");
        controllerPort.Write("PIDR|Z|D;");
        controllerPort.Write("PIDR|Z|I;");
    }

    public void SetPIDs(float[,] _pids)
    {
        string[] axis = { "X", "Y", "Z" };
        string[] param = { "P", "I", "D" };

        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                controllerPort.WriteLine($"PIDS|{axis[i]}|{param[j]}|{_pids[i, j]};");
            }
        }

    }

    






}

