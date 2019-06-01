(defun extractsize (lst)
  (if  (eq lst  nil)
      nil
    (if  (not (eq  (find (car lst) 
                           (quote Content-Length:)) 
                   nil))
        (print (atoi  (strdup (car lst)
                              (add 2 (find (car lst) (quote : )))
                              (strlen (car lst)))))
      (extractsize (cdr lst)))))

(defun  com(filesize material socket file)
  (if (eq (print filesize) 0)
      (progn
        (fclose file)
        (close socket))
    (if (eq material nil)
        (com filesize
             (recv socket) 
             socket 
             file)
      (progn
        (fwrite file material)
        (com (minus filesize (strlen material))
             (recv socket) 
             socket 
             file)))))

(defun strategy(socket material file)
  (if (eq material nil)
      (progn
        (print 'trying)
        (wrap_strategy socket file))
    (if (big 100 (print (strlen material)))
        (progn
          (print (quote strategy error))
          (print material)
          (close socket))
      (progn
        (fwrite file (strdup material
                             (add 4 (find material (concat 'LINE 'LINE)))
                             (strlen material)))
        (com
         (minus (extractsize (linesplit material))
                (minus (strlen material)
                       (add 4 (find material (concat 'LINE 'LINE)))))
         (recv socket)
         socket
         file)))))

(defun wrap_strategy(socket file)
  (strategy socket
            (recv socket 10000000)
            file))

(defun  remote(en filename socket)
  (progn
    (print 'filename) 
    (print filename)   
    (if (not (eq  filename nil))
        (progn
          (send socket 'ok)
          (wrap_strategy socket 
                         (fopen filename 'wb)))
      (print (quote remote error)))))

(defun  handle (en socket)
  (remote en
          (recv socket 1000000)
          socket))

(defun dispatch (en times)
  (progn
    (dispatch  (aepoll en (list times))
               (add times 1))))

(defun init(en socket)
  (progn
    (aeadd  
     'read
     (lambda (en socket times)   (progn
                                   (display)
                                   (handle en (accept  socket))))
     en
     socket)))

(pjoin (pcreate 500 
                'dispatch 
                (init 
                 (aecreate 'select) 
                 (bind  8778 (quote 0.0.0.0)))
                0))




