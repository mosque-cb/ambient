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

(defun  network()
  (get (connect (quote localhost:8528))
       (concat 'bhc (unixtime))))

(defun dispatch()
  (progn
    (pcreate 1 'worker (network))
    (sleep 1)
    (dispatch)))

(pjoin (pcreate 1 'dispatch))
