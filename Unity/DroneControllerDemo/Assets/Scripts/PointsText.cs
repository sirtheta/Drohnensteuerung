using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

[RequireComponent(typeof(TMP_Text))]
public class PointsText : MonoBehaviour
{
    TMP_Text text;
    // Start is called before the first frame update
    void Start()
    {
        text = GetComponent<TMP_Text>();
        
    }

    // Update is called once per frame
    void Update()
    {
        if (GameManager.instance != null)
        {
            text.text = GameManager.instance.CurrentPoints.ToString();
        }
    }
}
