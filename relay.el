(defun extractjson(material)
  (extract material 'BRACEL))

(defun genematerial(echo)
  (jtosx (jaddobject
          (jaddobject (jcreate) 
                      'key
                      (jcreatestring  (timetostring (unixtime))))
          'value
          (jaddobject
           (jcreate)
           'owner
           (jcreatestring  echo)))))

(defun  decide (echo)
  (if (debugp echo)
      (concat (quote eval material is ) echo)
    (if (not (quotep echo))
        (quote eval material shoule be quote)
      (if (big (strlen echo) 512)
          (print (strdup echo  1  512))
        echo))))

(defun  post (socket data)
  (progn
    (send socket (concat (lineconcat (list (quote POST /v1.1/device/192271/sensor/210631/datapoints/ HTTP/1.1) 
                                           (quote Host: api.yeelink.net) 
                                           (quote U-ApiKey:8b6c51b8a18ccbdae3c7ac74169ec3da) 
                                           (concat (quote Content-Length: ) (itoa (strlen data)))
                                           (quote User-Agent: http_post) 
                                           (quote Content-Type: application/json) 
                                           (quote Accept: */*) 
                                           (quote Accept-Language: utf8) 
                                           (quote Accept-Charset: iso-8859-1,*,utf-8) 
                                           (quote Authorization: Basic YWRtaW46YWRtaW4=) 
                                           (quote Connection: Keep-Alive)))
                         'LINE
                         'LINE))
    (send socket data)
    (print
     (recv socket 1000000))
    (close socket)))

(defun  relay (cmd)
  (post (connect (quote api.yeelink.net))  
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
                    (extractjson 
                     (recv  socket))))))
    (close socket)))
  
(defun  get (socket)
  (progn
    (send socket (concat (lineconcat (list (quote GET /v1.1/device/192271/sensor/210630/datapoints/ HTTP/1.0) 
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

(defun trigger(n)
  (progn
    (handlerecv (get (connect (quote api.yeelink.net))))
    (sleep  3600)
    (trigger (add n 1))))

(trigger 0)

