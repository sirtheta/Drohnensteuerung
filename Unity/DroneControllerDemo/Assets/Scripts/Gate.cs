using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

public class Gate : MonoBehaviour
{
    [SerializeField]
    private List<GameObject> objectsToDisable;

    private bool disabled = false;

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
            else if (this.transform.position.z < GameManager.instance.DistancetoDisable)
            {
                objectsToDisable.ForEach(x => x.SetActive(false));
                disabled = true;
            }
            else if (disabled == true)
            {
                objectsToDisable.ForEach(x => x.SetActive(true));
            }

        }


    }
}
