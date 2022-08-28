using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using AmazingAssets.CurvedWorld;

public class GameManager : MonoBehaviour
{
    public static GameManager instance;

    [SerializeField]
    private float distanceToDespawn;
    [SerializeField]
    private float distanceToDisable;
    [SerializeField]
    private float maxSpeed;
    [SerializeField]
    private float maxNegSpeed;
    [SerializeField]
    private float movementSpeed;
    [SerializeField]
    private float distanceTravelled;
    [SerializeField]
    private CurvedWorldController curvedWorldController;
    [SerializeField]
    private Transform spawnedObjectsParent;
    [SerializeField]
    private GateSpawner[] spawners;
    [SerializeField]
    private int currentPoints;

    private List<Transform> transformsToResetOnRestart = new List<Transform>();
    private List<Vector3> startPosOftransformsToResetOnSRestart = new List<Vector3>();

    public float MovementSpeed { get => movementSpeed; set => movementSpeed = value; }
    public float DistanceTravelled { get => distanceTravelled; set => distanceTravelled = value; }
    public float DistancetoDespawn { get => distanceToDespawn; set => distanceToDespawn = value; }
    public float MaxSpeed { get => maxSpeed; set => maxSpeed = value; }
    public float MaxNegSpeed { get => maxNegSpeed; set => maxNegSpeed = value; }
    public CurvedWorldController CurvedWorldController { get => curvedWorldController; set => curvedWorldController = value; }
    public int CurrentPoints => currentPoints;

    public float DistancetoDisable { get => distanceToDisable; set => distanceToDisable = value; }

    private void Awake()
    {
        if (instance == null)
        {
            instance = this;
        }
        else
        {
            Destroy(gameObject);
        }


    }
    // Start is called before the first frame update
    void Start()
    {
        Time.timeScale = 0;

    }

    // Update is called once per frame
    void Update()
    {
        distanceTravelled += movementSpeed * Time.deltaTime;

        if (Input.GetKeyDown(KeyCode.Escape))
        {
            ExitGame();
        }
    }

    public void RegisterObjectToReset(Transform _object)
    {
        if (!transformsToResetOnRestart.Contains(_object))
        {
            transformsToResetOnRestart.Add(_object);
            startPosOftransformsToResetOnSRestart.Add(_object.position);
        }
    }

    public void PauseGame(bool _pause)
    {
        Time.timeScale = _pause ? 0 : 1;
    }

    public void RestartGame()
    {
        while (spawnedObjectsParent.childCount > 0)
        {
            DestroyImmediate(spawnedObjectsParent.GetChild(0).gameObject);
        }

        if (transformsToResetOnRestart.Count > 0)
        {
            for (int i = 0; i < transformsToResetOnRestart.Count; i++)
            {
                transformsToResetOnRestart[i].position = startPosOftransformsToResetOnSRestart[i];
            }
        }

        foreach(GateSpawner gs in spawners)
        {
            gs.Reset();
        }

        distanceTravelled = 0;
        movementSpeed = 0;
        currentPoints = 0;
        Time.timeScale = 1;
    }

    public void AddPoints(int _points)
    {
        currentPoints += _points;
    }

    public void ExitGame()
    {
        Application.Quit();
    }
}

