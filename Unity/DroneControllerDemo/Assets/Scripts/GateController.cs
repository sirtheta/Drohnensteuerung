using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GateController : MonoBehaviour
{
    [SerializeField]
    private bool drawGizmos;
    [SerializeField]
    private int pointsReward;
    [SerializeField]
    private List<MeshRenderer> gateMesh;
    [SerializeField]
    private Light gateLight = null;
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
    [SerializeField]
    private float colliderRadius;
    [SerializeField]
    private float colliderThickness;




    private DroneController droneController;
    private bool passed = false;

    private void OnDrawGizmos()
    {
        if (!drawGizmos) { return; }

        Gizmos.color = Color.green;
        Gizmos.DrawWireSphere(transform.position, colliderRadius);
        Gizmos.DrawWireCube(transform.position, new Vector3(colliderRadius, colliderRadius, colliderThickness));

    }
    // Start is called before the first frame update
    void Start()
    {
        if (GameManager.instance != null)
        {
            droneController = GameManager.instance.Drone.GetComponent<DroneController>();
        }



    }

    // Update is called once per frame
    void FixedUpdate()
    {
        if (Vector3.Distance(droneController.transform.position, transform.position) < colliderRadius)
        {
            Debug.Log("Drone is near trigger");
            if (insideCollider())
            {
                Debug.Log("Drone is INSIDE trigger");
                if (DirectionOk())
                {

                    Debug.Log("Drone direction correct!");
                    GateCorrectlyPassed();
                }
            }
        }

    }

    private bool insideCollider()
    {
        Vector3 dronePosInLocalSpace = transform.InverseTransformVector(droneController.transform.position);
        return Mathf.Abs(dronePosInLocalSpace.z) < colliderThickness;
    }
    private bool DirectionOk()
    {
        return this.transform.InverseTransformDirection(droneController.Speed).z < 0;
    }

    private void GateCorrectlyPassed()
    {
        if (passed) { return; }
        passed = true;
        gateMesh.ForEach(x => x.material = gateMaterialPassed);
        if (gateLight != null)
        {
            gateLight.color = gatePassedApproach;
        }
        sound.Play();
        GameManager.instance.AddPoints(pointsReward);
    }


}
