(defun fibs(n)
  (if (eq  n 1)
      1
    (if (eq n 2)
        2
      (add (fibs  (minus n 1))
           (fibs  (minus n 2))))))

(defun routine()
  (print (concat (itoa (fibs (random 15)))
                 'SPACE
                 (pget))))

(defun recycle(lst)
  (if (eq  lst nil)
      nil
    (progn
      (pjoin (car lst))
      (recycle (cdr lst)))))

(defun dispatch(num contain)
  (if (eq num 0)
      (progn
        (recycle (geq 'unfinish))
        (seq 'unfinish nil)
        (print (concat (quote contain is ) contain))
        (dispatch contain contain))
    (progn
      (seq 'unfinish (cons  (print (pcreate  1 'routine)) (geq 'unfinish)))
      (dispatch (minus num 1)
                contain))))

(seq 'unfinish nil)
(pjoin (pcreate 100 'dispatch 10 10))
