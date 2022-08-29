using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GateSpawner : MonoBehaviour
{
    [SerializeField]
    private Gate[] gatePrefabs;
    [SerializeField]
    private float gateSpacing;
    [SerializeField]
    private float gateSpreading;
    [SerializeField]
    private Transform objectParent;
    [SerializeField]
    private Gate michaelPrefab;
    
    
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
        if (michaelPrefab!= null &&GameManager.instance.Seed.ToLower() == "michael")
        {
            Transform newGate = GameObject.Instantiate(michaelPrefab.gameObject).transform;
            newGate.transform.parent = objectParent;
            newGate.position = this.transform.position;
            lastSpawnedAt = GameManager.instance.DistanceTravelled -10;
        }
        else
        {

            int randomIndex = UnityEngine.Random.Range(0, gatePrefabs.Length);


            Transform newGate = GameObject.Instantiate(gatePrefabs[randomIndex].gameObject).transform;
            newGate.transform.parent = objectParent;
            newGate.position = this.transform.position + new Vector3(UnityEngine.Random.Range(-gateSpreading / 2, gateSpreading / 2), 0, 0);
            lastSpawnedAt = GameManager.instance.DistanceTravelled + gatePrefabs[randomIndex].length;
        }
    }

    public void Reset()
    {
        lastSpawnedAt = 0;
    }
}
