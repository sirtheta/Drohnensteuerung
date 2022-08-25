using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class SMCGUI : MonoBehaviour
{
    [Header("Serial Motion Controller")]
    [SerializeField]
    private SerialMotionController SMC;
    [Header("GUI Elements")]
    [SerializeField]
    private InputField serialMonitorIF;
    [SerializeField]
    private InputField baudrateIF;
    [SerializeField]
    private InputField portNameIF;
    [SerializeField]
    private Toggle rtsEnableTog;
    [SerializeField]
    private Toggle dtrEnableTog;
    [SerializeField]
    private Button applySettingsBut;
    [SerializeField]
    private Button reloadSettingsBut;
    [SerializeField]
    private Button connectDisconnectBut;
    [SerializeField]
    private Text connectDisconnectText;



    // Start is called before the first frame update
    private void Start()
    {
        baudrateIF.contentType = InputField.ContentType.IntegerNumber;
        portNameIF.contentType = InputField.ContentType.Standard;

        ReadSettings();

    }

    // Update is called once per frame
    private void Update()
    {
        serialMonitorIF.text = SMC.SerialMonitor;

        connectDisconnectText.text = SMC.PortIsOpen ? "DISCONNECT" : "CONNECT";

        if (ChangesToApply())
        {
            applySettingsBut.gameObject.SetActive(true);
            reloadSettingsBut.gameObject.SetActive(true);
        }
        else
        {
            applySettingsBut.gameObject.SetActive(false);
            reloadSettingsBut.gameObject.SetActive(false);
        }
        
    }

    private bool ChangesToApply()
    {
        return (baudrateIF.text != SMC.SerialBaudrate.ToString() ||
                portNameIF.text != SMC.PortName ||
                rtsEnableTog.isOn != SMC.RtsEnable ||
                dtrEnableTog.isOn != SMC.DtrEnable);
    }

    public void ReadSettings()
    {
        baudrateIF.text = SMC.SerialBaudrate.ToString();
        portNameIF.text = SMC.PortName;
        rtsEnableTog.isOn = SMC.RtsEnable;
        dtrEnableTog.isOn = SMC.DtrEnable;
    }

    public void ConnectDisconnect()
    {
        if (SMC == null) { return; }

        if (SMC.PortIsOpen)
        {
            SMC.Disconnect();
        }
        else
        {
            SMC.Connect();
        }
    }
    public void Connect() => SMC.Connect();
    public void Disconnect() => SMC.Disconnect();

    public void SetSettings()
    {
        int baudrate = int.Parse(baudrateIF.text);
        string portName = portNameIF.text;
        bool rts = rtsEnableTog.isOn;
        bool dtr = dtrEnableTog.isOn;
        SMC.SetSettings(baudrate, portName, rts, dtr);
    }

}
