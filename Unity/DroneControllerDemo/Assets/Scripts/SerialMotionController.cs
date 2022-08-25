using System;
using System.Collections;
using System.Collections.Generic;
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

    private string package;
    #endregion

    #region GETSET
    public string SerialMonitor  => serialMonitor; 
    public int SerialBaudrate    => serialBaudrate; 
    public string PortName       => portName; 
    public bool RtsEnable        => rtsEnable; 
    public bool DtrEnable        => dtrEnable;
    public bool PortIsOpen       => portIsOpen;
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
        //listenerThread.Abort(); 
        serialMonitor = "";
    }

    public void SetSettings(int _baudrate, string _portname, bool _rts, bool _dtr) 
    {
        if (portIsOpen)
        {
            Disconnect();
        }

        serialBaudrate = _baudrate;
        portName = _portname;
        rtsEnable = _rts;
        dtrEnable = _dtr;
    }
    
    private void ReadSerial()
    {
        while (portIsOpen) 
        {
            try
            {
                package = controllerPort.ReadLine();
                serialMonitor = package;
            }
            catch { }
        }
    }



}

