(defun  calc (a b cont)
  (progn
    (funcall cont 
             (add  a b))
    (calc 
     (add a 2) 
     (add b 3) 
     cont))))

(defun  work(n cont)
(if (eq  (mod  n  2) 0)
    (print n)
  nil))

(calc 1 2 (funcall  (lambda (cont)
                      (lambda (result)
                        (work result cont)))
                    (lambda (result) (print result))))
