(defun  reactor()
  (if (eofstdin)
      nil
    (progn
      (cons (strip (stdin))
            (reactor)))))

(comment
 (reactor)
 )

(defun incre(num ct)
  (if (eq ct 30)
      nil
    (cons (timetodata (minus (unixtime) (mul num 86400)))
          (incre (add num 1)
                 (add ct 1)))))

(defun prolog(a funp)
  (if (eq a nil)
      nil
    (progn
      (funcall funp (car a))
      (prolog (cdr a) funp))))

(prolog (print (incre 9 0))
        (lambda (appdix)
          (system (print (concat 
                          (storage nohup hadoop fs  -cat  /user/company/data/productB/ltr/ltr_datab/)
                          appdix
                          (storage /* | grep 摄像机高清家用  >) 
                          appdix)))))
