(defun add_proxy(en in out)
  (progn
    (aeadd
     'read
     (lambda (et timex)   (progn
                            (send  out
                                   (recv  in))
                            (print timex)))
     en
     in)
    (aeadd
     'read
     (lambda (et timex)   (progn
                            (send  in
                                   (recv  out))
                            (print timex)))
     en
     out)))

(defun structip(ip port)
  (print  (concat ip
                  (storage :)
                  (itoa (add (mul (car port)
                                  256)
                             (car (cdr port)))))))

(defun cmu(en et isocket osocket)
  (if (not (eq osocket nil))
      (progn
        (print 'cmu)
        (send  isocket
               (compressbytes (cons 5 
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
         osocket)
        (atadd et
               (add (unixtime) 300)
               (lambda() (progn
                           (aedel 'read 
                                  en
                                  isocket)
                           (close isocket)
                           (aedel 'read 
                                  en
                                  osocket)
                           (close osocket)))))
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
                     (dns (compressbytes (head material length)))
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
          (close socket))))))

(defun response(en et socket)
  (progn
    (send  socket
           (compressbytes (cons 5 (cons 0 nil))))
    (trans en
           et
           (decompressbytes (recv socket))
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
        (print echo)
        (close socket)))))

(defun  handle (en et socket)
  (remote en
          et
          (decompressbytes (recv  socket))
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

(pjoin (pcreate 500 'dispatch (init (aecreate 'select) (bind  8565 (storage 0.0.0.0))) (atcreate) 0))









