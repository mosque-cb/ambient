(defun exactsize (lst)
  (if  (eq lst  nil)
      nil
    (if  (not (eq  (find (car lst) 
                         (quote Content-Length:)) 
                   nil))
        (print (atoi  (strdup (car lst)
                              (add 2 (find (car lst) (quote : )))
                              (strlen (car lst)))))
      (exactsize (cdr lst)))))

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
          (print 'error)
          (print material)
          (close socket))
      (progn
        (fwrite file (strdup material
                             (add 4 (find material (concat 'LINE 'LINE)))
                             (strlen material)))
        (com
         (minus (exactsize (linesplit material))
                (minus (strlen material)
                       (add 4 (find material (concat 'LINE 'LINE)))))
         (recv socket)
         socket
         file)))))

(defun wrap_strategy(socket file)
  (strategy socket
            (recv socket 10000000)
            file))

(defun  get (socket file)
  (progn
    (send socket (concat (lineconcat (list (concat (quote GET ) file 'SPACE (quote HTTP/1.0)) 
                                           (quote Host: api.yeelink.net) 
                                           (quote U-ApiKey:8b6c51b8a18ccbdae3c7ac74169ec3da) 
                                           (quote Content-Length: 0)
                                           (quote User-Agent: http_get) 
                                           (quote Content-Type: application/json) 
                                           (quote Accept: */*) 
                                           (quote Accept-Language: utf8) 
                                           (quote Accept-Charset: iso-8859-1,*,utf-8) 
                                           (quote Authorization: Basic YWRtaW46YWRtaW4=) 
                                           (quote Connection: Keep-Alive)))
                         'LINE
                         'LINE))
    socket))

(defun dispatch (filename)
  (wrap_strategy 
   (get (connect (quote 127.0.0.1:8634))
        (concat (quote ./) filename))
   (fopen filename 'wb)))

(dispatch  (progn
             (print (quote input the filename:))
             (strip (stdin))))


