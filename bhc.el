(defun routine(pid)
  (progn
    (printchar 'routine)
    (print pid)
    nop))

(defun  worker(socket)
  (progn
    (print 'worker)
    (print (recv socket))
    (close socket)
    (routine (pget))))

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

(defun  network()
  (get (connect (storage localhost:8528))
       (concat 'bhc (unixtime))))

(defun dispatch()
  (progn
    (pcreate 1 'worker (network))
    (sleep 1)
    (dispatch)))

(pjoin (pcreate 1 'dispatch))
