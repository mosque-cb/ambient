(defun  skipzero(material)
  (if (eq (car material) 0)
      (cdr material) 
    (skipzero (cdr material))))

(defun  parseres(socket material)
  (if (and (eq (car material) 0)
           (eq (car (cdr material)) 0)
           (eq (car (cdr (cdr material))) 0)
           (eq (car (cdr (cdr (cdr material)))) 8))
      (print (concat (storage job create, handle is:) 
                     (compressbytes (tail material 8))))
    (if (and (eq (car material) 0)
             (eq (car (cdr material)) 0)
             (eq (car (cdr (cdr material))) 0)
             (eq (car (cdr (cdr (cdr material)))) 13))
        (progn
          (print (concat (storage job complete, handle is:) 
                         (compressbytes (tail material 8))))
          (print (concat (storage job complete, result is:) 
                         (compressbytes (skipzero (tail material 8))))))
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
    (send  socket (compressbytes (cons 0 (cons 0 (cons 0 (cons 7 nil))))))
    (send  socket (compressbytes (cons 0 (cons 0 (cons 0 (cons (strlen material) nil))))))
    (send  socket material)))

(defun  proxy()
  (connect (storage 0.0.0.0:4780)))

(defun  noop(socket)
  (progn
    (handler socket (decompressbytes (recv socket 1000000)))
    (noop socket)))

(defun  dispatch(socket req)
  (progn
    (print (storage input your func name))
    (submit socket (concat (strip (stdin))
                           (compressbytes (cons 0 (cons 0 nil)))
                           (storage from gearmanc.el)))

    (noop socket)))

(dispatch (proxy)
          (compressbytes (cons 0 (cons 82 (cons 69 (cons 81 nil))))))
