(setq limit 0)

(defun addhandler (event socket)
  (if (eq socket nil)
      nil
    (aeadd  
     'read
     (lambda (time)   (progn
                        (print (recv  socket))
                        (setq limit (minus limit 1))
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
      (setq limit (add limit 1))
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
      socket)))

(defun dispatch (event times)
  (if  (big limit 500)
      (progn
        (waithandler event times)
        (dispatch event times))
    (progn
      (addhandler event
                  (get (connect (storage localhost:8528))
                       (concat 'dhc (unixtime))))
      (dispatch event (add times 1)))))

(dispatch  (aecreate 'epoll) 1)


