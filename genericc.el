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

(post 
 (connect (storage api.yeelink.net))
 (genematerial (print
                (storage 
                 (progn
                   (setq outcome (progn  (system (storage  hostname -i  && date > haha)) (fload 'haha)))
                   (print outcome))))))




