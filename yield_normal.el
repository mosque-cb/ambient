(defun  calc (a b)
  (iter  (a b)
    (progn
      (yield (list (add  a b) 0))
      (self  (add a 2)  (add b 3)))))

(defun  work ( n label)
  (if (eq  (mod  n  2) 0)
      (yield (list n label))
    nil))

(defun  even (stream)
  (iter (stream)
    (progn
      (work (next stream) (isstop stream))
      (self stream))))

(defun  prints (stream)
  (if  (isstop stream)
      nil
    (progn
      (print (next stream))
      (prints  stream))))

(prints (even  (calc 1 2) ))
