(defun  worker(socket label)
  (if (eq socket nil)
      (progn
      (close socket)            
      (print (concat label 'SPACE  (storage not ok))))
    (progn
      (close socket)    
      (print (concat label 'SPACE  (storage ok))))))

(defun  blend(ip port)
  (concat ip (storage :) port))

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
                            (storage .)
                            (car lst)))
      (link pivot
            (cdr lst)))))

(defun prolog(a b funp)
  (if (eq a nil)
      nil
    (progn
      (link (car a) b funp)
      (prolog (cdr a) b funp))))

(dispatch (concat (storage 172.19.148.22)))
(dispatch (concat (storage 172.19.148.22)))
(dispatch (concat (storage 172.19.148.22)))
