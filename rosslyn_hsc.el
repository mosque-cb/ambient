(defun routine(pid)
  (progn
    (seq 'unfinish  (cons pid (geq 'unfinish)))
    (print 'routine)
    (printchar pid)))

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
       (concat 'rosslyn_hsc (unixtime))))

(defun recycle(lst)
  (if (eq  lst  nil)
      nil
    (progn
      (pjoin  (car lst))
      (recycle (cdr lst)))))

(defun flashx()
  (progn
    (print  'recycle)
    (printchar  'memory)
    (printchar (geq 'unfinish))
    (recycle (geq 'unfinish))
    (seq 'unfinish nil)
    (dispatch (geq 'pool))))

(defun dispatch(n)
  (if  (eq  n  0)
      (flashx) 
    (progn
      (pcreate  1 'worker (network))
      (sleep 1)
      (dispatch (minus n 1)))))

(seq 'unfinish nil)
(seq 'pool 10)
(pjoin (pcreate 100 'dispatch (geq 'pool)))
