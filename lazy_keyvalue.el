(defun key(db  n)
  (lambda  (db n)
    (progn
      (nosqlrowid db) 
      (yield (list (nosqlrow db 1 2) (eq n 0)))
      (nosqlnext  db)
      (self  db (minus n 1)))))

(defun  prints (stream)
  (if  (isstop stream)
      nil
    (progn
      (print (jtosx (next stream)))
      (prints  stream))))

(prints    (key (nosqlseekgt (nosqlinit 'small.db) 
                             111720) 
                60)))

(print  'over)
