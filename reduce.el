(defun cdar (lst)
  (cdr (car lst)))

(defun half (length)
  (div length 2))

(defun  merge (left right)
  (if  (eq left nil)
      right
    (if (eq right nil)
        left
      (if  (big  (car (car left))  (car (car right)))
          (cons (car right)  (merge left (cdr right)))
        (cons (car left)  (merge  (cdr left) right))))))

(defun skip (lst key  count)
  (if (eq  lst nil)
      (cons (cons key (cons count nil)) nil)
    (if  (eq (caar lst ) key)
        (skip (cdr lst) key (add count (car (cdar lst))))
      (cons (cons key (cons count nil)) lst))))

(defun reducex( lst)
  (cons (car lst)
        (reduce  (cdr lst))))

(defun reduce(lst)
  (if (eq lst nil)
      nil
    (reducex (skip (cdr lst)  (caar lst) (car (cdar lst))))))

(defun  waiting(left right)
  (reduce  (merge   (pjoin left)
                    (pjoin right))))

(defun dispatch(lst time)
  (if  (eq (size lst)  1)
      lst
    (waiting 
     (pcreate 
      (mul time 2)
      'dispatch 
      (head lst
		    (half (size lst)))
      (mul time 2))
     (pcreate
      (add (mul time 2) 1)
      'dispatch 
      (tail lst
		    (half (size lst)))
      (add (mul time 2) 1)))))

(defun  gendata(n)
  (if (eq n 0)
      nil
    (cons  (cons (add 1000 (random 20)) (cons 1 nil))
           (gendata (minus n 1)))))

(defun  autotest(n)
  (progn
    (print n)
    (display)
    (print (dispatch (gendata 64) 1))
    (display)
    (autotest (add n 1))))

(autotest 0)
(comment (pjoin (pcreate 1 'autotest  0)))



