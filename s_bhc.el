(defun routine(pid)
  (progn
    (printchar 'routine)
    (print pid)
    nop))

(defun handledata(socket time)
  (if (eq (alive socket) 1)
      (if (eq (print (recv socket)) nil)
          (handledata socket 0)
        nil)
  (if (big (print time) 5)
      nil
  (progn
    (handledata socket (add time 1))))))

(defun network(socket)
  (progn
    (print 'network)
    (send  socket (concat (storage s_bhc)
                          (timetostring (unixtime))))
    (handledata socket 0)
    (close socket)
    (routine (pget))
    nil))

(defun trans(echo socket)
  (progn
    (print 'trans)
        (if (and (eq  (car echo) 5)
                 (eq  (car (cdr echo)) 0)
                 (eq  (car (cdr (cdr echo))) 0)
                 (eq  (car (cdr (cdr (cdr echo)))) 1))
            (network socket)
      (progn
        (print 'transfailed)
        (print echo)
        (close socket)))))

(defun request(socket)
  (progn
    (send  socket
           (compressbytes (cons 5 
                              (cons 1 
                                    (cons 0 
                                          (cons 1 
                                                (cons 127 
                                                      (cons 0 
                                                            (cons 0 
                                                                  (cons 1 
                                                                        (cons 33 
                                                                              (cons 80 nil))))))))))))
    (trans (decompressbytes (recv socket))
                        socket)))

(defun  remote(echo socket)
  (progn
    (print 'remote)
    (if (and (eq  (car echo) 5)
             (eq  (car (cdr echo)) 0))
        (request socket)
    (progn
      (print 'remotefailed)
      (print echo)
      (close socket)))))

(defun  worker(socket)
  (progn
    (print 'header)
    (send  socket
           (compressbytes (cons 5 (cons 1 (cons 0 nil)))))
    (remote (decompressbytes (recv socket))
                         socket)))

(defun  proxy()
  (connect (storage localhost:8565)))

(defun dispatch(time)
  (progn
    (pcreate time 'worker (proxy))
    (sleep 1)
    (dispatch (add time 1))))

(pjoin (pcreate (unixtime) 'dispatch 0))
