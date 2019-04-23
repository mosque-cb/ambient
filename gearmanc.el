(defun  skipzero(material)
  (if (eq (car material) 0)
      (cdr material) 
    (skipzero (cdr material))))

(defun  parseres(socket material)
  (if (and (eq (car material) 0)
           (eq (car (cdr material)) 0)
           (eq (car (cdr (cdr material))) 0)
           (eq (car (cdr (cdr (cdr material)))) 8))
      (print (concat (quote job create, handle is:) 
                     (for_bytes (tail material 8))))
    (if (and (eq (car material) 0)
             (eq (car (cdr material)) 0)
             (eq (car (cdr (cdr material))) 0)
             (eq (car (cdr (cdr (cdr material)))) 13))
        (progn
          (print (concat (quote job complete, handle is:) 
                         (for_bytes (tail material 8))))
          (print (concat (quote job complete, result is:) 
                         (for_bytes (skipzero (tail material 8))))))
      (print 'unknown_res))))

(defun  handler(socket material)
  (if (and (eq (car material) 0)
           (eq (car (cdr material)) 82)
           (eq (car (cdr (cdr material))) 69)
           (eq (car (cdr (cdr (cdr material)))) 83))
      (parseres socket (tail material 4))
    (progn
      (print 'unknown_handler)
      (print material))))

(defun  submit(socket material)
  (progn
    (print 'submit)
    (send  socket req)
    (send  socket (for_bytes (cons 0 (cons 0 (cons 0 (cons 7 nil))))))
    (send  socket (for_bytes (cons 0 (cons 0 (cons 0 (cons (strlen material) nil))))))
    (send  socket material)))

(defun  proxy()
  (connect (quote 0.0.0.0:4780)))

(defun  noop(socket)
  (progn
    (handler socket (dump_bytes (recv socket 1000000)))
    (noop socket)))

(defun  dispatch(socket req)
  (progn
    (print (quote input your func name))
    (submit socket (concat (strip (stdin))
                           (for_bytes (cons 0 (cons 0 nil)))
                           (quote from gearmanc.el)))

    (noop socket)))

(dispatch (proxy)
          (for_bytes (cons 0 (cons 82 (cons 69 (cons 81 nil))))))
