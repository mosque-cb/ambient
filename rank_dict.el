(seq maclisper (dcreate))

(seq  redis     (dcreate))
(seq  queue     'queue)

(defun caar (lst)
  (car (car lst)))

(defun cadr (lst)
  (car (cdr lst)))

(defun cdar (lst)
  (cdr (car lst)))

(defun dispatch (event times)
  (dispatch  (aepoll  event (list times))
             (add times 1)))

(defun gene_keyvalue(keyvalue lst)
  (progn
	(dset  redis  
           queue 
           (car keyvalue)
           (print (car (cdr keyvalue))))
    (build_para   lst)))

(defun build_para(lst)
  (if  (eq  lst nil)
      obj
    (gene_keyvalue
     (eqlsplit (car lst))
     (cdr lst))))

(defun get_helper (socket)
  (if (eq (dget redis  queue 'action) 'incr)
      (progn
        (zincr maclisper 
               'maclisper 
               (dget redis  queue 'key)
               (atoi (dget redis  queue 'value)))
        (send socket (dget redis  queue 'value)))
    (if (eq (dget redis  queue 'action) 'rank)
        (send socket
              (itoa 
               (zrank maclisper 
                      'maclisper 
                      (dget redis  queue 'key))))
      (if (eq (dget redis  queue 'action) 'score)
          (send socket 
                (itoa 
                 (zscore maclisper 
                         'maclisper 
                         (dget redis  queue 'key))))
        (progn
          (print (zrange maclisper 'maclisper 0 2000))
          (send socket 'over))))))

(defun get_routine (socket material)
  (progn
    (build_para (andsplit material))
    (get_helper socket)))

(defun post_routine (socket material)
  nil)

(defun  strategy (socket lst)
  (if  (eq  (car lst) 'GET)
      (get_routine socket   (strdup (cadr lst) 1 (strlen (cadr lst))))
    (post_routine socket   (strdup (cadr lst) 1 (strlen (cadr lst))))))

(defun handle(en socket first)
  (aeadd  
   'read
   (lambda (second)   (progn
                        (strategy socket
                                  (spacesplit (recv  socket)))
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

