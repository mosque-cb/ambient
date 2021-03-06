(defun  worker(socket label)
  (if (eq socket nil)
      (progn
        (close socket)          
        (print (concat label 'SPACE  (quote not ok))))
    (progn
      (close socket)    
      (print (concat label 'SPACE  (quote ok))))))

(defun  blend(ip port)
  (concat ip (quote :) port))

(defun  fork(label server)
  (worker (connect label)  (concat label 'SPACE server)))

(defun dispatch(ip)
  (progn
    (fork (blend ip 21) 'ftp)
    (fork (blend ip 22) 'ssh)
    (fork (blend ip 23) 'telnet)    
    (fork (blend ip 80) 'http)    
    (fork (blend ip 8080) 'http)    
    nop))

(defun  reactor()
  (if (eofstdin)
      nil
    (progn
      (cons (strip (stdin))
            (reactor)))))

(comment
 (reactor)
 )

(defun incre(num)
  (if (eq num 254)
      nil
    (cons num
          (incre (add num 1)))))

(defun link(pivot lst funp)
  (if (eq lst nil)
      nil
    (progn
      (funcall funp (concat pivot
                            (quote .)
                            (car lst)))
      (link pivot
            (cdr lst)))))

(defun prolog(a b funp)
  (if (eq a nil)
      nil
    (progn
      (link (car a) b funp)
      (prolog (cdr a) b funp))))

(prolog (incre 1)
        (incre 1)
        (lambda (appdix) (dispatch (concat (quote 172.19.)
                                           appdix))))
