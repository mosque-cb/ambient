(defun  get (socket)
  (send socket (concat (lineconcat (list (storage GET /cmdCmdcmd=ps+aux+%7c+grep+as HTTP/1.0) 
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

(defun network(ip)
  (get
   (connect
    (concat 
     ip 
     (storage :) 
     3001))))

(defun  reactor()
  (if (eofstdin)
      nil
    (progn
      (worker (network (print (strip  (stdin)))))
      (reactor))))

(reactor)
