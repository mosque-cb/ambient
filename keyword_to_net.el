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

(defun  get (socket)
  (send socket (concat (lineconcat (list (storage GET /?debug_info=as&debug_id=123456&wwsy=yes&rows=60&start=0&wt=json&q=你好吗努比亚α我很好&fl=vendor_Name,partnumber,brand_Name,auxdescription,three_groupName,three_groupExtName,author,isbn,unit_searchable_attr,title,two_groupName,threeGroupIds,short_brand_Id HTTP/1.0) 
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
    (handlematerial
     (recv socket))
    (close socket)
    (routine (pget))))

(defun  network()
  (get (connect (storage 172.19.59.15:8888))))

(defun dispatch()
  (progn
    (pcreate 1 'worker (network))
    (sleep 1)
    (dispatch)))

(pjoin (pcreate 1 'dispatch))
