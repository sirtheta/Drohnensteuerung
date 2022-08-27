using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using AmazingAssets.CurvedWorld;

public class CurvedPositionOfParent : MonoBehaviour
{
    private CurvedWorldController curvedWorldController;
    // Start is called before the first frame update
    void Start()
    {
        try
        {
            curvedWorldController = GameManager.instance.CurvedWorldController;
        }
        catch
        {
            Debug.LogError("Could not get curvedWorldController");
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (transform.parent != null && curvedWorldController != null)
        {
            transform.position = curvedWorldController.TransformPosition(transform.parent.position);
        }
    }
}
