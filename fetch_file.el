(defun exactsize (lst)
  (if  (eq lst  nil)
      nil
    (if  (not (eq  (find (car lst) 
                         (storage Content-Length:)) 
                   nil))
        (print (atoi  (strdup (car lst)
                              (add 2 (find (car lst) (storage : )))
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
    (send socket (concat (lineconcat (list (concat (storage GET ) file 'SPACE (storage HTTP/1.0)) 
                                           (storage Host: api.yeelink.net) 
                                           (storage U-ApiKey:8b6c51b8a18ccbdae3c7ac74169ec3da) 
                                           (storage Content-Length: 0)
                                           (storage User-Agent: http_get) 
                                           (storage Content-Type: application/json) 
                                           (storage Accept: */*) 
                                           (storage Accept-Language: utf8) 
                                           (storage Accept-Charset: iso-8859-1,*,utf-8) 
                                           (storage Authorization: Basic YWRtaW46YWRtaW4=) 
                                           (storage Connection: Keep-Alive)))
                         'LINE
                         'LINE))
    socket))

(defun dispatch (filename)
  (wrap_strategy 
   (get (connect (storage 127.0.0.1:8634))
        (concat (storage ./) filename))
   (fopen filename 'wb)))

(dispatch  (progn
             (print (storage input the filename:))
             (strip (stdin))))


