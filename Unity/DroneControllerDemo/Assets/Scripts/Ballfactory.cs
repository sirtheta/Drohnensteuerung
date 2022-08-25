using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Ballfactory : MonoBehaviour
{
    public Transform ballPrefab;
    public float spawnTime;

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
        yield return new WaitForSeconds(spawnTime);

        Transform newBall = Instantiate(ballPrefab);
        newBall.position = transform.position;
        float size = Random.Range(0.05f, 0.3f);
        newBall.localScale = new Vector3(size, size, size);
        StartCoroutine(WaitToSpawn());
    }
}
