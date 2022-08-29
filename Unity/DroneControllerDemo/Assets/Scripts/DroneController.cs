using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DroneController : MonoBehaviour
{
    [SerializeField]
    private SerialMotionController controller;
    [SerializeField]
    private Transform minPos;
    [SerializeField]
    private Transform maxPos;
    [SerializeField]
    private float correctIonAcceleration;
    [SerializeField]
    private float smoothing;
    [SerializeField]
    private float accelFactorLR;
    [SerializeField]
    private float accelFactorFB;
    [SerializeField]
    private Vector3 angleOffsets;
    [SerializeField]
    private float accelerationOffset;
    [SerializeField]
    private ParticleSystem sparks;

    Vector3 targetAngles = Vector3.zero;
    Vector3 speed = Vector3.zero;

    public Vector3 Speed => speed;
    float speedRange = 0;
    float zMovementRange = 0;

    bool sparksPlayed = false;
    bool moveDrone = true;
    Rigidbody droneRb;

    // Start is called before the first frame update
    void Start()
    {
        if (GameManager.instance != null)
        {
            GameManager.instance.RegisterObjectToReset(this.transform);
            speedRange = GameManager.instance.MaxSpeed - GameManager.instance.MaxNegSpeed;
        }

        droneRb = GetComponent<Rigidbody>();
        zMovementRange = maxPos.position.z - minPos.position.z;
    }

    public void ResetDrone()
    {
        speed = Vector3.zero;
        droneRb.useGravity = false;
        droneRb.velocity = Vector3.zero;
        moveDrone = true;
        sparksPlayed = false;
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        if (moveDrone)
        {
            RotateDrone();

            MoveDrone();
        }
        else
        {
            StopGround();
        }
    }

    private Vector3 CheckBoundaries()
    {
        Vector3 correctionVector = Vector3.zero;

        if (this.transform.position.x > minPos.position.x && this.transform.position.x < maxPos.position.x &&
            this.transform.position.z > minPos.position.z && this.transform.position.z < maxPos.position.z)
        {
            return Vector3.zero;
        }
        else
        {
            if (this.transform.position.x < minPos.position.x)
            {
                correctionVector.x += correctIonAcceleration * Time.deltaTime;
            }

            if (this.transform.position.z < minPos.position.z)
            {
                correctionVector.z += correctIonAcceleration * Time.deltaTime;
            }

            if (this.transform.position.x > maxPos.position.x)
            {
                correctionVector.x -= correctIonAcceleration * Time.deltaTime;
            }

            if (this.transform.position.z > maxPos.position.z)
            {
                correctionVector.z -= correctIonAcceleration * Time.deltaTime;
            }

            return correctionVector;
        }

    }

    void StopGround()
    {
        speed.z = Mathf.Lerp(speed.z, 0, Time.deltaTime * 2);
        GameManager.instance.MovementSpeed = speed.z;
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
        Vector3 corrVect = CheckBoundaries();

        speed.z += -controller.CurrControllerAcceleration.x + accelerationOffset * Time.deltaTime * accelFactorFB;

        if (corrVect.magnitude == 0)
        {
            speed.x += -controller.CurrControllerAcceleration.y * Time.deltaTime * accelFactorLR;
        }
        else
        {
            speed.x += corrVect.x;
        }


        this.transform.position = this.transform.position + new Vector3(speed.x, 0, 0) * Time.deltaTime;



        if (GameManager.instance != null)
        {
            if (speed.z > GameManager.instance.MaxSpeed)
            {
                speed.z = GameManager.instance.MaxSpeed;
            }
            GameManager.instance.MovementSpeed = speed.z;
        }

        float zPos = (1 / speedRange * speed.z) * zMovementRange + minPos.position.z;

        this.transform.position = new Vector3(this.transform.position.x, this.transform.position.y, zPos);



    }

    private void OnCollisionEnter(Collision collision)
    {
        moveDrone = false;
        droneRb.useGravity = true;
        droneRb.velocity = speed;

        if (sparksPlayed == false)
        {
            sparksPlayed = true;
            sparks.transform.position = collision.GetContact(0).point;
            sparks.transform.LookAt(this.transform.up);
            sparks.Play();
        }

    }
}
