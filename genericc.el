(defun  get (socket)
  (send socket (concat (lineconcat (list (storage GET /qa?rewrite=0&fmt=json&gender=1&import=1&rett=1&semantic=1&shop=1&source=search&gbk=0&q=apple HTTP/1.0) 
                                         (storage Host: 127.0.0.1) 
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
                       'LINE)))

(defun routine(pid)
  (progn
    (print 'routine)
    (print pid)
    nop))

(defun  worker(socket)
  (progn
    (print 'worker)
    (print
     (recv socket))
    (close socket)
    (routine (pget))))

(defun  network()
  (get (connect (storage 10.242.157.220:9090))))

(defun dispatch()
  (progn
    (pcreate 1 'worker (network))
    (sleep 1)
    (dispatch)))

(pjoin (pcreate 1 'dispatch))
