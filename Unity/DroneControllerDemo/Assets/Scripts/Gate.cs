using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Gate : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if (GameManager.instance != null)
        {
            transform.position += transform.forward * GameManager.instance.MovementSpeed * Time.deltaTime;
            if (this.transform.position.z < GameManager.instance.DistancetoDespawn)
            {
                Destroy(gameObject);
            }
        }

        
    }
}
