(setq ok 0)
(setq notok 0)

(defun format_item(lst)
  (if  (eq lst nil)
      nil
    (concat
     (concat '$ 
             (strlen (car lst)) 
             'LINE 
             (car lst) 
             'LINE)
     (format_item (cdr lst)))))

(defun format_protocol(cmd)
  (concat
   '*   
   (size cmd) 
   'LINE
   (format_item cmd)))

(defun worker(material)
  (if (eq material nil)
      nil
    (if (eq (size material) 2)
        (if (eq (car (cdr material)) 'nothing)
            'find
          nil)
      'not_find)))

(defun execute(socket cmd)
  (progn
    (send socket  (format_protocol cmd))
    (worker
     (linesplit (recv socket 1000000)))))

(defun wrapsend(socket key)
  (if (eq socket nil)
      nil
    (execute  socket (list 'get 
                           (concat (storage st:) key)))))

(defun compare(online offline)
  (progn
    (if (eq (mod ok 1000) 0)
        (progn
          (print (concat 'ok 'SPACE ok))
          (print (concat 'notok 'SPACE notok)))
      nil)
    (if (big (atoi (car (cdr offline))) 3)
        nil
      (if (eq online nil)
          nil
        (progn
          (comment
           (print (jtos online))
           (print offline))
          (if (eq (matter online (car (cdr offline)))
                  (car offline))
              (setq  ok (add ok 1))
            (setq  notok (add notok 1)))
          (killjson online))))))

(defun switch(type)
  (if (eq type 0)
      'HOME_STATUS
    (if (eq type 1)
        'COMPANY_STATUS
      (if (eq type 2)
          'AREA_STATUS
        (if (eq type 3)
            'LOCAL_STATUS
          (if (eq type 4)
              'EWORK_STATUS
            (if (eq type 5)
                'ETRAVEL_STATUS
              (if (eq type 6)
                  'EFAMILY_STATUS
                (if (eq type 7)
                    'ELOW_STATUS
                  (if (eq type 8)
                      'EHIGH_STATUS
                    (if (eq type 9)
                        'ENEW_STATUS
                      (if (eq type 10)
                          'CITY_STATUS
                        (if (eq type 100)
                            'UNKNOW_STATUS
                          nil)))))))))))))

(defun matter(material type)
  (serial (switch (atoi type))
          (jgetobject
           material
           'value)
          0))

(defun serial(type jsons n)
  (if (eq n (jgetarraysize jsons))
      nil
    (if (eq (jgetstring (jgetobject (jgetarrayitem jsons n)
                                    'type))
            type)
        (concat (jgetint (jgetobject (jgetarrayitem jsons n)
                                     'longitude))
                'DOT
                (jgetint (jgetobject (jgetarrayitem jsons n)
                                     'latitude)))
      (serial type jsons (add n 1)))))

(defun fake(material socket)
  (progn
    (close socket)
    material))

(defun wraprecv(socket)
  (fake (makejson (exact (recv  socket 1000000) 'BRACEL))
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

(defun  terminal (socket lst)
  (if (or (big 16 (strlen (car lst)))
          (big (strlen (car lst)) 50))
      nil
    (if (eq   (wrapsend socket (car lst))
              'not_find)
        (compare 
         (wraprecv (post (connect (storage  10.94.32.33:8560))
                         (storage   10.94.32.33)
                         (genematerial (car lst))))
         (cdr (cdr lst)))
      nil)))

(defun  reactor(socket)
  (if (eofstdin)
      (progn
        (print (concat 'ok 'SPACE ok))
        (print (concat 'notok 'SPACE notok)))
    (progn
      (terminal 
       socket
       (tabsplit (strip (stdin))))
      (reactor))))

(reactor
 (connect (storage  10.195.253.39:16379)))



