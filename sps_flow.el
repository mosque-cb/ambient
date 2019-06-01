(seq sps (list
           (quote 10.94.32.33:8560)))

(defun mystrdup (material begin end)
  (if (or (eq material nil)
          (eq begin nil)
          (eq end nil))
      nil
    (strdup material 
            begin
            end)))

(defun extract_qps_helper (material label)
  (if (eq material nil)
      nil
    (mystrdup
     material
     0
     (find material (quote </span></p>)))))

(defun extract_qps (material label)
  (if (eq material nil)
      nil
    (extract_qps_helper
     (mystrdup material
               (add (strlen label) (find material label) 2)
               (strlen material))
     (quote </span></p>))))

(defun prepare_extract_qps (material)
  (if (eq material nil)
      nil
    (extract_qps
     (mystrdup material (find material (quote qps: )) (strlen material))
     (quote user_service_get_place_semantic_info_encrypt_qps))))

(defun process(material)
  (if (eq material nil)
      nil
    (prepare_extract_qps
     (mystrdup material
               (find material (quote <h4>get_place_semantic_info_encrypt ))
               (strlen material)))))

(defun phone(inf)
  (system (concat (quote gsmsend  -s emp01.company.com:15002  -s emp02.company.com:15002  13641166638@)
                  inf)))

(defun log(ip qps type)
  (print 
   (concat 
    (quote ip is:)
    ip
    'SPACE 
    (quote qps is:)
    qps)))

(defun decision(ip qps type)
  (if (eq type 'orcp_bj_nj)
      (if (big qps 1500)
          (phone (concat (quote big_than_1500_) ip (quote _qps_) qps (quote _type_) type))
        (if (big 100 qps)
            (phone (concat (quote small_than_100_) ip (quote _qps_) qps (quote _type_) type))
          (log ip qps type)))
    (if (eq type 'nj_old)
        (if (big qps 4000)
            (phone (concat (quote big_than_4000_) ip (quote _qps_) qps (quote _type_) type))
          (log ip qps type))
      (if (big qps 3000)
          (phone (concat (quote big_than_3000_) ip (quote _qps_) qps (quote _type_) type))
        (log ip qps type)))))
  
(defun  get (socket)
  (progn
    (send socket (concat (lineconcat (list (quote GET /status/ HTTP/1.0) 
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

(defun trigger(machines type)
  (if (eq machines nil)
      nil
    (progn    
      (decision
       (car machines)
       (atoi (process (recv (get (connect (car machines)))
                            1000000)))
       type)
      (trigger (cdr machines) type))))

(defun loop()
  (progn
    (print (timetostring (unixtime)))
    (print 'sps)
    (trigger sps 'sps)
    (sleep 60)
    (loop)))

(loop)

