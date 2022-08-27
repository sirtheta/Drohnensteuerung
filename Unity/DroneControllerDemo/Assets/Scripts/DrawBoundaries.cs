using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(LineRenderer))]
public class DrawBoundaries : MonoBehaviour
{
    private LineRenderer lineRenderer;
    [SerializeField]
    private Transform minPos;
    [SerializeField]
    private Transform maxPos;

    Vector3[] lrPositions = new Vector3[5];



    // Start is called before the first frame update
    void Start()
    {
        lineRenderer = GetComponent<LineRenderer>();
        lineRenderer.positionCount = 5;
        lrPositions[0] = minPos.position;
        lrPositions[1] = new Vector3(maxPos.position.x,minPos.position.y,minPos.position.z);
        lrPositions[2] = maxPos.position;
        lrPositions[3] = new Vector3(minPos.position.x, maxPos.position.y, maxPos.position.z);
        lrPositions[4] = minPos.position;

        lineRenderer.SetPositions(lrPositions);
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
