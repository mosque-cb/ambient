(defun exactjson(material)
  (exact material 'BRACEL))

(defun genematerial(echo)
  (jtosx (jaddobject
          (jaddobject (jcreate) 
                      'key
                      (jcreatestring  (timestring)))
          'value
          (jaddobject
           (jcreate)
           'owner
           (jcreatestring  echo)))))

(defun  decide (echo)
  (if (debugp echo)
      (concat (storage eval material is ) echo)
    (if (not (storagep echo))
        (storage eval material shoule be storage)
      (if (big (strlen echo) 512)
          (print (strdup echo  1  512))
        echo))))

(defun  post (socket data)
  (progn
    (send socket (concat (lineconcat (list (storage POST /v1.1/device/192271/sensor/210631/datapoints/ HTTP/1.1) 
                                           (storage Host: api.yeelink.net) 
                                           (storage U-ApiKey:8b6c51b8a18ccbdae3c7ac74169ec3da) 
                                           (concat (storage Content-Length: ) (itoa (strlen data)))
                                           (storage User-Agent: http_post) 
                                           (storage Content-Type: application/json) 
                                           (storage Accept: */*) 
                                           (storage Accept-Language: utf8) 
                                           (storage Accept-Charset: iso-8859-1,*,utf-8) 
                                           (storage Authorization: Basic YWRtaW46YWRtaW4=) 
                                           (storage Connection: Keep-Alive)))
                         'LINE
                         'LINE))
    (send socket data)
    (print
     (recv socket 1000000))
    (close socket)))

(defun  relay (cmd)
  (post (connect (storage api.yeelink.net))  
        (genematerial (decide (eval cmd)))))

(defun  handlejson (json)
  (if (eq json nil)
      nil
    (progn
      (relay (jgetstring 
              (jgetobject 
               (jgetobject json 
                           'value) 
               'owner)))
      json)))

(defun handlerecv(socket)
  (progn
    (print (jtosx (handlejson 
                   (makejson 
                    (exactjson 
                     (recv  socket))))))
    (close socket)))
  
(defun  get (socket)
  (progn
    (send socket (concat (lineconcat (list (storage GET /v1.1/device/192271/sensor/210630/datapoints/ HTTP/1.0) 
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

(defun trigger(n)
  (progn
    (handlerecv (get (connect (storage api.yeelink.net))))
    (sleep  3600)
    (trigger (add n 1))))

(trigger 0)

