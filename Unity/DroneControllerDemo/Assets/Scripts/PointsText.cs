using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

[RequireComponent(typeof(TMP_Text))]
public class PointsText : MonoBehaviour
{
    TMP_Text text;
    public Color normalColor;
    public Color pointsAddedColor;
    public float colorLerpSpeed;
    public float scaleFactor;
   
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
            if (GameManager.instance.PointsAdded)
            {
                GameManager.instance.PointsAdded = false;
                text.color = pointsAddedColor;
                transform.localScale = transform.localScale * scaleFactor;
            }

            transform.localScale = Vector3.Lerp(transform.localScale, Vector3.one, Time.deltaTime * colorLerpSpeed);
            text.color = Color.Lerp(text.color, normalColor, Time.deltaTime * colorLerpSpeed);
            text.text = GameManager.instance.CurrentPoints.ToString();
        }
    }
}
