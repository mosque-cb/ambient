(defun caar (lst)
  (car (car lst)))

(defun cadr (lst)
  (car (cdr lst)))

(defun cdar (lst)
  (cdr (car lst)))

(defun  y ( f)
  (funcall 
   (lambda (g)
     (funcall g  g))
   (lambda (h)
     (lambda (x)
       (funcall (print (funcall f  (funcall h h)))
		x)))))

(seq  factorial  (y  (lambda  ( mirror) 
			( lambda (x) 
			  (if  (eq (car x) 0) 1 
			    (mul  (car x ) 
				(funcall  mirror (list (minus (car x ) 1) (cadr x)))))))))

(funcall factorial (list 5  4) )

