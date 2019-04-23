(seq limit 0)

(defun addhandler (event socket)
  (if (eq socket nil)
      nil
    (aeadd  
     'read
     (lambda (time)   (progn
                        (print (recv  socket))
                        (seq limit (minus limit 1))
                        (print  time)
                        (close socket)
                        (aedel 'read 
                               event
                               socket)))
     event
     socket)))

(defun waithandler(event times)
  (if  (big  1 limit)
      nil
    (progn
      (aepoll event (list times))
      (waithandler event times))))

(defun  get (socket file)
  (if (eq socket nil)
      nil
    (progn
      (seq limit (add limit 1))
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
      socket)))

(defun dispatch (event times)
  (if  (big limit 500)
      (progn
        (waithandler event times)
        (dispatch event times))
    (progn
      (addhandler event
                  (get (connect (quote localhost:8528))
                       (concat 'dhc (unixtime))))
      (dispatch event (add times 1)))))

(dispatch  (aecreate 'epoll) 1)


