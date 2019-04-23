(defun add_proxy(event osocket isocket)
  (progn
    (aeadd
     'read
     (lambda (timex)   (progn
                         (send  isocket
                                (print (recv  osocket)))
                         (print  timex)))
     event
     osocket)
    (aeadd
     'read
     (lambda (timex)   (progn
                         (send  osocket
                                (print (recv  isocket)))
                         (print  timex)))
     event
     isocket)))

(defun structip(ip port)
  (print  (concat ip
                  (quote :)
                  (itoa (add (mul (car port)
                                  256)
                             (car (cdr port)))))))

(defun myaepoll(event isocket osocket time)
  (if (or (eq (alive isocket) 1)
          (eq (alive osocket) 1))
      (progn
        (aepoll event (list time))
        (myaepoll event isocket osocket 0))
    (if  (big 30 time)
        (progn
          (myaepoll event isocket osocket (add 1 time)))
      (progn
        (print 'destroy)
        (sleep 1)
        (close isocket)
        (close osocket)      
        (aedel 'read event isocket)
        (aedel 'read event osocket)
        (aedestroy event)
        nop))))

(defun cmu(event isocket osocket)
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
        (add_proxy 
         event
         isocket
         osocket)
        (myaepoll event isocket osocket 0))
    (progn
      (print 'cmufailed)
      (aedestroy event)
      (close socket))))

(defun network(echo socket)
  (progn
    (print 'network)
    (print echo)
    (cmu (aecreate 'select)
         socket 
         (connect (structip (dotconcat (head echo 4))
                              (tail echo 4))))))

(defun handledns(socket length material)
  (cmu (aecreate 'select)
       socket 
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










