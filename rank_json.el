(seq maclisper (dcreate))

(defun caar (lst)
  (car (car lst)))

(defun cadr (lst)
  (car (cdr lst)))

(defun cdar (lst)
  (cdr (car lst)))

(defun dispatch (event times)
  (dispatch  (aepoll  event (list times))
             (add times 1)))

(defun gene_keyvalue(keyvalue lst obj)
  (build_para
   lst
   (jaddobject
    obj
    (print (car keyvalue))
    (jcreatestring  (car  (cdr keyvalue))))))

(defun build_para(lst obj)
  (if  (eq  lst nil)
      obj
    (gene_keyvalue
     (eqlsplit (car lst))
     (cdr lst)
     obj)))

(defun wrap_get_helper (socket json)
  (progn
    (get_helper socket json)
    (killjson json)
    nil))

(defun get_helper (socket json)
  (if (eq (jgetstring (jgetobject json 'action)) 'incr)
      (progn
        (zincr maclisper 
               'maclisper 
               (jgetstring (jgetobject json 'key))
               (atoi (jgetstring (jgetobject json 'value))))
        (send socket (jgetstring (jgetobject json 'value))))
    (if (eq (jgetstring (jgetobject json 'action)) 'rank)
        (send socket
              (itoa 
               (zrank maclisper 
                      'maclisper 
                      (jgetstring (jgetobject json 'key)))))
      (if (eq (jgetstring (jgetobject json 'action)) 'score)
          (send socket 
                (itoa 
                 (zscore maclisper 
                         'maclisper 
                         (jgetstring (jgetobject json 'key)))))
        (progn
          (print (zrange maclisper 'maclisper 0 2000))
          (send socket 'over))))))

(defun get_routine (socket material)
  (wrap_get_helper
   socket
   (build_para  (andsplit material)
                (jcreate))))

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
