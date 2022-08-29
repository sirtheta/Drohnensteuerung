using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;
[RequireComponent(typeof(TMP_Text))]
public class TextReflection : MonoBehaviour
{
    TMP_Text myText;
    public TMP_Text targetText;
    // Start is called before the first frame update
    void Start()
    {
        myText = GetComponent<TMP_Text>();
    }

    // Update is called once per frame
    void Update()
    {
        myText.text = targetText.text;
        myText.color = targetText.color;
        transform.localScale = targetText.transform.localScale;
        
    }
}
