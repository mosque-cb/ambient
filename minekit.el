(defun caar (lst)
  (car (car lst)))

(defun cadr (lst)
  (car (cdr lst)))

(defun cdar (lst)
  (cdr (car lst)))

(defun  work (f n label)
  (if (eq (funcall f  n)  1)
      (yield (list n label))
    (if (eq label 1)
	(stop)
      nil)))

(defun  filter (f stream)
  (iter (f  stream)
    (progn
      (work f  (next stream) (isstop stream) )
      (self f  stream))))


(defun  map (f stream)
  (iter (f stream)
    (progn
      (yield (list (funcall f  (next stream)) 
		   (isstop stream)))
      (self  f stream))))



(defun  social (stream streamx)
  (iter (stream  streamx)
    (progn
      (yield (list (list (next stream)  (next streamx))
		   (or  (isstop stream) (isstop streamx))))
      (self  stream
	     streamx))))

(defun  reactor(n times start)
  (iter  (n times start value)
    (progn
      (yield (list  (random n) 
		    (big start times)))
      (self  n times  (add  start  1) ))))


(defun  prints (stream)
  (if  (isstop stream)
      nil
    (progn
      (print (next stream))
      (prints  stream))))



(prints (reactor  10 10  1 ))


(prints (social (reactor  100  10  1)  (reactor  10 10  1)))


(prints (filter  (lambda (lst ) 1) (social (reactor  100  10  1)  (reactor  10 10  1))))

(prints (filter  (lambda (lst ) (big  (mul (car lst) (cadr lst))  500)) (social (reactor  100  10  1)  (reactor  10 10  1))))


(prints (filter  (lambda (lst ) (big  (mul (car lst) (cadr lst))  500)) (social (map (lambda (n) (add 100 n)) (reactor  100  100000000  1) )  (reactor  10 100000000  1))))






