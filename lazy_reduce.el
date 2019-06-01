(defun cdar (lst)
  (cdr (car lst)))

(defun merge (single outcome)
  (if  (eq  outcome nil)
      (cons single nil)
    (if  (eq  (car single)  (caar outcome))
        (cons  (list  (car single) 
                      (add (cadr single) 
                           (car  (cdar outcome))))
               (cdr outcome))
      (cons (car outcome)
            (merge single (cdr outcome)) ))))

(defun dispatch(stream outcome)
  (if  (isstop stream)
      outcome
    (dispatch 
     stream
     (merge (print (next stream))
            outcome))))

(defun  gendata(n)
  (iter (n)
        (progn
          (yield (list (cons (add 1000 
                                  (random 20)) 
                             (cons 1 nil))
                       (eq n 0)))
          (self  (minus n 1)))))

(defun  autotest(n)
  (progn
    (print n)
    (display)
    (print (dispatch  (gendata 64) nil))
    (display)
    (autotest (add n 1))))

(comment (dispatch  (gendata 64) nil))
(autotest  0)



