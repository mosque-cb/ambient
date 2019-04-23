(defun cadr(a)
  (car (cdr a)))

(defun dispatch (event times)
  (dispatch  (aepoll event (list times)) 
             (add times 1)))

(defun get_routine (socket material)
  (send socket
        (concat 'get_routine 
                'SPACE
                material)))

(defun post_routine (socket material)
  (send socket
        (concat 'post_routine 
                'SPACE
                material)))

(defun  strategy (socket lst)
  (if  (eq  (car lst) 'GET)
      (get_routine socket   (strdup (cadr lst) 1 (strlen (cadr lst))))
    (progn
      (print lst)
      (post_routine socket   (strdup (cadr lst) 1 (strlen (cadr lst)))))))

(defun handle(en socket first)
  (aeadd  
   'read
   (lambda (second)   (progn
                        (strategy socket
                                  (spacesplit (recv  socket 10000)))
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
