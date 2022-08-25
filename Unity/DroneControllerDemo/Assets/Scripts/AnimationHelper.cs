using UnityEngine;
[RequireComponent(typeof(Animator))]
public class AnimationHelper : MonoBehaviour
{
    private Animator animator;

    private void Awake()
    {
        animator = this.GetComponent<Animator>();
    }

    public void SetBoolFalse(string _name)
    {
        try
        {
            animator.SetBool(_name, false);
        }
        catch { }
     
    }

    public void SetBoolTrue(string _name)
    {
        try
        {
            animator.SetBool(_name, true);
        }
        catch { }
        
    }

    public void TriggerBool(string _name)
    {
        try
        {
            animator.SetBool(_name, !animator.GetBool(_name));
        }
        catch { }
        
    }
}
