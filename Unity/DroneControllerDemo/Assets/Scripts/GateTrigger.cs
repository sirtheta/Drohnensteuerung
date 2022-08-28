using UnityEngine;

public class GateTrigger : MonoBehaviour
{
    [SerializeField]
    private bool isExitGate;
    [SerializeField]
    private GateController gc;

    private void OnTriggerEnter(Collider other)
    {
        if(other.transform.tag == "Drone")
        {
            if (isExitGate)
            {
                Debug.Log("Exited Gate");
                
                gc.ExitTriggerFired();
            }
            else
            {
                Debug.Log("Entered Gate");
                gc.EnterGateTriggerFired();
            }
        }
    }

}
