using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DroneModelController : MonoBehaviour
{

    [SerializeField]
    private DroneController controller;
    [SerializeField]
    private Transform drone;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        var lookPos = controller.transform.position + controller.Speed - transform.position ;
        lookPos.y = 0;
        var rotation = Quaternion.LookRotation(lookPos);
        transform.localRotation = Quaternion.Slerp(transform.rotation, rotation, Time.deltaTime * 5);
    }
}
