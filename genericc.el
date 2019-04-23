(defun  get (socket)
  (send socket (concat (lineconcat (list (quote GET /cmdCmdcmd=ps+aux+%7c+grep+as HTTP/1.0) 
                                         (quote Host: 127.0.0.1) 
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
     (quote :) 
     3001))))

(defun  reactor()
  (if (eofstdin)
      nil
    (progn
      (worker (network (print (strip  (stdin)))))
      (reactor))))

(reactor)
