using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Enemy : MonoBehaviour
{

    public float speed;

    // Update is called once per frame
    void FixedUpdate()
    {
        this.transform.position = transform.position + transform.forward * speed;
    }


    void OnTriggerEnter(Collider other)
    {
        if(other.transform.name == "Plattform") 
        {
            Debug.Log("VERLOREN");
        }

    }
}
