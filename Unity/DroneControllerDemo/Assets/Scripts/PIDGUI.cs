using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class PIDGUI : MonoBehaviour
{
    [SerializeField]
    private SerialMotionController controller;
    [SerializeField]
    public List<InputField> xPIDsIF;
    [SerializeField]
    private List<InputField> yPIDsIF;
    [SerializeField]
    private List<InputField> zPIDsIF;


    private List<InputField>[] inputFields = new List<InputField>[3];

    // Start is called before the first frame update
    void Start()
    {
        inputFields[0] = xPIDsIF;
        inputFields[1] = yPIDsIF;
        inputFields[2] = zPIDsIF;
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void RequestAndReadPIDs()
    {
        controller.RequestPIDs();
        StartCoroutine(WaitForPIDs());
    }

    public void ReadPIDs()
    {
       for(int i =0; i < 3; i++)
        {
            for(int j =0; j<3; j++)
            {
                inputFields[i][j].text = controller.CurrentPIDs[i, j].ToString();
            }
        }

    }

    IEnumerator WaitForPIDs()
    {
        yield return new WaitForSeconds(2);

        ReadPIDs();
    }

    public void SetPIDs()
    {
        float[,] pids = new float[3, 3];

        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                if(!float.TryParse(inputFields[i][j].text,out pids[i, j]))
                {
                    Debug.LogError("Failed to parse pids from inputfields");
                }
            }
        }

        controller.SetPIDs(pids);
    }


}
