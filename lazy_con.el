(defun  calc (a)
  (iter  (a)
    (progn
      (yield (list a (big  a  1190000000)))
      (self (add a 1)))))

(defun  prints (stream streamx)
  (if  (isstop stream)
      nil
    (progn
      (print (list (next stream)
                   (next streamx)))
      (prints  stream streamx))))

(prints (calc 1) 
        (calc 2))
(prints (calc 1) 
        (calc 10))
