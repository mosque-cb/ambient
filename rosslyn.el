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
        (setq unfinish nil)
    (progn
      (pjoin (car lst))
      (recycle (cdr lst)))))

(defun dispatch(num times)
  (if (eq num 0)
      (progn
        (sleep 1)
        (recycle unfinish)
        (print (concat (storage times is ) times))
        (dispatch all (add times 1)))
    (progn
      (setq unfinish (cons  (print (pcreate  1 'routine)) unfinish))
      (dispatch (minus num 1)
                times))))

(setq unfinish nil)
(setq all  10)
(pjoin (pcreate 100 'dispatch  all 0))
