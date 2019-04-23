(seq ok 0)
(seq notok 0)

(defun compare(online offline)
  (progn
    (if (eq (mod ok 1000) 0)
        (progn
          (print (concat 'ok 'SPACE ok))
          (print (concat 'notok 'SPACE notok)))
      nil)
    (if (eq online offline)
        (seq  ok (add ok 1))
      (progn
        (comment (print online))
        (comment (print offline))
        (seq  notok (add notok 1))))))

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
    (send socket (concat (lineconcat (list (quote POST /UserService/get_user_regular_stay_point/ HTTP/1.1) 
                                           (concat (quote Host: ) ip)
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
    socket))

(defun  terminal (paras)
  (compare 
   (wraprecv (post (connect (quote  10.52.94.16:8560))  
                   (quote 10.52.94.16)
                   paras))
   (wraprecv (post (connect (quote  10.50.38.32:8560))
                   (quote 10.50.38.32)
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
