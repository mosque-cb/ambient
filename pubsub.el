(defun cadr (lst)
  (car (cdr lst)))

(defun cddr (lst)
  (cdr (cdr lst)))

(seq  listredis (dcreate ))

(defun dispatch (event times)
  (dispatch  (aepoll  event (list times))
             (add times 1)))

(defun sendhelper(channel socket msg)
  (if  (eq  (send socket msg)  nil)
      (lrem listredis channel socket)
    nil))

(defun movement(channel sockets msg)
  (if (eq sockets nil)
      nil
    (progn
      (sendhelper channel (car sockets) msg)
      (movement channel (cdr sockets) msg))))

(defun strategy(socket cmd)
  (if  (eq cmd nil)
      nil
    (if  (eq (car cmd) 'publish)
        (progn
          (movement 
           (car (cdr cmd))
           (lrange listredis 
                   (cadr cmd) 
                   0 
                   (minus 0  1)) 
           (car (cddr cmd)))
          (close socket))
      (if  (eq (car cmd) 'subscribe)
          (rpush listredis (cadr cmd) socket)
        nil))))

(defun handle(en socket first)
  (aeadd  
   'read
   (lambda (second)   (progn
                        (strategy socket
                                  (print      (spacesplit (strip (recv  socket)))))
                        (print (concat 
                                (concat 'first 'SPACE first)
                                'SPACE
                                (concat 'second 'SPACE second)))
                        (close socket)
                        (aedel 'read 
                               en
                               socket)))
   en
   socket))

(defun init(en socket)
  (progn
    (aeadd  
     'read
     (lambda (first)   (progn
                         (print (concat 'first 'SPACE first))
                         (handle en (accept  socket) first)))
     en 
     socket)))

(dispatch 
 (init  (aecreate 'select)   
        (bind  8778  (quote 0.0.0.0)))
 0)
