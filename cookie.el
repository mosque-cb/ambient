(defun dispatch (event times)
  (dispatch  (aepoll  event (list times))
             (add times 1)))

(defun exactcookie (lst)
  (if  (eq  lst  nil)
      nil
    (if  (not (eq  (find  (car lst) (storage Cookie:)) nil))
        (print (car lst))
      (exactcookie (cdr lst)))))

(defun  strategy (socket material)
  (send socket
        (concat
         (peer socket)
         'LINE
         (exactcookie
          (entersplit  material)))))

(defun handle(en socket first)
  (aeadd  
   'read
   (lambda (second)   (progn
                        (strategy socket
                                  (print (recv  socket)))
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
        (bind  8543  (storage 0.0.0.0)))
 0)

