(setq ok 0)
(setq notok 0)

(defun compare(online offline)
  (progn
    (if (eq (mod ok 1000) 0)
        (progn
          (print (concat 'ok 'SPACE ok))
          (print (concat 'notok 'SPACE notok)))
      nil)
    (if (eq online offline)
        (setq  ok (add ok 1))
      (progn
        (comment (print online))
        (comment (print offline))
        (setq  notok (add notok 1))))))

(defun fake(material socket)
  (progn
    (close socket)
    material))

(defun wraprecv(socket)
  (fake (recv  socket)
        socket))

(defun genematerial(cuid)
  (jtosx  
   (jaddobject 
    (jaddobject 
     (jaddobject (jcreate) 
                 'cuid
                 (jcreatestring  cuid))
     'header
     (jaddobject (jcreate) 
                 'servicekey
                 (jcreatestring  'rd_test)))
    'need_cityid
    (jcreatetrue))))

(defun  post (socket ip data)
  (progn
    (send socket (concat (lineconcat (list (storage POST /UserService/get_user_regular_stay_point/ HTTP/1.1) 
                                           (concat (storage Host: ) ip)
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
    socket))

(defun  terminal (paras)
  (compare 
   (wraprecv (post (connect (storage  10.52.94.16:8560))  
                   (storage 10.52.94.16)
                   paras))
   (wraprecv (post (connect (storage  10.50.38.32:8560))
                   (storage 10.50.38.32)
                   paras))))

(defun  reactor()
  (if (eofstdin)
      (progn
        (print (concat 'ok 'SPACE ok))
        (print (concat 'notok 'SPACE notok)))
    (progn
      (terminal 
       (genematerial
        (car (tabsplit (strip (stdin))))))
      (reactor))))

(reactor)
