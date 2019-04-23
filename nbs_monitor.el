(defun main(socket addr)
  (if (eq socket nil)
      (system (print (concat (quote gsmsend  -s emp01.company.com:15002  -s emp02.company.com:15002  13641166638@)
                      addr)))
    (progn
      (close socket) 
      (print 'alive))))

(defun  leftstrip (material)
  (strdup material
          1 
          (len material)))

(defun  rightstrip (material)
  (strdup material
          0 
          (minus (len material) 1)))

(defun  handler (material)
  (if (eq material nil)
      nil
    (if (eq (rightstrip (car material))
            'server)
        (concat (leftstrip (car (cdr material)))
                (quote :)
                (leftstrip (car (cdr
                               (split (strip (stdin)) (quote :))))))
      nil)))

(defun  wrapcons(a b)
  (if (eq a nil)
      b
    (cons a b)))

(defun  exact_iplist()
  (if (eofstdin)
      nil
    (progn
      (wrapcons 
       (handler (split (strip (stdin)) (quote :)))
       (exact_iplist)))))

(defun loop(alist blist)
  (if (eq alist nil)
      (progn
        (sleep 600)
        (loop blist nil))
    (progn
      (print (car alist))
      (main (connect  (car alist))
            (car alist))
      (sleep 1)
      (loop (cdr alist) (cons (car alist) blist)))))

(loop (exact_iplist) nil)
