using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GateController : MonoBehaviour
{
    [SerializeField]
    private int pointsReward;
    [SerializeField]
    private MeshRenderer gateMesh;
    [SerializeField]
    private Light gateLight;
    [SerializeField]
    private Material gateMaterialApproach;
    [SerializeField]
    private Material gateMaterialPassed;
    [SerializeField]
    private Color gateColorApproach;
    [SerializeField]
    private Color gatePassedApproach;
    [SerializeField]
    private AudioSource sound;

    private bool enterTriggerOk = false;
    

    // Start is called before the first frame update
    void Start()
    {
        //gateMesh.material = gateMaterialApproach;
        //gateLight.color = gateColorApproach;
    }

    // Update is called once per frame
    void Update()
    {
        
    }


    public void EnterGateTriggerFired()
    {
        enterTriggerOk = true;
    }
    public void ExitTriggerFired()
    {
        if (enterTriggerOk)
        {
            
            gateMesh.material = gateMaterialPassed;
            gateLight.color = gatePassedApproach;
            sound.Play();
            GameManager.instance.AddPoints(pointsReward);

        }
    }

    
}
