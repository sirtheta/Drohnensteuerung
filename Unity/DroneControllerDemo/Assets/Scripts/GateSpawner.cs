using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GateSpawner : MonoBehaviour
{
    [SerializeField]
    private GameObject standartGatePrefab;
    [SerializeField]
    private float gateSpacing;
    [SerializeField]
    private float gateSpreading;
    [SerializeField]
    private Transform objectParent;

    private float lastSpawnedAt = 0;

    // Start is called before the first frame update
    void Start()
    {
        SpawnGate();

    }

    // Update is called once per frame
    void Update()
    {
        if (GameManager.instance != null)
        {
            if (GameManager.instance.DistanceTravelled > lastSpawnedAt + gateSpacing)
            {
                SpawnGate();
            }
        }
    }

    private void SpawnGate()
    {
        Transform newGate = GameObject.Instantiate(standartGatePrefab).transform;
        newGate.transform.parent = objectParent;
        newGate.position = this.transform.position + new Vector3(UnityEngine.Random.Range(-gateSpreading / 2, gateSpreading / 2), 0, 0);
        lastSpawnedAt = GameManager.instance.DistanceTravelled;
    }

    public void Reset()
    {
        lastSpawnedAt = 0;
    }
}
