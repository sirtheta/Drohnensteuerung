using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EnemySpawner : MonoBehaviour
{
    public GameObject enemyPrefab;

    public float minTime;
    public float maxTime;

    public float minX;
    public float maxX;

    

    // Start is called before the first frame update
    void Start()
    {
        StartCoroutine(WaitToSpawn());
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    IEnumerator WaitToSpawn()
    {
        yield return new WaitForSeconds(Random.Range(minTime,maxTime));

        Transform newEnemy = Instantiate(enemyPrefab).transform;
        newEnemy.transform.position = this.transform.position + new Vector3(Random.Range(minX,maxX),0,0);

        StartCoroutine(WaitToSpawn());
    }
}
