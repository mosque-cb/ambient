(seq  socket_heavn (jcreate))

(defun saferecv(en in material)
  (if (eq material 'invalid)
      (progn
        (if (eq (jgetobject socket_heavn
                            (itoa (fdtoint in)))
                nil)
            (progn
              (atadd et
                     (add (unixtime) 3)
                     (lambda() (progn
                                 (aedel 'read 
                                        en
                                        in)
                                 (close in)
                                 (jdeleteobject socket_heavn 
                                                (itoa (fdtoint in))))))
              (jaddobject socket_heavn 
                          (itoa (fdtoint in))
                          (jcreatestring 'hello)))
          nil)
        nil)
    material))

(defun add_proxy(en in out)
  (progn
    (aeadd
     'read
     (lambda (et timex)   (progn
                            (send  out
                                   (saferecv en in (recv in)))
                            (if (eq (mod timex 10000) 0)
                                (print timex)nil)))
     en
     in)
    (aeadd
     'read
     (lambda (et timex)   (progn
                            (send  in
                                   (saferecv en out (recv out)))))
     en
     out)))

(defun structip(ip port)
  (print  (concat ip
                  (quote :)
                  (itoa (add (mul (car port)
                                  256)
                             (car (cdr port)))))))

(defun cmu(en et isocket osocket)
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
         en
         isocket
         osocket))
    (progn
      (print 'cmufailed)
      (close isocket))))

(defun network(en et echo socket)
  (progn
    (print 'network)
    (print echo)
    (cmu en
         et
         socket 
         (connect (structip (dotconcat (head echo 4))
                              (tail echo 4))))))

(defun handledns(en et socket length material)
  (cmu en
       et
       socket 
       (connect   (structip
                     (dns (for_bytes (head material length)))
                     (tail material length)))))

(defun mydns(en et echo socket)
  (progn
    (print 'dns)
    (handledns
     en
     et
     socket
     (car echo)
     (cdr echo))))

(defun trans(en et echo socket)
  (progn
    (print 'trans)
    (if (eq (size echo) 10)
        (if (and (eq  (car echo) 5)
                 (eq  (car (cdr echo)) 1)
                 (eq  (car (cdr (cdr echo))) 0)
                 (eq  (car (cdr (cdr (cdr echo)))) 1))
            (network en
                     et
                     (cdr (cdr (cdr (cdr echo))))
                     socket)nil)
      (if (and (eq  (car echo) 5)
               (eq  (car (cdr echo)) 1)
               (eq  (car (cdr (cdr echo))) 0)
               (eq  (car (cdr (cdr (cdr echo)))) 3))
          (mydns en
                 et
                 (cdr (cdr (cdr (cdr echo))))
                 socket)
        (progn
          (print 'transfailed)
          (print echo))))))

(defun response(en et socket)
  (progn
    (send  socket
           (for_bytes (cons 5 (cons 0 nil))))
    (trans en
           et
           (dump_bytes (saferecv en socket (recv socket)))
           socket)))

(defun  remote(en et echo socket)
  (progn
    (print 'remote)
    (if (and (eq  (car echo) 5)
             (eq  (car (cdr echo)) 1)
             (eq  (car (cdr (cdr echo))) 0))
        (response en et socket)
      (progn
        (print 'remotefailed)
        (print echo)))))

(defun  handle (en et socket)
  (remote en
          et
          (dump_bytes (saferecv en socket (recv socket)))
          socket))

(defun dispatch (en et times)
  (progn
    (atpoll et nil)
    (dispatch  (aepoll en (list et times))
               et
               (add times 1))))

(defun init(en socket)
  (progn
    (aeadd  
     'read
     (lambda (et times)   (progn
                            (display)
                            (handle en et (accept  socket))))
     en 
     socket)))

(pjoin (pcreate 500 'dispatch (init (aecreate 'select)      
                                    (bind  8565 (quote 0.0.0.0))) 
                (atcreate) 
                0))









