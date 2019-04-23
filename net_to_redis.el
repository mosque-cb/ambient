(memory  10000000)
(seq redis (dcreate))

(defun caar (lst)
  (car (car lst)))

(defun cddr (lst)
  (cdr (cdr lst)))

(defun cadr (lst)
  (car (cdr lst)))

(defun cdar (lst)
  (cdr (car lst)))

(defun dispatch (event times)
  (dispatch  (aepoll  event (list times))
             (add times 1)))

(defun set_routine (socket key value)
  (if (eq (dget redis  'redis  key) nil)
      (progn
        (print (concat 'key  'SPACE value))
        (dset  redis 
               'redis 
               key
               value)
        (send socket 'ok))
    (send socket 'already)))

(defun get_routine (socket material)
(if (eq (dget redis  'redis material) nil)
    (progn
      (send socket 'nothing))
  (send socket (dget redis  'redis material))))

(defun routine (event socket material)
(if (eq material 'invalid)
    (aedel 'read
           event
           socket)
  material))

(defun strategy (socket material)
(if (eq material nil)
    (send socket 'invalid)
  (if (eq (car material) 'set)
      (set_routine socket 
                   (cadr material)
                   (car (cddr material)))
    (if (eq (car material) 'get)
        (get_routine socket 
                     (cadr material))
      (send socket 'unknow)))))

(defun handle(en socket first)
(aeadd  
 'read
 (lambda (second)   (progn
                      (strategy socket
                                (spacesplit (strip (routine en socket (recv  socket)))))
                      (print (concat 
                              (concat 'first 'SPACE first)
                              'SPACE
                              (concat 'second 'SPACE second)))))
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

