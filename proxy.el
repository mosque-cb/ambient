(defun dispatch (event times)
  (dispatch  (aepoll  event (list times))
             (add times 1)))

(defun add_proxy(event osocket isocket first)
  (progn
    (aeadd
     'read
     (lambda (second)   (progn
                          (send  isocket
                                 (recv  osocket))
                          (print (timestring))
                          (print (concat 
                                  (concat 'first 'SPACE first)
                                  'SPACE
                                  (concat 'second 'SPACE second)))
                          (aedel 'read
                                 event
                                 osocket)))
     event
     osocket)
    (aeadd
     'read
     (lambda (second)   (progn
                          (send  osocket
                                 (recv  isocket))
                          (print (timestring))
                          (print (concat 
                                  (concat 'first 'SPACE first)
                                  'SPACE
                                  (concat 'second 'SPACE second)))
                          (close isocket)
                          (close osocket)
                          (aedel 'read 
                                 event
                                 isocket)))
     event
     isocket)))

(defun init(en socket addr)
  (progn
    (aeadd  
     'read
     (lambda (first)   (progn
                         (display)
                         (print (concat 'first 'SPACE first))
                         (add_proxy en
                                    (accept socket)
                                    (connect addr)
                                    first)))
     en 
     socket)))

(dispatch 
 (init  (aecreate 'select)   
        (bind  8777  (storage 0.0.0.0))
        (strip (stdin)))
 0)








