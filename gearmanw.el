(defun append(a b)
  (if (eq a nil)
      (cons b nil)
    (cons (car a)
          (append (cdr a)
                  b))))

(defun  waiting(socket)
  (progn
    (print 'waiting)
    (send  socket req)
    (send  socket (compressbytes (cons 0 (cons 0 (cons 0 (cons 4 nil))))))
    (send  socket (compressbytes (cons 0 (cons 0 (cons 0 (cons 0 nil))))))))

(defun  response(socket material)
  (progn
    (print 'response)
    (send  socket req)
    (send  socket (compressbytes (cons 0 (cons 0 (cons 0 (cons 13 nil))))))
    (send  socket (compressbytes (cons 0 (cons 0 (cons 0 (cons (strlen material) nil))))))
    (send  socket material)
    (sleep 5)
    (waiting socket)))

(defun  findtrunk(socket handle material now)
  (if (eq (car material) 0)
      (progn
        (print (concat (storage func is:) (compressbytes now)))
        (if (eq (compressbytes now) 'echo)
            (response socket 
                      (concat handle
                              (compressbytes (cons 0 nil))
                              (storage gearmanw.el_echo_)
                              (compressbytes (cdr material))))
          (if (eq (compressbytes now) 'rr)
              (response socket 
                        (concat handle
                                (compressbytes (cons 0 nil))
                                (storage gearmanw.el_rr_)
                                (compressbytes (cdr material))))
            nil)))
    (findtrunk socket  handle (cdr material)
               (append now (car material)))))

(defun  business(socket material now)
  (if (eq (car material) 0)
      (progn
        (print (concat (storage handle is:) (compressbytes now)))
        (findtrunk socket 
                   (compressbytes now)
                   (cdr material)
                   nil))
    (business socket (cdr material)
              (append now (car material)))))

(defun  parseres(socket material)
  (if (and (eq (car material) 0)
           (eq (car (cdr material)) 0)
           (eq (car (cdr (cdr material))) 0)
           (eq (car (cdr (cdr (cdr material)))) 10))
      (progn
        (print (storage no job))
        (waiting socket))
    (if (and (eq (car material) 0)
             (eq (car (cdr material)) 0)
             (eq (car (cdr (cdr material))) 0)
             (eq (car (cdr (cdr (cdr material)))) 6))
        (progn
          (print (storage job coming))
          (check socket))
      (if (and (eq (car material) 0)
               (eq (car (cdr material)) 0)
               (eq (car (cdr (cdr material))) 0)
               (eq (car (cdr (cdr (cdr material)))) 11))
          (progn
            (print (storage job assign))
            (business socket 
                      (tail material 8)
                      nil))
        (progn
          (print 'unknown_res)
          (print material))))))

(defun  handler(socket material)
  (if (and (eq (car material) 0)
           (eq (car (cdr material)) 82)
           (eq (car (cdr (cdr material))) 69)
           (eq (car (cdr (cdr (cdr material)))) 83))
      (parseres socket (tail material 4))
    (progn
      (print 'unknown_handler)
      (print material))))

(defun  check(socket)
  (progn
    (print 'check)
    (send  socket req)
    (send  socket (compressbytes (cons 0 (cons 0 (cons 0 (cons 9 nil))))))
    (send  socket (compressbytes (cons 0 (cons 0 (cons 0 (cons 0 nil))))))
    (handler socket (decompressbytes (recv socket 1000000)))))

(defun  register(socket funname)
  (progn
    (print 'register)
    (send  socket req)
    (send  socket (compressbytes (cons 0 (cons 0 (cons 0 (cons 1 nil))))))
    (send  socket (compressbytes (cons 0 (cons 0 (cons 0 (cons (strlen funname) nil))))))
    (send  socket funname)))

(defun  proxy()
  (connect (storage 0.0.0.0:4780)))

(defun  noop(socket)
  (progn
    (handler socket (decompressbytes (recv socket 1000000)))
    (noop socket)))

(defun  dispatch(socket req)
  (progn
    (register socket 'echo)
    (check socket)

    (register socket 'rr)
    (check socket)

    (noop socket)))

(dispatch (proxy)
          (compressbytes (cons 0 (cons 82 (cons 69 (cons 81 nil))))))
