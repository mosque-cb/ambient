(defun extractjson(material)
  (extract material 'BRACEL))

(defun  handlejson (json)
  (jgetstring
   (jgetobject                   
    (jgetobject                
     (jgetobject             
      (jgetobject json 'id)
      'qp)
     'rsp)
    'qentity)))

(defun  text(one)
  (handlejson one))

(defun compare(query one two)
  (progn
    (if (eq (text one)
            (text two))
        (print (concat 'OK  'SPACE query))
      (print (concat 'NOTOK  'SPACE query)))
    (killjson one)
    (killjson two)))

(defun  network(socket key)
  (if (eq socket nil)
      nil
    (progn    
      (send socket (concat (lineconcat (list (quote GET /?debug_info=as&debug_id=123456&wwsy=yes&rows=0&start=0&wt=json&)
                                             key
                                             (quote & HTTP/1.0) 
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
                           'LINE)))))

(defun handlematerial(socket material)
  (progn
    (close socket)
    material))

(defun  worker(socket)
  (progn
    (handlematerial
     socket
     (recv socket))))

(defun  terminal (lst)
  (compare
   lst
   (makejson
    (extractjson
     (worker (network (connect (quote 127.0.0.1:8888))
                      (concat (quote q=) lst)))))
   (makejson
    (extractjson   
     (worker (network (connect (quote 127.0.0.1:8888))
                      (concat (quote q=) lst)))))))

(defun  reactor()
  (if (eofstdin)
      nil
    (progn
      (terminal 
       (strip (stdin)))
      (reactor))))

(reactor)
