(defun dispatch (event times)
  (dispatch  (aepoll  event (list times))
             (add times 1)))

(defun routine (event socket material)
  (if (eq material 'invalid)
      (aedel 'read
             event
             socket)
    material))

(defun add_shell(event socket first)
  (progn
    (send  socket (concat 
                   'LINE 
                   (storage  WELCOME!!) 
                   'LINE 
                   (storage arabic>)))
    (aeadd
     'read
     (lambda (second)   (progn
                          (send  socket (eval (routine event socket (recv socket 100000)) socket))
                          (send  socket (concat 'LINE 
                                                (display) 
                                                'LINE
                                                (timetostring (unixtime))
                                                'LINE 
                                                (storage arabic>)))
                          (print (timetostring (unixtime)))
                          (print (concat 
                                  (concat 'first 'SPACE first)
                                  'SPACE
                                  (concat 'second 'SPACE second)))))
     event
     socket)))

(defun init(en socket)
  (progn
    (aeadd  
     'read
     (lambda (first)   (progn
                         (print (concat 'first 'SPACE first))
                         (add_shell en (accept  socket) first)))
     en 
     socket)))

(dispatch 
 (init  (aecreate 'select)   
        (bind  8855  (storage 0.0.0.0)))
 0)




