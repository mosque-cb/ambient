(defun structip(ip port)
  (print  (concat ip
                  (quote :)
                  (itoa (add (mul (car port)
                                  256)
                             (car (cdr port)))))))

(defun swtich(isocket osocket time)
  (if (eq (alive isocket) 1)
      (progn
        (send  osocket
               (recv  isocket))
        (swtich isocket osocket 0))
    (if (eq (alive osocket) 1)
        (progn
          (send  isocket
                 (recv  osocket))
          (swtich isocket osocket 0))        
      (if  (big 3 time)
          (progn
            (swtich isocket osocket (add 1 time)))
        (progn
          (print 'destroy)
          (sleep 1)
          (close isocket)
          (close osocket)      
          nop)))))

(defun cmu(isocket osocket)
  (if (not (eq osocket nil))
      (progn
        (print 'cmu)
        (send  isocket
               (for_bytes (cons 5 
                                  (cons 0 
                                        (cons 0 
                                              (cons 1 
                                                    (cons 0 
                                                          (cons 0 
                                                                (cons 0 
                                                                      (cons 0 
                                                                            (cons 0 
                                                                                  (cons 0 nil))))))))))))
        (pcreate 1 'swtich isocket osocket 0))
    (progn
      (print 'cmufailed)
      (close socket))))

(defun network(echo socket)
  (progn
    (print 'network)
    (print echo)
    (cmu socket 
         (connect (structip (dotconcat (head echo 4))
                              (tail echo 4))))))

(defun handledns(socket length material)
  (cmu socket 
       (connect   (structip
                     (dns (for_bytes (head material length)))
                     (tail material length)))))

(defun mydns(echo socket)
  (progn
    (print 'dns)
    (handledns
     socket
     (car echo)
     (cdr echo))))

(defun trans(echo socket)
  (progn
    (print 'trans)
    (if (eq (size echo) 10)
        (if (and (eq  (car echo) 5)
                 (eq  (car (cdr echo)) 1)
                 (eq  (car (cdr (cdr echo))) 0)
                 (eq  (car (cdr (cdr (cdr echo)))) 1))
            (network (cdr (cdr (cdr (cdr echo))))
                     socket)nil)
      (if (and (eq  (car echo) 5)
               (eq  (car (cdr echo)) 1)
               (eq  (car (cdr (cdr echo))) 0)
               (eq  (car (cdr (cdr (cdr echo)))) 3))
          (mydns (cdr (cdr (cdr (cdr echo))))
                 socket)
        (progn
          (print 'transfailed)
          (close socket))))))

(defun response(socket)
  (progn
    (send  socket
           (for_bytes (cons 5 (cons 0 nil))))
    (trans (dump_bytes (recv socket))
           socket)))

(defun  remote(echo socket)
  (progn
    (print 'remote)
    (if (and (eq  (car echo) 5)
             (eq  (car (cdr echo)) 1)
             (eq  (car (cdr (cdr echo))) 0))
        (response socket)
      (progn
        (print 'remotefailed)
        (print echo)
        (close socket)))))

(defun  handle (socket)
  (remote (dump_bytes (recv  socket))
          socket))

(defun dispatch (socket)
  (progn
    (pcreate 1 'handle  (accept  socket))
    (print 'waiting)
    (sleep 1)
    (dispatch  socket)))

(pjoin (pcreate 500 'dispatch  (bind  8565 (quote 0.0.0.0))))










