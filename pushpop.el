(defun cadr (lst)
  (car (cdr lst)))

(defun cddr (lst)
  (cdr (cdr lst)))

(setq redislist (dcreate ))

(defun dispatch (event times)
  (dispatch  (aepoll  event (list times))
             (add times 1)))

(defun gene_keyvalue(keyvalue lst obj)
  (build_para
   lst
   (jaddobject
    obj
    (car keyvalue)
    (jcreatestring  (car  (cdr keyvalue))))))

(defun build_para(lst obj)
  (if  (eq  lst nil)
      obj
    (gene_keyvalue
     (eqlsplit (car lst))
     (cdr lst)
     obj)))

(defun switch (content first)
  (progn
    (killjson first)
    content))

(defun wrapmovement(json)
  (switch (movement json) 
          json))

(defun movement(json)
  (if  (eq  (jgetstring  (jgetobject json 'echostr)) 'fetch)
      (print (lpop redislist 'content))
    (if  (eq  (jgetstring  (jgetobject json 'echostr)) 'put)
        (rpush redislist 'content  (jgetstring  (jgetobject json 'material)))
      nil)))

(defun  wrap_get (event socket url)
  (send socket 
        (wrapmovement
         (build_para
          (andsplit (strdup url 1 (strlen url)))
          (jcreate)))))

(defun  wrap_post (event socket data)
  nil)

(defun  strategy (event socket str)
  (if (eq (car (spacesplit str)) 'GET)
      (wrap_get event socket (cadr (spacesplit str)))
    (wrap_post event socket (linesplit str) )))

(defun handle(en socket first)
  (aeadd  
   'read
   (lambda (second)   (progn
                        (strategy event 
                                  socket
                                  (recv  socket))
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
        (bind  8534  (storage 0.0.0.0)))
 0)


