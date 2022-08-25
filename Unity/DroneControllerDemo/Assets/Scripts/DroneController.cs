using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DroneController : MonoBehaviour
{
    [SerializeField]
    private SerialMotionController controller;
    [SerializeField]
    private float smoothing;
    [SerializeField]
    private float accelFactor;
    [SerializeField]
    private Vector3 angleOffsets;

    Vector3 targetAngles = Vector3.zero;
    Vector3 speed = Vector3.zero;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        RotateDrone();
        MoveDrone();
        //Debug.Log(controller.CurrControllerAngles);
    }

    void RotateDrone()
    {
        Vector3 inputAngles = new Vector3(controller.CurrControllerAngles.x, controller.CurrControllerAngles.z, -controller.CurrControllerAngles.y) + angleOffsets;
        targetAngles.x = Mathf.Lerp(inputAngles.x, targetAngles.x, smoothing);
        targetAngles.y = Mathf.Lerp(inputAngles.y, targetAngles.y, smoothing);
        targetAngles.z = Mathf.Lerp(inputAngles.z, targetAngles.z, smoothing);

        this.transform.eulerAngles = targetAngles;
    }

    void MoveDrone()
    {
        speed.z += -controller.CurrControllerAcceleration.x * Time.deltaTime * accelFactor;
        speed.x += -controller.CurrControllerAcceleration.y * Time.deltaTime * accelFactor;
        Debug.LogError(controller.CurrControllerAcceleration);

        this.transform.position = this.transform.position + speed * Time.deltaTime;
    }
}
