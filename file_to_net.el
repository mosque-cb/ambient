(defun exactjson(material)
  (exact material 'BRACEL))

(defun  handlejson (json)
  (progn
    (print (jgetstring
            (jgetobject
             (jgetobject             
              (jgetobject             
               (jgetobject             
                (jgetobject json 'as)
                (storage as_response))
               (storage as_result))
              (storage results))
             (storage num_found))))
    json))

(defun handlematerial(material)
  (if (eq material nil)
      nil
    (print material)))

(defun  network(socket key)
  (if (eq socket nil)
      nil
    (progn    
      (send socket (concat (lineconcat (list (storage GET /?debug_info=as&debug_id=123456&wwsy=yes&rows=60&start=0&wt=json&)
                                             key
                                             (storage &fl=vendor_Name,partnumber,brand_Name,auxdescription,three_groupName,three_groupExtName,author,isbn,unit_searchable_attr,title,two_groupName,threeGroupIds,short_brand_Id HTTP/1.0) 
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
                           'LINE)))))

(defun routine(pid)
  (progn
    (print 'routine)
    (print pid)
    nop))

(defun ddd(material)
  (if (eq (find material (storage seqid=)) nil)
      (concat (storage q=iphone))
    (print  (strdup
             material
             (find material (storage seqid=))
             (strlen material)))))
  
(defun  worker(socket)
  (progn
    (print 'worker)
    (handlematerial
     (recv socket))
    (close socket)
    (routine (pget))))

(defun  terminal (mode lst)
  (if (eq mode 0)
      (worker (network (connect (storage 127.0.0.1:8888))
                       (concat (storage q=) lst)))
    (if (eq mode 1)
        (pcreate 1 'worker (network (connect (storage 127.0.0.1:8888))
                                    (concat (storage q=) lst)))
      (if (eq mode 2)
          (worker (network (connect (storage 127.0.0.1:8888))
                           (ddd lst)))
        (pcreate 1 'worker (network (connect (storage 127.0.0.1:8888))
                                    (ddd lst)))))))

(defun  reactor(mode)
  (if (eofstdin)
      (print 'over)
    (progn
      (terminal
       mode
       (strip (stdin)))
      (reactor))))

(reactor 0)
